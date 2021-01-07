#include "allegrolib.h"
#include "gui.h"

#include "LuaModLoader.h"
#include "Settings.h"

#include <art/Dictionary.h>
#include <art/Sprite.h>
#include <art/Atlas.h>

#include <AGLAllegro5Backend/Allegro5EventHandler.h>
#include <AGLAllegro5Backend/Allegro5GraphicsHandler.h>

#include <ARTAllegro5Backend/Allegro5MainEventManager.h>
#include <ARTAllegro5Backend/Allegro5KeyboardEventManager.h>
#include <ARTAllegro5Backend/Allegro5Display.h>
#include <ARTAllegro5Backend/Allegro5ArtImage.h>

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
	art::Dictionary* dict;

	art::FileSystem* appdata_fs;

	std::vector<std::vector<std::pair<int, int>>> shortcuts;
	std::vector<std::vector<std::function<void(const agl::Event&)>>> shortcut_functions;

	std::map<agl::Block*, agl::Gui*> gui_openers;
	std::map<agl::Block*, agl::Gui*> gui_closers;

	double time;

	// Game
	Settings* settings;

	// Modding
	LuaModLoader* mod_loader;
	LuaSettings* mod_settings_loader;

	std::vector<std::string> loaded_mods;

	// Functions
	void initialize_agl();

	void draw();
	void update(double dt);
	void close();

	void change_loading_screen(std::string mes);
	void change_loading_screen(std::string mes, float per);

	void load();
	void run_file_in_mods(const std::string& file_name);

	void enable_debug(agl::Event e);
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

	void check_appdata();
};