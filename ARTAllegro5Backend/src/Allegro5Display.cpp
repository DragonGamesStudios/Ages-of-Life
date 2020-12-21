#include "..\include\ARTAllegro5Backend\Allegro5Display.h"
#include <AGLAllegro5Backend/Allegro5GraphicsHandler.h>

namespace art
{
	Allegro5Display::Allegro5Display(int w, int h, const std::string& title, int display_flags, agl::Image* icon)
		: Display(w, h)
	{
		if (!display_flags)
			display_flags = ALLEGRO_OPENGL | ALLEGRO_PROGRAMMABLE_PIPELINE;

		al_set_new_display_flags(display_flags);

		if (display_flags & ALLEGRO_FULLSCREEN)
		{
			ALLEGRO_MONITOR_INFO info;
			int i = 0;

			do
			{
				al_get_monitor_info(i++, &info);
			} while (!(info.x1 == 0 && info.y1 == 0));

			w = info.x2 - info.x1;
			h = info.y2 - info.y2;
		}

		display = 0;
		display = al_create_display(w, h);

		width = al_get_display_width(display);
		height = al_get_display_height(display);

		if (icon && ((agl::Allegro5Image*)icon)->bitmap)
			al_set_display_icon(display, ((agl::Allegro5Image*)icon)->bitmap);

		al_set_window_title(display, title.c_str());
	}

	Allegro5Display::~Allegro5Display()
	{
		al_destroy_display(display);
		display = 0;
	}

	ALLEGRO_DISPLAY* Allegro5Display::get_al_display() const
	{
		return display;
	}

}