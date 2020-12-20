#include "agl/EventHandler.h"
#include "agl/events.h"

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

	Mouse* EventHandler::get_mouse_state()
	{
		return &this->mouse_state;
	}

	EventHandler::EventHandler()
	{
		this->mouse_state = { 0, 0, 0 };
	}
}