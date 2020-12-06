#include "lib/Proto/Proto.h"
#include "globals.h"
#include "allegrolib.h"

//#define DOM_CONSTRUCTOR

#ifdef DOM_CONSTRUCTOR
#include "lib/Proto/DOM.h"
#endif

void AOL_main()
{
	bool AOLok = false;
	std::string err;

	try {
		define_colors();
		define_default_transform();

		Game game;
		
		game.run();

		game.quit();

		AOLok = true;
	}
	catch (std::string& e) {
		//std::wstring werr = s2ws(e);
		//err = werr.c_str();
		err = e;
	}
	catch (std::exception& e) {
		//std::cout << e.what();
		//err = werr.c_str();
		err = e.what();
	}
	catch (const char*& e) {
		//std::wstring werr = s2ws(e);
		//err = werr.c_str();
		err = e;
	}
	catch (...) {
		//err = L"An error occured.";
		err = "An error occured";
	}

	if (!AOLok) {
		al_show_native_message_box(NULL, "Exception Occured", "The game crashed. Please contact the developer.", ("Error: " + err).c_str(), "Ok", ALLEGRO_MESSAGEBOX_ERROR);
	}
}

#ifdef DOM_CONSTRUCTOR
void DOMGUI_constr()

{
	Proto proto;

	define_colors();
	define_default_transform();

	proto.createWindow(1000, 1000, 0, "DOM GUI creator", 0);

	DOM_element* base = new DOM_element();

	json base_ruleset = {
		{"padding", "20px"},
		{"background-color", "rgba(200, 0, 0, 128)"},
		{"height", "300px"},
		{"overflow", "scroll"}
	};

	base->set_rulesets({ base_ruleset });

	DOM_element* maintest = new DOM_element();

	json maintest_ruleset = {
		{"background-color", "#fe4"},
		{"height", "200px"},
		{"background-image", "url(base/graphics/gui/input.png)"},
		{"background-repeat", "repeat repeat"},
	};

	maintest->set_rulesets({ maintest_ruleset });

	base->add_child(maintest);

	DOM_document* testgui = new DOM_document(base, 700, 700);

	testgui->calculate();

	while (true) {
		std::pair <bool, bool> rundata = proto.update();
		if (!rundata.first) {
			break;
		}

		testgui->dispatch_event(proto.last_event);

		if (rundata.second) {
			double dt = proto.step();

			testgui->update(dt, 150, 150);

			al_clear_to_color(predefined_colors["black"]);

			testgui->draw(150, 150);

			proto.finish_frame();
			al_flip_display();
		}
	}

	delete testgui;

	DOM_quit();
}
#endif

int main()
{
	al_init();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();
	al_init_native_dialog_addon();

	al_install_mouse();
	al_install_keyboard();

#ifndef	DOM_CONSTRUCTOR // DOM gui system testing
	AOL_main();
#else
	DOMGUI_constr();
#endif

	al_uninstall_mouse();
	al_uninstall_keyboard();
}