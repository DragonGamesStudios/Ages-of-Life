#include "..\..\..\include\agl\builtins\CheckBox.h"

namespace agl::builtins
{
	CheckBox::CheckBox()
		: Button()
	{
		checked = false;
	}

	bool CheckBox::get_value() const
	{
		return checked;
	}

	void CheckBox::set_value(bool value)
	{
		checked = value;

		auto buff = background_color;
		set_background_color(other_background_color);
		other_background_color = buff;
	}

	void CheckBox::set_other_background_color(const Color& color)
	{
		other_background_color = color;
	}

	void CheckBox::set_click_function(std::function<void(const Event&, Button*)> fn)
	{
		click_function = fn;
	}

	void CheckBox::on_click_check(const agl::Event& e, Button* btn)
	{
		if (btn == this)
		{
			set_value(!checked);
			click_function(e, btn);
		}
	}

	void CheckBox::draw_background(const Point& base_location)
	{
		Button::draw_background(base_location);

		if (checked)
		{
			graphics_handler->draw_line(base_location + Point{ 5.f, 5.f }, base_location + Point{ get_inner_width() - 15.f, get_inner_height() - 5.f }, 3, agl::Color(0, 0, 0));
			graphics_handler->draw_line(
				base_location + Point{ get_inner_width() - 15.f, get_inner_height() - 5.f }, base_location + Point{ get_inner_width() - 5.f, get_inner_height() - 15.f }, 3, agl::Color(0, 0, 0)
			);
		}
	}

}