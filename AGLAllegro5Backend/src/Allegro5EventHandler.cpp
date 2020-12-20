#include "..\include\AGLAllegro5Backend\Allegro5EventHandler.h"
#include <agl/events.h>

namespace agl
{
	void Allegro5EventHandler::update_mouse_state()
	{
		al_get_mouse_state(&allegro_mouse);

		mouse_state.x = allegro_mouse.x;
		mouse_state.y = allegro_mouse.y;
		mouse_state.buttons = allegro_mouse.buttons;
	}

	Allegro5EventHandler::Allegro5EventHandler() : EventHandler()
	{
		allegro_mouse = ALLEGRO_MOUSE_STATE();

		update_mouse_state();
	}

	void Allegro5EventHandler::handle_event(ALLEGRO_EVENT e)
	{
		bool event_registered = true;
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
			update_mouse_state();
			new_event.buttons = mouse_state.buttons;
			break;

		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			new_event.type = AGL_EVENT_MOUSE_PRESSED;
			new_event.x = e.mouse.x;
			new_event.y = e.mouse.y;
			new_event.button = e.mouse.button;

			update_mouse_state();
			break;

		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			new_event.type = AGL_EVENT_MOUSE_RELEASED;
			new_event.x = e.mouse.x;
			new_event.y = e.mouse.y;
			new_event.button = e.mouse.button;

			update_mouse_state();
			break;

		case ALLEGRO_EVENT_KEY_CHAR:
			new_event.type = AGL_EVENT_CHAR_INPUT;
			new_event.keycode = e.keyboard.keycode;
			new_event.unicode = e.keyboard.unichar;
			new_event.mods = e.keyboard.modifiers;
			break;

		default:
			event_registered = false;
			break;
		}

		if (event_registered)
			event_queue.push_back(new_event);
	}

}