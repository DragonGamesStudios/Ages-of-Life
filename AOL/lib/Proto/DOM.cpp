#include "DOM.h"

json DOM_default_ruleset_json = {
	{"background-color", "rgba(0, 0, 0, 0)"},
	{"background-image", "none"},
	{"background-clip", "border-box"},
	{"background-origin", "border-box"},
	{"background-size", "auto auto"},
	{"background-position-x", "0px"},
	{"background-position-y", "0px"},
	{"background-repeat-x", "no-repeat"},
	{"background-repeat-y", "no-repeat"},
	{"width", "auto"},
	{"height", "auto"},
	{"margin-top", "0px"},
	{"margin-bottom", "0px"},
	{"margin-left", "0px"},
	{"margin-right", "0px"},
	{"padding-top", "0px"},
	{"padding-bottom", "0px"},
	{"padding-left", "0px"},
	{"padding-right", "0px"},
	{"border-width-top", "0px"},
	{"border-width-bottom", "0px"},
	{"border-width-left", "0px"},
	{"border-width-right", "0px"},
	{"border-style-top", "none"},
	{"border-style-bottom", "none"},
	{"border-style-left", "none"},
	{"border-style-right", "none"},
	{"border-color-top", "black"},
	{"border-color-bottom", "black"},
	{"border-color-left", "black"},
	{"border-color-right", "black"},
	{"overflow", "hidden"},
	{"scroll", "y"}
};

bool is_element_hovered(int x, int y, DOM_element* element)
{
	int hoverbox_x = x + element->computed->x + element->computed->margin_left,
		hoverbox_y = y + element->computed->y + element->computed->margin_top;

	if (
		hoverbox_x <= element->document->mouse_state.x && element->document->mouse_state.x <= hoverbox_x + element->get_width("cpb") &&
		hoverbox_y <= element->document->mouse_state.y && element->document->mouse_state.y <= hoverbox_y + element->get_height("cpb")
		)
	{
		return true;
	}
	return false;
}

DOM_ruleset DOM_default_ruleset = DOM_default_ruleset_json;

std::map<std::string, ALLEGRO_BITMAP*> DOM_loaded_images = {};

const std::map<std::string, float> unit_to_pixel = {
	{"px", 1}
};

std::pair<std::string, std::string> divide_value_unit(std::string dividing)
{
	bool reading_value = true;
	std::string value = "";
	std::string unit = "";

	for (auto c : dividing)
	{
		if (reading_value)
		{
			if (c >= '0' && c <= '9')
				value.push_back(c);
			else
				reading_value = false;
		}
		if (!reading_value)
			unit.push_back(c);
	}

	return std::make_pair(value, unit);
}

std::vector<std::vector<std::string>> parse_expression(std::string value)
{
	std::vector<std::vector<std::string>> result = {};
	std::vector<std::string> current_chunk = {};
	std::string current_value = "";

	bool escape = false;

	for (auto c : value)
	{
		if (c == ',' && !escape)
		{
			// If coma, add current value to chunk and chunk to result
			if (!current_value.empty())
				current_chunk.push_back(current_value);
			result.push_back(current_chunk);
			current_chunk.clear();
			current_value.clear();
		}
		else if (c == ' ' && !escape)
		{
			// If space, add current value to current chunk
			if (!current_value.empty())
				current_chunk.push_back(current_value);
			current_value.clear();
		}
		else if (c == '(')
		{
			escape = true;
			current_value.push_back(c);
		}
		else if (c == ')')
		{
			escape = false;
			current_value.push_back(c);
		}
		else
		{
			// Otherwise, add char to value
			current_value.push_back(c);
		}
	}

	if (!current_value.empty())
		current_chunk.push_back(current_value);
	result.push_back(current_chunk);

	return result;
}

std::pair<std::string, std::string> parse_value(std::string value)
{
	std::string fn_name = "";

	bool parsing_function = true;

	std::string current_value = "";

	for (auto c : value)
	{
		if (parsing_function)
		{
			if (c == '(')
				parsing_function = false;
			else
				fn_name.push_back(c);
		}
		else
		{
			if (c == ')')
				break;
			else
				current_value.push_back(c);
		}
	}

	if (parsing_function)
		std::swap(fn_name, current_value);

	return std::make_pair(fn_name, current_value);
}

void hsl_to_rgb(int h, float s, float l, unsigned char* r, unsigned char* g, unsigned char* b)
{
	int hps = h / 60;
	float c = (1 - std::abs(2 * l - 1)) * s;
	float x = c * (1 - std::abs(fmod(float(h) / float(60), 2) - 1));
	float m = l - c / 2;

	float frgb[3] = {0, 0, 0};

	switch (hps)
	{
	case 0:
		frgb[0] = c;
		frgb[1] = x;
		frgb[2] = 0;
		break;
	case 1:
		frgb[0] = x;
		frgb[1] = c;
		frgb[2] = 0;
		break;
	case 2:
		frgb[0] = 0;
		frgb[1] = c;
		frgb[2] = x;
		break;
	case 3:
		frgb[0] = 0;
		frgb[1] = x;
		frgb[2] = c;
		break;
	case 4:
		frgb[0] = x;
		frgb[1] = 0;
		frgb[2] = c;
		break;
	case 5:
		frgb[0] = c;
		frgb[1] = 0;
		frgb[2] = x;
		break;
	default:
		throw std::runtime_error("Incorrect Hue value in HSL color. Should be 0 <= H < 360");
		break;
	}

	unsigned char rgb[3];

	for (int i = 0; i < 3; i++)
		rgb[i] = (frgb[i] + m) * 255;

	*r = rgb[0];
	*g = rgb[1];
	*b = rgb[2];
}

DOM_ruleset::DOM_ruleset(json _info)
{
	this->info = _info;
}

DOM_ruleset DOM_ruleset::operator|(DOM_ruleset ruleset)
{
	// Ruleset merging simply puts all rules in the same ruleset with the second ruleset overriding first
	// and returns the value
	json result;
	for (const auto& [key, value] : this->info.items())
	{
		result[key] = value;
	}

	for (const auto& [key, value] : ruleset.info.items())
	{
		result[key] = value;
	}

	return result;
}

void DOM_ruleset::operator|=(DOM_ruleset ruleset)
{
	this->info = (this->operator|(ruleset)).info;
}

const json DOM_ruleset::get_rule(std::string rule_name)
{
	return this->info[rule_name];
}

