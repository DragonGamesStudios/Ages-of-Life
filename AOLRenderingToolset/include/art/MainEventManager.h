#pragma once

#include "EventManager.h"

#define ART_EVENT_DISPLAY_CLOSE -1

namespace art
{
	class MainEventManager : EventManager
	{
	private:
		ALLEGRO_EVENT_QUEUE* event_queue;
		ALLEGRO_TIMER* timer;

		Display* display;

		std::vector<EventManager*> managers;

		bool tick;
		bool should_close;
		double last_time_check;

	public:
		MainEventManager(int fps, bool start_timer = true);
		~MainEventManager();

		void dispatch_event(ALLEGRO_EVENT e);

		void connect_display(Display* _display);

		ALLEGRO_EVENT get_event();
		bool clock_ticked();
		bool window_closed() const;
		double get_delta();

		void add_manager(EventManager* manager);

		void enable_mouse();
		void enable_keyboard();

		void close();
	};
}