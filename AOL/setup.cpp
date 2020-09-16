#include "globals.h"
#include "classes/Age.h"

void Game::setup_ages()
{
	AgePrototype* stoneage = new AgePrototype(
		"vanilla-age-stone-age",
		"vanilla-age-copper-age",
		"base/graphics/ages/icons/stone-age.png",
		"base/graphics/ages/labels/stone-age.png",
		{51, 51, 51},
		{100, 100, 100},
		"after"
	);

	AgePrototype* copperage = new AgePrototype(
		"vanilla-age-copper-age",
		"vanilla-age-stone-age",
		"base/graphics/ages/icons/copper-age.png",
		"base/graphics/ages/labels/copper-age.png",
		{ 250, 174, 74 },
		{ 100, 100, 100 },
		"previous"
	);

	this->register_prototype(stoneage, "age");
	this->register_prototype(copperage, "age");

	this->api_load_datafile("testmod.lua");
}
