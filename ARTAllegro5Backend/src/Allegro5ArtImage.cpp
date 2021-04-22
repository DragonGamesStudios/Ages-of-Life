#include "..\include\ARTAllegro5Backend\Allegro5ArtImage.h"

namespace art
{
	ALLEGRO_COLOR get_color(const Color& color)
	{
		return al_map_rgba(color.r, color.g, color.b, color.a);
	}

	Allegro5ArtImage::Allegro5ArtImage(const std::string& path)
	{
		bitmap = al_load_bitmap(path.c_str());
		width = al_get_bitmap_width(bitmap);
		height = al_get_bitmap_height(bitmap);
		x = y = 0;
		source = 0;
	}

	Allegro5ArtImage::Allegro5ArtImage(ALLEGRO_BITMAP* bmp)
	{
		bitmap = bmp;
		width = al_get_bitmap_width(bmp);
		height = al_get_bitmap_height(bmp);
		x = y = 0;
		source = 0;
	}

	Allegro5ArtImage::Allegro5ArtImage(const ArtImage* reg_source, int reg_x, int reg_y, int reg_w, int reg_h)
		: ArtImage(reg_source, reg_x, reg_y, reg_w, reg_h)
	{
		bitmap = 0;
	}

	Allegro5ArtImage::~Allegro5ArtImage()
	{
		if (!source && bitmap)
		{
			al_destroy_bitmap(bitmap);
			bitmap = 0;
		}
	}

	void Allegro5ArtImage::draw(const DrawData& d)
	{
		if (!source)
		{
			al_draw_tinted_scaled_rotated_bitmap(
				bitmap,
				get_color(d.tint),
				d.ox,
				d.oy,
				d.dx,
				d.dy,
				d.sx, d.sy, d.r, 0
			);
		}
		else
		{
			Allegro5ArtImage* al_source = (Allegro5ArtImage*)source;
			al_draw_tinted_scaled_rotated_bitmap_region(
				al_source->bitmap,
				(float)x,
				(float)y,
				(float)width,
				(float)height,
				get_color(d.tint),
				d.ox,
				d.oy,
				d.dx,
				d.dy,
				d.sx, d.sy, d.r, 0
			);
		}
	}
}