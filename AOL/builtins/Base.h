#pragma once

#include "GameObject.h"

struct LuaBasePrototype : LuaGameObjectPrototype
{
	LuaBasePrototype(lua_State* L);

	std::string order;
	art::LocalisedString localised_name;
	art::LocalisedString localised_description;
};
