#include "App.h"
#include "gui.h"

#include <atlstr.h>
#include <agl/events.h>

agl::Color black(0, 0, 0);
agl::Color loading_screen_bg(20, 20, 20);
agl::Color white(255, 255, 255);

App::App()
{
	// Set non-pre-multiplied alpha
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

	AOLicon = al_load_bitmap("base/graphics/AOLIcon.png");

	event_manager = new art::Allegro5MainEventManager(60);
	keyboard_manager = new art::Allegro5KeyboardEventManager;
	display = new art::Allegro5Display(1900, 1180, "Ages of Life");
	event_manager->add_manager(keyboard_manager);

	event_manager->connect_display(display);
	event_manager->enable_mouse();
	event_manager->enable_keyboard();

	shortcuts = {
		{
			{ALLEGRO_KEY_F3, ALLEGRO_KEYMOD_CTRL}
		}
	};

	shortcut_functions = {
		{
			std::bind(&App::enable_debug, this, std::placeholders::_1)
		}
	};

	base_fs = new art::FileSystem;
	dict = new art::Dictionary;
	dict->set_filesystem(base_fs);
	dict->set_translation_dir_path("base/locale");
	dict->set_active_language("en");

	appdata_fs = new art::FileSystem(true);
	check_appdata();

	renderer = new art::Renderer;
	gui_layer = new art::Layer(0xff);

	renderer->add_layer(gui_layer);

	for (int i = 0; i < shortcuts.size(); i++)
	{
		for (int j = 0; j < shortcuts[i].size(); j++)
		{
			keyboard_manager->register_shortcut(
				shortcuts[i][j].first, shortcuts[i][j].second,
				shortcut_functions[i][j]
			);
		}
	}

	screenw = display->get_width();
	screenh = display->get_height();
	time = al_get_time();

	initialize_agl();
}

App::~App()
{
	delete main_menu_gui_instance;
	delete new_game_gui_instance;

	delete segoeUI_bold;

	delete bronze_age_hflow;
	delete horizontal_flow;
	delete bronze_age_scrollbar;

	delete renderer;

	delete event_handler;
	delete graphics_handler;

	delete guassian_blur;
	delete main_menu_background;

	delete event_manager;
	delete keyboard_manager;
	delete display;
	delete appdata_fs;
	delete base_fs;
}

void App::enable_debug(agl::Event e)
{
	agl::debug::debug = !agl::debug::debug;
}

void App::shortcut_capture(agl::Event e)
{
	if (e.type == AGL_EVENT_CHAR_INPUT && e.source->get_focus())
	{
		agl::Block* parent = e.source->get_parent();
		int opt = parent->get_child_index(e.source) - 1;
		int cat = (parent->get_parent()->get_child_index(parent) - 1) / 2;

		int mods = e.mods & keyboard_manager->supported_mods;

		agl::builtins::Label* lbl = (agl::builtins::Label*)e.source->get_child_by_index(0);

		lbl->set_text(shortcut_to_string(e.keycode, mods));
		auto s = shortcuts[cat][opt];

		keyboard_manager->remove_shortcut(s.first, s.second);
		keyboard_manager->register_shortcut(e.keycode, mods, shortcut_functions[cat][opt]);
		shortcuts[cat][opt] = std::make_pair(e.keycode, mods);

		e.source->set_focus(false);
	}
}

void App::open_gui(const agl::Event& e)
{
	auto open_it = gui_openers.find(e.source);

	if (open_it != gui_openers.end())
	{
		main_gui_group->add_gui(open_it->second);
	}
}

void App::close_gui(const agl::Event& e)
{
	auto open_it = gui_closers.find(e.source);

	if (open_it != gui_closers.end())
	{
		main_gui_group->remove_gui(open_it->second);
	}
}

void App::handle_create_game(const agl::Event& e)
{
	close_gui(e);

	std::string save_name = new_game_gui->name_input.get_value();
	std::string save_seed = new_game_gui->seed_input.get_value();

	if (can_save_be_created(save_name))
		create_game(save_name, get_seed(save_seed));

	new_game_gui->name_input.clear_value();
	new_game_gui->seed_input.clear_value();
}

bool App::can_save_be_created(const std::string& savename) const
{
	for (const auto& f : appdata_fs->get_files_in_directory("saves"))
	{
		if (f.is_directory() && f.path().filename().string() == savename)
			return false;
	}

	return true;
}

