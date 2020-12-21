#pragma once

#include "agl/Block.h"

#define AGL_SCALING_NONE 0
#define AGL_SCALING_COVER 1
#define AGL_SCALING_FIT 2
#define AGL_SCALING_TOSIZE 3

namespace agl::builtins
{
	class ImageBlock : public Block
	{
	private:
		const Image* image;
		int desired_width;
		int desired_height;
		char display_box;
		Point image_offset;
		char scaling;

		const Shader* shader;
		std::function<void()> shader_setup_fn;

	public:
		ImageBlock();

		void set_image(const Image* _image);
		void set_desired_width(int _width);
		void set_desired_height(int _height);
		void set_scaling(float x, float y);
		void set_scaling(char _scaling);
		void set_display_box(char _box);

		void set_offset(float x, float y);
		void set_offset(const Point& offset);
		
		virtual void draw_background(const Point& base_location);
		virtual void set_size(int width, int height);
		virtual void apply(const Style* _style);

		void set_shader(const Shader* _shader);
		void set_shader_setup(std::function<void()> fn);
	};
}