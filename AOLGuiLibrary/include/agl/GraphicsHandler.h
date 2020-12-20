#pragma once
#include <stack>

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

		Point operator+(const Point& pt);
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
		Rectangle(Point pt, float width, float h);
	};

	// Image struct. Defines width and height.
	struct Image
	{
		int width;
		int height;

		virtual ~Image() = default;
	};

	class GraphicsHandler
	{
	protected:

	public:
		virtual void draw_line(const Point& pt1, const Point& pt2, int thickness, const Color& color);
	};
}