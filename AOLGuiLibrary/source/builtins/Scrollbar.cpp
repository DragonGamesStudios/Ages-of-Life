#include "agl/builtins\Scrollbar.h"
#include "agl/events.h"

namespace agl::builtins
{
	void Scrollbar::connect_scrollable(Block* _scrollable)
	{
		scrollable = _scrollable;
		add_event_source(scrollable);
		config_to_scrollable();
	}

	void Scrollbar::set_size(int width, int height)
	{
		Block::set_size(width, height);

		config(full_size, visible_size);
	}

	void Scrollbar::apply(Style* _style)
	{
		Block::apply(_style);

		if (style->values["axis"].source)
			axis = std::get<int>(style->values["axis"].value);

		if (style->values["marker"].source)
			if (std::get<int>(style->values["marker"].value))
				create_marker();

		if (scrollable)
			config_to_scrollable();
	}

	void Scrollbar::set_step(int _step)
	{
		step = _step;
	}

	void Scrollbar::set_react_to_scrollwheel(bool react)
	{
		react_to_scrollwheel = react;
	}

	void Scrollbar::on_scrollable_resized(Event e)
	{
		if (e.type == AGL_EVENT_BLOCK_RESIZED && e.source == scrollable)
		{
			config_to_scrollable();
		}
	}

	void Scrollbar::on_scroll(Event e)
	{
		if (
			e.type = AGL_EVENT_MOUSE_MOVED && react_to_scrollwheel &&
			(e.source == this || e.source == marker || e.source == scrollable) &&
			e.dz
			)
		{
			change_scroll(-e.dz * step);
		}
	}

	Scrollbar::Scrollbar() : Block()
	{
		axis = AGL_VERTICAL;
		maintain_marker = false;
		full_size = 0;
		visible_size = 0;
		scroll = 0;

		marker = NULL;
		scrollable = NULL;

		step = 1;

		react_to_scrollwheel = true;

		add_event_function(std::bind(&Scrollbar::on_scrollable_resized, this, std::placeholders::_1));
		add_event_function(std::bind(&Scrollbar::on_scroll, this, std::placeholders::_1));
		add_event_function(std::bind(&Scrollbar::on_marker_drag, this, std::placeholders::_1));
	}

	void Scrollbar::config(int full, int visible)
	{
		full_size = full;
		visible_size = visible;

		if (marker)
		{
			float percent = std::min(
				1.f,
				(float)visible_size /
				std::max(full_size, 1)
			);

			if (percent == 0) percent = 1;

			if (axis == AGL_VERTICAL)
				marker->set_size(get_inner_width(), percent * get_inner_height());
			else
				marker->set_size(percent * get_inner_width(), get_inner_height());
		}

		set_scroll(scroll);
	}

	void Scrollbar::config_to_scrollable()
	{
		if (axis == AGL_VERTICAL)
			config(scrollable->get_total_height(), scrollable->get_height("cp"));
		else
			config(scrollable->get_total_width(), scrollable->get_width("cp"));
	}

	void Scrollbar::set_axis(char _axis)
	{
		axis = _axis;

		if (scrollable)
		{
			config_to_scrollable();
		}

	}

	void Scrollbar::connect_marker(Block* _marker)
	{
		marker = _marker;
		config(full_size, visible_size);
	}

	void Scrollbar::set_scroll(int _scroll)
	{
		int previous_scroll = scroll;

		scroll = std::min(full_size - visible_size, std::max(0, _scroll));

		if (marker)
		{
			float percent = (float)scroll / std::max(full_size - visible_size, 1);

			if (axis == AGL_VERTICAL)
				marker->set_location(0, percent * (get_inner_height() - marker->get_height()));
			else
				marker->set_location(percent * (get_inner_width() - marker->get_width()), 0);
		}

		if (previous_scroll != scroll)
			raise_event({ .type = AGL_EVENT_SCROLLBAR_SCROLLED, .source = this });
	}

	void Scrollbar::change_scroll(int delta)
	{
		set_scroll(scroll + delta);
	}

	int Scrollbar::get_scroll() const
	{
		return scroll;
	}

	void Scrollbar::create_marker()
	{
		maintain_marker = true;
		marker = new Block();
		marker->enable_dragging();
		marker->set_sizing(AGL_SIZING_BORDERBOX);
		marker->set_borders(1, 1, 1, 1);
		
		Color marker_bg;

		if (background_color.r + background_color.g + background_color.b > 3 * 128)
			marker_bg = Color(
				background_color.r - 40,
				background_color.g - 40,
				background_color.b - 40,
				background_color.a
			);
		else
			marker_bg = Color(
				background_color.r + 40,
				background_color.g + 40,
				background_color.b + 40,
				background_color.a
			);

		marker->set_background_color(marker_bg);

		Color marker_bc(
			background_color.r + 10,
			background_color.g + 10,
			background_color.b + 10,
			background_color.a
		);

		marker->set_borders(marker_bc);

		config(full_size, visible_size);

		add(marker);
		add_event_source(marker);
	}

	Block* Scrollbar::get_marker() const
	{
		return marker;
	}

	void Scrollbar::on_marker_drag(Event e)
	{
		if (
			e.type == AGL_EVENT_MOUSE_MOVED && e.source == marker && marker->get_dragged() &&
			(e.dx || e.dy) && (e.buttons & AGL_MOUSE_BBUTTON_PRIMARY)
			)
		{
			float proper_coord = e.dy;
			if (axis == AGL_HORIZONTAL) proper_coord = e.dx;
			
			int max_size = get_inner_height(), marker_size = marker->get_height();
			if (axis == AGL_HORIZONTAL)
			{
				max_size = get_inner_width();
				marker_size = marker->get_height();
			}

			float percent = proper_coord / std::max(max_size - marker_size, 1);
			change_scroll(percent * std::max(full_size - visible_size, 1));
		}
	}

}