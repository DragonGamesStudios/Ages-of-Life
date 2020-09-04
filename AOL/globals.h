#include "lib/Proto/Proto.h"
#include "allegrolib.h"

class Game {
public:
	Game();

	void createguis();
	void run();
	void quit();

protected:
	int screenw, screenh;
	Proto proto;

	Image mainmenuBg;
	Image menubutton;
	ALLEGRO_BITMAP* AOLicon;

	Font segoeuib;

	GUI mainmenu;

	bool menu;

	void draw();
	void update(double dt);
};