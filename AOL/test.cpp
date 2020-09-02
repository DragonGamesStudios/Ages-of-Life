#include "lib/Proto/Proto.h"
#include "allegrolib.h"

#include <iostream>
#include <assert.h>
#include <fstream>
#include <variant>


int main()
{
	al_init();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();

	al_install_mouse();

	ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
	ALLEGRO_COLOR blue = al_map_rgb(0, 255, 0);
	ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
	ALLEGRO_COLOR red = al_map_rgb(255, 0, 0);
	ALLEGRO_COLOR gray1 = al_map_rgb(64, 64, 64);
	ALLEGRO_COLOR gray2 = al_map_rgb(128, 128, 128);
	ALLEGRO_COLOR gray3 = al_map_rgb(192, 192, 192);

	Proto proto;
	
	proto.createWindow(1000, 1000, NULL, "Proto tests", 0);

	Image img("map-button.png", DrawData{ 50, 50 });
	proto.registerImage(&img);

	proto.loadDictionary("test.json");
	std::cout << proto.dict<1>("meh", { "maahah" }) << "\n";

	proto.setAppDataDir("thisistest");


	GUIPanel gp1(std::vector<GUIElement> { GUIElement{ PROTO_GUI_IMAGE, 0, 0 } }, imgvec{ img }, lblvec{}, btnvec{}, 0.5f, 0.5f, false, gray1, PROTO_GUIPANEL_BGCOLOR | PROTO_GUIPANEL_PERCENTDIMS);
	GUIPanel gp2(std::vector<GUIElement> { GUIElement{ PROTO_GUI_IMAGE, 0, 0 } }, imgvec{ img }, lblvec{}, btnvec{}, 0.5f, 0.5f, true, gray2, PROTO_GUIPANEL_BGCOLOR | PROTO_GUIPANEL_PERCENTDIMS);
	GUIPanel gp3(std::vector<GUIElement> { GUIElement{ PROTO_GUI_IMAGE, 0, 0 } }, imgvec{ img }, lblvec{}, btnvec{}, 0.5f, 0.5f, false, gray3, PROTO_GUIPANEL_BGCOLOR | PROTO_GUIPANEL_PERCENTDIMS);

	GUI testgui(PROTO_GUI_LAYOUT_PANELS, std::vector<GUIPanel>{gp1, gp2, gp3}, 100, 100, 500, 500);

	bool running = true;
	while (true) {
		bool tick;
		auto data = proto.update();
		running = data.first;
		tick = data.second;
		if (!running) break;

		if (tick) {
			al_clear_to_color(black);

			double dt = proto.step();
			testgui.draw();

			al_flip_display();
			proto.finish_frame();
		}
	}

	al_uninstall_mouse();

	return 0;
}