#include "..\include\agl\GraphicsHandler.h"

namespace agl
{
	Point::Point(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	Point Point::operator+(const Point& pt)
	{
		return Point(x + pt.x, y + pt.y);
	}

	Point& Point::operator+=(const Point& pt)
	{
		x += pt.x;
		y += pt.y;
		return *this;
	}

	Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a)
	{
	}

}