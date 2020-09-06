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

	al_install_mouse();

	bool AOLok = false;
	std::string err;

	try {

		Game game;

		game.createguis();

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
		std::wstring werr = s2ws(e.what());
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
		MessageBoxA(0, err.c_str(), "Exception", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
	}

	al_uninstall_mouse();
}