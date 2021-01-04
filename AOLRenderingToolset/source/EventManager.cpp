#include "../include/art/EventManager.h"

namespace art
{
	void EventManager::add_event_function(std::function<void(const agl::Event&)> fn)
	{
		event_functions.push_back(fn);
	}

	void EventManager::dispatch_event(const agl::Event& e)
	{
		for (auto& fn : event_functions)
			fn(e);
	}
}