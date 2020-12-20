#pragma once

#include <agl/AOLGuiLibrary.h>
#include <agl/builtins/SelectionList.h>
#include <agl/builtins/Scrollbar.h>
#include <agl/builtins/ScrollBlock.h>
#include <agl/builtins/Button.h>

#include <art/Dictionary.h>

#include <map>
#include <set>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

// Gui helpers
struct ShortcutOption
{
	agl::Block container;
	agl::builtins::Label label;
	agl::Block shortcut_listener;
	agl::builtins::Label shortcut_label;
};

class TwoPanelGui
{
protected:
	agl::Gui* gui_ptr;

	agl::Block main_frame;
	agl::builtins::Flow main_flow;

	// Left panel
	agl::builtins::ScrollBlock options_menu;
	agl::builtins::Flow options_flow;
	
	agl::builtins::Scrollbar options_scrollbar;

	// Right panel
	agl::builtins::ScrollBlock content_panel;
	agl::builtins::Scrollbar content_scrollbar;

	int left_width;
	int right_width;

public:
	TwoPanelGui(
		agl::Gui* gui, int side, art::Dictionary* dict,
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
	agl::builtins::ImageBlock main_menu_background;

	// Play GUI
	agl::Block play_gui;

	agl::builtins::Flow play_gui_flow;
	agl::builtins::Label play_main_label;

	agl::builtins::Flow play_game_selection_flow;
	agl::builtins::SelectionList play_game_selection_list;
	agl::builtins::ImageBlock play_game_selection_preview;

	agl::Block play_main_container;
	agl::builtins::Button play_new_button;
	agl::builtins::Button play_load_button;
	agl::builtins::Button play_delete_button;

	// Options GUI
	agl::Block options_gui;
	agl::builtins::Flow options_gui_flow;
	agl::builtins::Label options_main_label;

	static const int categories = 1;
	agl::builtins::Label options_category_labels[categories];
	agl::builtins::Flow options_category_containers[categories];

	// Licenses GUI
	agl::Block licenses_gui;
	agl::builtins::Flow licenses_gui_flow;
	agl::builtins::Label licenses_main_label;
	agl::builtins::Label licenses_text;

	// Creators GUI
	agl::Block creators_gui;
	agl::builtins::Flow creators_gui_flow;
	agl::builtins::Label creators_main_label;
	agl::builtins::Label creators_text;

	agl::Block mods_gui;

	agl::Block* subguis[5];

	void open_subgui(agl::Event e, agl::builtins::Button* btn);
	void generate_shortcut_options(
		ShortcutOption ops[], std::string ops_texts[], int size, int category, art::Dictionary* dict
	);
	void highlight_shortcut(agl::Event e);
public:
	agl::builtins::Button main_menu_buttons[6];

	ShortcutOption debug_category_options[1];

	MainMenuGui(agl::Gui* gui, int screenw, int screenh, art::Dictionary* dict);
};

std::string shortcut_to_string(int key, int mods);

extern agl::Style* horizontal_flow;
extern agl::Style* bronze_age_hflow;
extern agl::Style* bronze_age_scrollbar;
extern agl::Style* bronze_age_menubutton;

extern agl::Style* main_menu_subgui;
extern agl::Style* bronze_age_label;
extern agl::Style* bronze_age_main_label;

extern agl::Style* button_action;
extern agl::Style* button_danger;
extern agl::Style* button_neutral;
extern agl::Style* button_accept;

void setup_styles();