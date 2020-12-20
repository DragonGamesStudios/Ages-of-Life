#include "MainEventManager.h"

#include "agl/events.h"

namespace art
{
	MainEventManager::MainEventManager(int fps, bool start_timer)
	{
		event_queue = 0;
		event_queue = al_create_event_queue();

		timer = al_create_timer(1.f / fps);

		al_register_event_source(event_queue, al_get_timer_event_source(timer));

		display = 0;

		tick = false;
		should_close = false;

		last_time_check = al_get_time();

		if (start_timer)
			al_start_timer(timer);
	}

	MainEventManager::~MainEventManager()
	{
		al_destroy_timer(timer);
		al_destroy_event_queue(event_queue);
	}

	void MainEventManager::connect_display(Display* _display)
	{
		display = _display;
		al_register_event_source(
			event_queue, al_get_display_event_source(display->get_al_display())
		);
	}

	ALLEGRO_EVENT MainEventManager::get_event()
	{
		ALLEGRO_EVENT e;
		al_wait_for_event(event_queue, &e);

		dispatch_event(e);

		for (auto& manager : managers)
			manager->dispatch_event(e);

		return e;
	}

	bool MainEventManager::clock_ticked()
	{
		bool ret = tick;
		tick = false;
		return ret;
	}

	bool MainEventManager::window_closed() const
	{
		return should_close;
	}

	double MainEventManager::get_delta()
	{
		double time = al_get_time();
		double delta = time - last_time_check;
		last_time_check = time;
		return delta;
	}

	void MainEventManager::add_manager(EventManager* manager)
	{
		managers.push_back(manager);
	}

	void MainEventManager::dispatch_event(ALLEGRO_EVENT e)
	{
		agl::Event new_event;
		bool event_happened = true;

		switch (e.type)
		{
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			should_close = true;
			new_event.type = ART_EVENT_DISPLAY_CLOSE;
			break;

		case ALLEGRO_EVENT_TIMER:
			tick = true;
			event_happened = false;
			break;

		default:
			event_happened = false;
			break;
		}

		if (event_happened)
			for (auto& fn : event_functions)
				fn(new_event);
	}

	void MainEventManager::enable_mouse()
	{
		al_register_event_source(event_queue, al_get_mouse_event_source());
	}

	void MainEventManager::enable_keyboard()
	{
		al_register_event_source(event_queue, al_get_keyboard_event_source());
	}

	void MainEventManager::close()
	{
		should_close = true;
	}

}