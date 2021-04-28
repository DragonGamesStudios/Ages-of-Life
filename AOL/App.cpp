#include "App.h"
#include "gui.h"

#include <atlstr.h>
#include <agl/events.h>

#define MOD_DEP_CONFLICT 0b1
#define MOD_DEP_OPTIONAL 0b10
#define MOD_DEP_EQUAL 0b100
#define MOD_DEP_GREATER 0b1000
#define MOD_DEP_LESS 0b10000

agl::Color black(0, 0, 0);
agl::Color loading_screen_bg(20, 20, 20);
agl::Color white(255, 255, 255);

App::App()
{
	// Set non-pre-multiplied alpha
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

	AOLicon = al_load_bitmap("core/graphics/AOLIcon.png");

	event_manager = new art::Allegro5MainEventManager(60);
	keyboard_manager = new art::Allegro5KeyboardEventManager;
	display = new art::Allegro5Display(1900, 1180, "Ages of Life");
	event_manager->add_manager(keyboard_manager);

	event_manager->connect_display(display);
	event_manager->enable_mouse();
	event_manager->enable_keyboard();

	shortcuts = {
		{
			{ALLEGRO_KEY_F3, ALLEGRO_KEYMOD_CTRL}
		}
	};

	shortcut_functions = {
		{
			std::bind(&App::enable_debug, this, std::placeholders::_1)
		}
	};

	supported_languages = { "en", "pl" };

	local_fs = new art::FileSystem;
	dict = new art::CfgDictionary;
	dict->set_filesystem(local_fs);
	dict->set_dict_path_function(std::bind(&App::get_locale_paths, this, std::placeholders::_1, std::placeholders::_2));

	dict->add_locale_path(local_fs->get_current_path() / "core/locale");

	dict->set_active_language("en");

	appdata_fs = new art::FileSystem(true);
	save_fs = new art::FileSystem(true);

	check_appdata();

	for (int i = 0; i < shortcuts.size(); i++)
	{
		for (int j = 0; j < shortcuts[i].size(); j++)
		{
			keyboard_manager->register_shortcut(
				shortcuts[i][j].first, shortcuts[i][j].second,
				shortcut_functions[i][j]
			);
		}
	}

	screenw = display->get_width();
	screenh = display->get_height();
	time = al_get_time();

	initialize_agl();

	// Game
	storage = new Storage;
	active_game = 0;

	// Modding
	local_fs->add_path_template("__core__", local_fs->get_correct_path("core"));
	local_fs->add_path_template("__base__", local_fs->get_correct_path("base"));

	mod_loader = new LuaModLoader;
	mod_executor = new LuaModExecutor;
	mod_storage_loader = new LuaStorage;
	mod_savesystem = new LuaSaveSystem;

	storage->register_l_storage(mod_storage_loader);

	mod_loader->register_l_storage(mod_storage_loader);
	mod_loader->register_l_savesystem(mod_savesystem);
	mod_loader->register_filesystem(appdata_fs);

	mod_executor->register_l_savesystem(mod_savesystem);

	mod_savesystem->register_filesystem(save_fs);

	mod_storage_loader->register_storage(storage);

	to_run = {
		{ LoaderStage::STAGE_SETTINGS, { "settings.lua", "settings-fixes.lua", "settings-final-fixes.lua" } },
		{ LoaderStage::STAGE_PROTOTYPES, { "prototypes.lua", "prototypes-fixes.lua", "prototypes-final-fixes.lua" } },
		{ LoaderStage::STAGE_DATA, { "data.lua", "data-fixes.lua", "data-final-fixes.lua" } },
		// Settings migrations stage is executed in migrations directory, if there is one
		{ LoaderStage::STAGE_SETTINGS_MIGRATIONS, { "migrations/settings.lua", "migrations/settings-fixes.lua", "migrations/settings-final-fixes.lua" } },
		{ LoaderStage::STAGE_NEW_GAME, { "new-game.lua", "new-game-fixes.lua", "new-game-final-fixes.lua" } },
	};
}

