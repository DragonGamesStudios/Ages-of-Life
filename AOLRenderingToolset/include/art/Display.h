#pragma once
#include <agl/AOLGuiLibrary.h>

#include <string>

namespace art
{
	class Display
	{
	protected:
		int width;
		int height;

	public:
		// Flags default = 0 is necessary, because if not, we're all doomed

		Display(int w, int h);

		int get_width() const;
		int get_height() const;
	};
}