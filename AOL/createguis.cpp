#include "App.h"
#include "gui.h"
#include <new>


void App::createguis()
{
	change_loading_screen(dict->get("loading-guis1"));

	// Basic setup

	gui_layer->connect_guigroup(main_gui_group);

	main_gui_group->register_event_handler(event_handler);
	main_gui_group->register_graphics_handler(graphics_handler);

	main_gui_group->set_screen_dimensions(screenw, screenh);

	guassian_blur = new agl::Allegro5Shader(
		"core/shaders/guassian-blur-vertex.glsl",
		"core/shaders/guassian-blur-fragment.glsl"
	);

	agl::register_shader("guassian-blur", guassian_blur);

	main_menu_background = new agl::Allegro5Image("core/graphics/background.png");

	agl::register_image("main-menu-background", main_menu_background);

	agl::Allegro5Image* preview = new agl::Allegro5Image("core/graphics/gui/game-preview-placeholder.png");

	agl::register_image("game-preview-placeholder", preview);

	agl::Allegro5Image* close = new agl::Allegro5Image("core/graphics/gui/close.png");

	agl::register_image("close-button", close);

	// GUI setup

	// Main menu
	main_menu_gui_instance = new agl::Gui();

	main_menu_gui = new MainMenuGui(main_menu_gui_instance, screenw, screenh, dict);

	// New game
	new_game_gui_instance = new agl::Gui();
	new_game_gui_instance->set_z_index(1);

	new_game_gui = new NewGameGui(new_game_gui_instance, screenw, screenh, dict);

	// Binding

	// Main menu
	main_menu_gui->main_menu_buttons[5].set_click_function(
		std::bind(&App::close, this)
		);

	for (int i = 0; i < shortcuts[0].size(); i++)
	{
		main_menu_gui->debug_category_options[i].shortcut_label.set_text(
			shortcut_to_string(shortcuts[0][i].first, shortcuts[0][i].second)
		);

		main_menu_gui->debug_category_options[i].shortcut_listener.add_event_function(
			std::bind(&App::shortcut_capture, this, std::placeholders::_1)
		);
	}

	main_menu_gui->play_new_button.set_click_function(
		std::bind(&App::open_gui, this, std::placeholders::_1)
	);

	gui_openers.insert({ &main_menu_gui->play_new_button, new_game_gui_instance });

	main_menu_gui->play_delete_button.set_click_function(
		std::bind(&App::handle_delete_game, this, std::placeholders::_1)
	);

	// New Game
	new_game_gui->close_button.set_click_function(
		std::bind(&App::close_gui, this, std::placeholders::_1)
	);

	gui_closers.insert({ &new_game_gui->close_button, new_game_gui_instance });
	
	new_game_gui->create_btn.set_click_function(
		std::bind(&App::handle_create_game, this, std::placeholders::_1)
	);

	gui_closers.insert({ &new_game_gui->create_btn, new_game_gui_instance });

	// Opening the Main menu
	main_gui_group->add_gui(main_menu_gui_instance);
	reload_saves();
}