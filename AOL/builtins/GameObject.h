#pragma once
#include <memory>
#include <string>
#include <functional>

#include "..\lua_fn.h"

struct LuaGameObjectPrototype
{
	LuaGameObjectPrototype(lua_State* L);

	std::string name;

	bool is_valid;
	std::string error;
};