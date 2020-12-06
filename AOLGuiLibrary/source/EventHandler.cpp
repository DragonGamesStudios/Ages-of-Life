#include "EventHandler.h"
#include "events.h"

namespace agl
{
	std::vector<Event>::iterator EventHandler::get_event_queue_begin()
	{
		return this->event_queue.begin();
	}

	std::vector<Event>::iterator EventHandler::get_event_queue_end()
	{
		return this->event_queue.end();
	}

	void EventHandler::reset_event_queue()
	{
		return this->event_queue.clear();
	}

	void EventHandler::handle_event(ALLEGRO_EVENT e)
	{
		bool event_registered = false;
		Event new_event;
		switch (e.type)
		{
		case ALLEGRO_EVENT_MOUSE_AXES:
			new_event.type = AGL_EVENT_MOUSE_MOVED;
			new_event.x = e.mouse.x;
			new_event.y = e.mouse.y;
			new_event.z = e.mouse.z;
			new_event.w = e.mouse.w;
			new_event.dx = e.mouse.dx;
			new_event.dy = e.mouse.dy;
			new_event.dz = e.mouse.dz;
			new_event.dw = e.mouse.dw;
			al_get_mouse_state(&mouse_state);
			new_event.buttons = mouse_state.buttons;

			event_registered = true;
			break;

		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			new_event.type = AGL_EVENT_MOUSE_PRESSED;
			new_event.x = e.mouse.x;
			new_event.y = e.mouse.y;
			new_event.button = e.mouse.button;

			al_get_mouse_state(&mouse_state);

			event_registered = true;
			break;

		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			new_event.type = AGL_EVENT_MOUSE_RELEASED;
			new_event.x = e.mouse.x;
			new_event.y = e.mouse.y;
			new_event.button = e.mouse.button;

			al_get_mouse_state(&mouse_state);

			event_registered = true;
			break;

		default:
			break;
		}

		if (event_registered)
			this->event_queue.push_back(new_event);
	}

	ALLEGRO_MOUSE_STATE* EventHandler::get_mouse_state()
	{
		return &this->mouse_state;
	}

	EventHandler::EventHandler()
	{
		this->mouse_state = ALLEGRO_MOUSE_STATE();
		al_get_mouse_state(&this->mouse_state);
	}
}