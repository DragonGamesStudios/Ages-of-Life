#include "globals.h"



void Game::createguis()
{
	ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
	ALLEGRO_COLOR highlight1 = al_map_rgb(255, 255, 200);
	ALLEGRO_COLOR menutxtcol = al_map_rgb(238, 226, 93);
	ALLEGRO_COLOR menu_orange = al_map_rgb(255, 50, 40);
	ALLEGRO_COLOR lightyellow = al_map_rgb(247, 252, 200);

	mainmenuBg = Image("base/graphics/background.png", DrawData{});
	menubutton = Image("base/graphics/gui/mainmenubutton.png", DrawData{});
	defaultguiImage.reload("base/graphics/gui/defaultgui.png", DrawData{});
	closssssebutton.reload("base/graphics/gui/closeAge0.png", DrawData{});


	std::pair<float,float> s1, s2;

	s1 = proto.getScale(mainmenuBg, screenw, screenh);

	mainmenuBg.setScale(s1.first, s1.second);

	int defguiW = .5 * screenw, defguiH = .75 * screenh;

	s2 = proto.getScale(defaultguiImage, defguiW, defguiH);

	//creators gui & licenses gui
	{
		std::string license_str = "";

		{
			license_str += "Allegro 5<br><br>"
				"Copyright(c) 2008-2010 the Allegro 5 Development Team<br><br>"
				"This software is provided 'as - is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.<br><br>"
				"Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:<br><br>"
				"    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.<br>"
				"    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.<br>"
				"    3. This notice may not be removed or altered from any source distribution.<br><br>";

			license_str += "Nlohmann Json<br><br>MIT License<br><br>"

				"Copyright(c) 2013 - 2020 Niels Lohmann<br><br>"

				"Permission is hereby granted, free of charge, to any person obtaining a copy "
				"of this softwareand associated documentation files (the \"Software\"), to deal "
				"in the Software without restriction, including without limitation the rights "
				"to use, copy, modify, merge, publish, distribute, sublicense, and//or sell "
				"copies of the Software, and to permit persons to whom the Software is "
				"furnished to do so, subject to the following conditions:<br><br>"

				"The above copyright noticeand this permission notice shall be included in all "
				"copies or substantial portions of the Software.<br><br>"

				"THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR "
				"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
				"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE "
				"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
				"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, "
				"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE "
				"SOFTWARE.<br><br>";

			license_str += "FMT<br><br>Copyright (c) 2012 - present, Victor Zverovich<br><br>"

				"Permission is hereby granted, free of charge, to any person obtaining a copy "
				"of this softwareand associated documentation files (the \"Software\"), to deal "
				"in the Software without restriction, including without limitation the rights "
				"to use, copy, modify, merge, publish, distribute, sublicense, and//or sell "
				"copies of the Software, and to permit persons to whom the Software is "
				"furnished to do so, subject to the following conditions:<br><br>"

				"The above copyright noticeand this permission notice shall be included in all "
				"copies or substantial portions of the Software.<br><br>"

				"THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR "
				"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
				"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE "
				"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
				"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, "
				"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE "
				"SOFTWARE.<br><br>"

				"--- Optional exception to the license ---<br>"

				"As an exception, if, as a result of your compiling your source code, portions of this Software are embedded into a machine "
				"- executable object form of such source code, you may redistribute such embedded portions in such object form without including the above copyrightand permission notices.<br><br>";
		}

		std::vector<std::pair<std::string, std::vector<std::string>>> creators = {
			{"Programming", {"Michal Margos"}},
			{"Graphics & 3D modelling", {"Ivan Titkov", "Michal Margos"}},
			{"Music", {"Ivan Titkov"}}
		};

		std::string creatorstext = "";

		for (auto elem = creators.begin(); elem != creators.end(); elem++) {
			creatorstext += "<style:col1:segoe:24>" + elem->first + "<br><style:col2:segoe:18>";

			for (auto person = elem->second.begin(); person != elem->second.end(); person++) {
				creatorstext += *person + "<br>";
			}

			creatorstext += "<br>";
		}


		Label creators_l = Label(creatorstext, DrawData{ 0, 0 }, { {"col1", menutxtcol}, {"col2", menu_orange } }, { {"segoe", &segoeuib} }, "col1", "segoe", 24, 0, defguiW - 100, 2, 0);
		Label licenses_l = Label(license_str, DrawData{ 0, 0 }, { {"col1", menutxtcol} }, { {"segoe", &segoeuib} }, "col1", "segoe", 18, 0, defguiW - 200, 2, 0);
		
		GUIPanel creators_gp = GUIPanel(
			{ GUIElement{PROTO_GUI_LABEL, 0} },
			{},
			{ creators_l },
			{},
			1,
			1,
			false,
			black,
			PROTO_GUIPANEL_PERCENTDIMS
		);

		GUIPanel licenses_gp = GUIPanel(
			{ GUIElement{PROTO_GUI_LABEL, 0} },
			{},
			{ licenses_l },
			{},
			1,
			1,
			false,
			lightyellow,
			PROTO_GUIPANEL_PERCENTDIMS | PROTO_GUIPANEL_SCROLLABLE
		);

		creatorsgui = GUI(PROTO_GUI_LAYOUT_PANELS, { creators_gp }, screenw / 3 - defguiW / 2 + 50, screenh / 8 + 3*defguiH/20 + 50, defguiW-100, 16*defguiH/20 - 50);
		licensesgui = GUI(PROTO_GUI_LAYOUT_PANELS, { licenses_gp }, screenw / 3 - defguiW / 2 + 50, screenh / 8 + 3 * defguiH / 20 + 50, defguiW-100, 16*defguiH/20 - 50);

		licensesgui.panels[0].setScrollLimits(0, licenses_l.height - licensesgui.panels[0].calculated_height);

		script.register_open_gui_function(&creatorsgui, std::bind(&Script::close_gui, &this->script, &licensesgui));
		script.register_open_gui_function(&licensesgui, std::bind(&Script::close_gui, &this->script, &creatorsgui));
	}

	// about gui
	{

		auto btns = proto.getScale(menubutton, defguiW / 2 - 100, defguiH / 15);

		Label title = Label(proto.dict("mainlabel-about"), DrawData{defguiW/2, defguiH/20}, menutxtcol, &segoeuib, 36, PROTO_OFFSET_CENTER);

		Button creatorsButton = Button(100, 0, defguiW / 2 - 100, defguiH/15, menubutton, std::bind(&Script::open_gui, &this->script, &creatorsgui));
		Button licensesButton = Button(defguiW/2, 0, defguiW / 2 - 100, defguiH/15, menubutton, std::bind(&Script::open_gui, &this->script, &licensesgui));
		Button closeB = Button(defguiW - 100, 30, 70, 70, closssssebutton, std::bind(&Script::close_gui, &this->script, &aboutgui));

		Label creatorsLabel = Label(proto.dict("label-creators"), DrawData{ defguiW / 4 + 50, defguiH / 30 }, menutxtcol, &segoeuib, 27, PROTO_OFFSET_CENTER);
		Label licensesLabel = Label(proto.dict("label-licenses"), DrawData{ 3*defguiW / 4 - 50, defguiH / 30 }, menutxtcol, &segoeuib, 27, PROTO_OFFSET_CENTER);

		creatorsButton.setHoverEffect(highlight1);
		licensesButton.setHoverEffect(highlight1);

		creatorsButton.setImageDisplayParameters(DrawData{ 0, 0, 0, btns.first, btns.second });
		licensesButton.setImageDisplayParameters(DrawData{ 0, 0, 0, btns.first, btns.second });

		GUIPanel titlepanel = GUIPanel(
			{ GUIElement{PROTO_GUI_LABEL, 0}, GUIElement{PROTO_GUI_BUTTON, 0} },
			{  },
			{ title },
			{ closeB },
			1,
			0.1,
			false,
			black,
			PROTO_GUIPANEL_PERCENTDIMS
		);

		GUIPanel buttonspanel = GUIPanel(
			{ GUIElement{PROTO_GUI_BUTTON, 0}, GUIElement{PROTO_GUI_BUTTON, 1}, GUIElement{PROTO_GUI_LABEL, 0}, GUIElement{PROTO_GUI_LABEL, 1} },
			{},
			{ creatorsLabel, licensesLabel },
			{ creatorsButton, licensesButton },
			1,
			0.05,
			false,
			black,
			PROTO_GUIPANEL_PERCENTDIMS
		);

		aboutgui = GUI(PROTO_GUI_LAYOUT_PANELS, { titlepanel, buttonspanel }, screenw/3 - defguiW/2, screenh/8, defguiW, defguiH);
		aboutgui.setBackgroundImage(defaultguiImage, DrawData{ 0,0,0,s2.first,s2.second });
		script.register_close_gui_function(&aboutgui, [this]() {script.close_gui(&licensesgui); script.close_gui(&creatorsgui); });
	}

	// play menu gui
	{

	}

	// main menu gui
	{

		int mbx = .75 * screenw - 150;
		int mby = .5 * screenh - 120;

		int panel_amount = 6;
		int btnwidth = 250;
		int btnheight = 70;

		Button playbutton(0, 0, btnwidth, btnheight, menubutton);
		Button optionsbutton(0, 0, btnwidth, btnheight, menubutton);
		Button modsbutton(0, 0, btnwidth, btnheight, menubutton);
		Button updatebutton(0, 0, btnwidth, btnheight, menubutton);
		Button aboutbutton(0, 0, btnwidth, btnheight, menubutton, std::bind(&Script::open_gui, &this->script, &aboutgui));
		Button quitbutton(0, 0, btnwidth, btnheight, menubutton, std::bind(&Game::close, this));

		playbutton.setHoverEffect(highlight1);
		optionsbutton.setHoverEffect(highlight1);
		modsbutton.setHoverEffect(highlight1);
		updatebutton.setHoverEffect(highlight1);
		aboutbutton.setHoverEffect(highlight1);
		quitbutton.setHoverEffect(highlight1);

		auto btns = proto.getScale(menubutton, btnwidth, btnheight);

		DrawData btnscale = { 0, 0, 0, btns.first, btns.second };

		playbutton.setImageDisplayParameters(btnscale);
		optionsbutton.setImageDisplayParameters(btnscale);
		modsbutton.setImageDisplayParameters(btnscale);
		updatebutton.setImageDisplayParameters(btnscale);
		aboutbutton.setImageDisplayParameters(btnscale);
		quitbutton.setImageDisplayParameters(btnscale);

		DrawData labelpos = { btnwidth / 2, btnheight / 2 };
		int labsize = 36;

		Label playlabel(proto.dict("label-play"), labelpos, menutxtcol, &segoeuib, labsize, PROTO_OFFSET_CENTER);
		Label optionslabel(proto.dict("label-options"), labelpos, menutxtcol, &segoeuib, labsize, PROTO_OFFSET_CENTER);
		Label modslabel(proto.dict("label-mods"), labelpos, menutxtcol, &segoeuib, labsize, PROTO_OFFSET_CENTER);
		Label updatelabel(proto.dict("label-updatelog"), labelpos, menutxtcol, &segoeuib, labsize, PROTO_OFFSET_CENTER);
		Label aboutlabel(proto.dict("label-about"), labelpos, menutxtcol, &segoeuib, labsize, PROTO_OFFSET_CENTER);
		Label quitlabel(proto.dict("label-quit"), labelpos, menutxtcol, &segoeuib, labsize, PROTO_OFFSET_CENTER);

		GUIPanel playpanel(
			std::vector<GUIElement> {
			GUIElement{ PROTO_GUI_BUTTON, 0 },
				GUIElement{ PROTO_GUI_LABEL, 0 }
		},
			{},
			{playlabel},
			{ playbutton },
				1, 1.f / panel_amount, false, black, PROTO_GUIPANEL_PERCENTDIMS
				);

		GUIPanel optionspanel(
			std::vector<GUIElement> {
			GUIElement{ PROTO_GUI_BUTTON, 0 },
				GUIElement{ PROTO_GUI_LABEL, 0 }
		},
			{},
			{optionslabel},
			{ optionsbutton },
				1, 1.f / panel_amount, false, black, PROTO_GUIPANEL_PERCENTDIMS
				);
		GUIPanel modspanel(
			std::vector<GUIElement> {
			GUIElement{ PROTO_GUI_BUTTON, 0 },
				GUIElement{ PROTO_GUI_LABEL, 0 }
		},
			{},
			{ modslabel },
			{ modsbutton },
				1, 1.f / panel_amount, false, black, PROTO_GUIPANEL_PERCENTDIMS
				);
		GUIPanel updatepanel(
			std::vector<GUIElement> {
			GUIElement{ PROTO_GUI_BUTTON, 0 },
				GUIElement{ PROTO_GUI_LABEL, 0 }
		},
			{},
			{updatelabel},
			{ updatebutton },
				1, 1.f / panel_amount, false, black, PROTO_GUIPANEL_PERCENTDIMS
				);
		GUIPanel aboutpanel(
			std::vector<GUIElement> {
			GUIElement{ PROTO_GUI_BUTTON, 0 },
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


		mainmenu = GUI(PROTO_GUI_LAYOUT_PANELS, std::vector<GUIPanel>{playpanel, optionspanel, modspanel, updatepanel, aboutpanel, quitpanel}, mbx, mby, btnwidth, btnheight*panel_amount);
	}

	script.open_gui(&mainmenu);
}