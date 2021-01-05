#include "..\..\AOLRenderingToolset\include\art\ArtImage.h"

namespace art
{
	ArtImage::ArtImage(const ArtImage* reg_source, int reg_x, int reg_y, int reg_w, int reg_h)
	{
		source = reg_source;
		x = reg_x;
		y = reg_y;
		width = reg_w;
		height = reg_h;
	}

	ArtImage::ArtImage(int w, int h)
	{
		width = w;
		height = h;
		x = y = 0;
		source = 0;
	}

}