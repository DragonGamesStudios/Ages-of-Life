#pragma once
#include "LuaModule.h"

class LuaGameScript : public LuaModule
{
public:
	LuaGameScript();

	void prepare_state(lua_State* L);
};