#pragma once
#include "Flow.h"

namespace agl::builtins
{
	class SelectionList : public Block
	{
	private:
		Style* element_style;
		int element_height;
		int element_padding;
		Color element_selected_background_color;
		Color element_even_background_color;
		Color element_odd_background_color;

		Color element_label_color;
		Font* element_label_font;
		int element_label_size;

		bool multiselection;

		Flow* element_container;
		std::set<int> selected_elements;

		void select_on_click(Event e);

	public:
		SelectionList();

		void create_element_container();
		void connect_element_container(Flow* container);

		void add_element(std::string text);
		void set_default_element_background_color(Color color);
		void set_odd_element_background_color(Color color);
		void set_even_element_background_color(Color color);
		void set_element_paddings(int paddings);
		void set_element_height(int height);
		void set_element_label_size(int size);
		void set_element_label_color(Color color);
		void set_element_label_font(Font* font);
		void set_element_style(Style* _style);

		void unselect_all();
		void unselect_child(int index);
		
		std::set<int> get_selected_elements() const;
		Block* get_element_by_index() const;
		int get_element_amount();
	};
}