DOM_document::DOM_document(DOM_element* root, int width, int height)
{
	this->root = root;

	this->base = new DOM_element;
	this->base->add_child(root);
	std::vector<DOM_element*> buff = { this->base };
	std::copy(buff.begin(), buff.end(), std::back_inserter(root->path));

	this->base->set_rulesets({ json{
		{"background-color", "white"},
		{"width", std::to_string(width)+"px"},
		{"height", std::to_string(height) + "px"},
		{"overflow", "scroll"},
		{"scroll", "both"}
	} });

	add_element_with_children(base);

	this->event_listeners = {
		{DOM_EVENT_MOUSE_PRESSED, {}},
		{DOM_EVENT_MOUSE_RELEASED, {}},
		{DOM_EVENT_MOUSE_MOTION, {}},
		{DOM_EVENT_MOUSE_DRAG, {}},
		{DOM_EVENT_MOUSE_SCROLL, {}},
		{DOM_EVENT_KEY_PRESSED, {}},
		{DOM_EVENT_KEY_RELEASED, {}},
		{DOM_EVENT_KEY_INPUT, {}},
	};
}

void DOM_document::dispatch_event(ALLEGRO_EVENT e)
{
	switch (e.type)
	{
	case ALLEGRO_EVENT_MOUSE_AXES:
		if (!(e.mouse.dz || e.mouse.dw))
		{
			this->events_to_dispatch.push_back({ .type = DOM_EVENT_MOUSE_MOTION, .x = e.mouse.x, .y = e.mouse.y, .dx = e.mouse.dx, .dy = e.mouse.dy });

			// Mose drag detect

			if (this->mouse_state.buttons)
			{
				this->events_to_dispatch.push_back(
					{
						.type = DOM_EVENT_MOUSE_DRAG,
						.x = e.mouse.x,
						.y = e.mouse.y,
						.dx = this->mouse_state.x - e.mouse.x,
						.dy = this->mouse_state.y - e.mouse.y,
						.button = (unsigned int)this->mouse_state.buttons
					}
				);
			}
		}
		else
			this->events_to_dispatch.push_back({ .type = DOM_EVENT_MOUSE_SCROLL, .x = e.mouse.w, .y = e.mouse.z, .dx = e.mouse.dw, .dy = e.mouse.dz });

		break;
	case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
		this->events_to_dispatch.push_back({ .type = DOM_EVENT_MOUSE_PRESSED, .x = e.mouse.x, .y = e.mouse.y, .button = e.mouse.button });
		break;
	case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
		this->events_to_dispatch.push_back({ .type = DOM_EVENT_MOUSE_RELEASED, .x = e.mouse.x, .y = e.mouse.y, .button = e.mouse.button });
		break;
	case ALLEGRO_EVENT_KEY_DOWN:
		this->events_to_dispatch.push_back({
			.type = DOM_EVENT_KEY_PRESSED,
			.keycode = e.keyboard.keycode, 
			.character = (char)e.keyboard.unichar
		});
		break;
	case ALLEGRO_EVENT_KEY_UP:
		this->events_to_dispatch.push_back({
			.type = DOM_EVENT_KEY_RELEASED,
			.keycode = e.keyboard.keycode,
			.character = (char)e.keyboard.unichar
		});
		break;
	case ALLEGRO_EVENT_KEY_CHAR:
		this->events_to_dispatch.push_back({
			.type = DOM_EVENT_KEY_INPUT,
			.keycode = e.keyboard.keycode,
			.character = (char)e.keyboard.unichar,
			.modifiers = e.keyboard.modifiers
		});
		break;
	default:
		break;
	}
}

void DOM_document::draw(int x, int y)
{
	al_use_transform(&default_trans);
	this->base->draw(x, y);
}

void DOM_document::update(double dt, int x, int y)
{
	al_get_mouse_state(&this->mouse_state);

	this->base->update(dt, x, y);

	for (const auto& e : this->element_related_events)
	{
		e.element->event_functions[e.type](e);
	}

	for (const auto& e : this->events_to_dispatch)
	{
		for (auto& listener : this->event_listeners[e.type])
		{
			if (listener->hover)
			{
				listener->event_functions[e.type](e);
				if (e.type == DOM_EVENT_MOUSE_SCROLL && (listener->computed->scrollable_x || listener->computed->scrollable_y))
					listener->dispatch_scroll_event(e);
			}
		}
	}

	this->events_to_dispatch.clear();
	this->element_related_events.clear();
}

void DOM_document::calculate()
{
	for (auto element : this->elements)
		element->document_base = this->base;
	this->base->calculate();
}

void DOM_document::add_element_with_children(DOM_element* element)
{
	// Recursively add element's children to the document
	this->elements.push_back(element);
	element->document = this;
	if (element->computed->scrollable_x || element->computed->scrollable_y)
		this->event_listeners[DOM_EVENT_MOUSE_SCROLL].insert(element);
	for (auto child : element->children)
	{
		child->path = element->path;
		child->path.push_back(child);
		add_element_with_children(child);
	}
}

DOM_document::~DOM_document()
{
	for (auto ptr : this->elements)
	{
		delete ptr;
	}
}

DOM_element::DOM_element()
{
	this->computed = new DOM_computed;
	this->computed_background = new DOM_computed_background;
}

