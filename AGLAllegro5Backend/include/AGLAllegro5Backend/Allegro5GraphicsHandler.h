#pragma once

#include <agl/GraphicsHandler.h>

#include <allegro5/allegro.h>

#include <string>

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

	class Allegro5GraphicsHandler : public GraphicsHandler
	{
	};
}