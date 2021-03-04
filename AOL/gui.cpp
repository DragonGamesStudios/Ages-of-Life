#pragma once
#include "gui.h"
#include <agl/events.h>

#include <allegro5/allegro.h>


// Ingame guis

agl::Style* horizontal_flow;
agl::Style* bronze_age_hflow;
agl::Style* bronze_age_scrollbar;
agl::Style* bronze_age_menubutton;
agl::Style* main_menu_subgui;
agl::Style* bronze_age_label;
agl::Style* bronze_age_main_label;
agl::Style* button_action;
agl::Style* button_danger;
agl::Style* button_neutral;
agl::Style* button_accept;

TwoPanelGui::TwoPanelGui(
	agl::Gui* gui,
	int side,
	art::Dictionary* dict,
	int preferred_left_width,
	int preferred_right_width,
	bool add_border,
	bool add_frame
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
	if (add_frame)
	{
		main_frame.set_size(right_width + left_width + 38, side + 58);
		main_frame.set_background_color(agl::Color(143, 85, 50));

		content_frame.set_location(15.f, 35.f);
		agl::Color se(163, 105, 70);
		agl::Color nw(123, 65, 30);
		content_frame.set_borders({ nw, se, se, nw });
		content_frame.set_borders(2);

		// Label
		main_frame.add(&frame_label);

		frame_label.apply(bronze_age_label);
		frame_label.resize_always();
		frame_label.set_location(15.f, 3.f);

		// Dragger
		main_frame.add(&dragger);

		dragger.set_location(0, 0);
		dragger.set_size(right_width + left_width + 38, 35);
		dragger.set_moved(&main_frame);

		// Close button
		main_frame.add(&close_button);
		
		close_button.set_image(agl::loaded_images["close-button"]);
		close_button.set_size(24, 24);
		close_button.set_location(right_width + left_width + 38 - 15 - 28, 4);

		agl::Color cse(203, 145, 110);
		agl::Color cnw(100, 50, 0);
		close_button.set_borders(2);
		close_button.set_borders({ cnw, cse, cse, cnw });

		close_button.set_scaling(AGL_SCALING_TOSIZE);
		close_button.set_background_color(agl::Color(163, 105, 70));
		close_button.set_hover_background_color(agl::Color(163, 105, 70));
		close_button.set_click_background_color(agl::Color(163, 105, 70));
		close_button.set_tint(agl::Color(100, 50, 0));

		main_frame.set_location(300.f, 300.f);
	}
	else
	{
		main_frame.set_size(right_width + left_width, side);
	}

	if (add_border)
	{
		agl::Color nw(163, 105, 70);
		agl::Color se(123, 65, 30);
		main_frame.set_borders({ nw, se, se, nw });
		main_frame.set_borders(1);
	}

	main_frame.add(&content_frame);

	content_frame.set_size(right_width + left_width, side);
	content_frame.set_paddings(2, 2, 2, 2);
	content_frame.set_background_color(agl::Color(93, 35, 0));

	content_frame.add(&main_flow);

	main_flow.set_size(right_width + left_width, side);
	//main_flow.set_background_color(agl::Color(133, 75, 20));
	//main_flow.set_main_axis_spacing(2);

	main_flow.apply(bronze_age_hflow);

	// Left panel
	main_flow.add(&options_menu);

	options_menu.set_size(left_width - 16, side);
	options_menu.set_paddings(1, 1, 1, 1);
	options_menu.set_background_color(agl::Color(133, 75, 20));

	options_menu.connect_vscrollbar(&options_scrollbar);
	options_menu.connect_children_container(&options_flow);

	options_menu.direct_add(&options_flow);

	options_flow.set_size(left_width - 16, 0);
	options_flow.set_single_subflow(true);
	options_flow.set_main_axis(AGL_VERTICAL);
	options_flow.set_background_color(agl::Color(0, 0, 0, 0));

	// Scrollbar

	main_flow.add(&options_scrollbar);

	options_scrollbar.set_size(10, side);
	options_scrollbar.apply(bronze_age_scrollbar);
	options_scrollbar.set_step(10);

	// Right panel
	main_flow.add(&content_panel);
	main_flow.add(&content_scrollbar);

	content_panel.set_size(right_width - 12, side);
	content_panel.connect_vscrollbar(&content_scrollbar);
	content_panel.create_children_container();
	content_panel.set_background_color(agl::Color(133, 75, 20));

	content_scrollbar.apply(bronze_age_scrollbar);
	content_scrollbar.set_size(10, side);
}

