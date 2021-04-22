#include "agl/builtins/ImageBlock.h"

namespace agl::builtins
{
	ImageBlock::ImageBlock()
	{
		desired_width = 0;
		desired_height = 0;
		scaling = AGL_SCALING_NONE;
		image = 0;
		display_box = AGL_SIZING_BORDERBOX;
		shader = NULL;
	}

	void ImageBlock::set_image(const Image* _image)
	{
		image = _image;
		desired_width = image->width;
		desired_height = image->height;
		set_scaling(scaling);
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
			desired_width = image->width * (int)x;
			desired_height = image->height * (int)y;
		}
	}

	void ImageBlock::set_scaling(char _scaling)
	{
		scaling = _scaling;

		if (image)
		{
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
						desired_height = int(((float)image->width / maxwidth) * maxheight);
					}
					else
					{
						desired_height = maxheight;
						desired_width = int(((float)image->height / maxheight) * maxwidth);
					}
					break;

				case AGL_SCALING_FIT:
					if ((float)image->width / maxwidth > (float)image->height / maxheight)
					{
						desired_width = maxwidth;
						desired_height = int(((float)image->width / maxwidth) * maxheight);
					}
					else
					{
						desired_height = maxheight;
						desired_width = int(((float)image->height / maxheight) * maxwidth);
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

	void ImageBlock::set_tint(const Color& color)
	{
		tint = color;
	}

	void ImageBlock::set_offset(float x, float y)
	{
		set_offset(Point(x, y));
	}

	void ImageBlock::set_offset(const Point& offset)
	{
		image_offset = offset;
	}

	void ImageBlock::draw_background(const Point& base_location)
	{
		Block::draw_background(base_location);

		if (image)
		{
			graphics_handler->use_shader(shader);
			
			if (shader_setup_fn) shader_setup_fn();

			Point display_point = base_location + image_offset;

			switch (display_box)
			{

			case AGL_SIZING_CONTENTBOX:
				display_point += Point((float)box.padding.left, (float)box.padding.top);
				[[fallthrough]];

			case AGL_SIZING_PADDINGBOX:
				display_point += Point((float)box.border.left, (float)box.border.top);
				[[fallthrough]];

			case AGL_SIZING_MARGINBOX:
				display_point += Point(-(float)box.margin.left, -(float)box.margin.top);
				break;

			default:
				break;
			}

			graphics_handler->draw_scaled_tinted_image_target(display_point, image, { 0, 0 }, desired_width, desired_height, tint);

			graphics_handler->use_shader(0);
		}
	}

	void ImageBlock::set_size(int width, int height)
	{
		Block::set_size(width, height);

		set_scaling(scaling);
	}

	void ImageBlock::apply(const Style* _style)
	{
		Block::apply(_style);

		if (style->values.at("image_scaling").source)
			set_scaling((char)std::get<int>(style->values.at("image_scaling").value));
	}

	void ImageBlock::set_shader(const Shader* _shader)
	{
		shader = _shader;
	}

	void ImageBlock::set_shader_setup(std::function<void()> fn)
	{
		shader_setup_fn = fn;
	}

}