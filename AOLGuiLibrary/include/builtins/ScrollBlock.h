#pragma once

#include "Block.h"
#include "Scrollbar.h"

namespace agl::builtins
{
	class ChildrenContainer : public Block
	{
	private:
		void on_child_changed(Event e);

	public:
		ChildrenContainer();

		virtual void add(Block* block);
	};

	class ScrollBlock : public Block
	{
	private:
		Scrollbar* vscrollbar;
		Scrollbar* hscrollbar;

		Block* children_container;

		void on_children_container_resized(Event e);
		void on_scroll(Event e);

	public:
		ScrollBlock();

		void connect_children_container(Block* container);
		void create_children_container();

		void connect_vscrollbar(Scrollbar* scrollbar);
		void connect_hscrollbar(Scrollbar* scrollbar);

		virtual void add(Block* block);
		virtual void direct_add(Block* block);
		
		int get_total_width();
		int get_total_height();
	};
}