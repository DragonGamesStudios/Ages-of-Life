#include "lib/Proto/Proto.h"
#include "allegrolib.h"
#include "classes/GameObject.h"
#include "classes/Technology.h"

typedef unsigned int uint;
typedef const char* ccptr;
typedef unsigned char uchar;
typedef std::function<void()> fn00;
typedef std::pair<float, float> spair;

extern ALLEGRO_COLOR black;
extern ALLEGRO_COLOR white;
extern ALLEGRO_COLOR loading_screen_bg;
extern ALLEGRO_COLOR menutxtcol;
extern ALLEGRO_COLOR menu_orange;
extern ALLEGRO_COLOR lightyellow;
extern ALLEGRO_COLOR highlight1;

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

	void load_colors();
};

class Game {
public:
	Game();
	~Game();

	void createguis();
	void run();
	void quit();

	Proto proto;
	std::map<std::string, std::map<std::string, GameObject*>> object_tree;
	std::map<std::string, std::map<std::string, GameObjectPrototype*>> prototype_tree;

protected:
	int screenw, screenh;

	int lastmwpos;
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
	GUI* escapegui;

	basedata_struct basedata;

	bool menu;
	bool playing;

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

	void register_prototype(GameObjectPrototype* prototype, std::string group);

	void load();
	void load_prototypes();
	void apply_changes();
	void initialize_prototypes();

	void api_load_datafile(std::string path);

	std::unordered_map<Technology*, std::vector<Technology*>> costruct_groups();
	std::unordered_map<Technology*, TechnologyGroupData> plan_groups(std::unordered_map<Technology*, std::vector<Technology*>>* groups);
	void plan_group(std::unordered_map<Technology*, std::vector<Technology*>>* groups, std::unordered_map<Technology*, TechnologyGroupData>* groupdata, Technology* key);
	void render_groups(std::unordered_map<Technology*, std::vector<Technology*>>* groups, std::unordered_map<Technology*, TechnologyGroupData>* planned);
	void render_group(std::unordered_map<Technology*, std::vector<Technology*>>* groups, std::unordered_map<Technology*, TechnologyGroupData>* planned, Technology* key, int x=0);
};