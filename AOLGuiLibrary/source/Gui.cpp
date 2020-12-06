#include "Gui.h"

namespace agl
{
	Gui::Gui()
	{
		this->children = {};
		z_index = 0;
	}

	void Gui::add(Block* new_child)
	{
		children.push_back(new_child);
	}

	void Gui::update(Block** event_receiver, Point mouse_location)
	{
		for (auto& child : this->children)
			child->update(mouse_location, Point(0, 0), event_receiver);
	}

	void Gui::draw()
	{
		for (auto& child : this->children)
			child->draw(Point(0, 0));
	}
	int Gui::get_z_index() const
	{
		return z_index;
	}
	void Gui::set_z_index(int new_z_index)
	{
		z_index = new_z_index;
	}
}