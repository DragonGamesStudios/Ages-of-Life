#include "globals.h"
#include "classes/GameObject.h"
#include "classes/Age.h"
#include "classes/Technology.h"
#include <atlstr.h>
#include <lua.hpp>

Game::Game()
{
	path_templates.insert({ "base", "base" });

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

	this->playing = false;

	load_basedata();
	proto.loadDictionary((fs::path)"base" / "locale" / (this->basedata.language + ".json"));

	script.load_colors();

	this->object_tree = {
		{"age", {}},
		{"technology", {}}
	};

	this->prototype_tree = {
		{"age", {}},
		{"technology", {}}
	};

	DOM_element* test_root = new DOM_element;

	json testrlst = {
		{"background-color", "#3ef"},
		{"height", "50vw"},
		{"border-style-left", "double"},
		{"border-width", "5px"},
		{"border-color", "green"}
	};

	test_root->set_rulesets({testrlst});

	DOM_element* another_test = new DOM_element;
	//DOM_element* another_test2 = new DOM_element;

	json another = {
		{"height", "30%"},
		{"background-color", "brown"},
	};

	another_test->set_rulesets({ another });
	//another_test2->set_rulesets({ another });

	test_root->add_child(another_test);
	//test_root->add_child(another_test2);

	testgui = new DOM_document(test_root);
	testgui->calculate();
	std::cout << "Loaded\n";
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

	std::vector<std::string> deleted_keys = { "ages", "technology" };

	for (auto del = deleted_keys.begin(); del != deleted_keys.end(); del++)
	{
		for (auto const& [key, value] : this->prototype_tree[*del])
			delete this->prototype_tree[*del][key];

		for (auto const& [key, value] : this->object_tree[*del])
			delete this->object_tree[*del][key];
	}
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
		print_dumpable(fixed->dumped);
	}

	std::cout << "\nObjects:\n";
	for (const auto& [key, value] : this->object_tree["technology"]) {
		Technology* fixed = (Technology*)value;
		print_dumpable(fixed->dumped);
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
	api_load_datafile("core/setup-age.lua");
	api_load_datafile("core/setup-technology.lua");
}

void Game::apply_changes()
{
}

