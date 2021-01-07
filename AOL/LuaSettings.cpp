#include "LuaSettings.h"
#include "lua_fn.h"

#include <iostream>
#include <sstream>

LuaSettings::LuaSettings()
{
	settings = 0;
}

void LuaSettings::prepare_state(lua_State* L)
{
	// Creating global settings table
	lua_newtable(L);
	lua_setglobal(L, "settings");
	lua_getglobal(L, "settings");

	// Adding raw field
	lua_newtable(L);
	lua_setfield(L, -2, "raw");

	// Adding register function
	push_function(L, std::bind(&LuaSettings::register_prototypes, this, std::placeholders::_1));
	lua_setfield(L, -2, "register");

	// Cleanup
	lua_pop(L, 1);
}

void LuaSettings::load_prototypes(lua_State* L)
{
	std::cout << "Loading prototypes\n";
}

int LuaSettings::register_prototypes(lua_State* L)
{
	std::string err;

	if (!lua_istable(L, 2))
	{
		err = (std::string)"Invalid argument. Table expected, got " + lua_typename(L, lua_type(L, -1)) + ".\n";
		lua_pushstring(L, err.c_str());
		lua_error(L);
	}

	// Preparing settings.raw
	lua_getfield(L, 1, "raw");

	lua_pushvalue(L, 2);
	lua_remove(L, 2);

	/*
	Stack state:
	-1: prototype array
	-2: settings.raw
	*/

	lua_pushnil(L);
	while (lua_next(L, -2))
	{
		// Error checking
		if (!lua_isinteger(L, -2))
			err = (std::string)"Invalid prtotype array. Numeric array expected, got index of type: '" + lua_typename(L, lua_type(L, -2)) + "'.\n";

		if (!lua_istable(L, -1) && err.empty())
			err = (std::string)"Invalid prototype. Table expected, got " + lua_typename(L, lua_type(L, -1)) + ".\n";

		lua_getfield(L, -1, "name");
		if (lua_isnil(L, -1) && err.empty())
			err = "Invalid prototype. Field 'name' not found.\n";
		lua_pop(L, 1);

		if (!err.empty())
		{
			lua_pushstring(L, err.c_str());
			lua_error(L);
		}

		// Table registration
		/*
		Stack state:
		-1: value
		-2: key
		-3: prototype array
		-4: settings.raw :1
		*/

		// Get prototype name
		lua_getfield(L, -1, "name");

		// Push prototype
		lua_pushvalue(L, -2);

		/*
		Stack state:
		-1: value
		-2: prototype name
		-3: value
		-4: key
		-5: prototype array
		-6: settings.raw :1
		*/

		lua_settable(L, 2);

		/*
		Stack state:
		-1: value
		-2: key
		-3: prototype array
		-4: settings.raw :1
		*/

		// Pop value
		lua_pop(L, 1);
	}

	/*
	Stack state:
	-1: prototype array
	-2: settings.raw
	*/

	lua_pop(L, 2);

	return 0;
}

void LuaSettings::register_settings(Settings* s)
{
	settings = s;
}
