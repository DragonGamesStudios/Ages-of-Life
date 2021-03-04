#include "LuaModule.h"

void LuaModule::check_active(lua_State* L)
{
	if (!active)
		luaL_error(L, "Attemp to call inactive function.");
}

LuaModule::LuaModule()
{
	active = false;
}

void LuaModule::set_active(bool v)
{
	active = v;
}