App::~App()
{
	delete main_menu_gui_instance;
	delete new_game_gui_instance;

	delete segoeUI_bold;

	delete bronze_age_hflow;
	delete horizontal_flow;
	delete bronze_age_scrollbar;

	delete event_handler;
	delete graphics_handler;

	delete guassian_blur;
	delete main_menu_background;

	delete event_manager;
	delete keyboard_manager;
	delete display;
	delete appdata_fs;
	delete local_fs;

	delete mod_loader;

	for (const auto& [_, scenario] : scenarios)
		if (scenario.has_preview)
			delete scenario.preview;

	for (const auto& mod : loaded_mods)
		if (mod.second.has_thumbnail)
			delete mod.second.thumbnail;
}

void App::enable_debug(const agl::Event& e)
{
	agl::debug::debug = !agl::debug::debug;
}

void App::shortcut_capture(agl::Event e)
{
	if (e.type == AGL_EVENT_CHAR_INPUT && e.source->get_focus())
	{
		agl::Block* parent = e.source->get_parent();
		int opt = parent->get_child_index(e.source) - 1;
		int cat = (parent->get_parent()->get_child_index(parent) - 1) / 2;

		int mods = e.mods & keyboard_manager->supported_mods;

		agl::builtins::Label* lbl = (agl::builtins::Label*)e.source->get_child_by_index(0);

		lbl->set_text(shortcut_to_string(e.keycode, mods));
		auto s = shortcuts[cat][opt];

		keyboard_manager->remove_shortcut(s.first, s.second);
		keyboard_manager->register_shortcut(e.keycode, mods, shortcut_functions[cat][opt]);
		shortcuts[cat][opt] = std::make_pair(e.keycode, mods);

		e.source->set_focus(false);
	}
}

void App::open_gui(const agl::Event& e)
{
	auto open_it = gui_openers.find(e.source);

	if (open_it != gui_openers.end())
	{
		main_gui_group->add_gui(open_it->second);
	}
}

void App::close_gui(const agl::Event& e)
{
	auto open_it = gui_closers.find(e.source);

	if (open_it != gui_closers.end())
	{
		main_gui_group->remove_gui(open_it->second);
	}
}

void App::handle_create_game(const agl::Event& e)
{
	close_gui(e);

	std::string save_name = new_game_gui->name_input.get_value();
	std::string save_seed = new_game_gui->seed_input.get_value();

	if (can_save_be_created(save_name))
		create_game(save_name, get_seed(save_seed));

	new_game_gui->name_input.clear_value();
	new_game_gui->seed_input.clear_value();
}

bool App::can_save_be_created(const std::string& savename) const
{
	for (const auto& f : appdata_fs->get_files_in_directory("saves"))
	{
		if (f.is_directory() && f.path().filename().string() == savename)
			return false;
	}

	return !savename.empty();
}

void App::reload_saves()
{
	main_menu_gui->play_game_selection_list.clear_elements();

	std::map<fs::file_time_type, std::string, std::greater<fs::file_time_type>> saves;

	for (const auto& f : appdata_fs->get_files_in_directory("saves"))
	{
		if (f.is_directory())
		{
			saves.insert({ f.last_write_time(), f.path().filename().string() });
		}
	}

	appdata_fs->enter_dir((fs::path)"saves");

	for (const auto& [key, value] : saves)
	{
		main_menu_gui->play_game_selection_list.add_element(value, value);
		
		auto f = appdata_fs->open_file((fs::path)value / "base.dat");

		GameData data;
		std::string scenario;

		if (f.is_open())
		{
			load_base_data(f, data.aol_version, scenario, data.mod_config);
		}

		f.close();

		auto it = scenarios.find(scenario);

		data.scenario = it != scenarios.end() ? &it->second : nullptr;

		games.insert({ value, data });
	}

	appdata_fs->exit();
}

