#include "..\..\include\agl\builtins\Dragger.h"

#include "..\include\agl\events.h"

namespace agl::builtins
{
	void Dragger::on_drag_move(const Event& e)
	{
		if (e.source == this && e.type == AGL_EVENT_MOUSE_MOVED && (e.dx || e.dy) && is_dragged && moved)
		{
			moved->set_location(moved->get_location() + Point((float)e.dx, (float)e.dy));
		}
	}

	Dragger::Dragger()
		: Block()
	{
		set_background_color(agl::Color(0, 0, 0, 0));
		enable_dragging();

		add_event_function(
			std::bind(&Dragger::on_drag_move, this, std::placeholders::_1)
		);

		moved = nullptr;
	}

	void Dragger::set_moved(Block* new_moved)
	{
		moved = new_moved;
	}

}