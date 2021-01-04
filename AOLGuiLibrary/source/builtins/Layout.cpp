#include "agl/builtins/Layout.h"

namespace agl::builtins
{
	void Layout::add(Block* child)
	{
		Block::add(child);
		child->add_event_listener(this);
		layout_children();
	}

	void Layout::remove(Block* child)
	{
		Block::remove(child);
		layout_children();
	}
}