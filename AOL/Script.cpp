#include "globals.h"

Script::Script()
{
	guis = {};
	opened_guis_amount = 0;
	valid_chars = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
	this->active_input = NULL;
}

void Script::open_gui(GUI* gui)
{
	if (gui_open_functions.find(gui) != gui_open_functions.end()) gui_open_functions[gui]();
	guis.push_back(gui);
	opened_guis_amount++;
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

void Script::keyinput(int keycode, int ascii)
{
	if (this->active_input) {
		std::string buffer;
		switch (keycode)
		{
		case ALLEGRO_KEY_ENTER:
			this->activate_input();
			break;
		case ALLEGRO_KEY_BACKSPACE:
			buffer = this->active_input->text;
			if (!buffer.empty()) {
				buffer.pop_back();
				this->active_input->setTextChunk(buffer);
			}
			break;
		case ALLEGRO_KEY_DELETE:
			break;
		default:
			char c = ascii;
			this->active_input->setTextChunk(this->active_input->text + std::string(1, c));
			break;
		}
	}
}

void Script::activate_input(Label* lbl)
{
	this->active_input = lbl;
}

bool Script::validate_savename(std::string savename)
{
	for (auto svc = savename.begin(); svc != savename.end(); svc++) {
		if (std::find(this->valid_chars.begin(), this->valid_chars.end(), *svc) == this->valid_chars.end()) return false;
	}
	return true;
}
