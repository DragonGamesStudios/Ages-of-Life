#include "globals.h"

Script::Script()
{
	guis = {};
	opened_guis_amount = 0;
}

void Script::open_gui(GUI* gui)
{
	guis.push_back(gui);
	opened_guis_amount++;
	if (gui_open_functions.find(gui) != gui_open_functions.end()) gui_open_functions[gui]();
}

void Script::close_gui(GUI* gui)
{
	for (std::vector<GUI*>::iterator elem = guis.begin(); elem != guis.end(); elem++) {
		if (*elem == gui) {
			guis.erase(elem);
			opened_guis_amount--;
			if (gui_close_functions.find(gui) != gui_close_functions.end()) gui_close_functions[gui]();
			break;
		}
	}
}

bool Script::get_gui_opened(GUI* gui)
{
	for (std::vector<GUI*>::iterator elem = guis.begin(); elem != guis.end(); elem++) {
		if (*elem == gui) {
			return true;
		}
	}
	return false;
}

void Script::register_open_gui_function(GUI* gui, std::function<void()> fn)
{
	this->gui_open_functions.insert({ gui, fn });
}

void Script::register_close_gui_function(GUI* gui, std::function<void()> fn)
{
	this->gui_close_functions.insert({ gui, fn });
}
