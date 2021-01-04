#pragma once
#include <stack>
#include <string>

namespace agl
{
	/**
	Point struct. Holds x & y coordinates. Supports addition.
	*/
	struct Point
	{

		float x = 0;
		float y = 0;

		Point(float x = 0, float y = 0);

		Point operator+(const Point& pt) const;
		Point& operator+=(const Point& pt);
	};

	// Color struct. Accepts 0-255 format.
	struct Color
	{
		unsigned char r = 255;
		unsigned char g = 255;
		unsigned char b = 255;
		unsigned char a = 255;

		Color(unsigned char r = 255, unsigned char g = 255, unsigned char b = 255, unsigned char a = 255);
	};

	// Rectangle struct. Defines x, y, width & height.
	struct Rectangle
	{
		Point pt;
		float width;
		float height;

		Rectangle(float x, float y, float w, float h);
		Rectangle(const Point& pt_, float w, float h);
	};

	// Image struct. Defines width and height.
	struct Image
	{
		int width;
		int height;

		virtual ~Image() = default;
	};

	/*
	Abstract Font struct. Needs to implement:

	* int get_height(int size) const;
	* 
	* int get_width(int size, const std::string& text) const;
	* 
	* bool load(int size);
	*/
	struct Font
	{
		std::string path;

		Font(const std::string& font_path);

		virtual int get_height(int size) const = 0;
		virtual int get_width(int size, const std::string& text) const = 0;
		virtual bool load(int size) = 0;
	};

	struct Shader
	{
	};

	/*
	Abstract GraphicsHandler class. Handles drawing. Must implement:

	* void draw_line(const Point& pt1, const Point& pt2, int thickness, const Color& color) const;
	* 
	* void draw_rectangle(const Rectangle& rect, int thickness, const Color& color) const;
	* 
	* void draw_filled_rectangle(const Rectangle& rect, const Color& color) const;
	* 
	* void draw_filled_triangle(const Point& pt1, const Point& pt2, const Point& pt3, const Color& color) const;
	* 
	* void draw_text(const Point& pt, const std::string& text, const Font* font, const int size, const Color& color) const;
	* 
	* draw_scaled_image_target(const Point& pt, const Image* image, const Point& offset, int target_width, int target_height) const;
	* 
	* draw_scaled_tinted_image_target(const Point& pt, const Image* image, const Point& offset, int target_width, int target_height, const Color& tint) const;
	* 
	* use_shader(const Shader* shader) const;
	* 
	* void set_clipping_rectangle(const Rectangle& rect);
	* 
	* void pop_clipping_rectangle();
	*/
	class GraphicsHandler
	{
	protected:
		std::stack<Rectangle> clipping_stack;

	public:
		virtual void draw_line(const Point& pt1, const Point& pt2, int thickness, const Color& color) const = 0;

		virtual void draw_rectangle(const Rectangle& rect, int thickness, const Color& color) const = 0;

		virtual void draw_filled_rectangle(const Rectangle& rect, const Color& color) const = 0;

		virtual void draw_filled_triangle(const Point& pt1, const Point& pt2, const Point& pt3, const Color& color) const = 0;

		virtual void draw_text(const Point& pt, const std::string& text, const Font* font, int size, const Color& color) const = 0;

		virtual void draw_scaled_image_target(
			const Point& pt, const Image* image, const Point& offset, int target_width, int target_height
		) const = 0;

		virtual void draw_scaled_tinted_image_target(
			const Point& pt, const Image* image, const Point& offset, int target_width, int target_height, const Color& tint
		) const = 0;

		virtual void use_shader(const Shader* shader) const = 0;

		virtual void set_clipping_rectangle(const Rectangle& rect);

		Rectangle get_clipping_rectangle() const;

		virtual void pop_clipping_rectangle();

		int get_clipping_stack_size() const;

		void clear_clipping_stack();
	};
}