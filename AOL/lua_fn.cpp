#include "lua_fn.h"

#include <iostream>
#include <sstream>

extern const char* const fn_mt;
const char* const fn_mt = "LuaCPPFunctionMetatable";

int on_call(lua_State* L)
{
	lua_fn* f = static_cast<lua_fn*>(luaL_checkudata(L, 1, fn_mt));
	lua_remove(L, 1);
	return (*f)(L);
}

int on_garbage_collection(lua_State* L)
{
	lua_fn* f = static_cast<lua_fn*>(luaL_checkudata(L, 1, fn_mt));
	if (!f)
	{
		lua_pushstring(L, "C++ error: problem with function deleteion");
		lua_error(L);
	}
	else
	{
		f->~lua_fn();
		//delete f;
	}

	return 0;
}

int on_tostring(lua_State* L)
{
	lua_fn* f = static_cast<lua_fn*>(luaL_checkudata(L, 1, fn_mt));

	std::stringstream ss;
	ss << "C++ function on " << std::hex << f;
	lua_pushstring(L, ss.str().c_str());

	return 1;
}

void push_function(lua_State* L, const lua_fn& fn)
{
	// Allocate memory
	void * new_fn = lua_newuserdata(L, sizeof(lua_fn));

	// Set metatable to func
	luaL_getmetatable(L, fn_mt);
	lua_setmetatable(L, -2);

	// Set variable
	new (new_fn) lua_fn(fn);
}
