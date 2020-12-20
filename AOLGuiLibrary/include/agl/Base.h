#pragma once
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "GraphicsHandler.h"

#include <string>
#include <filesystem>
#include <utility>
#include <variant>
#include <map>
#include <set>

namespace agl
{

	class Block;

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

	void register_image(std::string name, Image* new_image);

	extern std::map<std::string, Image*> loaded_images;

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
		int width = 0;
		int height = 0;

		Image* image = 0;

		Font* font = 0;
		std::string text;
		Color color;
		int size = 0;
	};

	struct Shader
	{
		Shader(std::string vertex_shader_source, std::string fragment_shader_source);
		~Shader();

		ALLEGRO_SHADER* shader;
	};

	void register_shader(std::string name, Shader* new_shader);

	extern std::map<std::string, Shader*> loaded_shaders;

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