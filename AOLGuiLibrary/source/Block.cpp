#include "Block.h"
#include "events.h"
#include <allegro5/allegro_primitives.h>

#include <algorithm>

namespace agl
{
	Block::Block()
	{
		hover = false;
		visibility = true;
		style = NULL;
		parent = NULL;
		direct_hover = false;
		sizing = AGL_SIZING_CONTENTBOX;
		is_dragged = false;
		scroll_detection = false;
		parent_scroll_detection = false;
	}

	Block::~Block()
	{
		for (auto& child : this->children)
		{
			delete child;
			child = NULL;
		}

		this->children.clear();
	}

	void Block::set_size(int width, int height)
	{
		int new_width = width, new_height = height;
		std::string reduce;
		switch (sizing)
		{
		case AGL_SIZING_MARGINBOX:
			reduce.push_back('m');
			[[fallthrough]];

		case AGL_SIZING_BORDERBOX:
			reduce.push_back('b');
			[[fallthrough]];

		case AGL_SIZING_PADDINGBOX:
			reduce.push_back('p');
			break;
		default:
			break;
		}

		this->box.inner_width = width - get_width(reduce);
		this->box.inner_height = height - get_height(reduce);

		raise_event({
			.type = AGL_EVENT_BLOCK_RESIZED,
			.source = this,
			.block = this
			});
	}

	int Block::get_width(const std::string getter) const
	{
		int value = 0;
		for (char g : getter)
		{
			switch (g)
			{
			case 'c':
				value += box.inner_width;
				break;
			case 'p':
				value += box.padding.left + box.padding.right;
				break;
			case 'b':
				value += box.border.left + box.border.right;
				break;
			case 'm':
				value += box.margin.left + box.margin.right;
				break;
			default:
				break;
			}
		}

		return value;
	}

	int Block::get_height(const std::string getter) const
	{
		int value = 0;
		for (char g : getter)
		{
			switch (g)
			{
			case 'c':
				value += box.inner_height;
				break;
			case 'p':
				value += box.padding.top + box.padding.bottom;
				break;
			case 'b':
				value += box.border.top + box.border.bottom;
				break;
			case 'm':
				value += box.margin.top + box.margin.bottom;
				break;
			default:
				break;
			}
		}

		return value;
	}

	int Block::get_inner_width() const
	{
		return box.inner_width;
	}

	int Block::get_inner_height() const
	{
		return box.inner_height;
	}

	void Block::set_location(int x, int y)
	{
		set_location(Point(x, y));
	}

	void Block::set_location(Point point)
	{
		location = point;

		raise_event({ .type = AGL_EVENT_BLOCK_MOVED, .source = this });
	}

	Point Block::get_location() const
	{
		return location;
	}

	void Block::set_background_color(Color new_color)
	{
		this->background_color = new_color;
	}

	void Block::set_margins(int top, int right, int bottom, int left)
	{
		box.margin.top = top;
		box.margin.right = right;
		box.margin.bottom = bottom;
		box.margin.left = left;

		if (sizing == AGL_SIZING_MARGINBOX)
			set_size(get_box_width(sizing), get_box_height(sizing));
	}

	void Block::add(Block* block)
	{
		this->children.push_back(block);
		block->set_parent(this);
		if (parent_scroll_detection || scroll_detection)
			block->enable_parent_scroll_detection(true);
	}

