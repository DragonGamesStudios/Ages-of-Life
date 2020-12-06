#pragma once
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <string>
#include <filesystem>
#include <utility>
#include <variant>
#include <map>
#include <set>

namespace agl
{

	class Block;

	struct Point
	{
		float x = 0;
		float y = 0;

		Point(float x = 0, float y = 0);

		Point operator+(const Point& pt);
		Point& operator+=(const Point& pt);
	};

	struct Color
	{
		unsigned char r = 255;
		unsigned char g = 255;
		unsigned char b = 255;
		unsigned char a = 255;
		ALLEGRO_COLOR calculated;

		Color(unsigned char r = 255, unsigned char g = 255, unsigned char b = 255, unsigned char a = 255);
	};

	struct BoxValues
	{
		int top = 0;
		int right = 0;
		int bottom = 0;
		int left = 0;
	};

	struct BoxColors
	{
		Color top;
		Color right;
		Color bottom;
		Color left;
	};

	struct Box
	{
		int inner_width = 0;
		int inner_height = 0;
		BoxValues margin;
		BoxValues border;
		BoxValues padding;
		BoxColors border_colors;
	};

	struct Image
	{
		ALLEGRO_BITMAP* bitmap;
		int width;
		int height;
		bool maintain_bitmap;

		Image(ALLEGRO_BITMAP* _bitmap);
		Image(std::string path);
		Image(std::filesystem::path path);
		~Image();

		void set_bitmap_maintain(bool maintain);
	};

	class Style;

	struct value_with_source
	{
		Style* source = NULL;
		std::variant<int, bool, Color> value = 0;

		value_with_source(int value = 0);
		value_with_source(bool value);
		value_with_source(Color value);
	};

	class Style
	{
	public:
		std::map<std::string, value_with_source> values;
		std::string name;

		void apply(Style* style);
		void set_value(std::string key, std::variant<int, bool, Color> value);
		Style(std::string style_name);
	};

	struct Event
	{
		int type;
		int x = 0;
		int y = 0;
		int z = 0;
		int w = 0;
		int dx = 0;
		int dy = 0;
		int dz = 0;
		int dw = 0;
		int button = 0;
		int buttons = 0;
		
		char unicode = 0;
		int keycode = 0;
		char input = 0;

		Block* source = 0;
		Block* block = 0;
	};

	struct Font
	{
		std::string path;
		std::map<int, ALLEGRO_FONT*> sizes;
		
		Font(std::string font_path, std::set<int> sizes_to_load);
		ALLEGRO_FONT* get(int size) const;
		bool load(int size);
		int get_height(int size) const;
		int get_width(int size, std::string text) const;
	};

	void set_default_font(Font* default_font);
	void register_font(std::string name, Font* new_font);

	extern std::map<std::string, Font*> loaded_fonts;

	struct TextChunk
	{
		bool type = 0;
		int x = 0;
		int y = 0;

		Image* image = 0;
		int desired_width = 0;
		int desired_height = 0;

		Font* font = 0;
		std::string text;
		Color color;
		int size = 0;
	};

	namespace debug
	{
		extern Color highlight;
		extern Color background_color;
		extern Color default_color;
		extern Color hr_color;
		extern Color stylename_color;
		extern Color int_color;
		extern Color flag_color;
		extern Color bool_color;
		extern bool debug;
		const int font_size = 14;
		void init();
	}
}