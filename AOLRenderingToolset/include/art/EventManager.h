#pragma once

#include "Display.h"

namespace art
{
	class EventManager
	{
	protected:
		std::vector<std::function<void(const agl::Event&)>> event_functions;

	public:
		void add_event_function(std::function<void(const agl::Event&)> fn);
		void dispatch_event(const agl::Event& e);
	};
}