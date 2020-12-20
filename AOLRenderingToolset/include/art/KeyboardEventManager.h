#pragma once

#include "EventManager.h"

namespace art
{
	class KeyboardEventManager : public EventManager
	{
	private:
		ALLEGRO_KEYBOARD_STATE state;

		std::map<int, std::map<int, std::function<void(agl::Event)>>> shortcuts;

		int pressed_mods;
		void check_shortcut(int key);

	public:
		KeyboardEventManager();

		void dispatch_event(ALLEGRO_EVENT e);

		void register_shortcut(int key, int mods, std::function<void(agl::Event)> fn);
		void remove_shortcut(int key, int mods);

		const int supported_mods = ALLEGRO_KEYMOD_CTRL |
			ALLEGRO_KEYMOD_SHIFT |
			ALLEGRO_KEYMOD_ALT |
			ALLEGRO_KEYMOD_ALTGR;
	};
}