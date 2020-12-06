#pragma once

#include "Block.h"

namespace agl
{
	class Gui
	{
	private:
		std::vector<Block*> children;
		int z_index;
	public:
		Gui();

		void add(Block* new_child);
		void update(Block** event_receiver, Point mouse_location);
		void draw();
		int get_z_index() const;
		void set_z_index(int new_z_index);
	};
}