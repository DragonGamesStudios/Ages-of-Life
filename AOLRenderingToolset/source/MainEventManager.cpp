#include "../include/art/MainEventManager.h"

#include <agl/events.h>

namespace art
{
	MainEventManager::MainEventManager()
	{
		display = 0;
	}

	void MainEventManager::connect_display(const Display* _display)
	{
		display = _display;
	}

	void MainEventManager::add_manager(EventManager* manager)
	{
		managers.push_back(manager);
	}

}