void DOM_element::set_rulesets(std::vector<DOM_ruleset> rulesets)
{
	this->initialized = true;
	this->parsed_ruleset = {};

	// Apply all rulesets
	DOM_ruleset result = DOM_default_ruleset;
	for (auto ruleset : rulesets)
	{
		result |= ruleset;
	}

	// Parse rulesets
	/*int parsed_width = 0, parsed_height = 0;

	auto width_pair = divide_value_unit(this->ruleset.get_rule("width"));
	auto height_pair = divide_value_unit(this->ruleset.get_rule("height"));

	if (width_pair.second != "auto")
	{
		this->computed->auto_width = false;
	}*/

	for (auto rule : result.info.items())
	{
		std::string val = rule.value();
		if (DOM_default_ruleset.info.find(rule.key()) == DOM_default_ruleset.info.end())
		{
			auto key = rule.key();
			for (std::string rulename : std::vector<std::string> 
				{ "margin", "padding", "border-width", "border-style", "border-color" }
				)
			{
				if (key == rulename)
				{
					// Margin shorthand
					auto marginset = parse_expression(rule.value());

					if (marginset[0].size() == 1)
					{
						// All margins
						result.info[rulename + "-top"] = marginset[0][0];
						result.info[rulename + "-left"] = marginset[0][0];
						result.info[rulename + "-right"] = marginset[0][0];
						result.info[rulename + "-bottom"] = marginset[0][0];
					}
					else if (marginset[0].size() == 2)
					{
						// horizontal, vertical
						result.info[rulename + "-top"] = result.info[rulename + "-bottom"] = marginset[0][0];
						result.info[rulename + "-left"] = result.info[rulename + "-right"] = marginset[0][1];
					}
					else if (marginset[0].size() == 4)
					{
						// seperate
						result.info[rulename + "-top"] = marginset[0][0];
						result.info[rulename + "-right"] = marginset[0][1];
						result.info[rulename + "-bottom"] = marginset[0][2];
						result.info[rulename + "-left"] = marginset[0][3];
					}
				}
			}

			for (std::string rulename : std::vector<std::string>
				{ "background-position", "background-repeat" }
				)
			{
				if (key == rulename)
				{
					auto bgpos = parse_expression(result.info[rulename]);
					result.info[rulename + "-x"] = bgpos[0][0];
					result.info[rulename + "-y"] = bgpos[0][1];
				}
			}
		}
		else
		{
			if (rule.value() == "initial")
				val = DOM_default_ruleset.get_rule(rule.key());
			this->parsed_ruleset.insert({ rule.key(), parse_expression(val) });
		}
	}

	std::vector<bool*> color_deps = {
		&this->dependent.background_color,

		&this->dependent.border_color_left,
		&this->dependent.border_color_top,
		&this->dependent.border_color_right,
		&this->dependent.border_color_bottom,
	};

	std::vector<ALLEGRO_COLOR*> color_values = {
		&this->computed_background->background_color,

		& this->computed->border_color_left,
		& this->computed->border_color_top,
		& this->computed->border_color_right,
		& this->computed->border_color_bottom,
	};

	std::vector<std::string> color_names = {
		"background-color",

		"border-color-left",
		"border-color-top",
		"border-color-right",
		"border-color-bottom",
	};

	// Color values
	for (int i = 0; i < color_names.size(); i++)
	{
		auto currently_parsed = parse_value(this->parsed_ruleset[color_names[i]][0][0]);

		unsigned char fixed_color[4] = { 0, 0, 0, 255 };

		if (currently_parsed.second == "inherit")
		{
			*color_deps[i] = true;
		}

		if (currently_parsed.first.empty())
		{

			if (currently_parsed.second[0] == '#')
			{
				std::string color[4];

				if (currently_parsed.second.length() == 7)
				{
					for (int i = 0; i < 3; i++)
					{
						color[i].push_back(currently_parsed.second[i * 2 + 1]);
						color[i].push_back(currently_parsed.second[(i + 1) * 2]);
					}
				}
				else if (currently_parsed.second.length() == 4)
				{
					for (int i = 0; i < 3; i++)
						color[i] = std::string(2, currently_parsed.second[i + 1]);
				}
				else
				{
					throw std::runtime_error("Bad hex color value length!");
				}

				for (int i = 0; i < 3; i++)
					fixed_color[i] = std::stoul("0x" + color[i], nullptr, 16);

				*color_values[i] = al_map_rgba(fixed_color[0], fixed_color[1], fixed_color[2], fixed_color[3]);
			}
			else
			{
				if (predefined_colors.find(currently_parsed.second) != predefined_colors.end())
				{
					*color_values[i] = predefined_colors[currently_parsed.second];
				}
			}
		}
		else
		{
			if (currently_parsed.first == "rgb")
			{
				auto parsed_args = parse_expression(currently_parsed.second);

				if (parsed_args.size() != 3) throw std::runtime_error("Invalid amount of arguments in rgb function.");

				for (int i = 0; i < 3; i++)
					fixed_color[i] = std::atoi(parsed_args[i][0].c_str());

			}
			else if (currently_parsed.first == "rgba")
			{
				auto parsed_args = parse_expression(currently_parsed.second);

				if (parsed_args.size() != 4) throw std::runtime_error("Invalid amount of arguments in rgba function.");

				for (int i = 0; i < 4; i++)
					fixed_color[i] = std::atoi(parsed_args[i][0].c_str());

			}
			else if (currently_parsed.first == "hsl")
			{
				auto parsed_args = parse_expression(currently_parsed.second);

				if (parsed_args.size() != 3) throw std::runtime_error("Invalid amount of arguments in rgb function.");

				float fixed_s = 0, fixed_l = 0;

				auto sp = divide_value_unit(parsed_args[1][0]);
				auto lp = divide_value_unit(parsed_args[2][0]);

				if (sp.second == "%")
					fixed_s = (float)std::atoi(sp.first.c_str()) / 100;
				else
					throw std::runtime_error("Invalid unit of S value in HSL color");

				if (lp.second == "%")
					fixed_l = (float)std::atoi(lp.first.c_str()) / 100;
				else
					throw std::runtime_error("Invalid unit of L value in HSL color");

				hsl_to_rgb(
					std::atoi(parsed_args[0][0].c_str()),
					fixed_s,
					fixed_l,
					&fixed_color[0],
					&fixed_color[1],
					&fixed_color[2]
				);

			}
			else if (currently_parsed.first == "hsla")
			{
				auto parsed_args = parse_expression(currently_parsed.second);

				if (parsed_args.size() != 4) throw std::runtime_error("Invalid amount of arguments in rgba function.");

				float fixed_s = 0, fixed_l = 0;

				auto sp = divide_value_unit(parsed_args[0][1]);
				auto lp = divide_value_unit(parsed_args[0][2]);

				if (sp.second == "%")
					fixed_s = (float)std::atoi(sp.first.c_str()) / 100;
				else
					throw std::runtime_error("Invalid unit of S value in HSL color");

				if (lp.second == "%")
					fixed_l = (float)std::atoi(lp.first.c_str()) / 100;
				else
					throw std::runtime_error("Invalid unit of L value in HSL color");

				hsl_to_rgb(
					std::atoi(parsed_args[0][0].c_str()),
					fixed_s,
					fixed_l,
					&fixed_color[0],
					&fixed_color[1],
					&fixed_color[2]
				);

				fixed_color[3] = std::atoi(parsed_args[3][0].c_str());

			}

			*color_values[i] = al_map_rgba(fixed_color[0], fixed_color[1], fixed_color[2], fixed_color[3]);
		}
	}

	std::vector<std::string> names = {
		"width",
		"height",
		"margin-top",
		"margin-left",
		"margin-right",
		"margin-bottom",
		"padding-top",
		"padding-left",
		"padding-right",
		"padding-bottom",
		"border-width-top",
		"border-width-left",
		"border-width-right",
		"border-width-bottom",
		"background-position-x",
		"background-position-y",
	};

	std::vector<DOM_dependent_unit_value*> dep_ptrs = {
		&this->dependent.width,
		&this->dependent.height,

		&this->dependent.margin_top,
		&this->dependent.margin_left,
		&this->dependent.margin_right,
		&this->dependent.margin_bottom,

		& this->dependent.padding_top,
		& this->dependent.padding_left,
		& this->dependent.padding_right,
		& this->dependent.padding_bottom,

		& this->dependent.border_width_top,
		& this->dependent.border_width_left,
		& this->dependent.border_width_right,
		& this->dependent.border_width_bottom,

		& this->dependent.background_position_x,
		& this->dependent.background_position_y,
	};

	std::vector<int*> cmp_ptrs = {
		&this->computed->box_content_width,
		&this->computed->box_content_height,

		&this->computed->margin_top,
		&this->computed->margin_left,
		&this->computed->margin_right,
		&this->computed->margin_bottom,

		& this->computed->padding_top,
		& this->computed->padding_left,
		& this->computed->padding_right,
		& this->computed->padding_bottom,

		& this->computed->border_width_top,
		& this->computed->border_width_left,
		& this->computed->border_width_right,
		& this->computed->border_width_bottom,

		& this->computed_background->background_position_x,
		& this->computed_background->background_position_y,
	};

	for (int i = 0; i < names.size(); i++)
	{
		auto currently_parsed = parse_value(this->parsed_ruleset[names[i]][0][0]);

		if (currently_parsed.first.empty())
		{
			auto dv_pair = divide_value_unit(currently_parsed.second);
			if (unit_to_pixel.find(dv_pair.second) != unit_to_pixel.end())
				*cmp_ptrs[i] = std::stoi(dv_pair.first) * unit_to_pixel.at(dv_pair.second);
			else
			{
				dep_ptrs[i]->is = true;
				dep_ptrs[i]->unit = dv_pair.second;
				if (!dv_pair.first.empty())
					dep_ptrs[i]->value = std::stof(dv_pair.first) / 100;
			}
		}
	}

	// String values
	std::vector<std::string> str_names = {
		"border-style-left",
		"border-style-top",
		"border-style-right",
		"border-style-bottom",
		"background-clip",
		"background-origin",
	};

	std::vector<std::string*> str_values = {
		&this->computed->border_style_left,
		&this->computed->border_style_top,
		&this->computed->border_style_right,
		&this->computed->border_style_bottom,
		&this->computed_background->background_clip,
		&this->computed_background->background_origin,
	};

	std::vector<bool*> str_deps = {
		&this->dependent.border_style_left,
		&this->dependent.border_style_top,
		&this->dependent.border_style_right,
		&this->dependent.border_style_bottom,
		&this->dependent.background_clip,
		&this->dependent.background_origin,
	};

	for (int i = 0; i < str_names.size(); i++)
	{
		std::string val = this->parsed_ruleset[str_names[i]][0][0];

		if (val == "inherit")
		{
			*str_deps[i] = true;
		}
		*str_values[i] = val;
	}

	// Background Image
	{
		for (int i = 0; i < this->parsed_ruleset["background-image"].size(); i++)
		{
			auto currently_parsed = parse_value(this->parsed_ruleset["background-image"][i][0]);

			if (currently_parsed.first.empty() && currently_parsed.second != "none")
			{
				this->dependent.background_image = true;
			}
			else if (currently_parsed.first == "url")
			{
				if (DOM_loaded_images.find(currently_parsed.second) == DOM_loaded_images.end())
				{
					DOM_loaded_images.insert({ currently_parsed.second, al_load_bitmap(currently_parsed.second.c_str()) });
				}

				if (DOM_loaded_images[currently_parsed.second])
				{
					this->computed_background->background_images.push_back(DOM_loaded_images[currently_parsed.second]);
				}
				else
				{
					std::cout << "DOM gui warning: could not find image on path \"" + currently_parsed.second + "\"\n";
				}
			}
		}
	}

	// Background size
	{
		if (this->parsed_ruleset["background-size"][0].size() == 1)
		{
			this->dependent.background_size_type = this->parsed_ruleset["background-size"][0][0];
			this->dependent.background_size = true;
		}
		else
		{
			const auto buffer = this->parsed_ruleset["background-size"][0];
			// auto ___
			if (buffer[0] == "auto")
			{
				// auto auto
				if (buffer[1] == "auto")
				{
					this->dependent.background_size = true;
					this->dependent.background_size_type = "auto";
				}
				// auto !auto
				else
				{
					auto height_pair = divide_value_unit(buffer[1]);
					// auto __px
					if (unit_to_pixel.find(height_pair.second) != unit_to_pixel.end())
					{
						this->computed_background->background_size_height = std::stoi(height_pair.first) * unit_to_pixel.at(height_pair.second);
						this->dependent.background_size_width.is = true;
						this->dependent.background_size_width.unit = "auto";
					}
					// auto __%
					else
					{
						this->dependent.background_size_height.is = true;
						this->dependent.background_size_height.value = (float)std::stoi(height_pair.first)/100.f;
						this->dependent.background_size_height.unit = height_pair.second;
						this->dependent.background_size_width.is = true;
						this->dependent.background_size_width.unit = "auto";
					}
				}
			}
			// ___ ___
			else
			{
				// ___ auto
				if (buffer[1] == "auto")
				{
					//this->dependent.background_size = true;
					this->dependent.background_size_height.is = true;
					this->dependent.background_size_height.unit = "auto";
				}
				// ___ ___
				else
				{
					auto height_pair = divide_value_unit(buffer[1]);
					// ___ __px
					if (unit_to_pixel.find(height_pair.second) != unit_to_pixel.end())
					{
						this->computed_background->background_size_height = std::stoi(height_pair.first) * unit_to_pixel.at(height_pair.second);
					}
					// ___ __%
					else
					{
						this->dependent.background_size_height.is = true;
						this->dependent.background_size_height.value = (float)std::stoi(height_pair.first) / 100.f;
						this->dependent.background_size_height.unit = height_pair.second;
					}
				}

				auto width_pair = divide_value_unit(buffer[0]);
				// ___px ___
				if (unit_to_pixel.find(width_pair.second) != unit_to_pixel.end())
				{
					this->computed_background->background_size_width = std::stoi(width_pair.first) * unit_to_pixel.at(width_pair.second);
				}
				// __% ___
				else
				{
					this->dependent.background_size_width.is = true;
					this->dependent.background_size_width.value = (float)std::stoi(width_pair.first) / 100.f;
					this->dependent.background_size_width.unit = width_pair.second;
				}
			}
		}
	}

	// Background repeat
	{
		this->computed_background->background_repeat_x = (this->parsed_ruleset["background-repeat-x"][0][0] == "repeat");
		this->computed_background->background_repeat_y = (this->parsed_ruleset["background-repeat-y"][0][0] == "repeat");

		this->dependent.background_repeat_x = (this->parsed_ruleset["background-repeat-x"][0][0] == "inherit");
		this->dependent.background_repeat_y = (this->parsed_ruleset["background-repeat-y"][0][0] == "inherit");
	}
	//*/

	// Overflow
	{
		if (this->parsed_ruleset["overflow"][0][0] == "hidden")
		{
			this->computed->hide_overflow = true;
		}
		else if (this->parsed_ruleset["overflow"][0][0] == "scroll")
		{
			this->computed->hide_overflow = true;
			if (this->parsed_ruleset["scroll"][0][0] == "x" || this->parsed_ruleset["scroll"][0][0] == "both")
				this->computed->scrollable_x = true;
			if (this->parsed_ruleset["scroll"][0][0] == "y" || this->parsed_ruleset["scroll"][0][0] == "both")
				this->computed->scrollable_y = true;
		}
		else if (this->parsed_ruleset["overflow"][0][0] == "visible")
		{
			this->computed->hide_overflow = false;
		}
		else if (this->parsed_ruleset["overflow"][0][0] == "inherit")
			this->dependent.overflow = true;
	}

	// Finalizing
	this->computed->width = this->get_width();
	this->computed->height = this->get_height();
}