void App::create_game(const std::string& name, long long seed)
{
	save_fs->create_dir(name);
	save_fs->enter_dir(name);

	// Mod save space
	save_fs->create_dir("mod-data");
	save_fs->enter_dir((fs::path)"mod-data");

	mod_loader->begin_stage(LoaderStage::STAGE_NEW_GAME);

	appdata_fs->enter_dir((fs::path)"mods");

	for (const auto& file : to_run[LoaderStage::STAGE_NEW_GAME])
		run_file_in_mods(file);

	appdata_fs->exit();

	mod_loader->end_stage();

	save_fs->exit();

	// Create general save file
	std::ofstream of = save_fs->open_ofile((fs::path)"base.dat");
	save_base_data(of, selected_scenario, active_configuration);
	of.close();

	save_fs->exit_to("saves");

	reload_saves();
}

long long App::get_seed(const std::string& seed_str)
{
	long long seed = 0;

	for (const char c : seed_str)
		seed += (long long)c - (int)'0';

	return seed;
}

void App::handle_delete_game(const agl::Event& e)
{
	if (!main_menu_gui->play_game_selection_list.get_selected_elements().empty())
	{
		int selected_index = *main_menu_gui->play_game_selection_list.get_selected_elements().begin();

		main_menu_gui->play_game_selection_list.unselect_child(selected_index);

		auto selected = main_menu_gui->play_game_selection_list.get_element_by_index(selected_index);

		std::string deleted = ((agl::builtins::Label*)selected->get_child_by_index(0))->get_text();
		
		main_menu_gui->play_game_selection_list.remove_element(selected_index);

		delete_game(deleted);
		reload_saves();
	}
}

void App::delete_game(const std::string& name)
{
	appdata_fs->delete_dir_recursively(((fs::path)"saves" / name).string());
}

void App::handle_load_game(const agl::Event& e)
{
	if (!main_menu_gui->play_game_selection_list.get_selected_elements().empty())
	{
		main_menu_gui->close_subguis();

		int selected_index = *main_menu_gui->play_game_selection_list.get_selected_elements().begin();

		main_menu_gui->play_game_selection_list.unselect_child(selected_index);

		std::string loaded = main_menu_gui->play_game_selection_list.get_key_by_index(selected_index);

		load_game(loaded);
	}
}

void App::load_game(const std::string& name)
{
	change_loading_screen("Loading...");

	save_fs->enter_dir(name);

	active_game = new Game;

	active_game->register_filesystem(save_fs);
	
	if (!active_game->load(appdata_fs, local_fs, ordered_mods, active_configuration, games[name]))
	{
		al_show_native_message_box(
			display->get_al_display(),
			"Error",
			((std::string)"Error while executing mod: " + active_game->get_mod_error().first).c_str(),
			(active_game->get_mod_error().second.message + "\n\n" + active_game->get_mod_error().second.traceback).c_str(), "Ok", ALLEGRO_MESSAGEBOX_ERROR
		);
		close();
	}

	new_game_gui->builtin_scenarios_sl.select_child("freeplay");
}

void App::on_scenario_selected(char src, const std::string& scenario_name)
{
	auto& scenario = scenarios.at(scenario_name);

	new_game_gui->scenario_info_preview.set_image(scenario.preview);
	new_game_gui->scenario_info_title.set_text(scenario_name);

	if (scenario.dict->has_key("scenario-description"))
		new_game_gui->scenario_info_description.set_text(scenario.dict->format({ "scenario-description" }));
	else
		new_game_gui->scenario_info_description.set_text("");

	selected_scenario = scenario_name;
}

void App::check_appdata()
{
	// AOL directory
	appdata_fs->create_dir_if_necessary("AOL");
	appdata_fs->enter_dir((fs::path)"AOL");

	// Saves directory
	appdata_fs->create_dir_if_necessary("saves");

	// Mods directory
	appdata_fs->create_dir_if_necessary("mods");

	// Locate save_fs
	save_fs->enter_dir((fs::path)"AOL");
	save_fs->enter_dir((fs::path)"saves");
}

