#include "Display.h"

namespace art
{
	Display::Display(
		int _width, int _height, std::string title, int display_flags, agl::Image* icon
	)
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
			}
			while (!(info.x1 == 0 && info.y1 == 0));

			_width = info.x2 - info.x1;
			_height = info.y2 - info.y2;
		}

		display = 0;
		display = al_create_display(_width, _height);

		width = al_get_display_width(display);
		height = al_get_display_height(display);

		if (icon && icon->bitmap)

			al_set_display_icon(display, icon->bitmap);
		al_set_window_title(display, title.c_str());
	}

	Display::~Display()
	{
		al_destroy_display(display);
	}

	ALLEGRO_DISPLAY* Display::get_al_display() const
	{
		return display;
	}

	int Display::get_width() const
	{
		return width;
	}

	int Display::get_height() const
	{
		return height;
	}

}