void TwoPanelGui::create_buttons(
	std::vector<art::LocalisedString> btns,
	std::vector<agl::builtins::Button*> buttons,
	art::Dictionary* dict
)
{
	for (int i = 0; i < btns.size(); i++)
	{
		options_flow.add(buttons[i]);

		buttons[i]->set_size(left_width - 16, 60);
		buttons[i]->apply(bronze_age_menubutton);
		buttons[i]->create_label();
		buttons[i]->apply_to_label(bronze_age_label);
		buttons[i]->set_text_size(24);
		dict->set_label_key(buttons[i]->get_label(), btns[i]);
	}

	options_flow.resize_to_content();
}

void MainMenuGui::open_subgui(agl::Event e, agl::builtins::Button* btn)
{
	close_subguis();

	subguis[options_flow.get_child_index(btn)]->set_visible(true);

	content_scrollbar.set_scroll(0);

	content_panel.get_children_container()->set_size(
		content_panel.get_inner_width(),
		content_panel.get_children_container()->get_total_height(false, false)
	);
	content_scrollbar.set_step(content_panel.get_children_container()->get_height() / 100);
}

void MainMenuGui::generate_shortcut_options(
	ShortcutOption ops[], art::LocalisedString ops_texts[], int size, int category, art::Dictionary* dict
)
{
	for (int i = 0; i < size; i++)
	{
		ops[i].container.set_paddings(3, 3, 3, 3);
		ops[i].container.set_size(options_gui_flow.get_inner_width(), 30);
		ops[i].container.set_background_color(
			agl::Color((i % 2) * 255, (i % 2) * 255, (i % 2) * 255, 50)
		);

		ops[i].label.apply(bronze_age_label);
		ops[i].label.set_size(200, 30);
		ops[i].label.set_horizontal_align(AGL_ALIGN_CENTER);
		ops[i].label.set_vertical_align(AGL_ALIGN_CENTER);
		dict->set_label_key(&ops[i].label, ops_texts[i]);

		ops[i].container.add(&ops[i].label);

		ops[i].shortcut_listener.set_sizing(AGL_SIZING_BORDERBOX);
		ops[i].shortcut_listener.set_borders(1, 1, 1, 1);
		ops[i].shortcut_listener.set_borders(agl::Color(150, 100, 0));
		ops[i].shortcut_listener.set_size(200, 30);
		ops[i].shortcut_listener.set_background_color(agl::Color(100, 50, 0));
		ops[i].shortcut_listener.set_location(ops[i].container.get_inner_width() - 300, 0);
		ops[i].shortcut_listener.enable_focus();
		ops[i].shortcut_listener.add_event_function(
			std::bind(&MainMenuGui::highlight_shortcut, this, std::placeholders::_1)
		);

		ops[i].shortcut_label.apply(bronze_age_label);
		ops[i].shortcut_label.set_size(200, 30);
		ops[i].shortcut_label.set_base_size(14);
		ops[i].shortcut_label.set_horizontal_align(AGL_ALIGN_CENTER);
		ops[i].shortcut_label.set_vertical_align(AGL_ALIGN_CENTER);

		ops[i].shortcut_listener.add(&ops[i].shortcut_label);

		ops[i].container.add(&ops[i].shortcut_listener);

		options_category_containers[category].add(&ops[i].container);
	}
}

void MainMenuGui::highlight_shortcut(agl::Event e)
{
	if (
		e.source->get_parent()->get_parent()->get_parent() == &options_gui_flow
		)
	{
		if (e.type == AGL_EVENT_BLOCK_FOCUS_GAINED)
			e.source->set_background_color(agl::Color(125, 75, 25));
		else if (e.type == AGL_EVENT_BLOCK_FOCUS_LOST)
			e.source->set_background_color(agl::Color(100, 50, 0));
	}
}

