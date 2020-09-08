#include "lib/Proto/Proto.h"
#include "allegrolib.h"

typedef unsigned int uint;
typedef const char* ccptr;
typedef unsigned char uchar;
typedef std::function<void()> fn00;

class Script {
public:
	Script();

	std::vector<GUI*> guis;
	std::vector<GUI*>::size_type opened_guis_amount;
	std::unordered_map<GUI*, std::function<void()>> gui_open_functions;
	std::unordered_map<GUI*, std::function<void()>> gui_close_functions;
	
	void open_gui(GUI* gui);
	void close_gui(GUI* gui);
	bool get_gui_opened(GUI* gui);
	void register_open_gui_function(GUI* gui, std::function<void()> fn);
	void register_close_gui_function(GUI* gui, std::function<void()> fn);
};

class Game {
public:
	Game();

	void createguis();
	void run();
	void quit();

protected:
	int screenw, screenh;

	int lastmwpos;
	Proto proto;
	Script script;

	Image mainmenuBg;
	Image menubutton;
	Image defaultguiImage;
	Image closssssebutton;
	ALLEGRO_BITMAP* AOLicon;

	Font segoeuib;

	GUI mainmenu;
	GUI playmenu;
	GUI aboutgui;
	GUI creatorsgui;
	GUI licensesgui;

	bool menu;

	void draw();
	void update(double dt);
	void close();
	void scroll(int amt);
};