void App::reload_saves()
{
	main_menu_gui->play_game_selection_list.clear_elements();

	std::map<fs::file_time_type, fs::path, std::greater<fs::file_time_type>> saves;

	for (const auto& f : appdata_fs->get_files_in_directory("saves"))
	{
		if (f.is_directory())
		{
			saves.insert({ f.last_write_time(), f.path().filename() });
		}
	}

	for (const auto& [key, value] : saves)
	{
		main_menu_gui->play_game_selection_list.add_element(value.string());
	}
}

void App::create_game(const std::string& name, long long seed)
{
	appdata_fs->enter_dir("saves");

	appdata_fs->create_dir(name);
	appdata_fs->enter_dir(name);

	appdata_fs->exit_to("AOL");
	reload_saves();
}

long long App::get_seed(const std::string& seed_str)
{
	long long seed = 0;

	for (const char c : seed_str)
		seed += (long long)c - (int)'0';

	return seed;
}

void App::handle_delete_game(const agl::Event& e)
{
	if (!main_menu_gui->play_game_selection_list.get_selected_elements().empty())
	{
		int selected_index = *main_menu_gui->play_game_selection_list.get_selected_elements().begin();

		main_menu_gui->play_game_selection_list.unselect_child(selected_index);

		auto selected = main_menu_gui->play_game_selection_list.get_element_by_index(selected_index);

		std::string deleted = ((agl::builtins::Label*)selected->get_child_by_index(0))->get_text();
		
		main_menu_gui->play_game_selection_list.remove_element(selected_index);

		delete_game(deleted);
		reload_saves();
	}
}

void App::delete_game(const std::string& name)
{
	appdata_fs->delete_dir_recursively(((fs::path)"saves" / name).string());
}

void App::check_appdata()
{
	// AOL directory
	appdata_fs->create_dir_if_necessary("AOL");
	appdata_fs->enter_dir("AOL");

	// Saves directory
	appdata_fs->create_dir_if_necessary("saves");

	// Mods directory
	appdata_fs->create_dir_if_necessary("mods");
}

void App::run()
{
	load();

	while (true) {
		ALLEGRO_EVENT e = event_manager->get_event();
		if (event_manager->window_closed()) {
			break;
		}
		this->event_handler->handle_event(e);

		if (event_manager->clock_ticked()) {
			update(event_manager->get_delta());

			al_clear_to_color(graphics_handler->get_color(black));

			draw();

			al_flip_display();
		}
	}
}

void App::close()
{
	event_manager->close();
}

void App::load()
{
	createguis();
}

void App::quit()
{
	al_destroy_bitmap(AOLicon);
}

void App::initialize_agl()
{
	agl::debug::init();

	segoeUI_bold = new agl::Allegro5Font("base/fonts/segoeuib.ttf", { 18, 24, 27, 36, 56 });
	agl::set_default_font(segoeUI_bold);

	event_handler = new agl::Allegro5EventHandler;
	graphics_handler = new agl::Allegro5GraphicsHandler;

	main_gui_group = new agl::GuiGroup();

	setup_styles();
}

void App::draw()
{
	gui_layer->draw();
}

void App::update(double dt)
{
	this->time += dt;

	gui_layer->update(dt);

	this->event_handler->reset_event_queue();
}

void App::change_loading_screen(std::string mes, float per)
{
	al_clear_to_color(graphics_handler->get_color(loading_screen_bg));

	float py = 3.f * screenh / 4;
	std::string message = mes + " " + std::to_string(int(100 * per)) + "%";
	float px = (float)screenw / 2 - segoeUI_bold->get_width(24, message) / 2;
	float bx = 2.f * screenw / 6;
	float w = per * (screenw / 3 - 5);

	graphics_handler->draw_text({px, py - 50}, message, segoeUI_bold, 24, white);
	graphics_handler->draw_filled_rectangle({ agl::Point{ bx - 5, py }, (float)screenw / 3, 50.f }, white);
	graphics_handler->draw_rectangle({ agl::Point{bx, py + 5}, (float)w, 45.f }, 2, white);
	al_flip_display();
}

void App::change_loading_screen(std::string mes)
{
	al_clear_to_color(graphics_handler->get_color(loading_screen_bg));

	float py = 3.f * screenh / 4;
	float px = (float)screenw / 2 - segoeUI_bold->get_width(24, mes) / 2;

	graphics_handler->draw_text({ px, py - 50 }, mes, segoeUI_bold, 24, white);
	al_flip_display();
}
