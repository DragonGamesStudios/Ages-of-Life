#include "../include/art/Display.h"

namespace art
{
	Display::Display(int w, int h)
	{
		width = w;
		height = h;
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