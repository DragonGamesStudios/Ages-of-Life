#include "..\include\art\Atlas.h"

namespace art
{
	Atlas::Atlas()
	{
		image = 0;
	}

	Atlas::~Atlas()
	{
		delete image;
	}

	void Atlas::set_image(const ArtImage* image_)
	{
		image = image_;
	}

}