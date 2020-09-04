#include "globals.h"

Game::Game()
{
	this->proto = Proto();

	AOLicon = al_load_bitmap("base/graphics/AOLIcon.png");

	proto.createWindow(500, 500, AOLicon, "Ages of Life", PROTO_WINDOW_FULLSCREEN);

	proto.setAppDataDir(L"AOL");

	auto dims = proto.getWindowDimensions();
	screenw = dims.first;
	screenh = dims.second;

	int sizes[1] = { 56 };
	segoeuib = Font("base/fonts/segoeuib.ttf", sizes, 1);
}

void Game::run()
{
	ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
	menu = true;
	while (true) {
		std::pair <bool, bool> rundata = proto.update();
		if (!rundata.first) {
			break;
		}

		if (rundata.second) {
			double dt = proto.step();
			update(dt);

			al_clear_to_color(black);

			draw();

			proto.finish_frame();
			al_flip_display();
		}
	}
}

void Game::quit()
{
	al_destroy_bitmap(AOLicon);
}

void Game::draw()
{
	if (menu) {
		mainmenuBg.draw();
		mainmenu.draw();
	}
}

void Game::update(double dt)
{
	if (menu)
		mainmenu.update();
}