void DOM_element::add_child(DOM_element* child)
{
	this->children.push_back(child);
}

void DOM_element::calculate()
{
	if (this->parent && std::get<0>(this->box_model_deps).empty())
	{
		std::get<0>(this->box_model_deps) = {
			{
				&this->dependent.margin_top,
				&this->dependent.margin_bottom,
				&this->dependent.margin_left,
				&this->dependent.margin_right,

				&this->dependent.padding_top,
				&this->dependent.padding_bottom,
				&this->dependent.padding_left,
				&this->dependent.padding_right,

				&this->dependent.border_width_top,
				&this->dependent.border_width_bottom,
				&this->dependent.border_width_left,
				&this->dependent.border_width_right,
			}
		};

		std::get<1>(this->box_model_deps) = {
			{
				&this->computed->margin_top,
				&this->computed->margin_bottom,
				&this->computed->margin_left,
				&this->computed->margin_right,

				&this->computed->padding_top,
				&this->computed->padding_bottom,
				&this->computed->padding_left,
				&this->computed->padding_right,

				&this->computed->border_width_top,
				&this->computed->border_width_bottom,
				&this->computed->border_width_left,
				&this->computed->border_width_right,
			}
		};

		std::get<2>(this->box_model_deps) = {
			{
				false,
				false,
				true,
				true,

				false,
				false,
				true,
				true,

				false,
				false,
				true,
				true,
			}
		};

		std::get<3>(this->box_model_deps) = {
			{
				this->parent->computed->margin_top,
				this->parent->computed->margin_bottom,
				this->parent->computed->margin_left,
				this->parent->computed->margin_right,

				this->parent->computed->padding_top,
				this->parent->computed->padding_bottom,
				this->parent->computed->padding_left,
				this->parent->computed->padding_right,

				this->parent->computed->border_width_top,
				this->parent->computed->border_width_bottom,
				this->parent->computed->border_width_left,
				this->parent->computed->border_width_right,
			}
		};
	}

	if (this->parent)
	{
		for (int i = 0; i < std::get<0>(this->box_model_deps).size(); i++)
		{
			if (std::get<0>(this->box_model_deps)[i]->is)
			{
				auto unit = std::get<0>(this->box_model_deps)[i]->unit;
				auto value = std::get<0>(this->box_model_deps)[i]->value;
				auto ptr = std::get<1>(this->box_model_deps)[i];

				
				if (unit == "%")
				{
					int buff;
					if (std::get<2>(this->box_model_deps)[i]) buff = this->parent->computed->box_content_width;
					else buff = this->parent->computed->box_content_height;
					*ptr = value * buff;
				}
				else if (unit == "auto")
					*ptr = 0;
				else if (unit == "inherit")
					*ptr = std::get<3>(this->box_model_deps)[i];
			}
		}

		this->computed->width = this->get_width();
		this->computed->height = this->get_height();

		{
			if (this->dependent.background_color)
				this->computed_background->background_color = this->parent->computed_background->background_color;

			if (this->dependent.background_origin)
				this->computed_background->background_origin = this->parent->computed_background->background_origin;
			if (this->dependent.background_clip)
				this->computed_background->background_clip = this->parent->computed_background->background_clip;

			if (this->dependent.background_image)
				this->computed_background->background_images = this->parent->computed_background->background_images;

			if (this->dependent.border_style_left)
				this->computed->border_style_left = this->parent->computed->border_style_left;
			if (this->dependent.border_style_right)
				this->computed->border_style_right = this->parent->computed->border_style_right;
			if (this->dependent.border_style_top)
				this->computed->border_style_top = this->parent->computed->border_style_top;
			if (this->dependent.border_style_bottom)
				this->computed->border_style_bottom = this->parent->computed->border_style_bottom;

			if (this->dependent.border_color_left)
				this->computed->border_color_left = this->parent->computed->border_color_left;
			if (this->dependent.border_color_right)
				this->computed->border_color_right = this->parent->computed->border_color_right;
			if (this->dependent.border_color_top)
				this->computed->border_color_top = this->parent->computed->border_color_top;
			if (this->dependent.border_color_bottom)
				this->computed->border_color_bottom = this->parent->computed->border_color_bottom;

			if (this->dependent.overflow)
			{
				this->computed->hide_overflow = this->parent->computed->hide_overflow;
				this->computed->scrollable_x = this->parent->computed->scrollable_x;
				this->computed->scrollable_y = this->parent->computed->scrollable_y;
			}
		}

		if (this->dependent.width.is)
		{
			if (this->dependent.width.unit == "%")
				this->computed->box_content_width = this->dependent.width.value * this->parent->computed->box_content_width;
			else if (this->dependent.width.unit == "auto")
				this->computed->box_content_width = this->parent->computed->box_content_width - this->get_width("pbm");
			else if (this->dependent.width.unit == "vw")
				this->computed->box_content_width = this->dependent.width.value * this->document_base->computed->width;
			else if (this->dependent.width.unit == "vh")
				this->computed->box_content_width = this->dependent.width.value * this->document_base->computed->height;
			else if (this->dependent.width.unit == "inherit")
				this->computed->box_content_width = this->parent->computed->box_content_width;

			this->computed->width = this->get_width();
		}

		if (this->dependent.height.is)
		{
			if (this->dependent.height.unit == "%")
				this->computed->box_content_height = this->dependent.height.value * this->parent->computed->box_content_height;
			// auto value in height is not dependent value
			/*else if (this->dependent.height_unit == "auto")
				this->computed->height = this->parent->computed->height;*/
			else if (this->dependent.height.unit == "vw")
				this->computed->box_content_height = this->dependent.height.value * this->document_base->computed->width;
			else if (this->dependent.height.unit == "vh")
				this->computed->box_content_height = this->dependent.height.value * this->document_base->computed->height;
			else if (this->dependent.height.unit == "inherit")
				this->computed->box_content_height = this->parent->computed->box_content_height;

			this->computed->height = this->get_height();
		}

		if (this->dependent.background_size && !this->computed_background->background_images.empty())
		{
			if (this->dependent.background_size_type == "inherit")
			{
				this->computed_background->background_size_width = this->parent->computed_background->background_size_width;
				this->computed_background->background_size_height = this->parent->computed_background->background_size_height;
			}
			else if (this->dependent.background_size_type == "auto")
			{
				this->computed_background->background_size_width = al_get_bitmap_width(this->computed_background->background_images[0]);
				this->computed_background->background_size_height = al_get_bitmap_height(this->computed_background->background_images[0]);
			}
		}

		int bcw = 0, bch = 0;
		if (this->computed_background->background_origin == "border-box")
		{
			bcw = this->get_width("cpb");
			bch = this->get_height("cpb");
		}
		else if (this->computed_background->background_origin == "padding-box")
		{
			bcw = this->get_width("cp");
			bch = this->get_height("cp");
		}
		else if (this->computed_background->background_origin == "content-box")
		{
			bcw = this->get_width("c");
			bch = this->get_height("c");
		}

		if (this->dependent.background_repeat_x)
			this->computed_background->background_repeat_x = this->parent->computed_background->background_repeat_x;
		if (this->dependent.background_repeat_y)
			this->computed_background->background_repeat_y = this->parent->computed_background->background_repeat_y;

		bool bgsw = false, bgsh = false;
		if (this->dependent.background_size_width.is)
		{
			if (this->dependent.background_size_width.unit == "auto")
				bgsw = true;
			else
			{
				if (this->dependent.background_size_width.unit == "%")
					this->computed_background->background_size_width = this->dependent.background_size_width.value * bcw;
			}
		}

		if (this->dependent.background_size_height.is)
		{
			if (this->dependent.background_size_height.unit == "auto")
				bgsh = true;
			else
			{
				if (this->dependent.background_size_height.unit == "%")
					this->computed_background->background_size_height = this->dependent.background_size_height.value * bch;
			}
		}

		if (!this->computed_background->background_images.empty())
		{
			if (bgsw)
			{
				this->computed_background->background_size_width =
					this->computed_background->background_size_height *
					al_get_bitmap_width(this->computed_background->background_images[0]) / al_get_bitmap_height(this->computed_background->background_images[0]);
			}

			if (bgsh)
			{
				this->computed_background->background_size_height =
					this->computed_background->background_size_width *
					al_get_bitmap_height(this->computed_background->background_images[0]) / al_get_bitmap_width(this->computed_background->background_images[0]);
			}

			this->computed_background->background_size_sx =
				this->computed_background->background_size_width / al_get_bitmap_width(this->computed_background->background_images[0]);
			this->computed_background->background_size_sy = this->computed_background->background_size_height / al_get_bitmap_height(this->computed_background->background_images[0]);

			if (this->dependent.background_position_x.is)
			{
				if (this->dependent.background_position_x.unit == "inherit")
					this->computed_background->background_position_x = this->parent->computed_background->background_position_x;
				else if (this->dependent.background_position_x.unit == "left")
					this->computed_background->background_position_x = 0;
				else if (this->dependent.background_position_x.unit == "right")
					this->computed_background->background_position_x = bcw - al_get_bitmap_width(this->computed_background->background_images[0]);
				else if (this->dependent.background_position_x.unit == "center")
					this->computed_background->background_position_x = (bcw - al_get_bitmap_width(this->computed_background->background_images[0])) / 2;
				else if (this->dependent.background_position_x.unit == "%")
					this->computed_background->background_position_x = this->dependent.background_position_x.value * bcw;
			}

			if (this->dependent.background_position_y.is)
			{
				if (this->dependent.background_position_y.unit == "inherit")
					this->computed_background->background_position_y = this->parent->computed_background->background_position_y;
				else if (this->dependent.background_position_y.unit == "top")
					this->computed_background->background_position_y = 0;
				else if (this->dependent.background_position_y.unit == "bottom")
					this->computed_background->background_position_y = bch - al_get_bitmap_height(this->computed_background->background_images[0]);
				else if (this->dependent.background_position_y.unit == "center")
					this->computed_background->background_position_y = (bch - al_get_bitmap_width(this->computed_background->background_images[0])) / 2;
				else if (this->dependent.background_position_y.unit == "%")
					this->computed_background->background_position_y = this->dependent.background_position_y.value * bch;
			}
		}

		if (this->dependent.margin_left.unit == "auto" && this->dependent.margin_right.unit != "auto")
			this->computed->margin_left = this->parent->computed->width - this->get_width();
		else if (this->dependent.margin_left.unit != "auto" && this->dependent.margin_right.unit == "auto")
			this->computed->margin_right = this->parent->computed->width - this->get_width();
		else if (this->dependent.margin_left.unit == "auto" && this->dependent.margin_right.unit == "auto")
			this->computed->margin_right = this->computed->margin_left = (this->parent->computed->width - this->get_width()) / 2;
	}

	int last_y = 0;

	for (auto child : this->children)
	{
		child->parent = this;
		// Recursively calculate children
		child->calculate();

		// Set basic coordinates
		child->computed->x = 0;
		child->computed->y = last_y;

		last_y += child->computed->height;

		// Update dimensions
		if (this->dependent.height.unit == "auto")
		{
			this->computed->height += child->computed->height;
		}
	}
}

