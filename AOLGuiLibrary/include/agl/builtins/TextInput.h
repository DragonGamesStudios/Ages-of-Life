#pragma once

#include "..\Block.h"
#include "Label.h"

namespace agl::builtins
{
	class TextInput : public Block
	{
	private:
		agl::builtins::Label* label;

		int return_keycode;
		int backspace_keycode;
		int delete_keycode;

		long long time_since_blink;
		long long blink_interval;
		long long last_registered_time;

		bool draw_cursor;

		Color cursor_color;

		void on_key_pressed(const agl::Event& e);
		void on_char_received(const agl::Event& e);
		long long get_time();

	public:
		TextInput();

		void set_return_keycode(int keycode);
		void set_backspace_keycode(int keycode);
		void set_delete_keycode(int keycode);

		void connect_label(agl::builtins::Label* lbl);
		void create_label();
		void apply_to_label(const Style* _style);
		void set_text_color(const agl::Color& color);
		void set_text_size(int size);
		void set_text_font(const Font* font);

		void set_cursor_color(const Color& color);

		void set_blink_interval(long long interval);

		std::string get_value() const;
		void clear_value();
		void set_value(const std::string& value);

		void draw_block(const Point& base_location);
	};
}