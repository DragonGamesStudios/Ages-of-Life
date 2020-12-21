#pragma once

#include "EventManager.h"

#define ART_EVENT_DISPLAY_CLOSE -1

namespace art
{
	class MainEventManager
	{
	protected:
		const Display* display;

		std::vector<EventManager*> managers;

	public:
		MainEventManager();

		virtual void connect_display(const Display* _display);

		void add_manager(EventManager* manager);

		virtual void close() = 0;
	};
}