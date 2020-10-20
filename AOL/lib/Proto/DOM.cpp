#include "Proto.h"

json DOM_default_ruleset_json = {
	{"background-color", "rgba(0, 0, 0, 0)"},
	{"width", "auto"},
	{"height", "auto"}
};

DOM_ruleset DOM_default_ruleset = DOM_default_ruleset_json;

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
	add_element_with_children(root);

	this->base = new DOM_element;
	this->base->add_child(root);

	this->base->set_rulesets({ json{
		{"backgrund-color", "white"},
		{"width", std::to_string(width)+"px"},
		{"height", std::to_string(height) + "px"}
	} });
}

void DOM_document::draw(int x, int y)
{
	this->root->draw(x, y);
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
	for (auto child : element->children)
	{
		add_element_with_children(child);
	}
}

void DOM_element::set_rulesets(std::vector<DOM_ruleset> rulesets)
{
	this->initialized = true;

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
		this->computed.auto_width = false;
	}*/
	for (auto rule : result.info.items())
	{
		std::string val = rule.value();
		if (rule.value() == "initial")
			val = DOM_default_ruleset.get_rule(rule.key());
		this->parsed_ruleset.insert({ rule.key(), parse_expression(rule.value()) });
	}

	// Background color
	auto currently_parsed = parse_value(this->parsed_ruleset["background-color"][0][0]);

	unsigned char fixed_color[4] = {0, 0, 0, 0};

	if (currently_parsed.second == "inherit")
	{
		this->dependent.background_color = true;
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
					color[i].push_back(currently_parsed.second[i*2+1]);
					color[i].push_back(currently_parsed.second[(i+1)*2]);
				}
			}
			else if (currently_parsed.second.length() == 4)
			{
				for (int i = 0; i < 3; i++)
					color[i] = std::string(2, currently_parsed.second[i+1]);
			}
			else
			{
				throw std::runtime_error("Bad hex color value length!");
			}

			for (int i = 0; i < 3; i++)
				fixed_color[i] = std::stoul("0x" + color[i], nullptr, 16);

			this->computed.background_color = al_map_rgba(fixed_color[0], fixed_color[1], fixed_color[2], fixed_color[3]);
		}
		else
		{
			if (predefined_colors.find(currently_parsed.second) != predefined_colors.end())
			{
				this->computed.background_color = predefined_colors[currently_parsed.second];
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

		this->computed.background_color = al_map_rgba(fixed_color[0], fixed_color[1], fixed_color[2], fixed_color[3]);
	}

	// Width
	currently_parsed = parse_value(this->parsed_ruleset["width"][0][0]);

	if (currently_parsed.first.empty())
	{
		auto width_pair = divide_value_unit(currently_parsed.second);
		if (unit_to_pixel.find(width_pair.second) != unit_to_pixel.end())
			this->computed.width = std::stoi(width_pair.first) * unit_to_pixel.at(width_pair.second);
		else
		{
			this->dependent.width = true;
			this->dependent.width_unit = width_pair.second;
			if (!width_pair.first.empty())
				this->dependent.width_value = std::stof(width_pair.first) / 100;
		}
	}

	// Height
	currently_parsed = parse_value(this->parsed_ruleset["height"][0][0]);

	if (currently_parsed.first.empty())
	{
		auto height_pair = divide_value_unit(currently_parsed.second);
		if (unit_to_pixel.find(height_pair.second) != unit_to_pixel.end())
			this->computed.height = std::stoi(height_pair.first) * unit_to_pixel.at(height_pair.second);
		else
		{
			this->dependent.height = true;
			this->dependent.height_unit = height_pair.second;
			if (!height_pair.first.empty())
				this->dependent.height_value = std::stof(height_pair.first) / 100;
		}
	}
}

void DOM_element::add_child(DOM_element* child)
{
	this->children.push_back(child);
}

void DOM_element::calculate()
{

	if (this->parent)
	{
		if (this->dependent.background_color)
			this->computed.background_color = this->parent->computed.background_color;

		if (this->dependent.width)
		{
			if (this->dependent.width_unit == "%")
				this->computed.width = this->dependent.width_value * this->parent->computed.width;
			else if (this->dependent.width_unit == "auto")
				this->computed.width = this->parent->computed.width;
			else if (this->dependent.width_unit == "vw")
				this->computed.width = this->dependent.width_value * this->document_base->computed.width;
			else if (this->dependent.width_unit == "vh")
				this->computed.width = this->dependent.width_value * this->document_base->computed.height;
		}

		if (this->dependent.height)
		{
			if (this->dependent.height_unit == "%")
				this->computed.height = this->dependent.height_value * this->parent->computed.height;
			// auto value in height is not dependent value
			/*else if (this->dependent.height_unit == "auto")
				this->computed.height = this->parent->computed.height;*/
			else if (this->dependent.height_unit == "vw")
				this->computed.height = this->dependent.height_value * this->document_base->computed.width;
			else if (this->dependent.height_unit == "vh")
				this->computed.height = this->dependent.height_value * this->document_base->computed.height;
		}
	}

	for (auto child : this->children)
	{
		child->parent = this;
		// Recursively calculate children
		child->calculate();

		// Set basic coordinates
		child->computed.x = 0;
		child->computed.y = this->computed.height;

		// Update dimensions
		if (this->dependent.height_unit == "auto")
		{
			this->computed.height += child->computed.height;
		}
	}
}

void DOM_element::draw(int x, int y)
{
}
