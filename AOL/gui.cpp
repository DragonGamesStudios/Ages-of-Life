#pragma once
#include "gui.h"
// tEST
FontSet::FontSet(fs::path path, std::set<int> sizes)
{
	std::string str_path = path.string();
	for (auto const& size : sizes)
	{
		this->sizes.insert({ size, agui::Font::load(str_path, size) });
	}
}

FontSet::~FontSet()
{
	for (auto font : sizes)
	{
		delete font.second;
	}
}

VerticalFlow::VerticalFlow()
{
}

void VerticalFlow::LayoutChildren()
{
	int current_y = 0;

	for (agui::WidgetArray::iterator it = this->getChildBegin(); it != this->getChildEnd(); it++)
	{
		if (!(*it)->isVisible() && isFilteringVisibility())
		{
			continue;
		}

		(*it)->setLocation(0, current_y);
		current_y += (*it)->getHeight();
	}
}


// Ingame guis

TwoPanelGui::TwoPanelGui(agui::Gui* gui_instance, int side,	int preferred_left_width, int preferred_right_width)
{
	// Setup widths
	int left_width = preferred_left_width;
	if (preferred_left_width == -1)
		left_width = side / 3;

	int right_width = preferred_right_width;
	if (preferred_right_width == -1)
		right_width = side;

	// Initialize gui
	gui_ptr = gui_instance;
	gui_ptr->add(&main_frame);

	// Calculate magins
	int horizontal_margins = main_flow.getMargin(agui::SIDE_LEFT) + main_flow.getMargin(agui::SIDE_RIGHT)
		//+ menu_options_scrollpane.getMargin(agui::SIDE_RIGHT)
		+ menu_options_scrollpane.getMargin(agui::SIDE_LEFT)
		+ content_scrollpane.getMargin(agui::SIDE_RIGHT)
		//+ content_scrollpane.getMargin(agui::SIDE_LEFT)
		+ main_frame.getLeftMargin() + main_frame.getRightMargin();

	int vertical_margins = main_flow.getMargin(agui::SIDE_TOP) + main_flow.getMargin(agui::SIDE_BOTTOM)
		+ menu_options_scrollpane.getMargin(agui::SIDE_TOP) + menu_options_scrollpane.getMargin(agui::SIDE_BOTTOM)
		+ main_frame.getTopMargin() + main_frame.getBottomMargin();

	// Create gui base
	main_frame.setSize(right_width + left_width + 5 + horizontal_margins, side + vertical_margins);
	main_frame.setFontColor(agui::Color(0, 0, 0));

	main_frame.add(&main_flow);

	main_flow.setLocation(0, 0);
	main_flow.setHorizontalSpacing(0);

	// Left pane

	

	main_flow.add(&menu_options_scrollpane);
	menu_options_scrollpane.setSize(left_width, side);
	menu_options_scrollpane.setHScrollPolicy(agui::SHOW_NEVER);

	main_flow.add(&content_scrollpane);

	content_scrollpane.setSize(right_width, side);
}

MainMenuGui::MainMenuGui(agui::Gui* gui_instance)
	: TwoPanelGui(gui_instance, 750)
{
	const int button_amount = 5;
	const int button_height = 50;

	std::string button_texts[button_amount] = { "Play", "Options", "License & Creators", "update log", "Quit" };
	for (int i = 0; i < button_amount; i++)
	{
		menu_buttons[i].setText(button_texts[i]);
		menu_buttons[i].setSize(250, button_height);
		menu_buttons[i].setLocation(0, i * button_height);

		menu_options_scrollpane.add(&menu_buttons[i]);
	}
}
