#include "..\include\agl\builtins\TextInput.h"
#include "..\include\agl\events.h"

#include <chrono>

namespace ch = std::chrono;

namespace agl::builtins
{
	void TextInput::on_key_pressed(const agl::Event& e)
	{
		if (e.type == AGL_EVENT_KEY_PRESSED && focus && label)
		{
			if (e.keycode == return_keycode)
			{
				set_focus(false);
			}
			else if (e.keycode == backspace_keycode)
			{
				if (!label->get_text().empty())
				{
					std::string l_txt = label->get_text();
					l_txt.pop_back();
					label->set_text(l_txt);
				}
			}
		}
	}

	void TextInput::on_char_received(const agl::Event& e)
	{
		if (e.type == AGL_EVENT_CHAR_INPUT && label && focus)
		{
			if (
				e.keycode != return_keycode &&
				e.keycode != backspace_keycode
				)
				label->set_text(label->get_text() + std::string(1, e.unicode));
		}
	}

	long long TextInput::get_time()
	{
		return ch::duration_cast<ch::milliseconds>(ch::system_clock::now().time_since_epoch()).count();;
	}

	TextInput::TextInput()
		: Block()
	{
		return_keycode = 0;
		backspace_keycode = 0;
		delete_keycode = 0;

		label = 0;
		blink_interval = 500;
		last_registered_time = get_time();
		time_since_blink = 0;

		draw_cursor = true;
		cursor_color = agl::Color(0, 0, 0);

		enable_focus();

		add_event_function(
			std::bind(&TextInput::on_key_pressed, this, std::placeholders::_1)
		);

		add_event_function(
			std::bind(&TextInput::on_char_received, this, std::placeholders::_1)
		);
	}

	void TextInput::set_return_keycode(int keycode)
	{
		return_keycode = keycode;
	}

	void TextInput::set_backspace_keycode(int keycode)
	{
		backspace_keycode = keycode;
	}

	void TextInput::set_delete_keycode(int keycode)
	{
		delete_keycode = keycode;
	}

	void TextInput::connect_label(agl::builtins::Label* lbl)
	{
		label = lbl;
	}

	void TextInput::create_label()
	{
		label = new Label;

		label->resize_always();
		add(label);
	}

	void TextInput::apply_to_label(const Style* _style)
	{
		label->apply(_style);
	}

	void TextInput::set_text_color(const agl::Color& color)
	{
		label->set_base_color(color);
	}

	void TextInput::set_text_size(int size)
	{
		label->set_base_size(size);
	}

	void TextInput::set_text_font(const Font* font)
	{
		label->set_base_font(font);
	}

	void TextInput::set_cursor_color(const Color& color)
	{
		cursor_color = color;
	}

	void TextInput::set_blink_interval(long long interval)
	{
		blink_interval = interval;
	}

	std::string TextInput::get_value() const
	{
		return label->get_text();
	}

	void TextInput::clear_value()
	{
		if (label) label->set_text("");
	}

	void TextInput::set_value(const std::string& value)
	{
		if (label) label->set_text(value);
	}

	void TextInput::draw_block(const Point& base_location)
	{
		long long time = get_time();
		long long dt = time - last_registered_time;
		last_registered_time = time;

		time_since_blink += dt;

		if (time_since_blink > blink_interval)
		{
			draw_cursor = !draw_cursor;
			time_since_blink %= blink_interval;
		}

		Block::draw_block(base_location);

		if (label && draw_cursor && focus)
		{
			float x = (float)label->get_text_width() + 2;

			graphics_handler->draw_line(
				base_location + Point{ x, 0 }, base_location + Point{ x, (float)get_inner_height() }, 2, cursor_color
			);
		}
	}

}