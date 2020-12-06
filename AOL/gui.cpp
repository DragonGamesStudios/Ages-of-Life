#pragma once
#include "gui.h"


// Ingame guis

agl::Style* horizontal_flow;
agl::Style* bronze_age_hflow;
agl::Style* bronze_age_scrollbar;
agl::Style* bronze_age_menubutton;

TwoPanelGui::TwoPanelGui(
	agl::Gui* gui,
	int side,
	int preferred_left_width,
	int preferred_right_width
)
{
	// Setup widths
	left_width = preferred_left_width;
	if (preferred_left_width == -1)
		left_width = side / 3;

	right_width = preferred_right_width;
	if (preferred_right_width == -1)
		right_width = side;

	// Initialize gui
	gui_ptr = gui;
	gui_ptr->add(&main_frame);

	// Create gui base
	main_frame.set_size(right_width + left_width + 4, side);
	main_frame.set_paddings(2, 2, 2, 2);
	main_frame.set_background_color(agl::Color(93, 35, 0));

	main_frame.add(&main_flow);

	main_flow.set_size(right_width + left_width + 4, side);
	//main_flow.set_background_color(agl::Color(133, 75, 20));
	//main_flow.set_main_axis_spacing(2);

	main_flow.apply(bronze_age_hflow);

	// Left panel
	main_flow.add(&options_menu);

	options_menu.set_size(left_width - 12, side - 2);
	options_menu.set_paddings(1, 1, 1, 1);
	options_menu.set_background_color(agl::Color(133, 75, 20));

	options_menu.connect_vscrollbar(&options_scrollbar);
	options_menu.connect_children_container(&options_flow);

	options_menu.direct_add(&options_flow);

	options_flow.set_size(left_width - 10, 0);
	options_flow.set_single_subflow(true);
	options_flow.set_main_axis(AGL_VERTICAL);
	options_flow.set_background_color(agl::Color(0, 0, 0, 0));

	// Scrollbar

	main_flow.add(&options_scrollbar);

	options_scrollbar.set_size(10, side);
	options_scrollbar.apply(bronze_age_scrollbar);
	options_scrollbar.set_step(10);
}

void TwoPanelGui::create_buttons(
	std::vector<std::string> btns,
	std::vector<agl::builtins::Button*> buttons
)
{
	for (int i = 0; i < btns.size(); i++)
	{
		options_flow.add(buttons[i]);

		buttons[i]->set_size(left_width - 10, 60);
		buttons[i]->apply(bronze_age_menubutton);
		buttons[i]->create_label();
		buttons[i]->set_font(agl::loaded_fonts["default"]);
		buttons[i]->set_text_size(24);
		buttons[i]->set_text_color(agl::Color(255, 210, 103));
		buttons[i]->set_text(btns[i]);
	}

	options_flow.resize_to_content();
}

MainMenuGui::MainMenuGui(agl::Gui* gui, int screenw, int screenh)
	: TwoPanelGui(gui, screenh, -1, screenw - screenh / 3)
{
	std::vector<agl::builtins::Button*> btn_ptrs;
	for (int i = 0; i < 6; i++)
		btn_ptrs.push_back(&main_menu_buttons[i]);
	
	std::vector<std::string> btn_texts = {
		"Play",
		"Options",
		"Licenses",
		"Creators",
		"Mods",
		"Quit"
	};

	create_buttons(btn_texts, btn_ptrs);
}

void setup_styles()
{
	horizontal_flow = new agl::Style("horizontal-flow");
	bronze_age_hflow = new agl::Style("bronze-age-hflow");
	bronze_age_scrollbar = new agl::Style("bronze-age-scrollbar");
	bronze_age_menubutton = new agl::Style("bronze-age-menubutton");

	horizontal_flow->set_value("main_axis_spacing", 2);
	horizontal_flow->set_value("main_axis", AGL_HORIZONTAL);

	bronze_age_hflow->apply(horizontal_flow);
	bronze_age_hflow->set_value("background_color", agl::Color(123, 65, 10));

	bronze_age_scrollbar->set_value("background_color", agl::Color(93, 35, 0));
	bronze_age_scrollbar->set_value("marker", AGL_MARKER_CREATE);

	bronze_age_menubutton->set_value("background_color", agl::Color(163, 105, 70));
	bronze_age_menubutton->set_value("background_color_hover", agl::Color(153, 95, 60));
	bronze_age_menubutton->set_value("background_color_click", agl::Color(143, 85, 50));
	bronze_age_menubutton->set_value("sizing", AGL_SIZING_BORDERBOX);
	bronze_age_menubutton->set_value("border_left", 5);
	bronze_age_menubutton->set_value("border_right", 5);
	bronze_age_menubutton->set_value("border_top", 5);
	bronze_age_menubutton->set_value("border_bottom", 5);
	bronze_age_menubutton->set_value("border_color_left", agl::Color(168, 110, 75));
	bronze_age_menubutton->set_value("border_color_right", agl::Color(158, 100, 65));
	bronze_age_menubutton->set_value("border_color_top", agl::Color(168, 110, 75));
	bronze_age_menubutton->set_value("border_color_bottom", agl::Color(158, 100, 65));
}
