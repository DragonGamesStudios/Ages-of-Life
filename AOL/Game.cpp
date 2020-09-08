#include "globals.h"

Game::Game()
{
	this->proto = Proto();
	this->script = Script();

	AOLicon = al_load_bitmap("base/graphics/AOLIcon.png");

	proto.createWindow(1900, 1180, AOLicon, "Ages of Life", 0);

	proto.setAppDataDir(L"AOL");


	proto.loadDictionary("base/locale/en.json");

	auto dims = proto.getWindowDimensions();
	screenw = dims.first;
	screenh = dims.second;

	const int szm = 5;
	int sizes[szm] = { 18, 24, 27, 36, 56 };
	segoeuib.loadSizes("base/fonts/segoeuib.ttf", sizes, szm);

	this->lastmwpos = 0;
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

void Game::close()
{
	proto.close();
}

void Game::scroll(int amt)
{
	if (script.get_gui_opened(&licensesgui)) {
		licensesgui.panels[0].setScroll(licensesgui.panels[0].scroll - amt*100);
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
	}

	for (std::vector<GUI*>::iterator gui = script.guis.begin(); gui != script.guis.end(); gui++) {
		(*gui)->draw();
	}
}

void Game::update(double dt)
{
	if (proto.mouse.z - lastmwpos) {
		scroll(proto.mouse.z - lastmwpos);
		lastmwpos = proto.mouse.z;
	}

	for (int i = 0; i < script.opened_guis_amount; i++) {
		GUI* gui = script.guis[i];
		gui->update();
	}
}
