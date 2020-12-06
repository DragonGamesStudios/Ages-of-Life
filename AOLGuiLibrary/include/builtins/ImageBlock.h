#pragma once

#include "Block.h"

#define AGL_SCALING_NONE 0
#define AGL_SCALING_COVER 1
#define AGL_SCALING_FIT 2
#define AGL_SCALING_TOSIZE 3

namespace agl::builtins
{
	class ImageBlock : public Block
	{
	private:
		Image* image;
		int desired_width;
		int desired_height;
		char display_box;
		Point image_offset;
		char scaling;

	public:
		ImageBlock();

		void set_image(Image* _image);
		void set_desired_width(int _width);
		void set_desired_height(int _height);
		void set_scaling(float x, float y);
		void set_scaling(char _scaling);
		void set_display_box(char _box);

		void set_offset(int x, int y);
		void set_offset(Point offset);
		
		virtual void draw_background(Point base_location);
		virtual void set_size(int width, int height);
		virtual void apply(Style* _style);
	};
}