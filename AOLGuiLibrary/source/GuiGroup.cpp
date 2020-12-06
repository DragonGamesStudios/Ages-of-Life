#include "GuiGroup.h"

#include <iostream>
#include <allegro5/allegro_primitives.h>

std::map<std::string, std::vector<std::string>> style_flags = {
	{"axis", {"", "AGL_HORIZONTAL", "AGL_VERTICAL"}},
	{"main_axis", {"", "AGL_HORIZONTAL", "AGL_VERTICAL"}},
	{"sizing", {
		"",
		"AGL_SIZING_CONTENTBOX",
		"AGL_SIZING_PADDINGBOX",
		"AGL_SIZING_BORDERBOX",
		"AGL_SIZING_MARGINBOX"
	}},
	{"image_scaling", {
		"",
		"AGL_SIZING_CONTENTBOX",
		"AGL_SIZING_PADDINGBOX",
		"AGL_SIZING_BORDERBOX",
		"AGL_SIZING_MARGINBOX"
	}},
	{"image_display_box", {
		"",
		"AGL_SIZING_CONTENTBOX",
		"AGL_SIZING_PADDINGBOX",
		"AGL_SIZING_BORDERBOX",
		"AGL_SIZING_MARGINBOX"
	}},
	{"marker", {"AGL_MARKER_CUSTOM", "AGL_MARKER_CREATE"}},
};

namespace agl
{
	void GuiGroup::draw_hr_line(int mx, int width, int* y)
	{
		al_draw_line(
			mx + 15, *y, mx + width - 15, *y,
			debug::hr_color.calculated, 1
		);

		*y += 3;
	}

	void GuiGroup::register_event_handler(EventHandler* handler)
	{
		event_handler = handler;
	}

	void GuiGroup::add_gui(Gui* gui)
	{
		guis.insert(gui);
	}

	void GuiGroup::remove_gui(Gui* gui)
	{
		guis.erase(gui);
	}

	bool GuiGroup::is_opened(Gui* gui)
	{
		return guis.find(gui) != guis.end();
	}

	void GuiGroup::draw()
	{
		for (const auto& gui : guis)
			gui->draw();

		if (debug::debug && event_receiver)
		{
			int debug_height = 20;
			int debug_width = 300;
			int text_height = loaded_fonts["default"]->get_height(debug::font_size);
			int mx = event_handler->get_mouse_state()->x,
				my = event_handler->get_mouse_state()->y;
			debug_height += text_height;

			std::map<
				Style*,
				std::map<std::string, std::variant<int, bool, Color>>,
				StyleComparator
			> style_sorted;

			if (event_receiver->get_style())
			{
				for (const auto& [key, val] : event_receiver->get_style()->values)
				{
					if (val.source)
					{
						if (style_sorted.find(val.source) == style_sorted.end())
						{
							style_sorted.insert({ val.source, {} });
							debug_height += text_height + 3;
						}

						style_sorted[val.source].insert({ key, val.value });
						debug_height += text_height + 3;
					}
				}
			}

			int current_y = my - debug_height;

			if (my < debug_height)
				current_y = my;

			int current_x = mx;
			if (mx > screen_width - debug_width)
				current_x = mx - debug_width;

			al_draw_filled_rectangle(
				current_x, current_y, current_x + debug_width, current_y + debug_height,
				debug::background_color.calculated
			);

			current_y += 10;
			al_draw_text(
				loaded_fonts["default"]->get(debug::font_size),
				debug::default_color.calculated,
				current_x + 5, current_y, 0,
				typeid(*event_receiver).name()
			);

			current_y += text_height + 3;

			for (const auto& [key, value] : style_sorted)
			{

				draw_hr_line(current_x, debug_width, &current_y);

				std::string style_text = "Derived from \"" + key->name + "\":";

				al_draw_text(
					loaded_fonts["default"]->get(debug::font_size),
					debug::stylename_color.calculated,
					current_x + 5, current_y, 0,
					style_text.c_str()
				);

				current_y += text_height + 3;

				for (const auto& [rulename, ruleval] : value)
				{
					Color value_color;
					std::string value_text;
					switch (ruleval.index())
					{
					case 0:
						if (style_flags.find(rulename) == style_flags.end())
						{
							value_color = debug::int_color;
							value_text = std::to_string(std::get<int>(ruleval));
						}
						else
						{
							value_color = debug::flag_color;
							value_text = style_flags[rulename][std::get<int>(ruleval)];
						}
						break;
					case 2:
						value_color = debug::int_color;
						value_text = "{"
							+ std::to_string(std::get<Color>(ruleval).r) + ", "
							+ std::to_string(std::get<Color>(ruleval).g) + ", "
							+ std::to_string(std::get<Color>(ruleval).b) + "}";
						break;
					case 1:
						value_color = debug::bool_color;
						value_text = std::get<bool>(ruleval) ? "true" : "false";
						break;
					default:
						break;
					}

					al_draw_text(
						loaded_fonts["default"]->get(debug::font_size),
						debug::default_color.calculated,
						current_x + 5, current_y, 0,
						(rulename + ":").c_str()
					);

					int valx = current_x + 5 + loaded_fonts["default"]->get_width(
						debug::font_size,
						rulename + ": "
					);

					if (ruleval.index() == 2)
					{
						al_draw_filled_rectangle(
							valx, current_y, valx + text_height,
							current_y + text_height,
							std::get<Color>(ruleval).calculated
						);

						valx += text_height + 2;
					}

					al_draw_text(
						loaded_fonts["default"]->get(debug::font_size),
						value_color.calculated,
						valx,
						current_y, 0,
						value_text.c_str()
					);

					current_y += text_height + 3;
				}
			}

			//std::cout << typeid(*event_receiver).name() << '\n';
		}
	}

	void GuiGroup::update()
	{
		if (event_receiver && !event_receiver->get_dragged())
			event_receiver = NULL;

		Point mouse_location(
			event_handler->get_mouse_state()->x,
			event_handler->get_mouse_state()->y
		);

		for (const auto& gui : guis)
			gui->update(&event_receiver, mouse_location);

		if (event_receiver)
		{
			for (auto it = event_handler->get_event_queue_begin();
				it != event_handler->get_event_queue_end(); it++)
			{
				it->source = event_receiver;
				event_receiver->raise_event(*it);
			}

			event_receiver->set_direct_hover(true);
		}
	}

	void GuiGroup::set_screen_dimensions(int w, int h)
	{
		screen_width = w;
		screen_height = h;
	}

	
	bool ZBufferComparator::operator()(const Gui* gui1, const Gui* gui2) const
	{
		return gui1->get_z_index() < gui2->get_z_index();
	}

	bool StyleComparator::operator()(const Style* style1, const Style* style2) const
	{
		return style1->name < style2->name;
	}

}