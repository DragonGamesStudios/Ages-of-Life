#pragma once

#include "EventManager.h"

namespace art
{
	class KeyboardEventManager
	{
	protected:
		std::map<int, std::map<int, std::function<void(const agl::Event&)>>> shortcuts;

		int pressed_mods;
		void check_shortcut(int key);

	public:
		KeyboardEventManager();

		void register_shortcut(int key, int mods, std::function<void(const agl::Event&)> fn);
		void remove_shortcut(int key, int mods);
	};
}