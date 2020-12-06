#pragma once
#include "Base.h"

namespace agl
{
	class EventHandler
	{
	private:
		std::vector<Event> event_queue;
		ALLEGRO_MOUSE_STATE mouse_state;
	public:
		std::vector<Event>::iterator get_event_queue_begin();
		std::vector<Event>::iterator get_event_queue_end();
		void reset_event_queue();
		void handle_event(ALLEGRO_EVENT e);
		ALLEGRO_MOUSE_STATE* get_mouse_state();

		EventHandler();
	};
}