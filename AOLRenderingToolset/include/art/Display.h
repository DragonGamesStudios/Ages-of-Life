#pragma once
#include <agl/AOLGuiLibrary.h>

#include <string>

namespace art
{
	class Display
	{
	private:
		ALLEGRO_DISPLAY* display;

		int width;
		int height;

	public:
		// Flags default = 0 is necessary, because if not, we're all doomed


		Display(
			int width, int height, std::string title, int flags = 0,
			agl::Image* icon = 0
		);
		~Display();

		ALLEGRO_DISPLAY* get_al_display() const;
		int get_width() const;
		int get_height() const;
	};
}