std::vector<fs::path> App::get_locale_paths(const fs::path& d_path, const std::string& language)
{
	std::vector<fs::path> ret;
	std::string lan = "en";

	if (local_fs->exists(d_path / language))
		lan = language;
	else if (!local_fs->exists(d_path / "en"))
	{
		for (const auto& f : local_fs->get_files_in_directory(d_path))
		{
			if (f.is_directory() && supported_languages.find(f.path().filename().string()) != supported_languages.end())
			{
				lan = language;
				break;
			}
		}
	}

	for (const auto& f : local_fs->get_files_in_directory(d_path / lan))
	{
		if (f.path().extension() == ".cfg")
			ret.push_back(f.path());
	}

	return ret;
}

void App::run()
{
	load();

	while (true) {
		ALLEGRO_EVENT e = event_manager->get_event();
		if (event_manager->window_closed()) {
			break;
		}
		this->event_handler->handle_event(e);

		if (event_manager->clock_ticked()) {
			update(event_manager->get_delta());

			al_clear_to_color(graphics_handler->get_color(black));

			draw();

			al_flip_display();
		}
	}
}

void App::close()
{
	event_manager->close();
}

void App::load()
{
	// Load missing preview image
	agl::Allegro5Image* preview = new agl::Allegro5Image("core/graphics/gui/missing-preview.png");

	agl::register_image("missing-preview", preview);

	// Mod analysis stage
	Mod core;
	analyze_mod("core", &core, local_fs);

	Mod base;
	analyze_mod("base", &base, local_fs);

	core.ordered = true;
	base.ordered = true;

	loaded_mods.insert({ "core", core });
	loaded_mods.insert({ "base", base });

	appdata_fs->enter_dir((fs::path)"mods");

	for (const auto& path : appdata_fs->get_files_in_directory("."))
	{
		if (path.is_directory())
		{
			Mod new_mod;
			if (analyze_mod(path.path(), &new_mod, appdata_fs))
				loaded_mods.insert({ new_mod.code_name, new_mod });
		}
	}

	// Mod list
	load_mod_list();
	write_mod_list();

	// Mod ordering stage
	ordered_mods.push_back("core");
	ordered_mods.push_back("base");

	std::set<Mod*> mod_alp;

	for (auto& [_, mod] : loaded_mods)
	{
		mod_alp.insert(&mod);
	}

	// Codes
	// 0 - can be loaded
	// 1 - dependency unavailable/conflict (cannot load the mod at all)
	// 2 - dependency not yet available

	int operations = 1;
	bool can_be_loaded = false;

	// Delete unsatisfiable
	for (auto mod = mod_alp.begin(); mod != mod_alp.end();)
	{
		auto current = mod++;

		if (((*current)->aol_version[0] != AOL_VERSION[0] && (*current)->aol_version[1] != AOL_VERSION[1]) || !(*current)->should_load)
		{
			mod_alp.erase(current);
			continue;
		}

		for (const auto& dep : (*current)->dependencies)
		{
			// If unsatisfiable, delete
			if (!dependency_satisfied(dep))
				mod_alp.erase(current);
		}
	}

	// Delete to avoid reordering them
	mod_alp.erase(&loaded_mods.at("core"));
	mod_alp.erase(&loaded_mods.at("base"));

	// Order
	while (operations)
	{
		operations = 0;

		for (auto mod = mod_alp.begin(); mod != mod_alp.end();)
		{
			auto current = mod++;
			can_be_loaded = true;

			for (const auto& dep : (*current)->dependencies)
			{
				auto mod_it = loaded_mods.find(std::get<0>(dep));

				if (mod_it != loaded_mods.end() && ((std::get<2>(dep) & MOD_DEP_CONFLICT) == 0) && !mod_it->second.ordered)
				{
					can_be_loaded = false;
					break;
				}
			}

			if (can_be_loaded)
			{
				ordered_mods.push_back((*current)->code_name);
				(*current)->ordered = true;
				mod_alp.erase(current);
				operations++;
				break;
			}
		}
	}

	for (const auto& [_, mod] : loaded_mods)
		active_configuration.insert({ mod.code_name, mod.version });

	// Loading mods


	// Loading settings
	mod_loader->begin_stage(LoaderStage::STAGE_SETTINGS);

	for (const auto& file : to_run[LoaderStage::STAGE_SETTINGS])
		run_file_in_mods(file);

	mod_loader->end_stage();

	// Migrating settings
	mod_loader->begin_stage(LoaderStage::STAGE_SETTINGS_MIGRATIONS);

	for (const auto& file : to_run[LoaderStage::STAGE_SETTINGS_MIGRATIONS])
		run_file_in_mods(file);

	mod_loader->end_stage();

	// Loading prototypes
	mod_loader->begin_stage(LoaderStage::STAGE_PROTOTYPES);

	for (const auto& file : to_run[LoaderStage::STAGE_PROTOTYPES])
		run_file_in_mods(file);

	mod_loader->end_stage();

	// Loading data
	mod_loader->begin_stage(LoaderStage::STAGE_DATA);

	for (const auto& file : to_run[LoaderStage::STAGE_DATA])
		run_file_in_mods(file);

	mod_loader->end_stage();

	appdata_fs->exit_to("AOL");

	// Guis
	createguis();
}

