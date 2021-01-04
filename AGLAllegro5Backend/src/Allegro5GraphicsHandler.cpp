#include "..\include\AGLAllegro5Backend\Allegro5GraphicsHandler.h"
#include <stdexcept>

namespace agl
{
	Allegro5Image::Allegro5Image(ALLEGRO_BITMAP* _bitmap)
	{
		bitmap = _bitmap;
		maintain_bitmap = false;
		width = al_get_bitmap_width(bitmap);
		height = al_get_bitmap_height(bitmap);
	}

	Allegro5Image::Allegro5Image(std::string path)
	{
		bitmap = al_load_bitmap(path.c_str());
		maintain_bitmap = true;
		width = al_get_bitmap_width(bitmap);
		height = al_get_bitmap_height(bitmap);
	}

	Allegro5Image::~Allegro5Image()
	{
		if (maintain_bitmap)
		{
			al_destroy_bitmap(bitmap);
			bitmap = NULL;
		}
	}

	void Allegro5Image::set_bitmap_maintain(bool maintain)
	{
		maintain_bitmap = maintain;
	}

	Allegro5Shader::Allegro5Shader(const std::string& vertex_shader_path, const std::string& fragment_shader_path)
	{
		shader = al_create_shader(ALLEGRO_SHADER_GLSL);

		if (!al_attach_shader_source_file(
			shader, ALLEGRO_VERTEX_SHADER, vertex_shader_path.c_str())
			)
			throw std::runtime_error(al_get_shader_log(shader));

		if (!al_attach_shader_source_file(
			shader, ALLEGRO_PIXEL_SHADER, fragment_shader_path.c_str())
			)
			throw std::runtime_error(al_get_shader_log(shader));

		if (!al_build_shader(shader))
			throw std::runtime_error(al_get_shader_log(shader));
	}

	Allegro5Shader::~Allegro5Shader()
	{
		al_destroy_shader(shader);
	}

	Allegro5Font::Allegro5Font(const std::string& path, std::set<int> sizes)
		: Font(path)
	{
		bool success = true;

		for (const auto& size : sizes)
		{
			success &= load(size);
		}

		if (!success)
			throw std::runtime_error("Failed to load font on path \"" + path + "\".");
	}

	const ALLEGRO_FONT* Allegro5Font::get(int size) const
	{
		return loaded.at(size);
	}

	int Allegro5Font::get_height(int size) const
	{
		return al_get_font_line_height(get(size));
	}

	int Allegro5Font::get_width(int size, const std::string& text) const
	{
		return al_get_text_width(get(size), text.c_str());
	}

	bool Allegro5Font::load(int size)
	{
		if (loaded.find(size) == loaded.end())
		{
			ALLEGRO_FONT* new_font = al_load_ttf_font(path.c_str(), size, 0);

			if (new_font)
			{
				loaded.insert({ size, new_font });
				return true;
			}
		}

		return false;
	}

	ALLEGRO_COLOR Allegro5GraphicsHandler::get_color(const Color& color) const
	{
		return al_map_rgba(color.r, color.g, color.b, color.a);
	}

	Allegro5GraphicsHandler::Allegro5GraphicsHandler()
	{
		int cx, cy, cw, ch;
		al_get_clipping_rectangle(&cx, &cy, &cw, &ch);
		set_clipping_rectangle({ (float)cx, (float)cy, (float)cw, (float)ch });
	}

	void Allegro5GraphicsHandler::draw_line(const Point& pt1, const Point& pt2, int thickness, const Color& color) const
	{
		al_draw_line(pt1.x, pt1.y, pt2.x, pt2.y, get_color(color), (float)thickness);
	}

	void Allegro5GraphicsHandler::draw_rectangle(const Rectangle& rect, int thickness, const Color& color) const
	{
		al_draw_rectangle(rect.pt.x, rect.pt.y, rect.pt.x + rect.width, rect.pt.y + rect.height, get_color(color), thickness);
	}

	void Allegro5GraphicsHandler::draw_filled_rectangle(const Rectangle& rect, const Color& color) const
	{
		al_draw_filled_rectangle(rect.pt.x, rect.pt.y, rect.pt.x + rect.width, rect.pt.y + rect.height, get_color(color));
	}

	void Allegro5GraphicsHandler::draw_filled_triangle(const Point& pt1, const Point& pt2, const Point& pt3, const Color& color) const
	{
		al_draw_filled_triangle(pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y, get_color(color));
	}

	void Allegro5GraphicsHandler::draw_text(const Point& pt, const std::string& text, const Font* font, int size, const Color& color) const
	{
		al_draw_text(((Allegro5Font*)font)->get(size), get_color(color), pt.x, pt.y, 0, text.c_str());
	}

	void Allegro5GraphicsHandler::draw_scaled_image_target(const Point& pt, const Image* image, const Point& offset, int target_width, int target_height) const
	{
		al_draw_scaled_bitmap(((Allegro5Image*)image)->bitmap, offset.x, offset.y, image->width, image->height, pt.x, pt.y, target_width, target_height, 0);
	}

	void Allegro5GraphicsHandler::draw_scaled_tinted_image_target(const Point& pt, const Image* image, const Point& offset, int target_width, int target_height, const Color& tint) const
	{
		al_draw_tinted_scaled_bitmap(((Allegro5Image*)image)->bitmap, get_color(tint), offset.x, offset.y, image->width, image->height, pt.x, pt.y, target_width, target_height, 0);
	}

	void Allegro5GraphicsHandler::use_shader(const Shader* shader) const
	{
		if (shader)
			al_use_shader(((Allegro5Shader*)shader)->shader);
		else
			al_use_shader(nullptr);
	}

	void Allegro5GraphicsHandler::set_clipping_rectangle(const Rectangle& rect)
	{
		GraphicsHandler::set_clipping_rectangle(rect);

		al_set_clipping_rectangle(rect.pt.x, rect.pt.y, rect.width, rect.height);
	}

	void Allegro5GraphicsHandler::pop_clipping_rectangle()
	{
		GraphicsHandler::pop_clipping_rectangle();

		Rectangle clip = get_clipping_rectangle();

		al_set_clipping_rectangle(clip.pt.x, clip.pt.y, clip.width, clip.height);
	}

}