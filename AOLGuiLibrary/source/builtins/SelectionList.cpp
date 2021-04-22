#include "..\..\include\agl\builtins\SelectionList.h"
#include "..\..\include\agl\builtins\Label.h"
#include "..\..\include\\agl\events.h"

namespace agl::builtins
{
	void SelectionList::select_on_click(const Event& e)
	{
		if (
			(e.source->get_parent() == element_container ||
			(
				e.source->get_parent() &&
				e.source->get_parent()->get_parent() == element_container
			)
			)
			&& e.type == AGL_EVENT_MOUSE_RELEASED)
		{
			auto src = e.source;

			if (e.source->get_parent() && e.source->get_parent()->get_parent() == element_container)
				src = src->get_parent();

			int el_index = element_container->get_child_index(src);
			auto el_it = selected_elements.find(el_index);
			if (el_it == selected_elements.end())
			{
				if (!multiselection)
				{
					unselect_all();
				}

				selected_elements.insert(el_index);
				src->set_background_color(element_selected_background_color);
				raise_event({ .type = AGL_EVENT_ELEMENT_SELECTED, .keycode = el_index, .source = this });
			}
			else
			{
				unselect_child(el_index);
			}
		}
	}

	SelectionList::SelectionList()
	{
		element_style = nullptr;
		element_container = nullptr;
		element_height = debug::font_size + 6;
		element_label_size = debug::font_size;
		element_padding = 0;

		element_even_background_color = Color(0, 0, 0, 0);
		element_odd_background_color = Color(0, 0, 0, 0);

		multiselection = false;

		element_label_font = nullptr;

		add_event_function(
			std::bind(&SelectionList::select_on_click, this, std::placeholders::_1)
		);
	}

	void SelectionList::create_element_container()
	{
		Flow* flow = new Flow;
		flow->set_main_axis(AGL_VERTICAL);
		flow->set_single_subflow(true);
		flow->set_background_color(agl::Color(0, 0, 0, 0));

		connect_element_container(flow);
		add(flow);
	}

	void SelectionList::connect_element_container(Flow* flow)
	{
		element_container = flow;
	}

	void SelectionList::add_element(const std::string& text, const std::string& key)
	{
		if (element_container)
		{
			if (elements.find(key) != elements.end())
				return;

			Block* new_element = new Block;
			new_element->set_size(get_inner_width(), element_height);
			new_element->set_background_color(
				get_element_amount() & 1 ?
				element_odd_background_color : element_even_background_color
			);
			new_element->set_sizing(AGL_SIZING_BORDERBOX);

			if (element_style)
				new_element->apply(element_style);

			new_element->set_paddings(
				element_padding, element_padding, element_padding, element_padding
			);

			Label* lbl = new Label();
			lbl->set_base_color(element_label_color);
			lbl->set_base_font(element_label_font);
			lbl->set_base_size(element_label_size);
			lbl->set_text(text);
			lbl->set_size(lbl->get_text_width(), element_height);
			lbl->set_vertical_align(AGL_ALIGN_CENTER);

			new_element->add(lbl);
			new_element->add_event_source(lbl);

			add_event_source(new_element);
			add_event_source(lbl);
			element_container->add(new_element);
			element_container->resize_to_content();

			element_keys.push_back(key);
			elements.insert({ key, new_element });
		}
	}

	void SelectionList::remove_element(int index)
	{
		std::set<int> corrected;

		for (const int& ind : selected_elements)
			if (ind < 0)
				corrected.insert(ind);
			else if (ind > 0)
				corrected.insert(ind - 1);

		selected_elements = corrected;

		auto b = get_element_by_index(index);

		element_container->remove(b);
		
		auto l = b->get_child_by_index(0);

		b->remove(l);
		delete l;

		auto it = element_keys.begin();
		std::advance(it, index);

		elements.erase(element_keys[index]);
		element_keys.erase(it);

		delete b;
	}

	void SelectionList::clear_elements()
	{
		if (element_container)
		{
			element_container->clear();
			elements.clear();
			element_keys.clear();
		}
	}

	void SelectionList::set_default_element_background_color(const Color& color)
	{
		set_odd_element_background_color(color);
		set_even_element_background_color(color);
	}

	void SelectionList::set_odd_element_background_color(const Color& color)
	{
		element_even_background_color = color;
	}

	void SelectionList::set_even_element_background_color(const Color& color)
	{
		element_odd_background_color = color;
	}

	void SelectionList::set_element_paddings(int paddings)
	{
		element_padding = paddings;
	}

	void SelectionList::set_element_height(int height)
	{
		element_height = height;
	}

	void SelectionList::set_element_label_size(int size)
	{
		element_label_size = size;
	}

	void SelectionList::set_element_label_color(const Color& color)
	{
		element_label_color = color;
	}

	void SelectionList::set_element_label_font(const Font* font)
	{
		element_label_font = font;
	}

	void SelectionList::set_element_style(const Style* _style)
	{
		element_style = _style;
	}

	void SelectionList::unselect_all()
	{
		if (element_container)
			for (const auto index : selected_elements)
			{
				element_container->get_child_by_index(index)->set_background_color(
					index & 1 ?
					element_odd_background_color : element_even_background_color
				);
			}

		selected_elements.clear();
	}

	void SelectionList::unselect_child(int index)
	{
		if (element_container)
			element_container->get_child_by_index(index)->set_background_color(
				index & 1 ?
				element_odd_background_color : element_even_background_color
			);

		selected_elements.erase(index);
	}

	void SelectionList::select_child(int index)
	{
		if (element_container)
		{
			auto el_it = selected_elements.find(index);
			if (el_it == selected_elements.end())
			{
				if (!multiselection)
				{
					unselect_all();
				}

				selected_elements.insert(index);
				get_element_by_index(index)->set_background_color(element_selected_background_color);
				raise_event({ .type = AGL_EVENT_ELEMENT_SELECTED, .keycode = index, .source = this });
			}
		}
	}

	void SelectionList::select_child(const std::string& key)
	{
		int index = get_key_index(key);

		if (index == -1)
			return;

		select_child(index);
	}

	int SelectionList::get_key_index(const std::string& key)
	{
		for (int i = 0; i < element_keys.size(); i++)
			if (element_keys[i] == key)
				return i;

		return -1;
	}

	const std::set<int>& SelectionList::get_selected_elements() const
	{
		return selected_elements;
	}

	Block* SelectionList::get_element_by_index(int index) const
	{
		if (element_container)
			return element_container->get_child_by_index(index);

		return nullptr;
	}

	int SelectionList::get_element_amount()
	{
		if (element_container)
			return element_container->get_children_amount();

		return 0;
	}

	std::string SelectionList::get_key_by_index(int index) const
	{
		return element_keys[index];
	}

	Block* SelectionList::get_element_by_key(const std::string& key) const
	{
		return elements.at(key);
	}

}