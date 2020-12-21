#pragma once

#include "Allegro5EventManager.h"

#include <art/KeyboardEventManager.h>

namespace art
{
	class Allegro5KeyboardEventManager : public Allegro5EventManager, public KeyboardEventManager
	{
	private:
		ALLEGRO_KEYBOARD_STATE state;

	public:
		Allegro5KeyboardEventManager();

		void dispatch_event(const ALLEGRO_EVENT& e);

		const int supported_mods = ALLEGRO_KEYMOD_CTRL |
			ALLEGRO_KEYMOD_SHIFT |
			ALLEGRO_KEYMOD_ALT |
			ALLEGRO_KEYMOD_ALTGR;
	};
}