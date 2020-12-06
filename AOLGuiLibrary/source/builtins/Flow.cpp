
#include "builtins/Flow.h"
#include "events.h"

namespace agl::builtins
{
	void Flow::layout_children()
	{
		// Initialize
		std::vector<std::vector<Block*>> subflows = {};
		std::vector<int> main_axis_subflow_sizes = {};

		int main_axis = 0, second_axis = 0, second_axis_subflow_size = 0;

		std::vector<Block*> subflow = {};

		int main_axis_max_size = box.inner_width, second_axis_max_size = box.inner_height;
		if (main_axis_direction == AGL_VERTICAL)
			std::swap(main_axis_max_size, second_axis_max_size);


		for (const auto& child : children)
		{
			int main_axis_child_size = child->get_width(),
				second_axis_child_size = child->get_height();
			if (main_axis_direction == AGL_VERTICAL)
				std::swap(main_axis_child_size, second_axis_child_size);

			// Reset subflow if overflowed and isn't single subflow
			if (!single && main_axis + main_axis_child_size > main_axis_max_size)
			{
				if (!subflow.empty())
					main_axis -= main_axis_spacing;

				subflows.push_back(subflow);
				subflow.clear();
				main_axis_subflow_sizes.push_back(main_axis);

				second_axis += second_axis_subflow_size + second_axis_spacing;
				main_axis = 0;
				second_axis_subflow_size = 0;
			}

			// Add child to row
			subflow.push_back(child);
			main_axis += main_axis_child_size + main_axis_spacing;
			second_axis_subflow_size = std::max(second_axis_subflow_size, second_axis_child_size);
		}

		// Finalize planning
		subflows.push_back(subflow);
		subflow.clear();
		main_axis_subflow_sizes.push_back(main_axis - main_axis_spacing);
		second_axis += second_axis_subflow_size;

		// Locate children
		int second_axis_base = 0;

		if (second_axis_align == AGL_ALIGN_CENTER)
			second_axis_base = (second_axis_max_size - second_axis) / 2;
		else if (second_axis_align == AGL_ALIGN_END)
			second_axis_base = second_axis_max_size - second_axis;

		// Reset
		second_axis_subflow_size = 0;
		int current_main_axis = 0, current_second_axis = 0;
		second_axis_subflow_size = 0;

		// Resize points setup
		int leftest = main_axis_max_size, toppest = second_axis_base,
			rightest = 0, bottomest = second_axis_base + second_axis;

		for (int i = 0; i < subflows.size(); i++)
		{
			// Base setup
			int main_axis_base = 0;
			if (main_axis_align == AGL_ALIGN_CENTER)
				main_axis_base = (main_axis_max_size - main_axis_subflow_sizes[i]) / 2;
			else if (main_axis_align == AGL_ALIGN_END)
				main_axis_base = main_axis_max_size - main_axis_subflow_sizes[i];

			leftest = std::min(main_axis_base, leftest);
			rightest = std::max(main_axis_base + main_axis_subflow_sizes[i], rightest);

			// Children layout
			for (const auto& child : subflows[i])
			{
				int child_main_axis_size = child->get_width(),
					child_second_axis_size = child->get_height();

				int child_x = main_axis_base + current_main_axis,
					child_y = current_second_axis;

				if (main_axis_direction == AGL_VERTICAL)
				{
					std::swap(child_main_axis_size, child_second_axis_size);
					std::swap(child_x, child_y);
				}

				child->set_location(child_x, child_y);
				current_main_axis += child_main_axis_size + main_axis_spacing;
				second_axis_subflow_size =
					std::max(second_axis_subflow_size, child_second_axis_size);
			}

			current_main_axis = 0;
			current_second_axis += second_axis_subflow_size + second_axis_spacing;
			second_axis_subflow_size = 0;
		}

		// Set resize points
		if (main_axis_direction == AGL_VERTICAL)
		{
			std::swap(toppest, leftest);
			std::swap(bottomest, rightest);
		}

		resize_point_topleft = Point(leftest, toppest);
		resize_point_bottomright = Point(rightest, bottomest);
	}

	void Flow::on_children_changed(Event e)
	{
		if (e.type == AGL_EVENT_BLOCK_RESIZED && e.source->get_parent() == this)
		{
			layout_children();
		}
	}

	void Flow::resize_to_content()
	{
		set_size(
			resize_point_bottomright.x - resize_point_topleft.x,
			resize_point_bottomright.y - resize_point_topleft.y
		);
	}

	void Flow::set_main_axis_spacing(int spacing)
	{
		main_axis_spacing = spacing;
		layout_children();
	}

	void Flow::set_second_axis_spacing(int spacing)
	{
		second_axis_spacing = spacing;
		layout_children();
	}

	void Flow::set_main_axis_align(char align)
	{
		main_axis_align = align;
		layout_children();
	}

	void Flow::set_second_axis_align(char align)
	{
		second_axis_align = align;
		layout_children();
	}

	void Flow::set_single_subflow(bool value)
	{
		single = value;
		layout_children();
	}

	void Flow::set_main_axis(char axis)
	{
		main_axis_direction = axis;
		layout_children();
	}

	void Flow::add(Block* child)
	{
		Block::add(child);
		child->add_event_listener(this);
		layout_children();
	}

	void Flow::apply(Style* new_style)
	{
		Block::apply(new_style);

		if (style->values["main_axis"].source)
		{
			main_axis_direction = std::get<int>(style->values["main_axis"].value);

		}

		if (style->values["main_axis_spacing"].source)
			main_axis_spacing = std::get<int>(style->values["main_axis_spacing"].value);

		if (style->values["second_axis_spacing"].source)
			second_axis_spacing = std::get<int>(style->values["second_axis_spacing"].value);

		std::string axes[2] = { "main", "second" };
		int* spacing_values[2] = { &main_axis_spacing, &second_axis_spacing };
		char* align_values[2] = { &main_axis_align, &second_axis_align };
		
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				if (style->values[axes[i] + "_axis_spacing"].source)
					*spacing_values[i] =
					std::get<int>(style->values[axes[i] + "_axis_spacing"].value);

				if (style->values[axes[i] + "_axis_align"].source)
					*align_values[i] =
					std::get<int>(style->values[axes[i] + "_axis_align"].value);
			}
		}

		if (style->values["single"].source)
			single = std::get<bool>(style->values["single"].value);
	}

	void Flow::set_size(int width, int height)
	{
		Block::set_size(width, height);

		layout_children();
	}

	Flow::Flow() : Block()
	{
		single = false;
		main_axis_align = AGL_ALIGN_BEGIN;
		second_axis_align = AGL_ALIGN_BEGIN;
		main_axis_spacing = 0;
		second_axis_spacing = 0;
		main_axis_direction = AGL_HORIZONTAL;

		add_event_function(std::bind(&Flow::on_children_changed, this, std::placeholders::_1));
	}


}