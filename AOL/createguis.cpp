#include "globals.h"
#include <new>


void Game::createguis()
{
	change_loading_screen(proto.dict("loading-guis1"));

	gui_layer->connect_guigroup(main_gui_group);

	guassian_blur = new agl::Allegro5Shader(
		"core/shaders/guassian-blur-vertex.glsl",
		"core/shaders/guassian-blur-fragment.glsl"
	);

	agl::register_shader("guassian-blur", guassian_blur);

	main_menu_background = new agl::Allegro5Image("base/graphics/background.png");

	agl::register_image("main-menu-background", main_menu_background);

	agl::Allegro5Image* preview = new agl::Allegro5Image("base/graphics/gui/game-preview-placeholder.png");

	agl::register_image("game-preview-placeholder", preview);

	main_menu_gui_instance = new agl::Gui();
	main_gui_group->register_event_handler(event_handler);
	main_gui_group->register_graphics_handler(graphics_handler);
	main_gui_group->add_gui(main_menu_gui_instance);

	main_gui_group->set_screen_dimensions(screenw, screenh);

	main_menu_gui = new MainMenuGui(main_menu_gui_instance, screenw, screenh, dict);

	main_menu_gui->main_menu_buttons[5].set_click_function(
		std::bind(&Game::close, this)
		);

	for (int i = 0; i < shortcuts[0].size(); i++)
	{
		main_menu_gui->debug_category_options[i].shortcut_label.set_text(
			shortcut_to_string(shortcuts[0][i].first, shortcuts[0][i].second)
		);

		main_menu_gui->debug_category_options[i].shortcut_listener.add_event_function(
			std::bind(&Game::shortcut_capture, this, std::placeholders::_1)
		);
	}
}