int DOM_element::get_width(std::string boxes)
{
	int result = 0;

	for (auto c : boxes)
	{
		switch (c)
		{
		case 'c':
			result += this->computed->box_content_width;
			break;
		case 'p':
			result += this->computed->padding_left + this->computed->padding_right;
			break;
		case 'b':
			result += this->computed->border_width_left + this->computed->border_width_right;
			break;
		case 'm':
			result += this->computed->margin_left + this->computed->margin_right;
			break;
		default:
			break;
		}
	}

	return result;
}

int DOM_element::get_height(std::string boxes)
{
	int result = 0;

	for (auto c : boxes)
	{
		switch (c)
		{
		case 'c':
			result += this->computed->box_content_height;
			break;
		case 'p':
			result += this->computed->padding_top + this->computed->padding_bottom;
			break;
		case 'b':
			result += this->computed->border_width_top + this->computed->border_width_bottom;
			break;
		case 'm':
			result += this->computed->margin_top + this->computed->margin_bottom;
			break;
		default:
			break;
		}
	}

	return result;
}

void DOM_element::draw(int x, int y)
{
	int bbx = x + this->computed->x + this->computed->margin_left, bby = y + this->computed->y + this->computed->margin_top;
	int bbx2 = x + this->computed->x + this->computed->margin_left + this->get_width("cpb");
	int bby2 = y + this->computed->y + this->computed->margin_top + this->get_height("cpb");
	int tb = this->computed->border_width_top, bb = this->computed->border_width_bottom,
		lb = this->computed->border_width_left, rb = this->computed->border_width_right;

	int bgx1 = x + this->computed->x + this->computed->margin_left, bgy1 = y + this->computed->y + this->computed->margin_top,
		bgx2 = x + this->computed->x + this->computed->margin_left + this->get_width("cpb"),
		bgy2 = y + this->computed->y + this->computed->margin_top + this->get_height("cpb");

	int brx = x + this->computed->x + this->computed->margin_left + this->computed_background->background_position_x,
		bry = y + this->computed->y + this->computed->margin_top + this->computed_background->background_position_y;

	if (this->computed_background->background_clip == "padding-box")
	{
		bgx1 += this->computed->border_width_left;
		bgy1 += this->computed->border_width_top;
		bgx2 -= this->computed->border_width_right;
		bgy2 -= this->computed->border_width_bottom;
	}
	else if (this->computed_background->background_clip == "content-box")
	{
		bgx1 += this->computed->border_width_left + this->computed->padding_left;
		bgy1 += this->computed->border_width_top + this->computed->padding_top;
		bgx2 -= this->computed->border_width_right + this->computed->padding_right;
		bgy2 -= this->computed->border_width_bottom + this->computed->padding_bottom;
	}

	if (this->computed_background->background_origin == "padding-box")
	{
		brx += this->computed->border_width_left;
		bry += this->computed->border_width_top;
	}
	else if (this->computed_background->background_origin == "content-box")
	{
		brx += this->computed->border_width_left + this->computed->padding_left;
		bry += this->computed->border_width_top + this->computed->padding_top;
	}

	if (this->hover)
		al_draw_filled_rectangle(
			bgx1,
			bgy1,
			bgx2,
			bgy2,
			al_map_rgb_f(
				this->computed_background->background_color.r + 0.1,
				this->computed_background->background_color.g + 0.1,
				this->computed_background->background_color.b + 0.1
			)
		);
	else
		al_draw_filled_rectangle(
			bgx1,
			bgy1,
			bgx2,
			bgy2,
			this->computed_background->background_color
		);

	int times_x = 1, times_y = 1;
	int start_x = brx, start_y = bry;

	if (!this->computed_background->background_images.empty())
	{
		if (this->computed_background->background_repeat_x)
		{
			int w = this->computed_background->background_size_width;
			start_x = brx - std::ceil(float(brx - bgx1) / w) * w;
			times_x = std::ceil(float(bgx2 - start_x) / w);
		}

		if (this->computed_background->background_repeat_y)
		{
			int h = this->computed_background->background_size_height;
			start_y = bry - std::ceil(float(bry - bgy1) / h) * h;
			times_y = std::ceil(float(bgy2 - start_y) / h);
		}
	}

	int decrease_x = 0;
	int decrease_y = 0;
	if (this->parent)
	{
		if (this->parent->computed->scrollable_x)
			decrease_x = this->parent->computed->scroll_x;
		if (this->parent->computed->scrollable_y)
			decrease_y = this->parent->computed->scroll_y;
	}

	int bg_img_clip_x, bg_img_clip_y, bg_img_clip_w, bg_img_clip_h;
	al_get_clipping_rectangle(&bg_img_clip_x, &bg_img_clip_y, &bg_img_clip_w, &bg_img_clip_h);

	al_set_clipping_rectangle(bgx1, bgy1, bgx2 - bgx1, bgy2 - bgy1);

	for (auto bitmap : this->computed_background->background_images)
	{
		int y = start_y - decrease_y;
		for (int ty = 0; ty < times_y; ty++)
		{
			int x = start_x - decrease_x;
			for (int tx = 0; tx < times_x; tx++)
			{
				al_draw_scaled_bitmap(
					bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap),
					x, y, this->computed_background->background_size_width, this->computed_background->background_size_height, 0
				);
				x += this->computed_background->background_size_width;
			}
			y += this->computed_background->background_size_height;
		}
	}

	al_set_clipping_rectangle(bg_img_clip_x, bg_img_clip_y, bg_img_clip_w, bg_img_clip_h);

	// Top border
	if (this->computed->border_style_top == "solid")
	{
		al_draw_filled_rectangle(
			bbx,
			bby,
			bbx2,
			bby + tb,
			this->computed->border_color_top
		);
	}
	else if (this->computed->border_style_top == "double")
	{
		al_draw_line(
			bbx,
			bby,
			bbx2,
			bby,
			this->computed->border_color_top,
			1
		);
		al_draw_line(
			bbx,
			bby + tb,
			bbx2,
			bby + tb,
			this->computed->border_color_top,
			1
		);
	}

	// Bottom border
	if (this->computed->border_style_bottom == "solid")
	{
		al_draw_filled_rectangle(
			bbx,
			bby2 - bb,
			bbx2,
			bby2,
			this->computed->border_color_bottom
		);
	}
	else if (this->computed->border_style_bottom == "double")
	{
		al_draw_line(
			bbx,
			bby2 - bb,
			bbx2,
			bby2 - bb,
			this->computed->border_color_bottom,
			1
		);
		al_draw_line(
			bbx,
			bby2,
			bbx2,
			bby2,
			this->computed->border_color_bottom,
			1
		);
	}

	// Left border
	if (this->computed->border_style_left == "solid")
	{
		al_draw_filled_rectangle(
			bbx,
			bby,
			bbx + lb,
			bby2,
			this->computed->border_color_left
		);
	}
	else if (this->computed->border_style_left == "double")
	{
		al_draw_line(
			bbx,
			bby,
			bbx,
			bby2,
			this->computed->border_color_left,
			1
		);
		al_draw_line(
			bbx + lb,
			bby,
			bbx + lb,
			bby2,
			this->computed->border_color_left,
			1
		);
	}

	// Right border
	if (this->computed->border_style_right == "solid")
	{
		al_draw_filled_rectangle(
			bbx2 - rb,
			bby,
			x + this->computed->x + this->computed->margin_left + this->get_width("cpb"),
			bby2,
			this->computed->border_color_right
		);
	}
	else if (this->computed->border_style_right == "double")
	{
		al_draw_line(
			bbx2 - rb,
			bby,
			bbx2 - rb,
			bby2,
			this->computed->border_color_right,
			1
		);
		al_draw_line(
			bbx2,
			bby,
			bbx2,
			bby2,
			this->computed->border_color_right,
			1
		);
	}

	if (this->computed->hide_overflow)
	{
		int overflow_clip_x, overflow_clip_y, overflow_clip_w, overflow_clip_h;
		int target_x = x + this->computed->x + this->computed->margin_left + this->computed->border_width_left,
			target_y = y + this->computed->y + this->computed->margin_top + this->computed->border_width_top,
			target_w = this->get_width("cp"),
			target_h = this->get_height("cp");

		al_get_clipping_rectangle(&overflow_clip_x, &overflow_clip_y, &overflow_clip_w, &overflow_clip_h);

		al_set_clipping_rectangle(target_x, target_y, target_w, target_h);

		for (auto child : this->children)
			child->draw(
				x + this->computed->x + this->computed->margin_left + this->computed->padding_left
				+ this->computed->border_width_left - decrease_x,
				y + this->computed->y + this->computed->margin_top + this->computed->padding_top
				+ this->computed->border_width_top - decrease_y
			);

		al_set_clipping_rectangle(overflow_clip_x, overflow_clip_y, overflow_clip_w, overflow_clip_h);
	}
	else
	{
		for (auto child : this->children)
			child->draw(
				x + this->computed->x + this->computed->margin_left + this->computed->padding_left
				+ this->computed->border_width_left - decrease_x,
				y + this->computed->y + this->computed->margin_top + this->computed->padding_top
				+ this->computed->border_width_top - decrease_y
			);
	}
}

