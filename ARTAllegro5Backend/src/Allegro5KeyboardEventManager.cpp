#include "..\include\ARTAllegro5Backend\Allegro5KeyboardEventManager.h"

#include <agl/events.h>

const std::map<int, int> key_to_mod = {
	{ALLEGRO_KEY_LSHIFT, ALLEGRO_KEYMOD_SHIFT},
	{ALLEGRO_KEY_RSHIFT, ALLEGRO_KEYMOD_SHIFT},
	{ALLEGRO_KEY_LCTRL, ALLEGRO_KEYMOD_CTRL},
	{ALLEGRO_KEY_RCTRL, ALLEGRO_KEYMOD_CTRL},
	{ALLEGRO_KEY_ALT, ALLEGRO_KEYMOD_ALT},
	{ALLEGRO_KEY_ALTGR, ALLEGRO_KEYMOD_ALTGR},
};

namespace art
{
	Allegro5KeyboardEventManager::Allegro5KeyboardEventManager()
		: KeyboardEventManager(), Allegro5EventManager()
	{
		state = ALLEGRO_KEYBOARD_STATE();

		for (const auto& [key, mod] : key_to_mod)
		{
			al_get_keyboard_state(&state);
			if (al_key_down(&state, key))
				pressed_mods |= mod;
		}
	}

	void Allegro5KeyboardEventManager::dispatch_event(const ALLEGRO_EVENT& e)
	{
		agl::Event new_event;

		bool event_happened = true;

		switch (e.type)
		{
		case ALLEGRO_EVENT_KEY_DOWN:
			if (key_to_mod.find(e.keyboard.keycode) != key_to_mod.end())
				pressed_mods |= key_to_mod.at(e.keyboard.keycode);


			new_event.type = AGL_EVENT_KEY_PRESSED;
			new_event.keycode = e.keyboard.keycode;
			new_event.unicode = e.keyboard.unichar;
			new_event.mods = pressed_mods;

			al_get_keyboard_state(&state);

			check_shortcut(e.keyboard.keycode);

			break;

		case ALLEGRO_EVENT_KEY_UP:
			if (key_to_mod.find(e.keyboard.keycode) != key_to_mod.end())
				pressed_mods ^= key_to_mod.at(e.keyboard.keycode);

			new_event.type = AGL_EVENT_KEY_RELEASED;
			new_event.keycode = e.keyboard.keycode;
			new_event.unicode = e.keyboard.unichar;
			new_event.mods = pressed_mods;

			al_get_keyboard_state(&state);

			break;

		default:
			event_happened = false;
		}

		if (event_happened)
			EventManager::dispatch_event(new_event);
	}

}