#include "agl/Block.h"
#include "agl/events.h"

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
		focus = false;
		detects_focus = false;
		graphics_handler = 0;
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

	int Block::get_width(const std::string& getter) const
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

	int Block::get_height(const std::string& getter) const
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

	void Block::set_location(float x, float y)
	{
		set_location(Point(x, y));
	}

	void Block::set_location(const Point& point)
	{
		location = point;

		raise_event({ .type = AGL_EVENT_BLOCK_MOVED, .source = this });
	}

	Point Block::get_location() const
	{
		return location;
	}

	void Block::set_background_color(const Color& new_color)
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

	void Block::set_visible(bool visible)
	{
		visibility = visible;
	}

	void Block::add(Block* block)
	{
		this->children.push_back(block);
		block->set_parent(this);
		if (parent_scroll_detection || scroll_detection)
			block->enable_parent_scroll_detection(true);

		if (detects_focus)
			add_event_source(block);

		block->connect_graphics_handler(graphics_handler);
	}

	void Block::apply(const Style* applied_style)
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
				if (style->values.at(value_names[i] + "_" + sides[j]).source)
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

					*side_value = std::get<int>(style->values.at(value_names[i] + "_" + sides[j]).value);
				}
			}
		}

		// Border colors
		for (int j = 0; j < 4; j++)
		{
			if (style->values.at("border_color_" + sides[j]).source)
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

				*side_value = std::get<Color>(style->values.at("border_color_" + sides[j]).value);
			}
		}

		// background color, width, height
		if (style->values.at("background_color").source)
			set_background_color(std::get<Color>(style->values.at("background_color").value));

		if (style->values.at("width").source)
			box.inner_width = std::get<int>(style->values.at("width").value);

		if (style->values.at("height").source)
			box.inner_height = std::get<int>(style->values.at("height").value);

		if (style->values.at("sizing").source)
		{
			sizing = std::get<int>(style->values.at("sizing").value);

			set_size(get_inner_width(), get_inner_height());
		}
	}

	const Style* Block::get_style()
	{
		return this->style;
	}

	void Block::set_up_clipping(const Point& base_location)
	{
		Rectangle clip = graphics_handler->get_clipping_rectangle();

		float new_x = std::max(base_location.x, clip.pt.x),
			new_y = std::max(base_location.y, clip.pt.y),
			new_w = std::min(base_location.x + box.inner_width, clip.pt.x + clip.width) - new_x,
			new_h = std::min(base_location.y + box.inner_height, clip.pt.y + clip.height) - new_y;

		graphics_handler->set_clipping_rectangle({ new_x, new_y, new_w, new_h });
	}

	void Block::reset_clipping()
	{
		graphics_handler->pop_clipping_rectangle();
	}

	void Block::draw_background(const Point& base_location)
	{
		// l - large
		// s - small

		// n = north
		// s - south

		// e - east
		// w - west

		Point lnw = base_location;
		Point lse(lnw.x + get_width("cpb"), lnw.y + get_height("cpb"));
		Point snw(lnw.x + box.border.right, lnw.y + box.border.top);
		Point sse(snw.x + get_width("cp"), snw.y + get_height("cp"));

		Point lne(lse.x, lnw.y);
		Point lsw(lnw.x, lse.y);

		Point sne(sse.x, snw.y);
		Point ssw(snw.x, sse.y);

		// Background
		graphics_handler->draw_filled_rectangle({ lnw, (float)get_width("cpb"), (float)get_height("cpb") }, background_color);
		

		// Right border
		graphics_handler->draw_filled_triangle(sne, lse, sse, box.border_colors.right);
		graphics_handler->draw_filled_triangle(sne, lse, lne, box.border_colors.right);


		// Bottom border
		graphics_handler->draw_filled_triangle(ssw, lse, sse, box.border_colors.bottom);
		graphics_handler->draw_filled_triangle(ssw, lse, lsw, box.border_colors.bottom);

		// Left border
		graphics_handler->draw_filled_triangle(lnw, ssw, lsw, box.border_colors.left);
		graphics_handler->draw_filled_triangle(lnw, ssw, snw, box.border_colors.left);

		// Top border
		graphics_handler->draw_filled_triangle(lnw, sne, lne, box.border_colors.top);
		graphics_handler->draw_filled_triangle(lnw, sne, snw, box.border_colors.top);
	}

	void Block::draw_block(const Point& base_location)
	{
		for (const auto& child : children)
		{
			child->draw(base_location);
		}
	}

	void Block::on_press_drag(const Event& e)
	{
		if (e.type == AGL_EVENT_MOUSE_PRESSED && e.source == this)
			is_dragged = true;
	}

	void Block::on_release_drag(const Event& e)
	{
		if (e.type == AGL_EVENT_MOUSE_RELEASED && e.source == this)
			is_dragged = false;
	}

	void Block::on_press_focus(const Event& e)
	{
		if ((e.source == this || (e.source->parent && e.source->parent == this))
			&& e.type == AGL_EVENT_MOUSE_PRESSED)
			if (!focus)
			{
				focus = true;
				raise_event({ .type = AGL_EVENT_BLOCK_FOCUS_GAINED, .source = this });
			}
			else
				if (!hover)
				{
					focus = false;
					raise_event({ .type = AGL_EVENT_BLOCK_FOCUS_LOST, .source = this });
				}
	}

	void Block::draw(Point base_location)
	{
		if (visibility)
		{
			draw_background(Point(
				base_location.x + location.x + box.margin.left,
				base_location.y + location.y + box.margin.top
			));

			Point clip_base(
				base_location.x + location.x + box.margin.left + box.border.left + box.padding.left,
				base_location.y + location.y + box.margin.top + box.border.top + box.padding.top
			);


			if (this->direct_hover && debug::debug)
			{
				Point margin_topleft((float)box.margin.left, (float)box.margin.top);
				graphics_handler->draw_filled_rectangle(
					{ base_location + location + margin_topleft, (float)get_width("cpb"), (float)get_height("cpb") },
					debug::highlight
				);
			}

			set_up_clipping(clip_base);

			draw_block(clip_base);

			reset_clipping();
		}
	}

	void Block::update(
		const Point& mouse_location, const Point& base_location, Block** event_receiver,
		Block** focus_listener, bool force_fail
	)
	{
		if (!visibility) return;
		if (*event_receiver && (*event_receiver)->get_dragged()) return;

		if (focus)
			*focus_listener = this;

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
					event_receiver, focus_listener,
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

	void Block::add_event_function(std::function<void(const Event&)> function)
	{
		event_functions.push_back(function);
	}

	void Block::raise_event(const Event& e)
	{
		handle_event(e);

		for (const auto& listener : event_listeners)
			listener->handle_event(e);

		if (parent_scroll_detection && e.type == AGL_EVENT_MOUSE_MOVED && e.dz)
			handle_scroll(e);
	}

	void Block::handle_event(const Event& e)
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

	int Block::get_total_width(bool include_invisible, bool consider_width) const
	{
		int total_width = consider_width * this->get_width("cp");
		for (const auto& child : children)
			if (include_invisible || child->visibility)
				total_width = std::max(total_width, (int)child->get_location().x + child->get_width());

		return total_width;
	}
	
	int Block::get_total_height(bool include_invisible, bool consider_height) const
	{
		int total_height = consider_height * this->get_height("cp");
		for (const auto& child : children)
			if (include_invisible || child->visibility)
				total_height = std::max(total_height, (int)child->get_location().y + child->get_height());

		return total_height;
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

	void Block::enable_focus()
	{
		add_event_function(
			std::bind(&Block::on_press_focus, this, std::placeholders::_1)
		);
		detects_focus = true;
	}

	bool Block::get_dragged() const
	{
		return is_dragged;
	}

	bool Block::get_focus() const
	{
		return focus;
	}

	void Block::set_focus(bool val)
	{
		bool rec = focus;
		focus = val;
		if (rec && !focus)
			raise_event({ .type = AGL_EVENT_BLOCK_FOCUS_LOST, .source = this });
		else if (!rec && focus)
			raise_event({ .type = AGL_EVENT_BLOCK_FOCUS_GAINED, .source = this });
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

	void Block::handle_scroll(const Event& e)
	{
		if (scroll_detection)
		{
			Event new_event = e;
			new_event.source = this;
			raise_event(new_event);
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

	void Block::bring_to_top(Block* child)
	{
		auto it = std::find(children.begin(), children.end(), child);

		if (it != children.end())
		{
			children.erase(it);
			children.push_back(child);
		}
	}

	int Block::get_child_index(Block* child)
	{
		auto it = std::find(children.begin(), children.end(), child);

		if (it != children.end())
			return (int)std::distance(children.begin(), it);

		return -1;
	}

	Block* Block::get_child_by_index(int child)
	{
		return children[child];
	}

	int Block::get_children_amount() const
	{
		return (int)children.size();
	}

	void Block::connect_graphics_handler(GraphicsHandler* graphics_handler_)
	{
		graphics_handler = graphics_handler_;

		for (auto& child : children)
			child->connect_graphics_handler(graphics_handler_);
	}

	Block& Block::operator<<(Block& block)
	{
		add(&block);
		return *this;
	}


	void Block::set_bottom_margin(int bottom)
	{
		box.margin.bottom = bottom;
	}

	void Block::set_borders(int top, int right, int bottom, int left)
	{
		box.border.top = top;
		box.border.right = right;
		box.border.bottom = bottom;
		box.border.left = left;
	}

	void Block::set_borders(const BoxColors& borders)
	{
		box.border_colors = borders;
	}

	void Block::set_borders(const Color& border_col)
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