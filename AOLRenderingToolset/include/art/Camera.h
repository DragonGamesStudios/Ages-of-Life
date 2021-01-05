#pragma once

namespace art
{
	class Camera
	{
	private:
		int tile_ratio;
		float x;
		float y;

		int width;
		int height;

	public:
		Camera(int w, int h);

		void set_tile_ratio(int ratio);

		void set_position(float nx, float ny);
		void change_position(float dx, float dy);

		float get_x() const;
		float get_y() const;

		int get_center_x() const;
		int get_center_y() const;

		float get_x_on_pixel(int px) const;
		float get_y_on_pixel(int py) const;

		int get_x_on_tile(float tx);
		int get_y_on_tile(float ty);
	};
}