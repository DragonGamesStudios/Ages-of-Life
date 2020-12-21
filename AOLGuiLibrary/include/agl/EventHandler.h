#pragma once
#include <vector>

namespace agl
{
	struct Mouse
	{
		int x;
		int y;
		int buttons;
	};

	class Block;

	struct Event
	{
		int type;
		int x = 0;
		int y = 0;
		int z = 0;
		int w = 0;
		int dx = 0;
		int dy = 0;
		int dz = 0;
		int dw = 0;
		int button = 0;
		int buttons = 0;

		char unicode = 0;
		int keycode = 0;
		char input = 0;
		int mods = 0;

		Block* source = 0;
		Block* block = 0;
	};

	class EventHandler
	{
	protected:
		std::vector<Event> event_queue;
		Mouse mouse_state;
	public:
		std::vector<Event>::iterator get_event_queue_begin();
		std::vector<Event>::iterator get_event_queue_end();
		void reset_event_queue();
		Mouse* get_mouse_state();

		EventHandler();
	};
}