	void Block::apply(Style* applied_style)
	{
		style = applied_style;

		std::string sides[4] = { "left", "top", "bottom", "right" };
		std::string value_names[3] = { "padding", "border", "margin" };
		BoxValues* values[3] = { &box.padding, &box.border, &box.margin };

		// Margin, border, padding
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (style->values[value_names[i] + "_" + sides[j]].source)
				{
					int* side_value = 0;
					switch (j)
					{
					case 0:
						side_value = &values[i]->left;
						break;
					case 1:
						side_value = &values[i]->top;
						break;
					case 2:
						side_value = &values[i]->bottom;
						break;
					case 3:
						side_value = &values[i]->right;
						break;
					}

					*side_value = std::get<int>(style->values[value_names[i] + "_" + sides[j]].value);
				}
			}
		}

		// Border colors
		for (int j = 0; j < 4; j++)
		{
			if (style->values["border_color_" + sides[j]].source)
			{
				Color* side_value = 0;
				switch (j)
				{
				case 0:
					side_value = &box.border_colors.left;
					break;
				case 1:
					side_value = &box.border_colors.top;
					break;
				case 2:
					side_value = &box.border_colors.bottom;
					break;
				case 3:
					side_value = &box.border_colors.right;
					break;
				}

				*side_value = std::get<Color>(style->values["border_color_" + sides[j]].value);
			}
		}

		// background color, width, height
		if (style->values["background_color"].source)
			set_background_color(std::get<Color>(style->values["background_color"].value));

		if (style->values["width"].source)
			box.inner_width = std::get<int>(style->values["width"].value);

		if (style->values["height"].source)
			box.inner_height = std::get<int>(style->values["height"].value);

		if (style->values["sizing"].source)
		{
			sizing = std::get<int>(style->values["sizing"].value);

			set_size(get_inner_width(), get_inner_height());
		}
	}

	Style* Block::get_style()
	{
		return this->style;
	}

	void Block::set_up_clipping(Point base_location, int* old_x, int* old_y, int* old_width, int* old_height)
	{
		al_get_clipping_rectangle(old_x, old_y, old_width, old_height);

		int new_x = std::max(int(base_location.x), *old_x),
			new_y = std::max(int(base_location.y), *old_y),
			new_w = std::min(int(base_location.x) + box.inner_width, *old_x + *old_width) - new_x,
			new_h = std::min(int(base_location.y) + box.inner_height, *old_y + *old_height) - new_y;

		al_set_clipping_rectangle(new_x, new_y, new_w, new_h);
	}

	void Block::reset_clipping(int old_x, int old_y, int old_width, int old_height)
	{
		al_set_clipping_rectangle(old_x, old_y, old_width, old_height);
	}

	void Block::draw_background(Point base_location)
	{
		Point topleft = base_location;

		Point bottomright(
			topleft.x + get_width("cpb"),
			topleft.y + get_height("cpb")
		);

		Point inner_topleft = topleft + Point(box.border.left, box.border.top);
		Point inner_bottomright = bottomright + Point(-box.border.right, -box.border.bottom);

		const float top_border[8] = {
			topleft.x, topleft.y,
			inner_topleft.x, inner_topleft.y,
			inner_bottomright.x, inner_topleft.y,
			bottomright.x, topleft.y,
		};

		const float right_border[8] = {
			inner_bottomright.x, inner_topleft.y,
			inner_bottomright.x, inner_bottomright.y,
			bottomright.x, bottomright.y,
			bottomright.x, topleft.y,
		};

		const float bottom_border[8] = {
			inner_topleft.x, inner_bottomright.y,
			topleft.x, bottomright.y,
			bottomright.x, bottomright.y,
			inner_bottomright.x, inner_bottomright.y,
		};

		const float left_border[8] = {
			topleft.x, topleft.y,
			topleft.x, bottomright.y,
			inner_topleft.x, inner_bottomright.y,
			inner_topleft.x, inner_topleft.y,
		};

		// Background
		al_draw_filled_rectangle(
			topleft.x,
			topleft.y,
			bottomright.x,
			bottomright.y,
			background_color.calculated
		);

		// Right border
		al_draw_filled_polygon(
			right_border,
			4,
			box.border_colors.right.calculated
		);

		// Bottom border
		al_draw_filled_polygon(
			bottom_border,
			4,
			box.border_colors.bottom.calculated
		);

		// Left border
		al_draw_filled_polygon(
			left_border,
			4,
			box.border_colors.left.calculated
		);

		// Top border
		al_draw_filled_polygon(
			top_border,
			4,
			box.border_colors.top.calculated
		);

		if (this->direct_hover && debug::debug)
			al_draw_filled_rectangle(
				topleft.x,
				topleft.y,
				bottomright.x,
				bottomright.y,
				debug::highlight.calculated
			);
	}

	void Block::draw_block(Point base_location)
	{
		for (const auto& child : children)
		{
			child->draw(base_location);
		}
	}

	void Block::on_press_drag(Event e)
	{
		if (e.type == AGL_EVENT_MOUSE_PRESSED && e.source == this)
			is_dragged = true;
	}

	void Block::on_release_drag(Event e)
	{
		if (e.type == AGL_EVENT_MOUSE_RELEASED && e.source == this)
			is_dragged = false;
	}

	void Block::draw(Point base_location)
	{
		if (visibility)
		{
			draw_background(Point(
				base_location.x + location.x + box.margin.left,
				base_location.y + location.y + box.margin.top
			));

			int clip_x = 0, clip_y = 0, clip_w = 0, clip_h = 0;

			Point clip_base(
				base_location.x + location.x + box.margin.left + box.border.left + box.padding.left,
				base_location.y + location.y + box.margin.top + box.border.top + box.padding.top
			);

			set_up_clipping(clip_base, &clip_x, &clip_y, &clip_w, &clip_h);

			draw_block(clip_base);

			reset_clipping(clip_x, clip_y, clip_w, clip_h);
		}
	}

	void Block::update(
		Point mouse_location, Point base_location, Block** event_receiver, bool force_fail
	)
	{
		if (*event_receiver && (*event_receiver)->get_dragged()) return;

		bool previous_hover = this->hover;
		direct_hover = false;

		if (!force_fail && visibility)
		{
			Point topleft(
				base_location.x + this->location.x
				+ this->box.margin.left + this->box.border.left,
				base_location.y + this->location.y
				+ this->box.margin.top + this->box.border.top
			);

			Point bottomright(
				topleft.x + get_width("cp"),
				topleft.y + get_height("cp")
			);

			if (

				topleft.x < mouse_location.x &&
				mouse_location.x < bottomright.x &&
				topleft.y < mouse_location.y &&
				mouse_location.y < bottomright.y
				)
			{
				*event_receiver = this;
				this->hover = true;
			}
			else
			{
				this->hover = false;
			}
		}
		else
			this->hover = false;

		if ((this->hover || previous_hover))
		{
			for (auto& child : this->children)
			{
				child->update(
					mouse_location,
					Point(
						base_location.x + this->location.x + this->box.margin.left
						+ this->box.border.left + this->box.padding.left,
						base_location.y + this->location.y + this->box.margin.top
						+ this->box.border.top + this->box.padding.top
					),
					event_receiver,
					previous_hover && !this->hover
				);
			}
		}

		if (hover && !previous_hover)
			raise_event({ .type = AGL_EVENT_BLOCK_HOVER_GAINED, .source = this });
		else if (!hover && previous_hover)
			raise_event({ .type = AGL_EVENT_BLOCK_HOVER_LOST, .source = this });
	}

	void Block::add_event_listener(Block* listener)
	{
		event_listeners.push_back(listener);
	}

	void Block::add_event_source(Block* source)
	{
		source->add_event_listener(this);
	}

	void Block::add_event_function(std::function<void(Event)> function)
	{
		event_functions.push_back(function);
	}

	void Block::raise_event(Event e)
	{
		handle_event(e);

		for (const auto& listener : event_listeners)
			listener->handle_event(e);

		if (parent_scroll_detection && e.type == AGL_EVENT_MOUSE_MOVED && e.dz)
			handle_scroll(e);
	}

	void Block::handle_event(Event e)
	{
		for (const auto& event_function : event_functions)
			event_function(e);
	}

	void Block::set_parent(Block* block)
	{
		parent = block;
	}

	Block* Block::get_parent()
	{
		return parent;
	}

	void Block::set_direct_hover(bool value)
	{
		direct_hover = value;
	}

	int Block::get_total_width() const
	{
		int total_width = this->get_width("c");
		for (const auto& child : children)
			total_width = std::max(total_width, (int)child->get_location().x + child->get_width());

		return std::max(total_width, this->get_width("cp"));
	}
	
	int Block::get_total_height() const
	{
		int total_height = this->get_height("c");
		for (const auto& child : children)
			total_height = std::max(total_height, (int)child->get_location().y + child->get_height());

		return std::max(total_height, this->get_height("cp"));
	}

	void Block::set_sizing(char _sizing)
	{
		sizing = _sizing;
	}

	void Block::enable_dragging()
	{
		add_event_function(std::bind(&Block::on_press_drag, this, std::placeholders::_1));
		add_event_function(std::bind(&Block::on_release_drag, this, std::placeholders::_1));
	}

	bool Block::get_dragged() const
	{
		return is_dragged;
	}

	bool Block::detects_scroll() const
	{
		return scroll_detection;
	}

	bool Block::parent_detects_scroll() const
	{
		return parent_scroll_detection;
	}

	void Block::enable_scroll_detection(bool detection)
	{
		scroll_detection = detection;
	}

	void Block::enable_parent_scroll_detection(bool detection)
	{
		parent_scroll_detection = detection;
	}

	void Block::update_scroll_detection()
	{
		if (
			parent && (parent->detects_scroll() || parent->parent_detects_scroll()) ||
			parent_scroll_detection
			)
		{
			parent_scroll_detection = true;
			for (auto& child : children)
				update_scroll_detection();
		}
		else if (scroll_detection)
			for (auto& child : children)
				update_scroll_detection();
	}

	void Block::handle_scroll(Event e)
	{
		if (scroll_detection)
		{
			e.source = this;
			raise_event(e);
		}
		else if (parent && parent_scroll_detection)
			parent->handle_scroll(e);
	}

	int Block::get_box_width(char boxname) const
	{
		std::string boxes = "c";
		switch (boxname)
		{
		case AGL_SIZING_MARGINBOX:
			boxes.push_back('m');
			[[fallthrough]];

		case AGL_SIZING_BORDERBOX:
			boxes.push_back('b');
			[[fallthrough]];

		case AGL_SIZING_PADDINGBOX:
			boxes.push_back('p');
			break;
		default:
			break;
		}

		return get_width(boxes);
	}

	int Block::get_box_height(char boxname) const
	{
		std::string boxes = "c";
		switch (boxname)
		{
		case AGL_SIZING_MARGINBOX:
			boxes.push_back('m');
			[[fallthrough]];

		case AGL_SIZING_BORDERBOX:
			boxes.push_back('b');
			[[fallthrough]];

		case AGL_SIZING_PADDINGBOX:
			boxes.push_back('p');
			break;
		default:
			break;
		}

		return get_height(boxes);
	}


	void Block::set_borders(int top, int right, int bottom, int left)
	{
		box.border.top = top;
		box.border.right = right;
		box.border.bottom = bottom;
		box.border.left = left;
	}

	void Block::set_borders(Color border_col)
	{
		box.border_colors.top = border_col;
		box.border_colors.right = border_col;
		box.border_colors.bottom = border_col;
		box.border_colors.left = border_col;

		if (sizing >= AGL_SIZING_BORDERBOX)
			set_size(get_box_width(sizing), get_box_height(sizing));
	}

	void Block::set_paddings(int top, int right, int bottom, int left)
	{
		box.padding.top = top;
		box.padding.right = right;
		box.padding.bottom = bottom;
		box.padding.left = left;

		if (sizing >= AGL_SIZING_PADDINGBOX)
			set_size(get_box_width(sizing), get_box_height(sizing));
	}

}