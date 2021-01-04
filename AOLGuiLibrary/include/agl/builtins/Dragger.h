#pragma once

#include "..\Block.h"

namespace agl::builtins
{
	class Dragger : public Block
	{
	private:
		Block* moved;

		void on_drag_move(const Event& e);

	public:
		Dragger();

		void set_moved(Block* new_moved);
	};
}