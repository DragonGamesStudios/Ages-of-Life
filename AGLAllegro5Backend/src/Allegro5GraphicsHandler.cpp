#include "..\include\AGLAllegro5Backend\Allegro5GraphicsHandler.h"

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

}