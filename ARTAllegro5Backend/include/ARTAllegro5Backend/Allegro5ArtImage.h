#pragma once

#include <art/ArtImage.h>

#include <allegro5/allegro.h>

#include <string>

namespace art
{
	struct Allegro5ArtImage : public ArtImage
	{
		ALLEGRO_BITMAP* bitmap;

		Allegro5ArtImage(const std::string& path);
		Allegro5ArtImage(const ArtImage* reg_source, int reg_x, int reg_y, int reg_w, int reg_h);
		~Allegro5ArtImage();

		void draw(const DrawData& d = {});
	};
}