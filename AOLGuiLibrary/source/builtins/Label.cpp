#include "agl/builtins/Label.h"
#include "..\..\include\agl\events.h"

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
		base_size = 0;

		resizes_to_content = false;
	}

	int get_x_by_align(char align, int row_width, int max_width)
	{
		switch (align)
		{
		case AGL_ALIGN_CENTER:
			return (max_width - row_width) / 2;
			break;

		case AGL_ALIGN_END:
			return max_width - row_width;
			break;

		default:
			return 0;
			break;
		}
	}

	void Label::add_line(
		int* row_width, int max_width, float* current_y,
		std::string* chunk_text, int current_font_height,
		const Font* current_font, const Color& current_color, int current_size,
		float* current_x, int* row_begin
	)
	{
		chunks.push_back({
			.location = Point{*current_x, *current_y},
			.width = current_font->get_width(current_size, *chunk_text),
			.height = current_font->get_height(current_size),

			.font = current_font,
			.text = *chunk_text,
			.color = current_color,
			.size = current_size
			});

		float base_x = (float)get_x_by_align(text_horizontal_align, *row_width, max_width);

		for (auto i = chunks.begin() + *row_begin; i != chunks.end(); i++)
		{
			i->location.x = base_x;
			base_x += i->width;
		}

		*current_y += (!chunk_text->empty()) * current_font_height;
		chunk_text->clear();
		text_width = std::max(text_width, *row_width);
		*row_width = 0;
		*row_begin = (int)chunks.size();
		*current_x = 0;
	}

	void Label::set_text(const std::string& _text, bool raise)
	{
		text = _text;

		chunks.clear();

		float y = 0;

		switch (text_vertical_align)
		{
		case AGL_ALIGN_CENTER:
			y = float(get_inner_height() - base_font->get_height(base_size)) / 2;
			break;

		case AGL_ALIGN_END:
			y = (float)get_inner_height() - base_font->get_height(base_size);
			break;
		}

		if (!(supports_rich || multiline || text_wrap))
		{
			float x = (float)get_x_by_align(
				text_horizontal_align, base_font->get_width(base_size, text), get_inner_width()
				);

			chunks.push_back({
				.location = Point{ x, y },

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
			int new_row_index = 0;
			int row_width = 0;
			std::string chunk_text;
			std::string word;
			bool ignore_next = false;
			const Font* current_font = base_font;
			int current_size = base_size;
			Color current_color = base_color;
			int current_space_width = current_font->get_width(current_size, " ");
			int current_font_height = base_font->get_height(current_size);
			float current_x = 0;
			float current_y = 0;
			int max_width = get_inner_width();
			bool reading_setter = false;
			std::vector<std::string> setter_args;
			std::string setter_arg;

			for (const char c : text)
			{
				// TODO: Rich text support testing

				if (multiline && c == '\n' && !ignore_next)
				{
					// If we find '\n' char, we create new line

					// First, pushing the current word
					chunk_text += ((!chunk_text.empty()) ? " " : "") + word;
					row_width +=
						current_space_width + base_font->get_width(base_size, word);
					word.clear();

					// Second, in case of empty line, we should make sure we are
					// doing new line
					if (chunk_text.empty())
						current_y += current_font_height;

					// Third, adding the line itself
					add_line(
						&row_width, max_width,
						&current_y, &chunk_text,
						current_font_height,
						current_font, current_color,
						current_size, &current_x,
						&new_row_index
					);
				}
				else if (c == '/' && !ignore_next)
				{
					// Pretty self-explanatory, I think
					ignore_next = true;
				}
				else if (
					((text_wrap && c == ' ') || (c == '<' && supports_rich))
					&& !ignore_next)
				{
					// If the new line width would exceed max width, we add new line...
					int word_width = current_font->get_width(current_size, word);
					if (row_width + current_space_width + word_width > max_width)
					{
						add_line(
							&row_width, max_width,
							&current_y, &chunk_text,
							current_font_height,
							current_font, current_color,
							current_size, &current_x,
							&new_row_index
						);
					}

					// ... and add the word to the line below
					chunk_text += (!chunk_text.empty() ? " " : "") + word;
					row_width +=
						current_space_width + base_font->get_width(base_size, word);
					word.clear();
					
					if (!chunk_text.empty() && c == '<')
					{
						// If rich setter begins, add chunk
						chunks.push_back({
							.location = {current_x, current_y},

							.width = current_font->get_width(current_size, chunk_text),
							.height = current_font_height,

							.font = current_font,
							.text = chunk_text,
							.color = current_color,
							.size = current_size
							});

						chunk_text.clear();
						current_x += chunks.back().width;
					}
				}
				else if (supports_rich && reading_setter && !ignore_next)
				{
					if (c != '>')
					{
						if (c != ':')
							setter_arg.push_back(c);
						else
						{
							setter_args.push_back(setter_arg);
							setter_arg.clear();
						}
					}
					else
					{
						setter_args.push_back(setter_arg);
						setter_arg.clear();

						reading_setter = false;
						if (setter_args[0] == "text")
						{
							if (setter_args[1] == "def")
								current_font = base_font;
							else
								current_font =
								rich_fonts[std::stoi(setter_args[1])];

							if (setter_args[2] == "def")
								current_color = base_color;
							else
								current_color =
								rich_colors[std::stoi(setter_args[2])];

							if (setter_args[3] == "def")
								current_size = base_size;
							else
								current_size = std::stoi(setter_args[3]);

							current_font_height = current_font->get_height(current_size);
						}

						setter_args.clear();
					}
				}
				else
				{
					word.push_back(c);
					ignore_next = false;
				}

				if (c == '<')
					reading_setter = true;
			}

			if (!chunk_text.empty())
			{
				// Adding line on text ended, because otherwise
				// it would not be there
				add_line(
					&row_width, max_width,
					&current_y, &chunk_text,
					current_font_height,
					current_font, current_color,
					current_size, &current_x,
					&new_row_index
				);
			}

			// Offseting the y
			for (auto& chunk : chunks)
				chunk.location.y += y;

			text_height = (int)current_y;
		}

		if (resizes_to_content)
			if (get_box_width(sizing) != text_width || get_box_height(sizing) != text_height)
				resize_to_text();

		if (raise)
			raise_event({ .type = AGL_EVENT_TEXT_CHANGED, .source = this });
	}

	void Label::draw_block(const Point& base_location)
	{
		Block::draw_block(base_location);

		for (const auto& chunk :chunks)
		{
			if (!chunk.type)
			{
				graphics_handler->draw_text(base_location + chunk.location, chunk.text, chunk.font, chunk.size, chunk.color);
			}
			else
			{
				graphics_handler->draw_scaled_image_target(chunk.location, chunk.image, { 0, 0 }, chunk.width, chunk.height);
			}
		}
	}
	
	void Label::set_horizontal_align(char halign)
	{
		text_horizontal_align = halign;
		set_text(text);
	}

	void Label::set_vertical_align(char valign)
	{
		text_vertical_align = valign;
		set_text(text);
	}

	void Label::set_base_font(const Font* font)
	{
		base_font = font;
	}

	void Label::set_base_size(int size)
	{
		base_size = size;
	}

	void Label::set_base_color(const Color& color)
	{
		base_color = color;
	}

	void Label::resize_to_text()
	{
		set_size(text_width, text_height);

		set_text(text);
	}

	void Label::resize_always()
	{
		resizes_to_content = true;
	}

	void Label::set_text_wrap(bool wrap)
	{
		text_wrap = wrap;
		set_text(text);
	}

	void Label::set_multiline(bool _multiline)
	{
		multiline = _multiline;
		set_text(text);
	}

	void Label::set_rich_support(bool _rich)
	{
		supports_rich = _rich;

		set_text(text);
	}

	void Label::set_rich_colors(const std::vector<Color>& colors)
	{
		rich_colors = colors;
	}

	int Label::get_text_width() const
	{
		return text_width;
	}

	int Label::get_text_height() const
	{
		return text_height;
	}

	std::string Label::get_text() const
	{
		return text;
	}

	void Label::apply(const Style* _style)
	{
		Block::apply(_style);

		if (style->values.at("base_size").source)
			base_size = std::get<int>(style->values.at("base_size").value);

		if (style->values.at("base_color").source)
			base_color = std::get<Color>(style->values.at("base_color").value);

		if (style->values.at("base_font").source)
		{
			if (!std::get<int>(style->values.at("base_font").value))
				base_font = loaded_fonts["default"];
		}
	}

}