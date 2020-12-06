#pragma once

#include "Base.h"

#include <vector>
#include <functional>

#define AGL_SIZING_CONTENTBOX 1
#define AGL_SIZING_PADDINGBOX 2
#define AGL_SIZING_BORDERBOX 3
#define AGL_SIZING_MARGINBOX 4

#define AGL_ALIGN_BEGIN 1
#define AGL_ALIGN_END 2
#define AGL_ALIGN_CENTER 3

#define AGL_HORIZONTAL 1
#define AGL_VERTICAL 2

#define AGL_MARKER_CUSTOM 0
#define AGL_MARKER_CREATE 1

#define AGL_TEXTCHUNK_TEXT 0
#define AGL_TEXTCHUNK_IMAGE 1

namespace agl
{
	class Block;

	class Block
	{
	protected:
		Box box;
		Point location;
		Color background_color;
		Style* style;

		bool visibility;
		bool hover;
		bool direct_hover;
		std::vector<Block*> children;

		std::vector<std::function<void(Event)>> event_functions;
		std::vector<Block*> event_listeners;

		Block* parent;

		char sizing;

		bool is_dragged;
		bool scroll_detection;
		bool parent_scroll_detection;

		virtual void set_up_clipping(Point base_location, int* old_x, int* old_y, int* old_width, int* old_height);
		virtual void reset_clipping(int old_x, int old_y, int old_width, int old_height);
		virtual void draw_background(Point base_location);
		virtual void draw_block(Point base_location);
		void on_press_drag(Event e);
		void on_release_drag(Event e);
	public:
		Block();
		virtual ~Block();

		virtual void set_size(int width, int height);

		int get_width(const std::string getter = "cpbm") const;
		int get_height(const std::string getter = "cpbm") const;

		int get_inner_width() const;
		int get_inner_height() const;

		void set_location(int x, int y);
		void set_location(Point point);

		Point get_location() const;

		virtual void set_background_color(Color new_color);

		void set_margins(int top, int right, int bottom, int left);
		void set_margins(BoxValues margins);

		void set_top_margin(int top);
		void set_right_margin(int right);
		void set_bottom_margin(int bottom);
		void set_left_margin(int left);

		void set_borders(int top, int right, int bottom, int left);
		void set_borders(BoxValues borders);
		void set_borders(BoxColors borders);
		void set_borders(int border_w);
		void set_borders(Color border_col);

		void set_top_border(int top);
		void set_right_border(int right);
		void set_bottom_border(int bottom);
		void set_left_border(int left);

		void set_top_border(Color top);
		void set_right_border(Color right);
		void set_bottom_border(Color bottom);
		void set_left_border(Color left);

		void set_paddings(int top, int right, int bottom, int left);
		void set_paddings(BoxValues margins);

		void set_top_padding(int top);
		void set_right_padding(int right);
		void set_bottom_padding(int bottom);
		void set_left_padding(int left);

		void set_box(Box box);

		void set_visible(bool visible);

		virtual void add(Block* block);
		virtual void apply(Style* style);
		Style* get_style();

		void draw(Point base_location);

		void update(Point mouse_location, Point base_location,
			Block** event_receiver, bool force_fail = false);

		void add_event_listener(Block* listener);
		void add_event_source(Block* source);

		void add_event_function(std::function<void(Event)> function);

		void raise_event(Event e);
		void handle_event(Event e);

		void set_parent(Block* block);
		Block* get_parent();

		void set_direct_hover(bool value);

		virtual int get_total_width() const;
		virtual int get_total_height() const;

		void set_sizing(char _sizing);
		void enable_dragging();

		bool get_dragged() const;

		bool detects_scroll() const;
		bool parent_detects_scroll() const;

		void enable_scroll_detection(bool detection);
		void enable_parent_scroll_detection(bool detection);

		void update_scroll_detection();
		void handle_scroll(Event e);

		int get_box_width(char boxname) const;
		int get_box_height(char boxname) const;
	};
}