void DOM_element::update(double dt, int x, int y, bool force_fail)
{
	// To see if we've just lost the hover
	bool recorded_state = this->hover;

	// Case 3 - no forced fail and hover
	if (!force_fail && is_element_hovered(x, y, this))
	{
		this->hover = true;

		// Hover gained
		if (!recorded_state && this->hover_gain_event)
			this->document->element_related_events.push_back({
				.type = DOM_EVENT_ELEMENT_HOVER_GAINED,
				.x = this->document->mouse_state.x,	
				.y = this->document->mouse_state.y,
				.element = this
			});
		

		// We update children since they can have hover
		for (auto child : this->children)
			child->update(
				dt,
				x + this->computed->x + this->computed->margin_left + this->computed->padding_left + this->computed->border_width_left,
				y + this->computed->y + this->computed->margin_top + this->computed->padding_top + this->computed->border_width_top
			);
	}
	// no hover
	else
	{
		this->hover = false;

		// Case 2 and no forced failure
		if (!this->computed->hide_overflow && !force_fail) 
		{

			// We update all children. If one of them has hover, the parent should immediately have hover as well
			for (auto child : this->children)
			{
				child->update(
					dt,
					x + this->computed->x + this->computed->margin_left + this->computed->padding_left + this->computed->border_width_left,
					y + this->computed->y + this->computed->margin_top + this->computed->padding_top + this->computed->border_width_top
				);

				this->hover |= child->hover;
			}
		}
		// Case 1a - the hover loss is fresh
		else if (recorded_state)
		{
			// Hover lost event
			if (this->hover_loss_event)
				this->document->element_related_events.push_back({
					.type = DOM_EVENT_ELEMENT_HOVER_LOST,
					.x = this->document->mouse_state.x,
					.y = this->document->mouse_state.y,
					.element = this
				});

			// We update children with forcing failure
			for (auto child : this->children)
				child->update(
					dt,
					x + this->computed->x + this->computed->margin_left + this->computed->padding_left + this->computed->border_width_left,
					y + this->computed->y + this->computed->margin_top + this->computed->padding_top + this->computed->border_width_top,
					true
				);
		}
	}

	// Case 1b - no hover and overflow is hidden
}

void DOM_element::on(char event, std::function<void(DOM_event e)> fun)
{
	if (this->document)
	{
		this->document->event_listeners[event].insert(this);
		this->event_functions[event] = fun;

		if (event == DOM_EVENT_ELEMENT_HOVER_GAINED) this->hover_gain_event = true;
		if (event == DOM_EVENT_ELEMENT_HOVER_LOST) this->hover_loss_event = true;
	}
}

void DOM_element::dispatch_scroll_event(DOM_event e)
{
	this->computed->scroll_x = e.dx * this->computed->scrollable_x;
	this->computed->scroll_y = e.dy * this->computed->scrollable_y;
}

void DOM_quit()
{
	for (auto iter = DOM_loaded_images.begin(); iter != DOM_loaded_images.end(); iter++)
	{
		al_destroy_bitmap(iter->second);
	}
}
