#pragma once

#include <AOLGuiLibrary.h>
#include <builtins/Flow.h>
#include <builtins/Scrollbar.h>
#include <builtins/ScrollBlock.h>
#include <builtins/button.h>

#include <map>
#include <set>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

// Gui helpers

class TwoPanelGui
{
private:
	agl::Gui* gui_ptr;

	agl::Block main_frame;
	agl::builtins::Flow main_flow;

	// Left panel
	agl::builtins::ScrollBlock options_menu;
	agl::builtins::Flow options_flow;
	
	agl::builtins::Scrollbar options_scrollbar;

	// Right panel
	agl::Block content_panel;

	int left_width;
	int right_width;

public:
	TwoPanelGui(
		agl::Gui* gui, int side,
		int preferred_left_width = -1, int preferred_right_width = -1
	);

	void create_buttons(
		std::vector<std::string> btns,
		std::vector<agl::builtins::Button*> buttons
	);
};

class MainMenuGui : public TwoPanelGui
{
private:
	agl::builtins::Button main_menu_buttons[6];
public:
	MainMenuGui(agl::Gui* gui, int screenw, int screenh);
};

extern agl::Style* horizontal_flow;
extern agl::Style* bronze_age_hflow;
extern agl::Style* bronze_age_scrollbar;
extern agl::Style* bronze_age_menubutton;

void setup_styles();