#include "Base.h"

namespace agl
{

	std::map<std::string, Font*> loaded_fonts = { { "default", NULL } };

	Point::Point(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	Point Point::operator+(const Point& pt)
	{
		return Point(x + pt.x, y + pt.y);
	}

	Point& Point::operator+=(const Point& pt)
	{
		x += pt.x;
		y += pt.y;
		return *this;
	}

	Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a)
	{
		calculated = al_map_rgba(r, g, b, a);
	}

	void Style::apply(Style* style)
	{
		for (const auto& [key, value] : style->values)
		{
			if (value.source)
			{
				this->values[key] = value;
			}
		}
	}

	void Style::set_value(std::string key, std::variant<int, bool, Color> value)
	{
		values[key].value = value;
		values[key].source = this;
	}

	Style::Style(std::string style_name)
	{
		name = style_name;

		Color def(0, 0, 0, 0);

		values = {
			{"margin_left", 0},
			{"margin_right", 0},
			{"margin_top", 0},
			{"margin_bottom", 0},

			{"border_left", 0},
			{"border_right", 0},
			{"border_top", 0},
			{"border_bottom", 0},

			{"border_color_left", def},
			{"border_color_right", def},
			{"border_color_top", def},
			{"border_color_bottom", def},

			{"padding_left", 0},
			{"padding_right", 0},
			{"padding_top", 0},
			{"padding_bottom", 0},

			{"width", 0},
			{"height", 0},

			{"background_color", Color()},

			{"sizing", 0},

			// Flow
			{"main_axis", 0},
			{"main_axis_spacing", 0},
			{"second_axis_spacing", 0},
			{"main_axis_align", 0},
			{"second_axis_align", 0},
			{"single", false},

			// Scrollbar
			{"axis", 0},
			{"marker", 0},

			// Button
			{"background_color_click", def},
			{"background_color_hover", def},

			// ImageBlock
			{"image_scaling", 0},
		};
	}
	
	value_with_source::value_with_source(int value)
	{
		this->source = NULL;
		this->value = value;
	}

	value_with_source::value_with_source(bool value)
	{
		this->source = NULL;
		this->value = value;
	}

	value_with_source::value_with_source(Color value)
	{
		this->source = NULL;
		this->value = value;
	}

	Font::Font(std::string font_path, std::set<int> sizes_to_load)
	{
		path = font_path;
		for (int size : sizes_to_load)
			if (!load(size))
				throw std::runtime_error(("Could not load font \"" + path + "\"").c_str());
	}

	ALLEGRO_FONT* Font::get(int size) const
	{
		return sizes.at(size);
	}

	bool Font::load(int size)
	{
		if (sizes.find(size) == sizes.end())
		{
			sizes.insert({ size, al_load_ttf_font(path.c_str(), size, 0) });
			return sizes[size] != NULL;
		}

		return 1;
	}

	int Font::get_height(int size) const
	{
		return al_get_font_line_height(get(size));
	}

	int Font::get_width(int size, std::string text) const
	{
		return al_get_text_width(get(size), text.c_str());
	}


	void set_default_font(Font* default_font)
	{
		loaded_fonts["default"] = default_font;
		if (default_font->sizes.find(debug::font_size) == default_font->sizes.end())
			default_font->load(debug::font_size);
	}

	void register_font(std::string name, Font* new_font)
	{
		if (loaded_fonts.find(name) == loaded_fonts.end())
			loaded_fonts.insert({ name, new_font });
	}

	namespace debug
	{
		Color highlight;
		Color background_color;
		Color default_color;
		Color hr_color;
		Color stylename_color;
		Color int_color;
		Color flag_color;
		Color bool_color;
		bool debug = false;

		void init()
		{
			highlight = Color(255, 255, 255, 128);
			background_color = Color(30, 30, 30);
			default_color = Color(255, 255, 255);
			hr_color = Color(100, 100, 100);
			stylename_color = Color(250, 250, 100);
			int_color = Color(50, 230, 150);
			flag_color = Color(200, 78, 200);
			bool_color = Color(0, 20, 100);
		}
	}

	Image::Image(ALLEGRO_BITMAP* _bitmap)
	{
		bitmap = _bitmap;
		maintain_bitmap = false;
		width = al_get_bitmap_width(bitmap);
		height = al_get_bitmap_height(bitmap);
	}

	Image::Image(std::string path)
	{
		bitmap = al_load_bitmap(path.c_str());
		maintain_bitmap = true;
		width = al_get_bitmap_width(bitmap);
		height = al_get_bitmap_height(bitmap);
	}

	Image::Image(std::filesystem::path path)
	{
		bitmap = al_load_bitmap(path.string().c_str());
		maintain_bitmap = true;
		width = al_get_bitmap_width(bitmap);
		height = al_get_bitmap_height(bitmap);
	}

	Image::~Image()
	{
		if (maintain_bitmap)
		{
			al_destroy_bitmap(bitmap);
			bitmap = NULL;
		}
	}

	void Image::set_bitmap_maintain(bool maintain)
	{
		maintain_bitmap = maintain;
	}

}