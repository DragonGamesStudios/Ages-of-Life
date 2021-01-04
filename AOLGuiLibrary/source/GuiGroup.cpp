#include "agl/GuiGroup.h"

#include <iostream>

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
	{"base_font", {"AGL_FONT_DEFAULT"}},
};

namespace agl
{
	void GuiGroup::draw_hr_line(float mx, int width, float* y)
	{
		graphics_handler->draw_line({ mx + 15, *y }, { mx + width - 15, *y }, 1, debug::hr_color);

		*y += 3;
	}

	GuiGroup::GuiGroup()
	{
		event_handler = 0;
		graphics_handler = 0;

		event_receiver = 0;
		focus_listener = 0;

		screen_width = 0;
		screen_height = 0;
	}

	GuiGroup::~GuiGroup()
	{
		screen_width = 0;
		screen_height = 0;
		event_receiver = 0;
		focus_listener = 0;
		event_handler = 0;
		graphics_handler = 0;
		guis.clear();
	}

	void GuiGroup::register_event_handler(EventHandler* handler)
	{
		event_handler = handler;
	}

	void GuiGroup::register_graphics_handler(GraphicsHandler* handler)
	{
		graphics_handler = handler;
		for (auto& child : guis)
			child->connect_graphics_handler(handler);
	}

	void GuiGroup::add_gui(Gui* gui)
	{
		guis.insert(gui);
		gui->connect_graphics_handler(graphics_handler);
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
			float debug_height = 20;
			float debug_width = 300;
			float text_height = (float)loaded_fonts["default"]->get_height(debug::font_size);
			float mx = (float)event_handler->get_mouse_state()->x,
				my = (float)event_handler->get_mouse_state()->y;
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

			float current_y = my - debug_height;

			if (my < debug_height)
				current_y = my;

			float current_x = mx;
			if (mx > screen_width - debug_width)
				current_x = mx - debug_width;

			graphics_handler->draw_filled_rectangle(
				{ current_x, current_y, debug_width, debug_height }, debug::background_color
			);

			current_y += 10;

			graphics_handler->draw_text(
				{ current_x + 5, current_y }, typeid(*event_receiver).name(), loaded_fonts["default"], debug::font_size, debug::default_color
			);

			current_y += text_height + 3;

			for (const auto& [key, value] : style_sorted)
			{

				draw_hr_line(current_x, (int)debug_width, &current_y);

				std::string style_text = "Derived from \"" + key->name + "\":";

				graphics_handler->draw_text(
					{ current_x + 5, current_y }, style_text, loaded_fonts["default"], debug::font_size, debug::stylename_color
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
							+ std::to_string(std::get<Color>(ruleval).b) + ", "
							+ std::to_string(std::get<Color>(ruleval).a) + "}";
						break;
					case 1:
						value_color = debug::bool_color;
						value_text = std::get<bool>(ruleval) ? "true" : "false";
						break;
					default:
						break;
					}

					graphics_handler->draw_text(
						{ current_x + 5, current_y }, rulename + ":", loaded_fonts["default"], debug::font_size, debug::default_color
					);

					float valx = current_x + 5 + loaded_fonts["default"]->get_width(
						debug::font_size,
						rulename + ": "
					);

					if (ruleval.index() == 2)
					{

						graphics_handler->draw_filled_rectangle({ valx, current_y, text_height, text_height }, std::get<Color>(ruleval));

						valx += text_height + 2;
					}

					graphics_handler->draw_text(
						{ valx, current_y }, value_text, loaded_fonts["default"], debug::font_size, value_color
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
			(float)event_handler->get_mouse_state()->x,
			(float)event_handler->get_mouse_state()->y
		);

		for (const auto& gui : guis)
			gui->update(&event_receiver, &focus_listener, mouse_location);

		if (event_receiver)
		{
			for (auto it = event_handler->get_event_queue_begin();
				it != event_handler->get_event_queue_end(); it++)
			{

				it->source = event_receiver;
				event_receiver->raise_event(*it);

				if (focus_listener && focus_listener != event_receiver)
				{
					it->source = focus_listener;
					focus_listener->raise_event(*it);
					if (!focus_listener->get_focus())
						focus_listener = NULL;
				}
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