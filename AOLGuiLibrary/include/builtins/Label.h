#pragma once

#include "Block.h"

namespace agl::builtins
{
	class Label : public Block
	{
	private:
		bool supports_rich;
		bool multiline;
		bool text_wrap;
		char text_horizontal_align;
		char text_vertical_align;

		int text_width;
		int text_height;

		Font* base_font;
		Color base_color;
		int base_size;

		std::string text;
		std::vector<TextChunk> chunks;

		std::vector<Font*> rich_fonts;
		std::vector<Image> rich_images;

	public:
		Label();

		void set_text(std::string text);
		void draw_block(Point base_location);

		void set_horizontal_align(char halign);
		void set_vertical_align(char valign);

		void set_base_font(Font* font);
		void set_base_size(int size);
		void set_base_color(Color color);

		void resize_to_text();
	};
}