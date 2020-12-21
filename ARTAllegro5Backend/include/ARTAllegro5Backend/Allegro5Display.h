#pragma once

#include <art/Display.h>

#include "allegro5/allegro.h"

namespace art
{
	class Allegro5Display : public Display
	{
	private:
		ALLEGRO_DISPLAY* display;

	public:
		Allegro5Display(
			int w, int h, const std::string& title, int display_flags = 0,
			agl::Image* icon = 0
		);
		~Allegro5Display();

		ALLEGRO_DISPLAY* get_al_display() const;

	};
}