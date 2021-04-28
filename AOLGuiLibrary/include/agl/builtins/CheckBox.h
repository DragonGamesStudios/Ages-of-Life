#pragma once
#include "Button.h"

namespace agl::builtins
{
	class CheckBox : public Button
	{
	public:
		Color other_background_color;
		std::function<void(const Event&, Button*)> click_function;

		CheckBox();

		bool get_value() const;
		void set_value(bool value);

		void set_other_background_color(const Color& color);

		void set_click_function(std::function<void(const Event&, Button*)> fn);

	private:
		bool checked;

		void on_click_check(const agl::Event& e, Button* btn);
		void draw_background(const Point& base_location);
	};
}