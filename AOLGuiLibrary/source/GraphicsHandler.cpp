#include "..\include\agl\GraphicsHandler.h"

namespace agl
{
	Point::Point(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	Point Point::operator+(const Point& pt) const
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

	Font::Font(const std::string& font_path)
	{
		path = font_path;
	}

	Rectangle::Rectangle(float x, float y, float w, float h)
	{
		pt = { x, y };
		width = w;
		height = h;
	}

	Rectangle::Rectangle(const Point& pt_, float w, float h)
	{
		pt = pt_;
		width = w;
		height = h;
	}

	void GraphicsHandler::set_clipping_rectangle(const Rectangle& rect)
	{
		clipping_stack.push(rect);
	}

	Rectangle GraphicsHandler::get_clipping_rectangle() const
	{
		return clipping_stack.top();
	}

	void GraphicsHandler::pop_clipping_rectangle()
	{
		clipping_stack.pop();
	}

	int GraphicsHandler::get_clipping_stack_size() const
	{
		return clipping_stack.size() -1;
	}

	void GraphicsHandler::clear_clipping_stack()
	{
		for (int i = 0; i < clipping_stack.size() - 1; i++)
		{
			pop_clipping_rectangle();
		}
	}

}