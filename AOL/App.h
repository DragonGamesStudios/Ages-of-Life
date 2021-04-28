#include "allegrolib.h"
#include "gui.h"

#include "LuaModLoader.h"

#include "Game.h"

#include <art/CfgDictionary.h>
#include <art/Sprite.h>
#include <art/Atlas.h>

#include <AGLAllegro5Backend/Allegro5EventHandler.h>
#include <AGLAllegro5Backend/Allegro5GraphicsHandler.h>

#include <ARTAllegro5Backend/Allegro5MainEventManager.h>
#include <ARTAllegro5Backend/Allegro5KeyboardEventManager.h>
#include <ARTAllegro5Backend/Allegro5Display.h>
#include <ARTAllegro5Backend/Allegro5ArtImage.h>

struct Mod
{
	std::string code_name;
	std::string full_name;
	std::string description;
	version_t version;
	std::vector<std::tuple<std::string, version_t, char>> dependencies;
	version_t aol_version;
	std::string homepage;
	std::string author;
	std::string contact;
	agl::Image* thumbnail;
	bool has_thumbnail = true;
	bool should_load = true;
	bool ordered = false;
};

class App {
public:
	App();
	~App();

	void createguis();
	void run();
	void quit();

protected:
	int screenw, screenh;

	// Base
	ALLEGRO_BITMAP* AOLicon;

	agl::Allegro5Font* segoeUI_bold;

	agl::GuiGroup* main_gui_group;

	agl::Gui* main_menu_gui_instance;
	MainMenuGui* main_menu_gui;

	agl::Gui* new_game_gui_instance;
	NewGameGui* new_game_gui;

	agl::Allegro5Shader* guassian_blur;
	agl::Allegro5Image* main_menu_background;

	agl::Allegro5EventHandler* event_handler;
	agl::Allegro5GraphicsHandler* graphics_handler;

	art::Allegro5MainEventManager* event_manager;
	art::Allegro5KeyboardEventManager* keyboard_manager;
	art::Allegro5Display* display;

	art::FileSystem* local_fs;
	art::CfgDictionary* dict;
	std::set<std::string> supported_languages;

	art::FileSystem* appdata_fs;
	art::FileSystem* save_fs;

	std::vector<std::vector<std::pair<int, int>>> shortcuts;
	std::vector<std::vector<std::function<void(const agl::Event&)>>> shortcut_functions;

	std::map<agl::Block*, agl::Gui*> gui_openers;
	std::map<agl::Block*, agl::Gui*> gui_closers;

	double time;

	// Game
	Storage* storage;
	Game* active_game;
	std::string selected_scenario;

	std::map<std::string, GameData> games;

	// Modding
	LuaModLoader* mod_loader;
	LuaModExecutor* mod_executor;
	LuaStorage* mod_storage_loader;
	LuaSaveSystem* mod_savesystem;

	std::vector<std::string> ordered_mods;
	std::map<std::string, Mod> loaded_mods;
	std::unordered_map<LoaderStage, std::vector<std::string>> to_run;
	std::map<std::string, Scenario> scenarios;

	std::map<std::string, version_t> active_configuration;

	// Functions
	void initialize_agl();

	void draw();
	void update(double dt);
	void close();

	void change_loading_screen(std::string mes);
	void change_loading_screen(std::string mes, float per);

	void load();
	void run_file_in_mods(const std::string& file_name);
	bool analyze_mod(const fs::path& mod_path, Mod* mod, art::FileSystem* mod_fs);
	bool analyze_scenario(const fs::path& scenario_path, Scenario* scenario, art::FileSystem* mod_fs);
	version_t get_version(const std::string& version_str) const;
	std::pair<bool, std::tuple<std::string, version_t, char>> get_dependency(const json& dep_json);
	bool dependency_satisfied(const std::tuple<std::string, version_t, char>& dep);
	void load_mod_list();
	void write_mod_list();

	void enable_debug(const agl::Event& e);
	void shortcut_capture(agl::Event e);

	void open_gui(const agl::Event& e);
	void close_gui(const agl::Event& e);

	void handle_create_game(const agl::Event& e);
	bool can_save_be_created(const std::string& savename) const;
	void reload_saves();
	void create_game(const std::string& name, long long seed);
	long long get_seed(const std::string& seed);
	void handle_delete_game(const agl::Event& e);
	void delete_game(const std::string& name);

	void handle_load_game(const agl::Event& e);
	void load_game(const std::string& name);

	void on_scenario_selected(char src, const std::string& scenario_name);

	void check_appdata();

	std::vector<fs::path> get_locale_paths(const fs::path& d_path, const std::string& language);
};