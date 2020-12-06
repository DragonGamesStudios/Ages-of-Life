#include "builtins/Label.h"

namespace agl::builtins
{
	Label::Label() : Block()
	{
		text_wrap = false;
		supports_rich = false;
		text_horizontal_align = AGL_ALIGN_BEGIN;
		text_vertical_align = AGL_ALIGN_BEGIN;
		multiline = false;

		text_width = 0;
		text_height = 0;

		set_background_color(Color(0, 0, 0, 0));

		base_font = 0;
	}

	void Label::set_text(std::string _text)
	{
		text = _text;

		chunks.clear();

		if (!(supports_rich || multiline || text_wrap))
		{
			int x = 0;
			int y = 0;

			switch (text_horizontal_align)
			{
			case AGL_ALIGN_CENTER:
				x = (get_inner_width() - base_font->get_width(base_size, text)) / 2;
				break;

			case AGL_ALIGN_END:
				x = get_inner_width() - base_font->get_width(base_size, text);
				break;
			}

			switch (text_vertical_align)
			{
			case AGL_ALIGN_CENTER:
				y = (get_inner_height() - base_font->get_height(base_size)) / 2;
				break;

			case AGL_ALIGN_END:
				y = get_inner_height() - base_font->get_height(base_size);
				break;
			}

			chunks.push_back({
				.x = x,
				.y = y,

				.font = base_font,
				.text = text,
				.color = base_color,
				.size = base_size
				});

			text_width = base_font->get_width(base_size, text);
			text_height = base_font->get_height(base_size);
		}
		else
		{
			std::vector<TextChunk> row;
			int row_width = 0;
			std::string chunk_text;
			std::string word;
			bool ignore_next = false;
			Font* current_font = base_font;
			int current_size = base_size;
			int current_space_width = current_font->get_width(current_size, " ");

			for (const char c : text)
			{
				// TODO: Rich text support

				if (c == '\n' && !ignore_next)
				{
					// TODO: Multiline support
				}
				else if (c == '/' && !ignore_next)
				{
					ignore_next = true;
				}
				else if (text_wrap && c == ' ' && !ignore_next)
				{
					int word_width = current_font->get_width(current_size, word);
					if (row_width + current_space_width + word_width > get_inner_width())
					{
						// TODO: Text wraping
					}
				}
				else
				{
					word.push_back(c);
				}
			}
		}

	}

	void Label::draw_block(Point base_location)
	{
		Block::draw_block(base_location);

		for (const auto& chunk :chunks)
		{
			if (!chunk.type)
				al_draw_text(
					chunk.font->get(chunk.size),
					chunk.color.calculated,
					base_location.x + chunk.x, base_location.y + chunk.y, 0,
					chunk.text.c_str()
				);
			else
				al_draw_scaled_bitmap(
					chunk.image->bitmap,
					0, 0,
					chunk.image->width,
					chunk.image->height,
					chunk.x, chunk.y,
					chunk.desired_width,
					chunk.desired_height,
					0
				);
		}
	}

	void Label::set_horizontal_align(char halign)
	{
		text_horizontal_align = halign;
	}

	void Label::set_vertical_align(char valign)
	{
		text_vertical_align = valign;
	}

	void Label::set_base_font(Font* font)
	{
		base_font = font;
	}

	void Label::set_base_size(int size)
	{
		base_size = size;
	}

	void Label::set_base_color(Color color)
	{
		base_color = color;
	}

	void Label::resize_to_text()
	{
		set_size(text_width, text_height);

		set_text(text);
	}

}