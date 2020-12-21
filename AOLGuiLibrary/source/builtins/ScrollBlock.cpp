#include "agl/builtins/ScrollBlock.h"
#include "agl/events.h"

namespace agl::builtins
{
	void ScrollBlock::on_children_container_resized(const Event& e)
	{
		if (e.type == AGL_EVENT_BLOCK_RESIZED && e.source == children_container)
		{
			if (vscrollbar)
				vscrollbar->config_to_scrollable();

			if (hscrollbar)
				hscrollbar->config_to_scrollable();
		}
	}

	void ScrollBlock::on_scroll(const Event& e)
	{
		if (e.type == AGL_EVENT_SCROLLBAR_SCROLLED && children_container)
		{
			if (e.source == vscrollbar)
				children_container->set_location(Point(
					children_container->get_location().x,
					-(float)vscrollbar->get_scroll()
				));

			else if (e.source == hscrollbar)
				children_container->set_location(Point(
					-(float)hscrollbar->get_scroll(),
					children_container->get_location().y
				));
		}
	}

	ScrollBlock::ScrollBlock() : Block()
	{
		vscrollbar = nullptr;
		hscrollbar = nullptr;
		children_container = nullptr;

		enable_scroll_detection(true);
	}

	void ScrollBlock::connect_children_container(Block* container)
	{
		children_container = container;

		if (vscrollbar)
		{
			vscrollbar->add_event_source(container);
			vscrollbar->config_to_scrollable();
		}

		if (hscrollbar)
		{
			hscrollbar->add_event_source(container);
			vscrollbar->config_to_scrollable();
		}

		add_event_source(children_container);

		add_event_function(
			std::bind(&ScrollBlock::on_children_container_resized, this, std::placeholders::_1)
		);

		add_event_function(
			std::bind(&ScrollBlock::on_scroll, this, std::placeholders::_1)
		);
	}

	void ScrollBlock::create_children_container()
	{
		Block* container = new ChildrenContainer();

		container->set_background_color(Color(0, 0, 0, 0));

		connect_children_container(container);

		direct_add(children_container);
	}

	void ScrollBlock::connect_vscrollbar(Scrollbar* scrollbar)
	{
		vscrollbar = scrollbar;
		scrollbar->connect_scrollable(this);
		add_event_source(scrollbar);
	}

	void ScrollBlock::connect_hscrollbar(Scrollbar* scrollbar)
	{
		hscrollbar = scrollbar;
		scrollbar->connect_scrollable(this);
		add_event_source(scrollbar);
	}

	void ScrollBlock::add(Block* block)
	{
		if (children_container)
		{
			children_container->add(block);
		}
		else
		{
			direct_add(block);
		}
	}

	void ScrollBlock::direct_add(Block* block)
	{
		Block::add(block);
	}

	int ScrollBlock::get_total_width()
	{
		if (children_container)
			return children_container->get_width();

		return get_inner_width();
	}

	int ScrollBlock::get_total_height()
	{
		if (children_container)
			return children_container->get_height();

		return get_inner_height();
	}

	Block* ScrollBlock::get_children_container()
	{
		return children_container;
	}

	void ChildrenContainer::on_child_changed(const Event& e)
	{
		if ((e.type == AGL_EVENT_BLOCK_RESIZED || e.type == AGL_EVENT_BLOCK_MOVED) &&
			e.source->get_parent() == this)
		{
			set_size(get_total_width(), get_total_height());
		}
	}

	ChildrenContainer::ChildrenContainer() : Block()
	{
		add_event_function(
			std::bind(&ChildrenContainer::on_child_changed, this, std::placeholders::_1)
		);
	}

	void ChildrenContainer::add(Block* block)
	{
		Block::add(block);
		add_event_source(block);
		set_size(get_total_width(), get_total_height());
	}

}