MainMenuGui::MainMenuGui(agl::Gui* gui, int screenw, int screenh, art::Dictionary* dict)
	: TwoPanelGui(gui, screenh - 4, dict, -1, screenw - screenh / 3 - 4, false, false)
{
	std::vector<agl::builtins::Button*> btn_ptrs;
	for (int i = 0; i < 6; i++)
		btn_ptrs.push_back(&main_menu_buttons[i]);
	
	std::vector<art::LocalisedString> btn_texts = {
		{"gui-element.play"},
		{"gui-element.options"},
		{"gui-element.licenses"},
		{"gui-element.creators"},
		{"gui-element.mods"},
		{"gui-element.quit"}
	};

	create_buttons(btn_texts, btn_ptrs, dict);

	content_panel.direct_add(&main_menu_background);

	main_menu_background.set_image(agl::loaded_images["main-menu-background"]);
	main_menu_background.set_scaling(AGL_SCALING_TOSIZE);
	main_menu_background.set_size(
		content_panel.get_inner_width(),
		content_panel.get_inner_height()
	);
	main_menu_background.set_shader(agl::loaded_shaders["guassian-blur"]);
	main_menu_background.set_shader_setup([screenw, screenh]() {
		float screen[2] = { (float)screenw, (float)screenh };
		float dir[2] = { 1.f, 1.f };
		al_set_shader_float("radius", 2.5);
		al_set_shader_float_vector("resolution", 2, &screen[0], 1);
		al_set_shader_float_vector("dir", 2, &dir[0], 1);
		});

	content_panel.bring_to_top(content_panel.get_children_container());

	// Play
	content_panel.add(&play_gui);
	play_gui.set_size(screenh - 4, screenh - 4);
	play_gui.set_location((content_panel.get_inner_width() - play_gui.get_width()) / 2, 0);
	play_gui.apply(main_menu_subgui);

	play_gui.add(&play_gui_flow);

	play_gui_flow.set_main_axis(AGL_VERTICAL);
	play_gui_flow.set_second_axis_align(AGL_ALIGN_CENTER);
	play_gui_flow.set_size(play_gui.get_inner_width(), play_gui.get_inner_height());
	play_gui_flow.set_background_color(agl::Color(0, 0, 0, 0));
	play_gui_flow.set_single_subflow(true);

	play_main_label.apply(bronze_age_main_label);
	dict->set_label_key(&play_main_label, { "gui-element.play" });
	play_main_label.resize_always();

	play_gui_flow.add(&play_main_label);

	// Selection
	play_game_selection_flow.set_single_subflow(true);
	play_game_selection_flow.set_background_color(agl::Color(0, 0, 0, 0));
	play_game_selection_flow.set_size(
		play_gui_flow.get_inner_width() - 100, (play_gui_flow.get_inner_width() - 100) / 2
	);

	play_game_selection_list.set_background_color(agl::Color(0, 0, 0, 0));
	play_game_selection_list.set_element_label_font(agl::loaded_fonts["default"]);
	play_game_selection_list.set_element_label_size(18);
	play_game_selection_list.set_element_label_color(agl::Color(255, 210, 103));
	play_game_selection_list.set_element_paddings(1);
	play_game_selection_list.set_odd_element_background_color(agl::Color(255, 255, 255, 30));
	play_game_selection_list.set_element_height(30);
	play_game_selection_list.set_size(
		(play_gui_flow.get_inner_width() - 100) / 2, (play_gui_flow.get_inner_width() - 100) / 2
	);
	play_game_selection_list.create_element_container();

	play_game_selection_flow.add(&play_game_selection_list);

	play_game_selection_preview.set_size(
		(play_gui_flow.get_inner_width() - 100) / 2, (play_gui_flow.get_inner_width() - 100) / 2
	);
	play_game_selection_preview.set_image(agl::loaded_images["game-preview-placeholder"]);
	play_game_selection_preview.set_scaling(AGL_SCALING_TOSIZE);

	play_game_selection_flow.add(&play_game_selection_preview);

	play_game_selection_flow.set_bottom_margin(20);

	play_gui_flow.add(&play_game_selection_flow);

	// Button container
	play_main_container.set_size(play_gui_flow.get_inner_width() - 100, 30);
	play_main_container.set_background_color(agl::Color(0, 0, 0, 0));

	// Load
	play_load_button.set_size(100, 30);
	play_load_button.apply(button_accept);

	play_load_button.create_label();
	play_load_button.apply_to_label(bronze_age_label);
	play_load_button.set_text_color(agl::Color(0, 0, 0));
	dict->set_label_key(play_load_button.get_label(), { "gui-element.load-game" });

	play_main_container.add(&play_load_button);

	// New
	play_new_button.set_size(100, 30);
	play_new_button.set_location(play_main_container.get_inner_width() / 2 - 50, 0);
	play_new_button.apply(button_neutral);

	play_new_button.create_label();
	play_new_button.apply_to_label(bronze_age_label);
	play_new_button.set_text_color(agl::Color(0, 0, 0));
	dict->set_label_key(play_new_button.get_label(), { "gui-element.create" });

	play_main_container.add(&play_new_button);

	// Delete
	play_delete_button.set_size(100, 30);
	play_delete_button.set_location(play_main_container.get_inner_width() - 100, 0);
	play_delete_button.apply(button_danger);

	play_delete_button.create_label();
	play_delete_button.apply_to_label(bronze_age_label);
	play_delete_button.set_text_color(agl::Color(0, 0, 0));
	dict->set_label_key(play_delete_button.get_label(), { "gui-element.delete" });

	play_main_container.add(&play_delete_button);

	play_gui_flow.add(&play_main_container);

	play_main_container.set_size(
		play_main_container.get_total_width(),
		play_main_container.get_total_height()
	);

	play_gui_flow.set_size(
		play_gui_flow.get_inner_width(), play_gui_flow.get_total_height()
	);

	play_gui.set_size(play_gui_flow.get_width(), play_gui_flow.get_height());

	play_gui.set_visible(false);

	main_menu_buttons[0].set_click_function(
		std::bind(&MainMenuGui::open_subgui, this, std::placeholders::_1, std::placeholders::_2)
	);

	subguis[0] = &play_gui;

	// Options
	content_panel.add(&options_gui);
	options_gui.set_size(screenh - 4, screenh - 4);
	options_gui.set_location((content_panel.get_inner_width() - options_gui.get_width()) / 2, 0);
	options_gui.apply(main_menu_subgui);

	options_gui.add(&options_gui_flow);

	options_gui_flow.set_main_axis(AGL_VERTICAL);
	options_gui_flow.set_second_axis_align(AGL_ALIGN_CENTER);
	options_gui_flow.set_size(options_gui.get_inner_width(), options_gui.get_inner_height());
	options_gui_flow.set_background_color(agl::Color(0, 0, 0, 0));
	options_gui_flow.set_single_subflow(true);

	options_main_label.apply(bronze_age_main_label);
	dict->set_label_key(&options_main_label, { "gui-element.label-options" });
	options_main_label.resize_always();

	options_gui_flow.add(&options_main_label);

	art::LocalisedString options_category_names[categories] = {
		{"setting-category.debugging"}
	};

	int ops_sizes[categories] = { 1 };

	for (int category = 0; category < categories; category++)
	{
		options_category_containers[category].set_main_axis(AGL_VERTICAL);
		options_category_containers[category].set_single_subflow(true);
		options_category_containers[category].set_background_color(agl::Color(0, 0, 0, 0));

		options_category_labels[category].apply(bronze_age_label);
		options_category_labels[category].set_size(options_gui_flow.get_inner_width(), 30);
		options_category_labels[category].set_horizontal_align(AGL_ALIGN_CENTER);
		options_category_labels[category].set_vertical_align(AGL_ALIGN_CENTER);
		dict->set_label_key(&options_category_labels[category], options_category_names[category]);

		options_category_containers[category].add(&options_category_labels[category]);

		art::LocalisedString debug_names[1] = { {"shortcut-name.style-debug"} };

		generate_shortcut_options(debug_category_options, debug_names, 1, category, dict);

		options_category_containers[category].set_size(
			options_gui_flow.get_inner_width(),
			options_category_containers[category].get_total_height()
		);

		options_gui_flow.add(&options_category_containers[category]);
	}

	options_gui_flow.set_size(
		options_gui_flow.get_inner_width(), options_gui_flow.get_total_height()
	);

	options_gui.set_size(options_gui_flow.get_width(), options_gui_flow.get_height());

	options_gui.set_visible(false);

	main_menu_buttons[1].set_click_function(
		std::bind(&MainMenuGui::open_subgui, this, std::placeholders::_1, std::placeholders::_2)
	);

	subguis[1] = &options_gui;

	// Licenses
	content_panel.add(&licenses_gui);
	licenses_gui.set_size(screenh - 4, screenh - 4);
	licenses_gui.set_location((content_panel.get_inner_width() - licenses_gui.get_width()) / 2, 0);
	licenses_gui.apply(main_menu_subgui);

	licenses_gui.add(&licenses_gui_flow);

	licenses_gui_flow.set_main_axis(AGL_VERTICAL);
	licenses_gui_flow.set_second_axis_align(AGL_ALIGN_CENTER);
	licenses_gui_flow.set_size(licenses_gui.get_inner_width(), licenses_gui.get_inner_height());
	licenses_gui_flow.set_background_color(agl::Color(0, 0, 0, 0));
	licenses_gui_flow.set_single_subflow(true);

	licenses_gui_flow.add(&licenses_main_label);
	licenses_gui_flow.add(&licenses_text);

	licenses_main_label.apply(bronze_age_main_label);
	dict->set_label_key(&licenses_main_label, { "gui-element.licenses" });
	licenses_main_label.resize_always();

	std::string licenses_text_text;

	{
		licenses_text_text += ""
			"Copyright (c) 2013-2019 Niels Lohmann\n\n"

			"Permission is hereby granted, free of charge, to any person obtaining"
			" a copy of this softwareand associated documentation files (the \"Software\"),"
			" to deal in the Software without restriction, including without limitation the"
			" rights to use, copy, modify, merge, publish, distribute, sublicense, and//or"
			" sell copies of the Software, and to permit persons to whom the Software is"
			" furnished to do so, subject to the following conditions:\n\n"

			"The above copyright noticeand this permission notice shall be included in all"
			" copies or substantial portions of the Software.\n\n"

			"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,"
			" INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A"
			" PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT"
			" HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION"
			" OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE"
			" SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n\n\n";

		licenses_text_text += ""
			"Copyright (c) 2008 - 2010 the Allegro 5 Development Team\n\n"

			"This software is provided \'as - is\', without any express or implied"
			" warranty. In no event will the authors be held liable for any damages"
			" arising from the use of this software.\n\n"

			"Permission is granted to anyone to use this software for any purpose,"
			" including commercial applications, and to alter itand redistribute"
			" it freely, subject to the following restrictions:\n\n"

			"  1. The origin of this software must not be misrepresented; you must not"
			" claim that you wrote the original software. If you use this software"
			" in a product, an acknowledgment in the product documentation would"
			" be appreciated but is not required.\n\n"

			"  2. Altered source versions must be plainly marked as such, and must not"
			" be misrepresented as being the original software.\n\n"

			"  3. This notice may not be removed or altered from any source"
			" distribution\n\n\n";

		licenses_text_text += ""
			"Copyright(c) 2012 - present, Victor Zverovich\n\n"

			"Permission is hereby granted, free of charge, to any person obtaining"
			" a copy of this softwareand associated documentation files(the \"Software\")"
			", to deal in the Software without restriction, including without limitation"
			" the rights to use, copy, modify, merge, publish, distribute, sublicense,"
			" and//or sell copies of the Software, and to permit persons to whom the"
			" Software is furnished to do so, subject to the following conditions:\n\n"

			"  1. The above copyright noticeand this permission notice shall be included"
			" in all copies or substantial portions of the Software.\n\n"

			"  2. THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND,"
			" EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF"
			" MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT."
			"IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY"
			" CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,"
			" TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE"
			" SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n\n"

			"-- - Optional exception to the license-- -\n\n"

			"As an exception, if, as a result of your compiling your source code,"
			" portions of this Software are embedded into a machine - executable"
			" object form of such source code, you may redistribute such embedded"
			" portions in such object form without including the above copyrightand"
			" permission notices.\n\n\n";

		licenses_text_text += ""
			"Copyright (c) 1994–2019 Lua.org, PUC - Rio.\n"
			"Permission is hereby granted, free of charge, to any person obtaining a"
			" copy of this softwareand associated documentation files(the \"Software\"),"
			" to deal in the Software without restriction, including without limitation"
			" the rights to use, copy, modify, merge, publish, distribute, sublicense,"
			" and//or sell copies of the Software, and to permit persons to whom the"
			" Software is furnished to do so, subject to the following conditions:\n\n"

			"The above copyright noticeand this permission notice shall be included in"
			" all copies or substantial portions of the Software.\n\n"

			"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS"
			" OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,"
			" FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL"
			" THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER"
			" LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,"
			" OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN"
			" THE SOFTWARE.\n\n\n";

		licenses_text_text += ""
			"Copyright(c) 2014, jmasterx\n"
			"All rights reserved.\n\n"

			"Redistribution and use in sourceand binary forms, with or without"
			" modification, are permitted provided that the following conditions are met:\n\n"

			"  * Redistributions of source code must retain the above copyright notice, this"
			" list of conditions and the following disclaimer.\n\n"

			"  * Redistributions in binary form must reproduce the above copyright notice,"
			" this list of conditionsand the following disclaimer in the documentation"
			" and /or other materials provided with the distribution.\n\n"

			"  * Neither the name of the { organization } nor the names of its"
			" contributors may be used to endorse or promote products derived from"
			" this software without specific prior written permission.\n\n"

			"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\""
			" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE"
			" IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE"
			" DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE"
			" FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL"
			" DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR"
			" SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER"
			" CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,"
			" OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE"
			" OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n\n\n";
	}

	licenses_text.apply(bronze_age_label);
	licenses_text.set_size(screenh - 200, 100);
	licenses_text.set_multiline(true);
	licenses_text.set_text_wrap(true);
	licenses_text.set_text(licenses_text_text);
	licenses_text.set_base_color(agl::Color(255, 230, 230));
	licenses_text.resize_to_text();

	licenses_gui_flow.set_size(
		licenses_gui_flow.get_inner_width(), licenses_gui_flow.get_total_height()
	);

	licenses_gui.set_size(licenses_gui_flow.get_width(), licenses_gui_flow.get_height());

	licenses_gui.set_visible(false);

	main_menu_buttons[2].set_click_function(
		std::bind(&MainMenuGui::open_subgui, this, std::placeholders::_1, std::placeholders::_2)
	);

	subguis[2] = &licenses_gui;

	// Creators
	content_panel.add(&creators_gui);
	creators_gui.set_size(screenh - 4, screenh - 4);
	creators_gui.set_location((content_panel.get_inner_width() - creators_gui.get_width()) / 2, 0);
	creators_gui.apply(main_menu_subgui);

	creators_gui.add(&creators_gui_flow);

	creators_gui_flow.set_main_axis(AGL_VERTICAL);
	creators_gui_flow.set_second_axis_align(AGL_ALIGN_CENTER);
	creators_gui_flow.set_size(creators_gui.get_inner_width(), creators_gui.get_inner_height());
	creators_gui_flow.set_background_color(agl::Color(0, 0, 0, 0));
	creators_gui_flow.set_single_subflow(true);

	creators_gui_flow.add(&creators_main_label);
	creators_gui_flow.add(&creators_text);

	creators_main_label.apply(bronze_age_main_label);
	dict->set_label_key(&creators_main_label, { "gui-element.creators" });
	creators_main_label.resize_always();

	std::vector<std::pair<std::string, std::vector<std::string>>> creators = {
			{"Programming", {"Michal Margos"}},
			{"Graphics & 3D modelling", {"Ivan Titkov", "Michal Margos"}},
			{"Music", {"Ivan Titkov"}}
	};

	std::string creators_text_text;

	for (const auto& category : creators)
	{
		creators_text_text += "<text:def:0:24>" + category.first + "\n<text:def:def:def>";

		for (const auto& creator : category.second)
		{
			creators_text_text += creator + "\n";
		}

		creators_text_text.push_back('\n');
	}

	creators_text.apply(bronze_age_label);
	creators_text.set_size(screenh - 200, 100);
	creators_text.set_multiline(true);
	creators_text.set_rich_support(true);
	creators_text.set_base_color(agl::Color(255, 230, 230));
	creators_text.set_rich_colors({ agl::Color(200, 86, 13) });
	creators_text.set_text(creators_text_text);
	creators_text.set_size(creators_text.get_inner_width(), creators_text.get_text_height());

	creators_gui_flow.set_size(
		creators_gui_flow.get_inner_width(), creators_gui_flow.get_total_height()
	);

	creators_gui.set_size(creators_gui_flow.get_width(), creators_gui_flow.get_height());

	creators_gui.set_visible(false);

	main_menu_buttons[3].set_click_function(
		std::bind(&MainMenuGui::open_subgui, this, std::placeholders::_1, std::placeholders::_2)
	);

	subguis[3] = &creators_gui;

	subguis[4] = &mods_gui;

	// Finalization
	content_panel.get_children_container()->set_size(
		content_panel.get_inner_width(),
		0
	);

	content_panel.get_children_container()->set_size(
		content_panel.get_inner_width(),
		content_panel.get_children_container()->get_total_height(false)
	);

	content_scrollbar.set_step(content_panel.get_children_container()->get_height() / 100);
}

