#pragma once

#include "ImageBlock.h"
#include "Label.h"

namespace agl::builtins
{
	class Button : public ImageBlock
	{
	private:
		Color default_background_color;
		Color hover_background_color;
		Color click_background_color;

		int allowed_buttons;
		std::function<void(const Event&, Button*)> click_function;
		std::function<void(const Event&, Button*)> hover_gained_function;
		std::function<void(const Event&, Button*)> hover_lost_function;

		Label* label;
		bool maintain_label;

		void dispath_events(const Event& e);
		void on_label_text_changed_relocate(const Event& e);
	public:
		Button();

		void set_background_color(const Color& color);
		void change_background_color(const Color& color);
		void set_hover_background_color(const Color& color);
		void set_click_background_color(const Color& color);

		void set_allowed_buttons(int buttons);

		void set_click_function(std::function<void(const Event&, Button*)> fn);
		void set_hover_gained_function(std::function<void(const Event&, Button*)> fn);
		void set_hover_lost_function(std::function<void(const Event&, Button*)> fn);

		void connect_label(Label* _label);
		void create_label();
		void set_text(const std::string& text);
		void set_font(const Font* font);
		void set_text_color(const Color& color);
		void set_text_size(int size);

		virtual void apply(const Style* _style);
		virtual void set_size(int width, int height);

		void apply_to_label(const Style* _style);
		Label* get_label();
	};
}