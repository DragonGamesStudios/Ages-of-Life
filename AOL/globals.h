#include "lib/Proto/Proto.h"
#include "allegrolib.h"

typedef unsigned int uint;
typedef const char* ccptr;
typedef unsigned char uchar;
typedef std::function<void()> fn00;
typedef std::pair<float, float> spair;

struct basedata_struct {
	std::vector<std::string> saves = {};
	std::string language = "en";
};

struct image_with_hover {
	Image* normal;
	Image* hover;
};

class Script {
public:
	Script();

	std::vector<GUI*> guis;
	std::vector<GUI*>::size_type opened_guis_amount;
	std::unordered_map<GUI*, std::function<void()>> gui_open_functions;
	std::unordered_map<GUI*, std::function<void()>> gui_close_functions;
	std::vector<char> valid_chars;

	Label* active_input;
	
	void open_gui(GUI* gui);
	void close_gui(GUI* gui);
	bool get_gui_opened(GUI* gui);
	void register_open_gui_function(GUI* gui, std::function<void()> fn);
	void register_close_gui_function(GUI* gui, std::function<void()> fn);
	void keyinput(int keycode, int ascii);

	void activate_input(Label* lbl = NULL);
	bool validate_savename(std::string savename);
};

class Game {
public:
	Game();
	~Game();

	void createguis();
	void run();
	void quit();

protected:
	int screenw, screenh;

	int lastmwpos;

	Proto proto;
	Script script;

	Image* mainmenuBg;
	Image* menubutton;
	Image* defaultguiImage;
	Image* closebutton;
	Image* inputbutton;
	Image* logo;
	image_with_hover loadbutton;
	ALLEGRO_BITMAP* AOLicon;

	Font* segoeuib;

	Label* name_input_label;

	GUI* mainmenu;
	GUI* playgui;
	GUI* aboutgui;
	GUI* creatorsgui;
	GUI* licensesgui;

	basedata_struct basedata;

	bool menu;

	DrawData displayed_save_scale;

	double time;

	void draw();
	void update(double dt);
	void close();
	void scroll(int amt);
	void close_all_menu_guis();
	void draw_active_input();
	void load_basedata();
	void save_basedata();
	void get_displayed_saves();

	void change_loading_screen(std::string mes);
	void change_loading_screen(std::string mes, float per);

	void create_save();
	void delete_save(int index);
};