void App::run_file_in_mods(const std::string& file_name)
{
	for (const auto& mod : ordered_mods)
	{
		if (mod == "core" || mod == "base")
			mod_loader->register_filesystem(local_fs);
		if (!mod_loader->load_mod(mod, file_name))
		{
			al_show_native_message_box(
				display->get_al_display(),
				"Error",
				((std::string)"Error while executing mod: " + loaded_mods[mod].full_name).c_str(),
				(mod_loader->get_last_error().message + "\n\n" + mod_loader->get_last_error().traceback).c_str(), "Ok", ALLEGRO_MESSAGEBOX_ERROR
			);
			close();
		}
		if (mod == "core" || mod == "base")
			mod_loader->register_filesystem(appdata_fs);
	}
}

bool App::analyze_mod(const fs::path& mod_path, Mod* mod, art::FileSystem* mod_fs)
{
	json info;
	auto info_f = mod_fs->open_file(mod_path / "info.json");

	if (!info_f.good())
	{
		return false;
	}

	info_f >> info;

	info_f.close();

	// code_name
	auto info_it = info.find("name");

	if (info_it != info.end())
		mod->code_name = info_it.value();
	else
		throw std::runtime_error("Mod: " + mod_path.string() + "; Missing info.json parameter: 'name'");

	// author
	info_it = info.find("author");

	if (info_it != info.end())
		mod->author = info_it.value();

	// aol_version
	info_it = info.find("aol_version");

	if (info_it != info.end())
		mod->aol_version = get_version(info_it.value());
	else
		throw std::runtime_error("Mod: " + mod->code_name + "; Missing info.json parameter: 'aol_version'");

	// contact
	info_it = info.find("contact");

	if (info_it != info.end())
		mod->contact = info_it.value();

	// dependencies
	info_it = info.find("dependencies");

	if (info_it != info.end())
	{
		if (info_it.value().type() == json::value_t::array)
		{
			for (const auto& dep_json : info_it.value())
			{
				auto dep = get_dependency(dep_json);

				if (dep.first)
					mod->dependencies.push_back(dep.second);
			}
		}
	}

	// description
	info_it = info.find("description");

	if (info_it != info.end())
		mod->description = info_it.value();

	// full name
	info_it = info.find("title");

	if (info_it != info.end())
		mod->full_name = info_it.value();
	else
		throw std::runtime_error("Mod: " + mod->code_name + "; Missing info.json parameter: 'title'");

	// homepage
	info_it = info.find("homepage");

	if (info_it != info.end())
		mod->homepage = info_it.value();

	// version
	info_it = info.find("version");

	std::string version_string;

	if (info_it != info.end())
	{
		version_string = info_it.value();
		mod->version = get_version(version_string);
	}
	else
		throw std::runtime_error("Mod: " + mod->code_name + "; Missing info.json parameter: 'version'");

	// Validation - mod directory name
	fs::path expected1 = mod->code_name;
	fs::path expected2 = mod->code_name + "_" + version_string;
	fs::path expected3 = mod->code_name + "_" + version_string + ".zip";

	fs::path given = mod_path.filename();

	if (given != expected1 && given != expected2 && given != expected3)
		throw std::runtime_error("Path " + given.string() + " does not match the expected " + expected1.string() + " / " + expected2.string() + " / " + expected3.string() + ".");

	// Add path template
	appdata_fs->add_path_template("__" + mod->code_name + "__", mod_fs->get_current_path());

	// Look for thumbnail
	if (mod_fs->exists(mod_path / "thumbnail.png"))
	{
		mod->thumbnail = new agl::Allegro5Image(mod_fs->get_correct_path(mod_path / "thumbnail.png").string());
	}
	else
	{
		mod->thumbnail = agl::loaded_images["missing-preview"];
		mod->has_thumbnail = false;
	}

	// Analyze scenarios
	if (mod_fs->exists(mod_path / "scenarios"))
	{
		Scenario new_scenario;


		for (const auto& scenario : mod_fs->get_files_in_directory(mod_path / "scenarios"))
		{
			if (scenario.is_directory())
			{
				if (analyze_scenario(scenario.path(), &new_scenario, mod_fs))
					scenarios.insert({ new_scenario.name, new_scenario });
			}
		}
	}

	return true;
}

