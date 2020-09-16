#include "globals.h"
#include "classes/GameObject.h"
#include "classes/Age.h"
#include <atlstr.h>
#include <lua.hpp>

Game::Game()
{
	this->proto = Proto();
	this->script = Script();

	AOLicon = al_load_bitmap("base/graphics/AOLIcon.png");

	proto.createWindow(1900, 1180, AOLicon, "Ages of Life", PROTO_WINDOW_FULLSCREEN);

	proto.setAppDataDir("AOL");

	auto dims = proto.getWindowDimensions();
	screenw = dims.first;
	screenh = dims.second;

	const int szm = 5;
	int sizes[szm] = { 18, 24, 27, 36, 56 };
	this->segoeuib = new Font("base/fonts/segoeuib.ttf", sizes, szm);
	this->time = al_get_time();

	this->lastmwpos = 0;

	this->proto.setInputCallback(std::bind(&Script::keyinput, &this->script, std::placeholders::_1, std::placeholders::_2));

	this->loadbutton.normal = new Image("base/graphics/icons/load-arrow/normal.png", DrawData{});
	this->loadbutton.hover = new Image("base/graphics/icons/load-arrow/hover.png", DrawData{});

	this->displayed_save_scale = {};

	this->logo = new Image("base/graphics/promethean-logo.png", DrawData{});

	spair sc = proto.getScale(this->logo, 25 * screenw / 32, 100);
	this->logo->setPosition((screenw - this->logo->width) / 2 - 65 * sc.first, screenh / 8);
	this->logo->setScale(sc.first, sc.first);

	load_basedata();
	proto.loadDictionary((fs::path)"base" / "locale" / (this->basedata.language + ".json"));

	script.load_colors();

	this->object_tree = {
		{"age", {}},
		{"technologies", {}}
	};

	this->prototype_tree = {
		{"age", {}},
		{"technologies", {}}
	};
}

Game::~Game()
{
	delete
		mainmenuBg,
		menubutton,
		defaultguiImage,
		closebutton,
		inputbutton,
		logo,
		segoeuib,
		name_input_label;

	delete
		mainmenu,
		playgui,
		aboutgui,
		creatorsgui,
		licensesgui;

	delete
		loadbutton.normal,
		loadbutton.hover;

	for (auto const& [key, value] : this->prototype_tree["ages"])
		delete this->prototype_tree["ages"][key];

	for (auto const& [key, value] : this->object_tree["ages"])
		delete this->object_tree["ages"][key];
}

void Game::run()
{
	this->load();

	std::cout << "Prototypes:\n";
	for (const auto& [key, value] : this->prototype_tree["age"]) {
		std::cout << key << " (" << ((AgePrototype*)value)->neighbor_specifier << "): " << ((AgePrototype*)value)->neighbor_age << "\n";
	}

	std::cout << "\nObjects:\n";
	for (const auto& [key, value] : this->object_tree["age"]) {
		Age* fixed = (Age*)value;
		std::cout << key << ": {\n";
		for (const auto& [field, fval] : fixed->dumped) {
			std::cout << "    " << fval.first << " " << field << " = " << fval.second << ",\n";
		}
		std::cout << "}\n";
	}

	this->menu = true;
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
	if (script.get_gui_opened(licensesgui)) {
		licensesgui->panels[0].setScroll(licensesgui->panels[0].scroll - amt*100);
	}
}

void Game::close_all_menu_guis()
{
	script.close_gui(aboutgui);
	script.close_gui(playgui);
}

void Game::draw_active_input()
{
	if (this->script.active_input && (int)this->time % 2 == 0) {
		ALLEGRO_TRANSFORM deft;
		al_identity_transform(&deft);
		al_use_transform(&deft);
		al_draw_filled_rectangle(
			this->script.active_input->data.x + this->script.active_input->width,
			this->script.active_input->data.y,
			this->script.active_input->data.x + this->script.active_input->width + 3,
			this->script.active_input->data.y + this->script.active_input->height,
			al_map_rgb(0, 0, 0)
		);
	}
}

void Game::load_basedata()
{
	std::ifstream ifs;
	if (proto.openAppDataFile("basedata.dat", &ifs)) {
		std::vector<DataCell> loaded_base = std::get<std::vector<DataCell>>(deserializeData(&ifs));
		std::vector<DataCell> loaded_saves = std::get<std::vector<DataCell>>(loaded_base[0]);
		for (auto sv = loaded_saves.begin(); sv != loaded_saves.end(); sv++) {
			this->basedata.saves.push_back(std::get<std::string>(*sv));
		}
		this->basedata.language = std::get<std::string>(loaded_base[1]);
	}
	ifs.close();
}

void Game::save_basedata()
{
	std::ofstream ofs;
	proto.openAppDataFile("basedata.dat", &ofs);

	std::vector<DataCell> sv_saves = {};
	for (auto sv = this->basedata.saves.begin(); sv != this->basedata.saves.end(); sv++) {
		sv_saves.push_back(*sv);
	}

	DataCell base_saved = std::vector<DataCell>{ sv_saves, this->basedata.language };

	serializeData(base_saved, &ofs);

	ofs.close();
}