void MainMenuGui::close_subguis()
{
	for (int i = 0; i < 5; i++)
		subguis[i]->set_visible(false);
}

std::string shortcut_to_string(int key, int mods)
{
	std::string ret;

	std::unordered_map<int, std::string> keymod_to_name = {
		{ ALLEGRO_KEYMOD_CTRL, "CTRL" },
		{ ALLEGRO_KEYMOD_SHIFT, "SHIFT" },
		{ ALLEGRO_KEYMOD_ALT, "lALT" },
		{ ALLEGRO_KEYMOD_ALTGR, "rALT" }
	};
	
	for (const auto& [mod, name] : keymod_to_name)
	{
		if (mods & mod)
			ret += name + " + ";
	}

	ret += al_keycode_to_name(key);
	return ret;
}

void setup_styles()
{
	horizontal_flow = new agl::Style("horizontal-flow");
	bronze_age_hflow = new agl::Style("bronze-age-hflow");
	bronze_age_scrollbar = new agl::Style("bronze-age-scrollbar");
	bronze_age_menubutton = new agl::Style("bronze-age-menubutton");
	main_menu_subgui = new agl::Style("main-menu-subgui");
	bronze_age_label = new agl::Style("bronze-age-label");
	bronze_age_main_label = new agl::Style("bronze-age-main-label");

	button_action = new agl::Style("button-action");
	button_danger = new agl::Style("button-danger");
	button_neutral = new agl::Style("button-neutral");
	button_accept = new agl::Style("button-accept");

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

	main_menu_subgui->set_value("background_color", agl::Color(123, 65, 10, 192));
	main_menu_subgui->set_value("border_left", 2);
	main_menu_subgui->set_value("border_right", 2);
	main_menu_subgui->set_value("border_top", 2);
	main_menu_subgui->set_value("border_bottom", 2);
	main_menu_subgui->set_value("border_color_left", agl::Color(103, 45, 0));
	main_menu_subgui->set_value("border_color_right", agl::Color(103, 45, 0));
	main_menu_subgui->set_value("border_color_top", agl::Color(103, 45, 0));
	main_menu_subgui->set_value("border_color_bottom", agl::Color(103, 45, 0));
	main_menu_subgui->set_value("margin_top", 50);
	main_menu_subgui->set_value("margin_bottom", 50);

	bronze_age_label->set_value("base_color", agl::Color(255, 210, 103));
	bronze_age_label->set_value("base_font", AGL_FONT_DEFAULT);
	bronze_age_label->set_value("base_size", 18);

	bronze_age_main_label->apply(bronze_age_label);
	bronze_age_main_label->set_value("base_size", 36);
	bronze_age_main_label->set_value("margin_top", 20);
	bronze_age_main_label->set_value("margin_bottom", 30);

	button_action->set_value("sizing", AGL_SIZING_BORDERBOX);
	button_action->set_value("border_top", 2);
	button_action->set_value("border_right", 2);
	button_action->set_value("border_bottom", 2);
	button_action->set_value("border_left", 2);

	button_danger->apply(button_action);
	button_danger->set_value("background_color", agl::Color(255, 128, 128));
	button_danger->set_value("background_color_hover", agl::Color(235, 108, 108));

	agl::Color d_ne(235, 180, 180);
	agl::Color d_sw(128, 50, 50);

	button_danger->set_value("border_color_top", d_ne);
	button_danger->set_value("border_color_right", d_sw);
	button_danger->set_value("border_color_bottom", d_sw);
	button_danger->set_value("border_color_left", d_ne);

	button_neutral->apply(button_action);
	button_neutral->set_value("background_color", agl::Color(240, 240, 240));
	button_neutral->set_value("background_color_hover", agl::Color(210, 210, 210));

	agl::Color n_ne(255, 255, 255);
	agl::Color n_sw(200, 200, 200);

	button_neutral->set_value("border_color_top", n_ne);
	button_neutral->set_value("border_color_right", n_sw);
	button_neutral->set_value("border_color_bottom", n_sw);
	button_neutral->set_value("border_color_left", n_ne);

	button_accept->apply(button_action);
	button_accept->set_value("background_color", agl::Color(100, 240, 180));
	button_accept->set_value("background_color_hover", agl::Color(130, 255, 210));
	
	agl::Color a_ne(120, 255, 200);
	agl::Color a_sw(70, 210, 150);
	
	button_accept->set_value("border_color_top", a_ne);
	button_accept->set_value("border_color_right", a_sw);
	button_accept->set_value("border_color_bottom", a_sw);
	button_accept->set_value("border_color_left", a_ne);
}

