#pragma once

#include "Display.h"

namespace art
{
	class EventManager
	{
	protected:
		std::vector<std::function<void(agl::Event)>> event_functions;

	public:
		virtual void dispatch_event(ALLEGRO_EVENT e) = 0;
		void add_event_function(std::function<void(agl::Event)> fn);
	};
}