void Game::get_displayed_saves()
{
	ALLEGRO_COLOR menutxtcol = al_map_rgb(238, 226, 93);

	std::vector<GUIElement> new_elements;
	lblvec new_labels;
	btnvec new_buttons;

	int y = .275 * screenh;
	int x = .125 * screenw + 50;

	int i = 0;

	new_labels.push_back(this->playgui->panels[1].labels[0]);
	new_elements.push_back(this->playgui->panels[1].elements[0]);

	for (auto savename = this->basedata.saves.begin(); savename != this->basedata.saves.end(); savename++) {
		new_labels.push_back(Label(*savename, DrawData{ x, y }, menutxtcol, segoeuib, 18, 0));
		Button newbtn = Button(x - 30, y + 2, 25, 25, *loadbutton.normal);
		Button newclosebtn = Button(x - 70, y + 2, 25, 25, *closebutton, std::bind(&Game::delete_save, this, i));

		newbtn.setHoverEffect(*loadbutton.hover);
		newclosebtn.setImageDisplayParameters(this->displayed_save_scale);

		new_buttons.push_back(newbtn);
		new_buttons.push_back(newclosebtn);

		new_elements.push_back(GUIElement{ PROTO_GUI_LABEL, i+1 });
		new_elements.push_back(GUIElement{ PROTO_GUI_BUTTON, 2*i });
		new_elements.push_back(GUIElement{ PROTO_GUI_BUTTON, 2*i+1 });
		i++;
		y += 25;
	}

	this->playgui->panels[1].labels.clear();
	this->playgui->panels[1].buttons.clear();
	this->playgui->panels[1].elements.clear();

	std::copy(new_labels.begin(), new_labels.end(), std::back_inserter(this->playgui->panels[1].labels));
	std::copy(new_buttons.begin(), new_buttons.end(), std::back_inserter(this->playgui->panels[1].buttons));
	std::copy(new_elements.begin(), new_elements.end(), std::back_inserter(this->playgui->panels[1].elements));
}

void Game::create_save()
{
	std::string name = this->name_input_label->text;

	if (!name.empty()) {
		if (script.validate_savename(name)) {
			if (std::find(this->basedata.saves.begin(), this->basedata.saves.end(), name) == this->basedata.saves.end()) {
				this->basedata.saves.push_back(name);
				proto.createDir(proto.AppDataPath / "saves" / name);
			}
		}
	}

	script.activate_input();
	this->name_input_label->setTextChunk("");
	this->get_displayed_saves();
}

void Game::delete_save(int index)
{
	proto.destroyRecursively(proto.AppDataPath / "saves" / this->basedata.saves[index]);
	this->basedata.saves.erase(this->basedata.saves.begin() + index);
	get_displayed_saves();
}

void Game::register_prototype(GameObjectPrototype* prototype, std::string group)
{
	this->prototype_tree[group].insert(std::make_pair(prototype->name, prototype));
}

void Game::load()
{
	load_prototypes();
	apply_changes();
	initialize_prototypes();

	createguis();
}

void Game::load_prototypes()
{
	setup_ages();
}

void Game::apply_changes()
{
}

void Game::initialize_prototypes()
{
	for (const auto& [key, value] : this->prototype_tree["age"]) {
		this->object_tree["age"].insert({ key, new Age((AgePrototype*)value) });
	}

	for (const auto& [key, value] : this->object_tree["age"]) {
		((Age*)value)->fill_dependencies(&this->object_tree);
	}
}

void Game::quit()
{
	this->save_basedata();
	al_destroy_bitmap(AOLicon);
}

void Game::draw()
{
	if (menu) {
		mainmenuBg->draw();
	}

	for (std::vector<GUI*>::iterator gui = script.guis.begin(); gui != script.guis.end(); gui++) {
		(*gui)->draw();
	}

	this->draw_active_input();
}

void Game::update(double dt)
{
	this->time += dt;

	if (proto.mouse.z - lastmwpos) {
		scroll(proto.mouse.z - lastmwpos);
		lastmwpos = proto.mouse.z;
	}

	for (int i = 0; i < script.opened_guis_amount; i++) {
		GUI* gui = script.guis[i];
		gui->update();
	}
}

void Game::change_loading_screen(std::string mes, float per)
{
	al_clear_to_color(loading_screen_bg);
	ALLEGRO_TRANSFORM def;
	al_identity_transform(&def);
	int py = 3 * screenh / 4;
	this->logo->draw();
	al_use_transform(&def);
	std::string message = mes + " " + std::to_string(int(100 * per)) + "%";
	int px = screenw / 2 - this->segoeuib->getWidth(24, message) / 2;
	al_draw_text(this->segoeuib->fonts[24], white, px, py - 50, 0, message.c_str());
	int bx = 2 * screenw / 6;
	al_draw_rectangle(bx - 5, py, bx - 5 + screenw / 3, py + 50, white, 2);
	int w = per * (screenw / 3 - 5);
	al_draw_filled_rectangle(bx, py + 5, bx + w, py + 45, white);
	al_flip_display();
}

void Game::change_loading_screen(std::string mes)
{
	al_clear_to_color(loading_screen_bg);
	ALLEGRO_TRANSFORM def;
	al_identity_transform(&def);
	int py = 3 * screenh / 4;
	this->logo->draw();
	al_use_transform(&def);
	int px = screenw / 2 - this->segoeuib->getWidth(24, mes) / 2;
	al_draw_text(this->segoeuib->fonts[24], white, px, py - 50, 0, mes.c_str());
	al_flip_display();
}