#include "lib/Proto/Proto.h"
#include "globals.h"
#include "allegrolib.h"

int main()
{
	al_init();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();

	al_install_mouse();

	Game game;

	game.createguis();
	
	game.run();

	game.quit();

	al_uninstall_mouse();
}