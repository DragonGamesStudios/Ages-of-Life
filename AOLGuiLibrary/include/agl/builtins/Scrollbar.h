#pragma once
#include "agl/Block.h"

#define AGL_EVENT_SCROLLBAR_SCROLLED 0x21


namespace agl::builtins
{
	class Scrollbar : public Block
	{
	private:
		char axis;
		int full_size;
		int visible_size;
		int scroll;

		Block* marker;
		bool maintain_marker;

		Block* scrollable;

		int step;

		bool react_to_scrollwheel;

		void on_scrollable_resized(const Event& e);
		void on_scroll(const Event& e);

		void on_marker_drag(const Event& e);
	public:
		Scrollbar();

		void config(int _full, int _visible);
		void config_to_scrollable();
		void set_axis(char _axis);

		void connect_marker(Block* _marker);
		void set_scroll(int _scroll);
		void change_scroll(int delta);
		int get_scroll() const;

		void create_marker();
		Block* get_marker() const;

		void connect_scrollable(Block* scrollable);

		virtual void set_size(int width, int height);
		virtual void apply(const Style* _style);

		void set_step(int _step);
		void set_react_to_scrollwheel(bool react);
	};
}