void Game::initialize_prototypes()
{
	for (const auto& [key, value] : this->prototype_tree["age"]) {
		this->object_tree["age"].insert({ key, new Age((AgePrototype*)value) });
	}


	//creating master technologies for ages
	for (const auto& [key, value] : this->object_tree["age"]) {
		Age* fixed = (Age*)value;
		//if there is no previous age, use the "beginning" icon
		std::string correct_icon_path = "";
		if (fixed->previous_age) {
			correct_icon_path = fixed->previous_age->prototype->icon_path;
		}
		else {
			correct_icon_path = "__base__/graphics/technology/icons/beginning.png";
		}

		TechnologyPrototype* newtech = new TechnologyPrototype(
			fixed->name + ":master",
			fixed->name,
			correct_icon_path,
			-1,
			0,
			{},
			"0",
			0,
			0,
			false,
			true,
			false,
			true
		);

		this->register_prototype(newtech, "technology");
	}

	for (const auto& [key, value] : this->prototype_tree["technology"]) {
		//update parents for root technologies
		TechnologyPrototype* fixed = (TechnologyPrototype*)value;
		if (fixed->is_root) fixed->parents.push_back(fixed->age + ":master");

		this->object_tree["technology"].insert({ key, new Technology(fixed) });
	}

	for (const auto& [key, value] : this->object_tree["age"]) {
		((Age*)value)->fill_dependencies(&this->object_tree);
	}

	for (const auto& [key, value] : this->object_tree["technology"]) {
		((Technology*)value)->fill_dependencies(&this->object_tree);
	}


	// Post everything - finalization
	auto groupmap = this->costruct_groups();
	auto plannedmap = this->plan_groups(&groupmap);
	this->render_groups(&groupmap, &plannedmap);
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

	testgui->draw(100, 100);

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

std::unordered_map<Technology*, std::vector<Technology*>> Game::costruct_groups()
{
	std::unordered_map<Technology*, std::vector<Technology*>> groups = {};

	// Give every technology its field in map. The value is vector of technology poitners - those are groups. Groups are stored under technology pointer like that:
	/*
	ptr1 = group of ptr1
	ptr2 = group of ptr2
	*/
	// The groups don't contain root technologies, since they are stored as keys
	for (const auto& [key, gbj] : this->object_tree["technology"])
	{
		Technology* technology = (Technology*)gbj;
		groups.insert({ technology, {} });
	}

	for (const auto& [key, gbj] : this->object_tree["technology"])
	{
		Technology* technology = (Technology*)gbj;
		// For every technology get its group (see Technology::get_group()) and under that key push itself
		groups[technology->get_group()].push_back(technology);
	}

	return groups;
}

std::unordered_map<Technology*, TechnologyGroupData> Game::plan_groups(std::unordered_map<Technology*, std::vector<Technology*>>* groups)
{
	std::unordered_map<Technology*, TechnologyGroupData> result = {};

	for (auto p = groups->begin(); p != groups->end(); p++)
	{
		result.insert({ p->first, {} });
	}

	for (const auto& [key, g_age] : this->object_tree["age"])
	{
		auto fixed = (Age*)g_age;

		this->plan_group(groups, &result, (Technology*)this->object_tree["technology"][fixed->name + ":master"]);
	}

	return result;
}

void Game::plan_group(std::unordered_map<Technology*, std::vector<Technology*>>* groups, std::unordered_map<Technology*, TechnologyGroupData>* groupdata, Technology* key)
{
	std::map<int, std::pair<std::vector<Technology*>, int>> technologies = {};

	for (auto technology : groups->at(key))
	{
		// Plan children groups first
		plan_group(groups, groupdata, technology);
		if (technologies.find(technology->level) == technologies.end())
		{
			// Create technology entry if doesn't exist
			technologies.insert({ technology->level, std::make_pair(std::vector<Technology*>{}, 0) });
		}
		// Register technology
		technologies[technology->level].first.push_back(technology);
		if (technology->count_to_width)
		{
			technologies[technology->level].second += groupdata->at(technology).width + tech_interval_x;
		}
	}

	// Repeat the same steps for root
	if (technologies.find(key->level) == technologies.end())
	{
		technologies.insert({ key->level, std::make_pair(std::vector<Technology*>{}, 0) });
	}
	technologies[key->level].first.push_back(key);
	technologies[key->level].second += tech_label_width + tech_interval_x;

	// Find group width
	int current_max = 0;

	for (auto p = technologies.begin(); p != technologies.end(); p++)
	{
		// Fix level width. This is necessary beacause of the additional interval after last technology
		p->second.second -= tech_interval_x;
		if (p->second.second > current_max)
		{
			current_max = p->second.second;
		}
	}


	int min_level = technologies.begin()->first;
	int max_level = technologies.rbegin()->first;

	// Fill basic data of group
	groupdata->at(key).level_y = min_level;
	groupdata->at(key).level_height = max_level - min_level;
	groupdata->at(key).width = current_max;
	groupdata->at(key).y = min_level * (tech_label_height + tech_interval_y);

	// For every technology give it x - coordinate based on its position in level
	for (auto&[level, techs_on_level] : technologies)
	{
		// Sort level groups by root order
		std::sort(techs_on_level.first.begin(), techs_on_level.first.end(), [](const Technology* tech1, const Technology* tech2) { return tech1->order < tech2->order; });

		// Fix width once again (after defining correct group width. It is done to correctly set x for groups
		techs_on_level.second -= tech_label_width;
		int x = -techs_on_level.second / 2;
		for (auto technology : techs_on_level.first)
		{
			// Consider forced offset
			x += technology->force_offset * (tech_label_width + tech_interval_x);

			// Add technology and increase x
			groupdata->at(technology).x = x;
			x += tech_label_width + tech_interval_x;
		}
	}
}

void Game::render_groups(std::unordered_map<Technology*, std::vector<Technology*>>* groups, std::unordered_map<Technology*, TechnologyGroupData>* planned)
{
	for (const auto& [key, g_age] : this->object_tree["age"])
	{
		auto fixed = (Age*)g_age;

		this->render_group(groups, planned, (Technology*)this->object_tree["technology"][fixed->name + ":master"], 600);
	}
}

void Game::render_group(std::unordered_map<Technology*, std::vector<Technology*>>* groups, std::unordered_map<Technology*, TechnologyGroupData>* planned, Technology* key, int x)
{
	// Set group root coordinates
	key->blit.first = x - tech_label_width / 2;
	key->blit.second = planned->at(key).y;

	// Render children
	for (auto group : groups->at(key))
	{
		render_group(groups, planned, group, x + planned->at(group).x);
	}
}
