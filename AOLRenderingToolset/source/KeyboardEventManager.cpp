#include "KeyboardEventManager.h"
#include "agl/events.h"

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
	pressed_mods = 0;
}

void art::KeyboardEventManager::register_shortcut(
	int key, int mods, std::function<void(const agl::Event&)> fn
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
