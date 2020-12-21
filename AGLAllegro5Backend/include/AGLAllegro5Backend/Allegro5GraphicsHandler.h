#pragma once

#include <agl/GraphicsHandler.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

#include <string>
#include <set>
#include <map>

namespace agl
{
	struct Allegro5Image : public Image
	{
		ALLEGRO_BITMAP* bitmap;
		bool maintain_bitmap;

		Allegro5Image(ALLEGRO_BITMAP* bitmap_);
		Allegro5Image(std::string path);
		~Allegro5Image();

		void set_bitmap_maintain(bool maintain);
	};

	struct Allegro5Shader : public Shader
	{
		ALLEGRO_SHADER* shader = 0;

		Allegro5Shader(const std::string& vertex_shader_path, const std::string& fragment_shader_path);
		~Allegro5Shader();
	};

	struct Allegro5Font : public Font
	{
		std::map<int, const ALLEGRO_FONT*> loaded;

		Allegro5Font(const std::string& path, std::set<int> sizes);

		const ALLEGRO_FONT* get(int size) const;

		int get_height(int size) const;
		int get_width(int size, const std::string& text) const;
		bool load(int size);
	};

	class Allegro5GraphicsHandler : public GraphicsHandler
	{
	private:
		ALLEGRO_COLOR get_color(const Color& color) const;

	public:
		Allegro5GraphicsHandler();

		void draw_line(const Point& pt1, const Point& pt2, int thickness, const Color& color) const;

		void draw_filled_rectangle(const Rectangle& rect, const Color& color) const;

		void draw_filled_triangle(const Point& pt1, const Point& pt2, const Point& pt3, const Color& color) const;

		void draw_text(const Point& pt, const std::string& text, const Font* font, int size, const Color& color) const;

		void draw_scaled_image_target(
			const Point& pt, const Image* image, const Point& offset, int target_width, int target_height
		) const;

		void use_shader(const Shader* shader) const;

		void set_clipping_rectangle(const Rectangle& rect);

		void pop_clipping_rectangle();
	};
}