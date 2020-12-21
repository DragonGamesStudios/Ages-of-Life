#include "..\include\ARTAllegro5Backend\Allegro5MainEventManager.h"
#include "..\include\ARTAllegro5Backend\Allegro5Display.h"

#include <agl/events.h>

namespace art
{
	Allegro5MainEventManager::Allegro5MainEventManager(int fps, bool start_timer)
		: MainEventManager(), Allegro5EventManager()
	{
		event_queue = 0;
		event_queue = al_create_event_queue();

		timer = al_create_timer(1.f / fps);

		al_register_event_source(event_queue, al_get_timer_event_source(timer));

		tick = false;
		should_close = false;

		last_time_check = al_get_time();

		if (start_timer)
			al_start_timer(timer);
	}

	Allegro5MainEventManager::~Allegro5MainEventManager()
	{
		al_destroy_timer(timer);
		al_destroy_event_queue(event_queue);
	}

	void Allegro5MainEventManager::connect_display(const Display* display_)
	{
		MainEventManager::connect_display(display_);

		al_register_event_source(
			event_queue, al_get_display_event_source(((Allegro5Display*)display)->get_al_display())
		);
	}

	void Allegro5MainEventManager::dispatch_event(const ALLEGRO_EVENT& e)
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
			EventManager::dispatch_event(new_event);
	}

	ALLEGRO_EVENT Allegro5MainEventManager::get_event()
	{
		ALLEGRO_EVENT e;
		al_wait_for_event(event_queue, &e);

		dispatch_event(e);

		for (auto& manager : managers)
			((Allegro5EventManager*)manager)->dispatch_event(e);

		return e;
	}

	bool Allegro5MainEventManager::clock_ticked()
	{
		bool ret = tick;
		tick = false;
		return ret;
	}

	bool Allegro5MainEventManager::window_closed() const
	{
		return should_close;
	}

	double Allegro5MainEventManager::get_delta()
	{
		double time = al_get_time();
		double delta = time - last_time_check;
		last_time_check = time;
		return delta;
	}

	void Allegro5MainEventManager::enable_mouse()
	{
		al_register_event_source(event_queue, al_get_mouse_event_source());
	}

	void Allegro5MainEventManager::enable_keyboard()
	{
		al_register_event_source(event_queue, al_get_keyboard_event_source());
	}

	void Allegro5MainEventManager::close()
	{
		should_close = true;
	}

}