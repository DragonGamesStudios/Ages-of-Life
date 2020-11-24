#pragma once
#include <Agui/Agui.hpp>
#include <Agui/FlowLayout.hpp>
#include <Agui/Widgets/Button/Button.hpp>
#include <Agui/Widgets/ScrollPane/ScrollPane.hpp>
#include <Agui/Widgets/Frame/Frame.hpp>
#include <Agui/Widgets/ListBox/ListBox.hpp>
#include <Agui/Backends/Allegro5/Allegro5.hpp>

#include <map>
#include <set>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

// Gui helpers

struct FontSet
{
	FontSet(fs::path path, std::set<int> sizes);
	~FontSet();

	std::map<int, agui::Font*> sizes = {};
};

class VerticalFlow : public agui::FlowLayout
{
public:
	VerticalFlow();
private:
	virtual void LayoutChildren();
};

// In-game guis

class TwoPanelGui
{
protected:
	// Gui
	agui::Gui* gui_ptr;
	agui::Frame main_frame;
	agui::FlowLayout main_flow;

	// Left panel
	agui::ScrollPane menu_options_scrollpane;

	// Right panel
	agui::ScrollPane content_scrollpane;
public:
	TwoPanelGui(agui::Gui* gui_instance, int side,
		int preferred_left_width=-1, int preferred_right_width=-1);
};

class MainMenuGui : public TwoPanelGui
{
private:
	agui::Button menu_buttons[5];
public:
	MainMenuGui(agui::Gui* gui_instance);
};