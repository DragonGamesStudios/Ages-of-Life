#include "builtins\Button.h"
#include "events.h"

namespace agl::builtins
{
	void Button::dispath_events(Event e)
	{
		if (e.source == this)
			switch (e.type)
			{
			case AGL_EVENT_MOUSE_PRESSED:
				if (allowed_buttons & (1 << (e.button - 1)))
				{
					change_background_color(click_background_color);
				}
				break;

			case AGL_EVENT_MOUSE_RELEASED:
				if (allowed_buttons & (1 << (e.button - 1)))
				{
					if (hover)
						change_background_color(hover_background_color);
					else
						change_background_color(default_background_color);
					if (click_function)
						click_function(e, this);
				}
				break;

			case AGL_EVENT_BLOCK_HOVER_GAINED:
				change_background_color(hover_background_color);
				if (hover_gained_function)
					hover_gained_function(e, this);
				break;

			case AGL_EVENT_BLOCK_HOVER_LOST:
				change_background_color(default_background_color);
				if (hover_lost_function)
					hover_lost_function(e, this);
				break;

			default:
				return;
			}
	}

	Button::Button() : ImageBlock()
	{
		allowed_buttons = AGL_MOUSE_BBUTTON_PRIMARY;
		click_function = 0;
		hover_gained_function = 0;
		hover_lost_function = 0;
		label = 0;
		maintain_label = false;

		add_event_function(
			std::bind(&Button::dispath_events, this, std::placeholders::_1)
		);
	}

	void Button::set_background_color(Color color)
	{
		Block::set_background_color(color);

		default_background_color = color;
	}

	void Button::change_background_color(Color color)
	{
		Block::set_background_color(color);
	}

	void Button::set_hover_background_color(Color color)
	{
		hover_background_color = color;
	}

	void Button::set_click_background_color(Color color)
	{
		click_background_color = color;
	}

	void Button::set_allowed_buttons(int buttons)
	{
		allowed_buttons = buttons;
	}

	void Button::set_click_function(std::function<void(Event, Button*)> fn)
	{
		click_function = fn;
	}

	void Button::set_hover_gained_function(std::function<void(Event, Button*)> fn)
	{
		hover_gained_function = fn;
	}

	void Button::set_hover_lost_function(std::function<void(Event, Button*)> fn)
	{
		hover_lost_function = fn;
	}

	void Button::connect_label(Label* _label)
	{
		label = _label;
		add_event_source(label);
		maintain_label = false;
	}

	void Button::create_label()
	{
		Label* _label = new Label();
		
		connect_label(_label);

		add(_label);
		maintain_label = true;
	}

	void Button::set_text(std::string text)
	{
		if (label)
		{
			label->set_text(text);
			if (maintain_label)
			{
				label->resize_to_text();
				label->set_location(
					(get_inner_width() - label->get_width()) / 2,
					(get_inner_height() - label->get_height()) / 2
				);
			}
		}
	}

	void Button::set_font(Font* font)
	{
		if (label)
			label->set_base_font(font);
	}

	void Button::set_text_color(Color color)
	{
		if (label)
			label->set_base_color(color);
	}

	void Button::set_text_size(int size)
	{
		if (label)
			label->set_base_size(size);
	}

	void Button::apply(Style* _style)
	{
		ImageBlock::apply(_style);

		if (style->values["background_color_hover"].source)
			set_hover_background_color(std::get<Color>(style->values["background_color_hover"].value));

		if (style->values["background_color_click"].source)
			set_click_background_color(std::get<Color>(style->values["background_color_click"].value));
	}

	void Button::set_size(int width, int height)
	{
		ImageBlock::set_size(width, height);

		if (label && maintain_label)
			label->set_size(get_inner_width(), get_inner_height());
	}

}