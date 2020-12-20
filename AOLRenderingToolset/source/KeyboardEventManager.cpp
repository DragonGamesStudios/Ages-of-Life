#include "KeyboardEventManager.h"
#include "agl/events.h"

const std::map<int, int> key_to_mod = {
	{ALLEGRO_KEY_LSHIFT, ALLEGRO_KEYMOD_SHIFT},
	{ALLEGRO_KEY_RSHIFT, ALLEGRO_KEYMOD_SHIFT},
	{ALLEGRO_KEY_LCTRL, ALLEGRO_KEYMOD_CTRL},
	{ALLEGRO_KEY_RCTRL, ALLEGRO_KEYMOD_CTRL},
	{ALLEGRO_KEY_ALT, ALLEGRO_KEYMOD_ALT},
	{ALLEGRO_KEY_ALTGR, ALLEGRO_KEYMOD_ALTGR},
};

void art::KeyboardEventManager::check_shortcut(int key)
{
	auto shortgroup = shortcuts.find(key);
	if (shortgroup != shortcuts.end())
	{
		auto shortcut = shortgroup->second.find(pressed_mods);
		if (shortcut != shortgroup->second.end())
			shortcut->second({
					.type = AGL_EVENT_KEY_PRESSED,
					.keycode = key,
					.mods = pressed_mods
				});
	}
}

art::KeyboardEventManager::KeyboardEventManager()
{
	al_get_keyboard_state(&state);
	pressed_mods = 0;

	for (const auto& [key, mod] : key_to_mod)
	{
		if (al_key_down(&state, key))
			pressed_mods |= mod;
	}
}

void art::KeyboardEventManager::dispatch_event(ALLEGRO_EVENT e)
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
		for (auto& fn : event_functions)
			fn(new_event);
}

void art::KeyboardEventManager::register_shortcut(
	int key, int mods, std::function<void(agl::Event)> fn
)
{
	if (shortcuts.find(key) == shortcuts.end())
		shortcuts.insert({ key, {} });

	if (shortcuts[key].find(mods) == shortcuts[key].end())
		shortcuts[key].insert({ mods, 0 });

	shortcuts[key][mods] = fn;
}

void art::KeyboardEventManager::remove_shortcut(int key, int mods)
{
	auto key_it = shortcuts.find(key);
	if (key_it == shortcuts.end())
		return;

	auto mods_it = key_it->second.find(mods);
	if (mods_it == key_it->second.end())
		return;

	key_it->second.erase(mods_it);
}