NewGameGui::NewGameGui(agl::Gui* gui, int screenw, int screenh, art::Dictionary* dict)
	: TwoPanelGui(gui, screenh/2, dict)
{
	dict->set_label_key(&frame_label, { "gui-element.new-game" });

	content_panel.add(&general_section);

	general_section.set_size(content_panel.get_inner_width(), content_panel.get_inner_height());
	general_section.set_background_color(agl::Color(0, 0, 0, 0));

	general_section.add(&name_label);

	name_label.apply(bronze_age_label);
	name_label.set_location(5.f, 5.f);
	name_label.resize_always();
	dict->set_label_key(&name_label, {"gui-element.name"});
	
	general_section.add(&name_input);

	name_input.set_size(120, 30);
	name_input.set_location(5.f, 5.f + name_label.get_text_height() + 5.f);
	name_input.set_background_color(agl::Color(113, 55, 0));
	name_input.set_cursor_color(agl::Color(200, 150, 30));
	name_input.create_label();
	name_input.apply_to_label(bronze_age_label);

	name_input.set_return_keycode(ALLEGRO_KEY_ENTER);
	name_input.set_backspace_keycode(ALLEGRO_KEY_BACKSPACE);

	general_section.add(&seed_label);

	seed_label.apply(bronze_age_label);
	seed_label.set_location(5.f, name_label.get_text_height() + 45.f);
	seed_label.resize_always();
	dict->set_label_key(&seed_label, { "setting-name.game-seed" });

	general_section.add(&seed_input);

	seed_input.set_size(120, 30);
	seed_input.set_location(5.f, 2 * name_label.get_text_height() + 50.f);
	seed_input.set_background_color(agl::Color(113, 55, 0));
	seed_input.set_cursor_color(agl::Color(200, 150, 30));
	seed_input.create_label();
	seed_input.apply_to_label(bronze_age_label);

	seed_input.set_return_keycode(ALLEGRO_KEY_ENTER);
	seed_input.set_backspace_keycode(ALLEGRO_KEY_BACKSPACE);

	create_buttons({ {"gui-element.general"}, {"gui-element.create"} }, { &general_section_btn, &create_btn }, dict);
}
