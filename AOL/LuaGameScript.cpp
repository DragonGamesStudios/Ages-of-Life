#include "LuaGameScript.h"

LuaGameScript::LuaGameScript() : LuaModule()
{
}

void LuaGameScript::prepare_state(lua_State* L)
{
	lua_newtable(L);

	lua_setglobal(L, "game");
}
