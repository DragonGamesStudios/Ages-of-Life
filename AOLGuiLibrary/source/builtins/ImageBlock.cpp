#include "builtins/ImageBlock.h"

namespace agl::builtins
{
	ImageBlock::ImageBlock()
	{
		desired_width = 0;
		desired_height = 0;
		scaling = AGL_SCALING_NONE;
		image = 0;
		display_box = AGL_SIZING_BORDERBOX;
	}

	void ImageBlock::set_image(Image* _image)
	{
		image = _image;
		desired_width = image->width;
		desired_height = image->height;
	}

	void ImageBlock::set_desired_width(int _width)
	{
		desired_width = _width;
	}

	void ImageBlock::set_desired_height(int _height)
	{
		desired_height = _height;
	}

	void ImageBlock::set_scaling(float x, float y)
	{
		if (image)
		{
			desired_width = image->width * x;
			desired_height = image->height * y;
		}
	}

	void ImageBlock::set_scaling(char _scaling)
	{
		if (image)
		{
			scaling = _scaling;

			if (scaling)
			{
				int maxwidth = get_box_width(display_box);
				int maxheight = get_box_height(display_box);

				switch (scaling)
				{
				case AGL_SCALING_COVER:
					if ((float)image->width / maxwidth < (float)image->height / maxheight)
					{
						desired_width = maxwidth;
						desired_height = ((float)image->width / maxwidth) * maxheight;
					}
					else
					{
						desired_height = maxheight;
						desired_width = ((float)image->height / maxheight) * maxwidth;
					}
					break;

				case AGL_SCALING_FIT:
					if ((float)image->width / maxwidth > (float)image->height / maxheight)
					{
						desired_width = maxwidth;
						desired_height = ((float)image->width / maxwidth) * maxheight;
					}
					else
					{
						desired_height = maxheight;
						desired_width = ((float)image->height / maxheight) * maxwidth;
					}
					break;

				case AGL_SCALING_TOSIZE:
					desired_width = maxwidth;
					desired_height = maxheight;
					break;

				default:
					break;
				}
			}
			else
			{
				desired_width = image->width;
				desired_height = image->height;
			}
		}
	}

	void ImageBlock::set_display_box(char _box)
	{
		display_box = _box;

		set_scaling(scaling);
	}

	void ImageBlock::set_offset(int x, int y)
	{
		set_offset(Point(x, y));
	}

	void ImageBlock::set_offset(Point offset)
	{
		image_offset = offset;
	}

	void ImageBlock::draw_background(Point base_location)
	{
		Block::draw_background(base_location);

		if (image)
		{
			Point display_point = base_location + image_offset;

			switch (display_box)
			{

			case AGL_SIZING_CONTENTBOX:
				display_point += Point(box.padding.left, box.padding.top);
				[[fallthrough]];

			case AGL_SIZING_PADDINGBOX:
				display_point += Point(box.border.left, box.border.top);
				[[fallthrough]];

			case AGL_SIZING_MARGINBOX:
				display_point += Point(-box.margin.left, -box.margin.top);
				break;

			default:
				break;
			}

			al_draw_scaled_bitmap(
				image->bitmap,
				0, 0,
				image->width, image->height,
				display_point.x, display_point.y,
				desired_width, desired_height,
				0
			);
		}
	}

	void ImageBlock::set_size(int width, int height)
	{
		Block::set_size(width, height);

		set_scaling(scaling);
	}

	void ImageBlock::apply(Style* _style)
	{
		Block::apply(_style);

		if (style->values["image_scaling"].source)
			set_scaling((char)std::get<int>(style->values["image_scaling"].value));
	}

}