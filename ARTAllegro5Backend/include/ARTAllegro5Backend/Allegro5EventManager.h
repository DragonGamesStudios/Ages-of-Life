#pragma once

#include <art/EventManager.h>

#include <allegro5/allegro.h>

namespace art
{
	class Allegro5EventManager : public EventManager
	{
	public:
		Allegro5EventManager();

		virtual void dispatch_event(const ALLEGRO_EVENT& e) = 0;
	};
}