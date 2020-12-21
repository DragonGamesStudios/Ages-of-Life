#pragma once

#include "agl/Block.h"

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

		const Font* base_font;
		Color base_color;
		int base_size;

		bool resizes_to_content;

		std::string text;
		std::vector<TextChunk> chunks;

		std::vector<const Font*> rich_fonts;
		std::vector<Color> rich_colors;
		std::vector<Image*> rich_images;

		void add_line(
			int* row_width, int max_width, float* current_y,
			std::string* chunk_text, int current_font_height,
			const Font* current_font, const Color& current_color, int current_size,
			float* current_x, int* row_begin
		);

	public:
		Label();

		void set_text(const std::string& text, bool raise = true);
		void draw_block(const Point& base_location);

		void set_horizontal_align(char halign);
		void set_vertical_align(char valign);

		void set_base_font(const Font* font);
		void set_base_size(int size);
		void set_base_color(const Color& color);

		void resize_to_text();
		void resize_always();

		void set_text_wrap(bool wrap);
		void set_multiline(bool _multiline);
		void set_rich_support(bool _rich);

		void set_rich_colors(const std::vector<Color>& colors);

		int get_text_width() const;
		int get_text_height() const;
		std::string get_text() const;

		virtual void apply(const Style* _style);
	};
}