bool App::analyze_scenario(const fs::path& scenario_path, Scenario* scenario, art::FileSystem* mod_fs)
{
	scenario->name = scenario_path.filename().string();

	scenario->is_local = mod_fs == local_fs;

	scenario->path = mod_fs->get_correct_path(scenario_path);

	scenario->dict = new art::CfgDictionary;
	scenario->dict->set_filesystem(mod_fs);
	scenario->dict->set_dict_path_function(std::bind(&App::get_locale_paths, this, std::placeholders::_1, std::placeholders::_2));

	if (mod_fs->exists(scenario_path / "locale"))
		scenario->dict->add_locale_path(scenario->path / "locale");

	scenario->dict->set_active_language("en");

	if (mod_fs->exists(scenario->path / "preview.png"))
		scenario->preview = new agl::Allegro5Image((scenario->path / "preview.png").string());
	else
	{
		scenario->has_preview = false;
		scenario->preview = agl::loaded_images["missing-preview"];
	}

	return true;
}

version_t App::get_version(const std::string& version_str) const
{
	version_t version = { 0, 0, 0 };

	std::string number_str;
	int insert_to = 0;

	for (const char c : version_str)
	{
		if (c >= '0' && c <= '9')
			number_str.push_back(c);
		else if (c == '.')
		{
			if (number_str.empty() || insert_to == 3)
				throw std::runtime_error("Invalid version string '" + version_str + "'.");

			int num = std::stoi(number_str);

			if (num < 0 || num > 255)
				throw std::runtime_error("Invalid version string '" + version_str + "'.");

			version[insert_to] = (unsigned char)num;
			number_str.clear();
			insert_to++;
		}
		else
			throw std::runtime_error("Invalid version string '" + version_str + "'.");
	}

	// Last char
	if (number_str.empty() || insert_to == 3)
		throw std::runtime_error("Invalid version string '" + version_str + "'.");

	int num = std::stoi(number_str);

	if (num < 0 || num > 255)
		throw std::runtime_error("Invalid version string '" + version_str + "'.");

	version[insert_to] = (unsigned char)num;

	if (version.empty())
		throw std::runtime_error("Invalid version string '" + version_str + "'.");

	return version;
}

std::pair<bool, std::tuple<std::string, version_t, char>> App::get_dependency(const json& dep_json)
{
	std::string name;
	version_t version;
	char spec_mask = 0;

	bool is_valid = true;
	auto it = dep_json.find("name");

	if (it != dep_json.end())
		name = it.value();
	else
		is_valid = false;

	it = dep_json.find("version");

	if (it != dep_json.end())
		version = get_version(it.value());
	else
		is_valid = false;

	it = dep_json.find("specification");

	if (it != dep_json.end())
	{
		for (const char c : (std::string)it.value())
		{
			switch (c)
			{
			case '<':
				spec_mask |= MOD_DEP_LESS;
				break;

			case '>':
				spec_mask |= MOD_DEP_GREATER;
				break;

			case '=':
				spec_mask |= MOD_DEP_EQUAL;
				break;

			case '!':
				spec_mask |= MOD_DEP_CONFLICT;
				break;

			case '?':
				spec_mask |= MOD_DEP_OPTIONAL;
				break;
			}
		}
	}
	else
		is_valid = false;
	
	return { is_valid, {name, version, spec_mask} };
}

