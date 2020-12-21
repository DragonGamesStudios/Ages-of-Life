#pragma once

#include "Allegro5EventManager.h"

#include <art\MainEventManager.h>

namespace art
{
	class Allegro5MainEventManager : public Allegro5EventManager, public MainEventManager
	{
	private:
		ALLEGRO_EVENT_QUEUE* event_queue;
		ALLEGRO_TIMER* timer;

		bool tick;
		bool should_close;
		double last_time_check;

	public:
		Allegro5MainEventManager(int fps, bool start_timer = true);
		~Allegro5MainEventManager();

		void connect_display(const Display* display_);

		void dispatch_event(const ALLEGRO_EVENT& e);
		ALLEGRO_EVENT get_event();
		bool clock_ticked();
		bool window_closed() const;
		double get_delta();

		void enable_mouse();
		void enable_keyboard();

		void close();
	};
}