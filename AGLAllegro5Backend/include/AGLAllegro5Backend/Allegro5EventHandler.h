#pragma once

#include <agl/EventHandler.h>

#include <allegro5/allegro.h>

namespace agl
{
	class Allegro5EventHandler : public EventHandler
	{
	private:
		ALLEGRO_MOUSE_STATE allegro_mouse;
		void update_mouse_state();
	public:
		Allegro5EventHandler();

		void handle_event(ALLEGRO_EVENT e);
	};
}