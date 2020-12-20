#include "EventManager.h"

namespace art
{
	void EventManager::add_event_function(std::function<void(agl::Event)> fn)
	{
		event_functions.push_back(fn);
	}
}