#pragma once

#include "agl/Block.h"
#include "Scrollbar.h"

namespace agl::builtins
{
	class ChildrenContainer : public Block
	{
	private:
		void on_child_changed(const Event& e);

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

		void on_children_container_resized(const Event& e);
		void on_scroll(const Event& e);

	public:
		ScrollBlock();

		void connect_children_container(Block* container);
		void create_children_container();

		void connect_vscrollbar(Scrollbar* scrollbar);
		void connect_hscrollbar(Scrollbar* scrollbar);

		virtual void add(Block* block);
		virtual void clear();
		virtual void remove(Block* block);
		virtual void direct_add(Block* block);
		virtual void direct_remove(Block* block);
		virtual void direct_clear();
		
		int get_total_width();
		int get_total_height();

		Block* get_children_container();
	};
}