bool App::dependency_satisfied(const std::tuple<std::string, version_t, char>& dep)
{
	bool version_correct = false;

	// Get version dependency specification
	const Mod& mod = loaded_mods.at(std::get<0>(dep));
	char dep_spec = std::get<2>(dep);
	version_t dep_vers = std::get<1>(dep);

	if (dep_spec & MOD_DEP_OPTIONAL)
		return true;

	if (dep_spec & MOD_DEP_EQUAL)
		version_correct |= (dep_vers == mod.version);

	if (dep_spec & MOD_DEP_GREATER)
		version_correct |= (mod.version > dep_vers);

	if (dep_spec & MOD_DEP_LESS)
		version_correct |= (mod.version < dep_vers);

	version_correct &= mod.should_load;

	if (dep_spec & MOD_DEP_CONFLICT)
		return !version_correct;

	return version_correct;
}

void App::load_mod_list()
{
	auto f = appdata_fs->open_file((fs::path)"mod_list.json");
	
	if (f.is_open())
	{
		json mod_list;
		f >> mod_list;

		for (auto element : mod_list.items())
		{
			auto mod_it = loaded_mods.find(element.value()["name"]);

			if (mod_it != loaded_mods.end())
				mod_it->second.should_load = element.value()["enabled"];
		}


		f.close();
	}
}

void App::write_mod_list()
{
	auto f = appdata_fs->open_ofile((fs::path)"mod_list.json");

	if (f.is_open())
	{
		json mod_list;

		for (const auto& [key, value] : loaded_mods)
		{
			json new_mod = {
				{ "name", value.code_name },
				{ "enabled", value.should_load }
			};

			mod_list.push_back(new_mod);
		}

		f << mod_list;
	}

	f.close();
}
 
void App::quit()
{
	al_destroy_bitmap(AOLicon);
}

void App::initialize_agl()
{
	agl::debug::init();

	segoeUI_bold = new agl::Allegro5Font("core/fonts/segoeuib.ttf", { 14, 18, 24, 27, 36, 56 });
	agl::set_default_font(segoeUI_bold);

	event_handler = new agl::Allegro5EventHandler;
	graphics_handler = new agl::Allegro5GraphicsHandler;

	main_gui_group = new agl::GuiGroup();

	setup_styles();
}

void App::draw()
{
	main_gui_group->draw();
}

void App::update(double dt)
{
	this->time += dt;

	main_gui_group->update();

	this->event_handler->reset_event_queue();

	if (active_game)
		active_game->update(dt);
}

void App::change_loading_screen(std::string mes, float per)
{
	al_clear_to_color(graphics_handler->get_color(loading_screen_bg));

	float py = 3.f * screenh / 4;
	std::string message = mes + " " + std::to_string(int(100 * per)) + "%";
	float px = (float)screenw / 2 - segoeUI_bold->get_width(24, message) / 2;
	float bx = 2.f * screenw / 6;
	float w = per * (screenw / 3 - 5);

	graphics_handler->draw_text({px, py - 50}, message, segoeUI_bold, 24, white);
	graphics_handler->draw_filled_rectangle({ agl::Point{ bx - 5, py }, (float)screenw / 3, 50.f }, white);
	graphics_handler->draw_rectangle({ agl::Point{bx, py + 5}, (float)w, 45.f }, 2, white);
	al_flip_display();
}

void App::change_loading_screen(std::string mes)
{
	al_clear_to_color(graphics_handler->get_color(loading_screen_bg));

	float py = 3.f * screenh / 4;
	float px = (float)screenw / 2 - segoeUI_bold->get_width(24, mes) / 2;

	graphics_handler->draw_text({ px, py - 50 }, mes, segoeUI_bold, 24, white);
	al_flip_display();
}
