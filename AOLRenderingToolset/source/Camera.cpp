#include "..\include\art\Camera.h"
namespace art
{
	Camera::Camera(int w, int h)
	{
		x = y = 0.f;
		tile_ratio = 100;
		width = w;
		height = h;
	}

	void Camera::set_tile_ratio(int ratio)
	{
		tile_ratio = ratio;
	}

	void Camera::set_position(float nx, float ny)
	{
		x = nx;
		y = ny;
	}

	void Camera::change_position(float dx, float dy)
	{
		x += dx;
		y += dy;
	}

	float Camera::get_x() const
	{
		return x;
	}

	float Camera::get_y() const
	{
		return y;
	}

	int Camera::get_center_x() const
	{
		return int(x);
	}

	int Camera::get_center_y() const
	{
		return int(y);
	}

	float Camera::get_x_on_pixel(int px) const
	{
		return x + float(-width / 2 + px) / tile_ratio;
	}

	float Camera::get_y_on_pixel(int py) const
	{
		return y + float(height / 2 - py) / tile_ratio;
	}

	int Camera::get_x_on_tile(float tx)
	{
		return int((tx - x) * tile_ratio);
	}

	int Camera::get_y_on_tile(float ty)
	{
		return int((y - ty) * tile_ratio);
	}

}