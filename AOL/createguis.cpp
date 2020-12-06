#include "globals.h"
#include <new>


void Game::createguis()
{
	change_loading_screen(proto.dict("loading-guis1"));

	mainmenuBg = new Image("base/graphics/background.png", DrawData{});
	menubutton = new Image("base/graphics/gui/mainmenubutton.png", DrawData{});
	defaultguiImage = new Image("base/graphics/gui/defaultgui.png", DrawData{});
	closebutton = new Image("base/graphics/gui/closeAge0.png", DrawData{});
	inputbutton = new Image("base/graphics/gui/input.png", DrawData{});


	auto sc = proto.getScale(this->closebutton, 25, 25);
	this->displayed_save_scale.sx = sc.first;
	this->displayed_save_scale.sy = sc.second;
	this->displayed_save_scale.create_transform();


	std::pair<float,float> s1, s2;

	s1 = proto.getScale(mainmenuBg, screenw, screenh);

	mainmenuBg->setScale(s1.first, s1.second);

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
				"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE "
				"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
				"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, "
				"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE "
				"SOFTWARE.<br><br>"

				"--- Optional exception to the license ---<br>"

				"As an exception, if, as a result of your compiling your source code, portions of this Software are embedded into a machine "
				"- executable object form of such source code, you may redistribute such embedded portions in such object form without including the above copyrightand permission notices.<br><br>";

			license_str += "Agui<br><br>Copyright (c) 2011 Joshua Larouche<br><br>"

				"License: (BSD)<br>"
				"Redistributionand use in sourceand binary forms, with or without "
				"modification, are permitted provided that the following conditions "
				"are met:<br>"
				"    1. Redistributions of source code must retain the above copyright "
				"notice, this list of conditionsand the following disclaimer.<br>"
				"    2. Redistributions in binary form must reproduce the above copyright "
				"notice, this list of conditions and the following disclaimer in "
				"the documentation and/or other materials provided with the distribution.<br>"
				"    3. Neither the name of Agui nor the names of its contributors may "
				"be used to endorse or promote products derived from this software "
				"without specific prior written permission.<br><br>"

				"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "
				"\"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT "
				"LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR "
				"A PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT "
				"OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,"
				"SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED "
				"TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR "
				"PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF "
				"LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING "
				"NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS "
				"SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.";
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


		Label* creators_l = new Label(creatorstext, DrawData{ 0, 0 }, { {"col1", menutxtcol}, {"col2", menu_orange } }, { {"segoe", segoeuib} }, "col1", "segoe", 24, 0, defguiW - 100, 2, 0);
		Label* licenses_l = new Label(license_str, DrawData{ 0, 0 }, { {"col1", menutxtcol} }, { {"segoe", segoeuib} }, "col1", "segoe", 18, 0, defguiW - 200, 2, 0);
		
		GUIPanel* creators_gp = new GUIPanel(
			{ GUIElement{PROTO_GUI_LABEL, 0} },
			{},
			{ *creators_l },
			{},
			1,
			1,
			false,
			black,
			PROTO_GUIPANEL_PERCENTDIMS
		);

		GUIPanel* licenses_gp = new GUIPanel(
			{ GUIElement{PROTO_GUI_LABEL, 0} },
			{},
			{ *licenses_l },
			{},
			1,
			1,
			false,
			lightyellow,
			PROTO_GUIPANEL_PERCENTDIMS | PROTO_GUIPANEL_SCROLLABLE
		);

		creatorsgui = new GUI(PROTO_GUI_LAYOUT_PANELS, { *creators_gp }, screenw / 3 - defguiW / 2 + 50, screenh / 8 + 3*defguiH/20 + 50, defguiW-100, 16*defguiH/20 - 50);
		licensesgui = new GUI(PROTO_GUI_LAYOUT_PANELS, { *licenses_gp }, screenw / 3 - defguiW / 2 + 50, screenh / 8 + 3 * defguiH / 20 + 50, defguiW-100, 16*defguiH/20 - 50);

		licensesgui->panels[0].setScrollLimits(0, licenses_l->height - licensesgui->panels[0].calculated_height);

		script.register_open_gui_function(creatorsgui, std::bind(&Script::close_gui, &this->script, licensesgui));
		script.register_open_gui_function(licensesgui, std::bind(&Script::close_gui, &this->script, creatorsgui));
	}


	// about gui
	{
		auto btns = proto.getScale(menubutton, defguiW / 2 - 100, defguiH / 15);

		Label* title = new Label(proto.dict("label-about"), DrawData{defguiW/2, defguiH/20}, menutxtcol, segoeuib, 36, PROTO_OFFSET_CENTER);

		Button* creatorsButton = new Button(100, 0, defguiW / 2 - 100, defguiH/15, *menubutton, std::bind(&Script::open_gui, &this->script, creatorsgui));
		Button* licensesButton = new Button(defguiW/2, 0, defguiW / 2 - 100, defguiH/15, *menubutton, std::bind(&Script::open_gui, &this->script, licensesgui));
		Button* closeB = new Button(defguiW - 100, 30, 70, 70, *closebutton);

		Label* creatorsLabel = new Label(proto.dict("label-creators"), DrawData{ defguiW / 4 + 50, defguiH / 30 }, menutxtcol, segoeuib, 27, PROTO_OFFSET_CENTER);
		Label* licensesLabel = new Label(proto.dict("label-licenses"), DrawData{ 3*defguiW / 4 - 50, defguiH / 30 }, menutxtcol, segoeuib, 27, PROTO_OFFSET_CENTER);

		creatorsButton->setHoverEffect(highlight1);
		licensesButton->setHoverEffect(highlight1);

		creatorsButton->setImageDisplayParameters(DrawData{ 0, 0, 0, btns.first, btns.second });
		licensesButton->setImageDisplayParameters(DrawData{ 0, 0, 0, btns.first, btns.second });

		GUIPanel* titlepanel = new GUIPanel(
			{ GUIElement{PROTO_GUI_LABEL, 0}, GUIElement{PROTO_GUI_BUTTON, 0} },
			{  },
			{ *title },
			{ *closeB },
			1,
			0.1,
			false,
			black,
			PROTO_GUIPANEL_PERCENTDIMS
		);

		GUIPanel* buttonspanel = new GUIPanel(
			{ GUIElement{PROTO_GUI_BUTTON, 0}, GUIElement{PROTO_GUI_BUTTON, 1}, GUIElement{PROTO_GUI_LABEL, 0}, GUIElement{PROTO_GUI_LABEL, 1} },
			{},
			{ *creatorsLabel, *licensesLabel },
			{ *creatorsButton, *licensesButton },
			1,
			0.05,
			false,
			black,
			PROTO_GUIPANEL_PERCENTDIMS
		);

		aboutgui = new GUI(PROTO_GUI_LAYOUT_PANELS, { *titlepanel, *buttonspanel }, screenw/3 - defguiW/2, screenh/8, defguiW, defguiH);
		aboutgui->setBackgroundImage(*defaultguiImage, DrawData{ 0,0,0,s2.first,s2.second });
		aboutgui->panels[0].buttons[0].setClickFunction(std::bind(&Script::close_gui, &this->script, aboutgui));
		script.register_open_gui_function(aboutgui, std::bind(&Game::close_all_menu_guis, this));
		script.register_close_gui_function(aboutgui, [this]() {script.close_gui(licensesgui); script.close_gui(creatorsgui);});

		delete title, creatorsButton, licensesButton, closeB, creatorsLabel, licensesLabel, titlepanel, buttonspanel;
	}

	// play menu gui
	{
		auto input_s = proto.getScale(inputbutton, 240, 50);
		auto div_s = proto.getScale(inputbutton, 4, 4 * defguiH / 5);

		Image* divider = new Image(*inputbutton);
		divider->setScale(div_s.first, div_s.second);
		divider->setPosition(-2, 0);

		Label* title = new Label(proto.dict("label-play"), DrawData{ defguiW / 2, defguiH / 20 }, menutxtcol, segoeuib, 36, PROTO_OFFSET_CENTER);
		Label* loadtitle = new Label(proto.dict("label-load-game"), DrawData{ defguiW / 4, defguiH / 20 }, menutxtcol, segoeuib, 24, PROTO_OFFSET_CENTER);
		Label* createtitle = new Label(proto.dict("label-create-game"), DrawData{ defguiW / 4, defguiH / 20 }, menutxtcol, segoeuib, 24, PROTO_OFFSET_CENTER);

		Button* closeB = new Button(defguiW - 100, 30, 70, 70, *closebutton);

		Label* newname = new Label(proto.dict("label-name"), DrawData{ 30, defguiH / 10 + 10 }, menutxtcol, segoeuib, 24, 0);
		Label* local_name_input_label = new Label("", DrawData{ defguiW / 2 - 260, defguiH / 10 + 14 }, black, segoeuib, 18, 0);
		Label* newsubmit_l = new Label(proto.dict("label-create"), DrawData{ defguiW / 4, defguiH / 10 + 115 }, menutxtcol, segoeuib, 24, PROTO_OFFSET_CENTER);

		Button* newinput = new Button(defguiW / 2 - 270, defguiH / 10 + 5, 240, 50, *inputbutton);
		Button* newsubmit = new Button(defguiW / 4 - 125, defguiH / 10 + 80, 250, 70, *menubutton, std::bind(&Game::create_save, this));

		newsubmit->setHoverEffect(highlight1);

		auto btns = proto.getScale(menubutton, 250, 70);

		DrawData btnscale = { 0, 0, 0, btns.first, btns.second };

		newinput->setImageDisplayParameters(DrawData{ 0, 0, 0, input_s.first, input_s.second });
		newsubmit->setImageDisplayParameters(btnscale);

		GUIPanel* titlepanel = new GUIPanel(
			{ GUIElement{PROTO_GUI_LABEL, 0}, GUIElement{PROTO_GUI_BUTTON, 0} },
			{},
			{ *title },
			{ *closeB },
			1.f,
			0.1f,
			false,
			black,
			PROTO_GUIPANEL_PERCENTDIMS
		);

		GUIPanel* loadpanel = new GUIPanel(
			{ GUIElement{PROTO_GUI_LABEL, 0} },
			{},
			{ *loadtitle },
			{},
			.5f,
			0.9f,
			false,
			black,
			PROTO_GUIPANEL_PERCENTDIMS
		);

		GUIPanel* createpanel = new GUIPanel(
			{
				GUIElement{PROTO_GUI_IMAGE, 0},
				GUIElement{PROTO_GUI_LABEL, 0},
				GUIElement{PROTO_GUI_BUTTON, 0},
				GUIElement{PROTO_GUI_LABEL, 1},
				GUIElement{PROTO_GUI_LABEL, 2},
				GUIElement{PROTO_GUI_BUTTON, 1},
				GUIElement{PROTO_GUI_LABEL, 3}
			},
			{ *divider },
			{ *createtitle, *newname, *local_name_input_label, *newsubmit_l },
			{ *newinput, *newsubmit },
			.5f,
			0.9f,
			false,
			black,
			PROTO_GUIPANEL_PERCENTDIMS
		);

		playgui = new GUI(PROTO_GUI_LAYOUT_PANELS, { *titlepanel, *loadpanel, *createpanel }, screenw / 3 - defguiW / 2, screenh / 8, defguiW, defguiH);
		playgui->setBackgroundImage(*defaultguiImage, DrawData{ 0, 0, 0, s2.first, s2.second });
		this->name_input_label = &playgui->panels[2].labels[2];
		playgui->panels[2].buttons[0].setClickFunction(std::bind(&Script::activate_input, &this->script, this->name_input_label));
		playgui->panels[0].buttons[0].setClickFunction(std::bind(&Script::close_gui, &this->script, playgui));
		script.register_close_gui_function(playgui, std::bind(&Script::activate_input, &this->script, (Label*)NULL));
		script.register_open_gui_function(playgui, std::bind(&Game::close_all_menu_guis, this));

		delete createpanel, loadpanel, divider, title, loadtitle, createtitle, closeB, newname, local_name_input_label, newinput, newsubmit, newsubmit_l;
	}

	// main menu gui
	{

		int mbx = .75 * screenw - 150;
		int mby = .5 * screenh - 120;

		int panel_amount = 6;
		int btnwidth = 250;
		int btnheight = 70;

		Button* playbutton = new Button(0, 0, btnwidth, btnheight, *menubutton, std::bind(&Script::open_gui, &this->script, playgui));
		Button* optionsbutton = new Button(0, 0, btnwidth, btnheight, *menubutton);
		Button* modsbutton = new Button(0, 0, btnwidth, btnheight, *menubutton);
		Button* updatebutton = new Button(0, 0, btnwidth, btnheight, *menubutton);
		Button* aboutbutton = new Button(0, 0, btnwidth, btnheight, *menubutton, std::bind(&Script::open_gui, &this->script, aboutgui));
		Button* quitbutton = new Button(0, 0, btnwidth, btnheight, *menubutton, std::bind(&Game::close, this));

		playbutton->setHoverEffect(highlight1);
		optionsbutton->setHoverEffect(highlight1);
		modsbutton->setHoverEffect(highlight1);
		updatebutton->setHoverEffect(highlight1);
		aboutbutton->setHoverEffect(highlight1);
		quitbutton->setHoverEffect(highlight1);

		auto btns = proto.getScale(menubutton, btnwidth, btnheight);

		DrawData btnscale = { 0, 0, 0, btns.first, btns.second };

		playbutton->setImageDisplayParameters(btnscale);
		optionsbutton->setImageDisplayParameters(btnscale);
		modsbutton->setImageDisplayParameters(btnscale);
		updatebutton->setImageDisplayParameters(btnscale);
		aboutbutton->setImageDisplayParameters(btnscale);
		quitbutton->setImageDisplayParameters(btnscale);

		DrawData labelpos = { btnwidth / 2, btnheight / 2 };
		int labsize = 36;

		Label* playlabel = new Label(proto.dict("label-play"), labelpos, menutxtcol, segoeuib, labsize, PROTO_OFFSET_CENTER);
		Label* optionslabel = new Label(proto.dict("label-options"), labelpos, menutxtcol, segoeuib, labsize, PROTO_OFFSET_CENTER);
		Label* modslabel = new Label(proto.dict("label-mods"), labelpos, menutxtcol, segoeuib, labsize, PROTO_OFFSET_CENTER);
		Label* updatelabel = new Label(proto.dict("label-updatelog"), labelpos, menutxtcol, segoeuib, labsize, PROTO_OFFSET_CENTER);
		Label* aboutlabel = new Label(proto.dict("label-about"), labelpos, menutxtcol, segoeuib, labsize, PROTO_OFFSET_CENTER);
		Label* quitlabel = new Label(proto.dict("label-quit"), labelpos, menutxtcol, segoeuib, labsize, PROTO_OFFSET_CENTER);

		GUIPanel* playpanel = new GUIPanel(
			std::vector<GUIElement> {
			GUIElement{ PROTO_GUI_BUTTON, 0 },
				GUIElement{ PROTO_GUI_LABEL, 0 }
		},
			{},
			{ *playlabel },
			{ *playbutton },
				1, 1.f / panel_amount, false, black, PROTO_GUIPANEL_PERCENTDIMS
				);

		GUIPanel* optionspanel = new GUIPanel(
			std::vector<GUIElement> {
			GUIElement{ PROTO_GUI_BUTTON, 0 },
				GUIElement{ PROTO_GUI_LABEL, 0 }
		},
			{},
			{ *optionslabel },
			{ *optionsbutton },
				1, 1.f / panel_amount, false, black, PROTO_GUIPANEL_PERCENTDIMS
				);
		GUIPanel* modspanel = new GUIPanel(
			std::vector<GUIElement> {
			GUIElement{ PROTO_GUI_BUTTON, 0 },
				GUIElement{ PROTO_GUI_LABEL, 0 }
		},
			{},
			{ *modslabel },
			{ *modsbutton },
				1, 1.f / panel_amount, false, black, PROTO_GUIPANEL_PERCENTDIMS
				);
		GUIPanel* updatepanel = new GUIPanel(
			std::vector<GUIElement> {
			GUIElement{ PROTO_GUI_BUTTON, 0 },
				GUIElement{ PROTO_GUI_LABEL, 0 }
		},
			{},
			{ *updatelabel },
			{ *updatebutton },
				1, 1.f / panel_amount, false, black, PROTO_GUIPANEL_PERCENTDIMS
				);
		GUIPanel* aboutpanel = new GUIPanel(
			std::vector<GUIElement> {
			GUIElement{ PROTO_GUI_BUTTON, 0 },
				GUIElement{ PROTO_GUI_LABEL, 0 }
		},
			{},
			{ *aboutlabel },
			{ *aboutbutton },
				1, 1.f / panel_amount, false, black, PROTO_GUIPANEL_PERCENTDIMS
				);
		GUIPanel* quitpanel = new GUIPanel(
			std::vector<GUIElement> {
			GUIElement{ PROTO_GUI_BUTTON, 0 },
				GUIElement{ PROTO_GUI_LABEL, 0 }
		},
			{},
			{ *quitlabel },
			{ *quitbutton },
				1, 1.f / panel_amount, false, black, PROTO_GUIPANEL_PERCENTDIMS
				);


		mainmenu = new GUI(PROTO_GUI_LAYOUT_PANELS, std::vector<GUIPanel>{*playpanel, * optionspanel, * modspanel, * updatepanel, * aboutpanel, * quitpanel}, mbx, mby, btnwidth, btnheight* panel_amount);

		delete playbutton, optionsbutton, modsbutton, aboutbutton, updatebutton, quitbutton, playlabel, optionslabel, modslabel, aboutlabel, updatelabel, quitlabel;
		delete playpanel, optionspanel, modspanel, aboutpanel, quitpanel;
	}

	/*/ escape gui
	{
		int btn_w = screenw / 4 - 50, btn_h = (screenh / 2 - 50)/3;

		auto esc_scale = proto.getScale(defaultguiImage, screenw / 4, screenh / 2);
		auto btns = proto.getScale(menubutton, btn_w, btn_h);

		int def_x = (screenw / 4 - btn_w) / 2;

		Button* save_btn = new Button(def_x, 0, btn_w, btn_h, *menubutton);
		Button* load_last_btn = new Button(def_x, 0, btn_w, btn_h, *menubutton);
		Button* quit_btn = new Button(def_x, 0, btn_w, btn_h, *menubutton);

		save_btn->setHoverEffect(highlight1);
		load_last_btn->setHoverEffect(highlight1);
		quit_btn->setHoverEffect(highlight1);

		Label* save_lbl = new Label(proto.dict("label-save"), DrawData{ screenw / 8, btn_h / 2 }, menutxtcol, segoeuib, 24, PROTO_OFFSET_CENTER);
		Label* load_last_lbl = new Label(proto.dict("label-load-last"), DrawData{ screenw / 8, btn_h / 2 }, menutxtcol, segoeuib, 24, PROTO_OFFSET_CENTER);
		Label* quit_lbl = new Label(proto.dict("label-quit"), DrawData{ screenw / 8, btn_h / 2 }, menutxtcol, segoeuib, 24, PROTO_OFFSET_CENTER);

		GUIPanel* save_panel = new GUIPanel(
			{GUIElement{PROTO_GUI_BUTTON, 0}, GUIElement{PROTO_GUI_LABEL, 1}},
			{},
			{save_lbl},
			{save_btn},
			1,

		)

		escapegui = new GUI(PROTO_GUI_LAYOUT_PANELS, {}, screenw/3 * 8, screenh/4, screenw/4, screenh/2);
		escapegui->setBackgroundImage(*defaultguiImage, DrawData{ 0, 0, 0, esc_scale.first, esc_scale.second });
	}
	//*/

	//script.open_gui(mainmenu);

	this->get_displayed_saves();
}