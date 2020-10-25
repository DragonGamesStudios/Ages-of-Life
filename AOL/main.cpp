#include "lib/Proto/Proto.h"
#include "globals.h"
#include "allegrolib.h"

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
	catch(std::string& e) {
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
		al_show_native_message_box(NULL, "Exception Occured", "The game crashed. Please contact the developer.", ("Error: "+err).c_str(), "Ok", ALLEGRO_MESSAGEBOX_ERROR);
	}

	al_uninstall_mouse();
	al_uninstall_keyboard();
}