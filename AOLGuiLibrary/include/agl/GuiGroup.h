#pragma once

#include "Gui.h"
#include "EventHandler.h"
#include "GraphicsHandler.h"

#include <set>

namespace agl
{
	struct ZBufferComparator
	{
		bool operator() (const Gui* gui1, const Gui* gui2) const;
	};

	struct StyleComparator
	{
		bool operator() (const Style* style1, const Style* style2) const;
	};

	class GuiGroup
	{
	private:
		std::set<Gui*, ZBufferComparator> guis;
		EventHandler* event_handler;
		GraphicsHandler* graphics_handler;
		Block* event_receiver;
		Block* focus_listener;

		int screen_width;
		int screen_height;

		void draw_hr_line(int mx, int width, int* y);
	public:
		GuiGroup();
		~GuiGroup();

		void register_event_handler(EventHandler* handler);
		void register_graphics_handler(GraphicsHandler* handler);

		void add_gui(Gui* gui);
		void remove_gui(Gui* gui);
		bool is_opened(Gui* gui);

		void draw();
		void update();

		void set_screen_dimensions(int w, int h);
	};
}