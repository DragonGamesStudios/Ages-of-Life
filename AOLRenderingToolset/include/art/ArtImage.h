#pragma once
#include "Color.h"

namespace art
{
	struct DrawData
	{
		float dx = 0;
		float dy = 0;
		float sx = 1;
		float sy = 1;
		float r = 0;
		float ox = 0;
		float oy = 0;
		Color tint;
	};

	struct ArtImage
	{
		const ArtImage* source;

		int x;
		int y;

		int width;
		int height;

		ArtImage(const ArtImage* reg_source, int reg_x, int reg_y, int reg_w, int reg_h);
		ArtImage(int w = 0, int h = 0);
		virtual ~ArtImage() = default;

		virtual void draw(const DrawData& d = {}) = 0;
	};
}