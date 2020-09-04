#include "globals.h"



void Game::createguis()
{
	ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
	ALLEGRO_COLOR highlight1 = al_map_rgb(255, 255, 200);
	ALLEGRO_COLOR menutxtcol = al_map_rgb(238, 226, 93);

	mainmenuBg = Image("base/graphics/background.png", DrawData{});
	proto.registerImage(&mainmenuBg);
	menubutton = Image("base/graphics/gui/mainmenubutton.png", DrawData{});
	proto.registerImage(&menubutton);


	std::pair<float,float> s1;

	s1 = proto.getScale(mainmenuBg, screenw, screenh);

	mainmenuBg.setScale(s1.first, s1.second);

	// main menu gui
	{

		int mbx = .75 * screenw - 150;
		int mby = .5 * screenh - 120;

		int panel_amount = 5;

		Button playbutton(0, 0, 300, 80, menubutton);
		Button optionsbutton(0, 0, 300, 80, menubutton);
		Button updatebutton(0, 0, 300, 80, menubutton);
		Button aboutbutton(0, 0, 300, 80, menubutton);
		Button quitbutton(0, 0, 300, 80, menubutton);

		playbutton.setHoverEffect(highlight1);
		optionsbutton.setHoverEffect(highlight1);
		updatebutton.setHoverEffect(highlight1);
		aboutbutton.setHoverEffect(highlight1);
		quitbutton.setHoverEffect(highlight1);

		Label playlabel("Play", DrawData{ 150, 40 }, menutxtcol, &segoeuib, 56, PROTO_OFFSET_CENTER);
		Label optionslabel("Options", DrawData{ 150, 40 }, menutxtcol, &segoeuib, 56, PROTO_OFFSET_CENTER);
		Label updatelabel("Update log", DrawData{ 150, 40 }, menutxtcol, &segoeuib, 56, PROTO_OFFSET_CENTER);
		Label aboutlabel("About", DrawData{ 150, 40 }, menutxtcol, &segoeuib, 56, PROTO_OFFSET_CENTER);
		Label quitlabel("Quit", DrawData{ 150, 40 }, menutxtcol, &segoeuib, 56, PROTO_OFFSET_CENTER);

		GUIPanel playpanel(
			std::vector<GUIElement> {
			GUIElement{ 0, PROTO_GUI_BUTTON },
				GUIElement{ PROTO_GUI_LABEL, 0 }
		},
			{},
			{playlabel},
			{ playbutton },
				1, 1.f / panel_amount, false, black, PROTO_GUIPANEL_PERCENTDIMS
				);

		GUIPanel optionspanel(
			std::vector<GUIElement> {
			GUIElement{ 0, PROTO_GUI_BUTTON },
				GUIElement{ PROTO_GUI_LABEL, 0 }
		},
			{},
			{optionslabel},
			{ optionsbutton },
				1, 1.f / panel_amount, false, black, PROTO_GUIPANEL_PERCENTDIMS
				);
		GUIPanel updatepanel(
			std::vector<GUIElement> {
			GUIElement{ 0, PROTO_GUI_BUTTON },
				GUIElement{ PROTO_GUI_LABEL, 0 }
		},
			{},
			{updatelabel},
			{ updatebutton },
				1, 1.f / panel_amount, false, black, PROTO_GUIPANEL_PERCENTDIMS
				);
		GUIPanel aboutpanel(
			std::vector<GUIElement> {
			GUIElement{ 0, PROTO_GUI_BUTTON },
				GUIElement{ PROTO_GUI_LABEL, 0 }
		},
			{},
			{aboutlabel},
			{ aboutbutton },
				1, 1.f / panel_amount, false, black, PROTO_GUIPANEL_PERCENTDIMS
				);
		GUIPanel quitpanel(
			std::vector<GUIElement> {
				GUIElement{ PROTO_GUI_BUTTON, 0 },
				GUIElement{PROTO_GUI_LABEL, 0}
			},
			{},
			{quitlabel},
			{ quitbutton },
			1, 1.f / panel_amount, false, black, PROTO_GUIPANEL_PERCENTDIMS
		);


		mainmenu = GUI(PROTO_GUI_LAYOUT_PANELS, std::vector<GUIPanel>{playpanel, optionspanel, updatepanel, aboutpanel, quitpanel}, mbx, mby, 300, 400);
	}
}