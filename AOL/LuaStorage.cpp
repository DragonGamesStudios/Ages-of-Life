#include "LuaStorage.h"
#include "lua_fn.h"
#include "builtins/builtins.h"

#include <iostream>
#include <sstream>

LuaStorage::LuaStorage()
{
	storage = 0;
	builtin_prototypes = {
		{"game-object", GameObject::class_id},
		{"setting", Setting::class_id}
	};
}

void LuaStorage::prepare_state(lua_State* L, const std::vector<std::string>& fields)
{
	// Creating global settings table
	lua_newtable(L);

	// Adding raw field
	lua_newtable(L);
	lua_setfield(L, -2, "raw");

	// Adding fields
	lua_getfield(L, -1, "raw");

	for (const auto& field : fields)
	{
		lua_newtable(L);
		lua_setfield(L, -2, field.c_str());
	}

	lua_pop(L, 1);

	// Adding register function
	push_function(L, std::bind(&LuaStorage::register_prototypes, this, std::placeholders::_1));
	lua_setfield(L, -2, "register");

	// Cleanup
	lua_setglobal(L, "data");
}

bool LuaStorage::load_prototype(lua_State* L)
{
	std::string prototype_type;

	lua_getfield(L, -1, "type");

	if (lua_type(L, -1) != LUA_TSTRING)
		return false;

	prototype_type = lua_tostring(L, -1);
	lua_pop(L, 1);

	auto type_it = builtin_prototypes.find(prototype_type);

	if (type_it == builtin_prototypes.end())
	{
		last_error = "Invalid prototype '" + prototype_type + "'.";
		return false;
	}
	else
	{
		switch (type_it->second)
		{
		case GameObject::class_id:
			std::cout << "Registering game-object";
			break;

		case Setting::class_id:
			std::cout << "Registering setting";
			break;
		}
	}

	return true;
}

bool LuaStorage::load_prototypes(lua_State* L, const std::vector<std::string>& allowed_prototypes)
{
	std::cout << "Loading prototypes\n";

	// Get settings.raw on top of the stack
	lua_getglobal(L, "data");
	lua_getfield(L, -1, "raw");
	lua_remove(L, -2);

	for (const auto& p_type : allowed_prototypes)
	{
		lua_getfield(L, -1, p_type.c_str());

		// Iterate
		lua_pushnil(L);
		while (lua_next(L, -2))
		{
			if (!load_prototype(L))
				return false;

			lua_pop(L, 1);
		}

		lua_pop(L, 1);
	}

	// Pop settings.raw
	lua_pop(L, 1);
	return true;
}

int LuaStorage::register_prototypes(lua_State* L)
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
	-1: settings.raw
	-2: prototype array
	*/

	lua_pushnil(L);
	while (lua_next(L, -2))
	{
		std::string p_type = "";
		std::string p_name = "";

		// Error checking
		if (!lua_isinteger(L, -2))
			err = (std::string)"Invalid prtotype array. Numeric array expected, got index of type: '" + lua_typename(L, lua_type(L, -2)) + "'.\n";

		if (!lua_istable(L, -1) && err.empty())
			err = (std::string)"Invalid prototype. Table expected, got " + lua_typename(L, lua_type(L, -1)) + ".\n";

		lua_pushstring(L, "name");
		lua_rawget(L, -2);
		if (lua_isnil(L, -1) && err.empty())
			err = "Invalid prototype. Field 'name' not found.\n";
		else
			p_name = lua_tostring(L, -1);
		lua_pop(L, 1);

		lua_pushstring(L, "type");
		lua_rawget(L, -2);
		if (lua_isnil(L, -1) && err.empty())
			err = "Invalid prototype. Field 'type' not found.\n";
		else
			p_type = lua_tostring(L, -1);
		lua_pop(L, 1);

		// Get proper prototype table
		lua_pushstring(L, p_type.c_str());
		lua_rawget(L, 2);

		if (lua_isnil(L, -1) && err.empty())
			err = "Invalid prototype. Unknown type " + p_type + ".\n";

		if (!err.empty())
		{
			lua_pushstring(L, err.c_str());
			lua_error(L);
		}

		// Table registration
		/*
		Stack state:
		-1: settings.raw.type
		-2: value
		-3: key
		-4: settings.raw
		-5: prototype array :1
		*/

		// Get prototype name
		lua_pushstring(L, p_name.c_str());

		// Push prototype
		lua_pushvalue(L, -3);

		/*
		Stack state:
		-1: value
		-2: prototype name
		-3: settings.raw.type
		-4: value
		-5: key
		-6: settings.raw
		-7: prototype array :1
		*/

		lua_settable(L, -3);

		/*
		Stack state:
		-1: settings.raw.type
		-2: value
		-3: key
		-4: settings.raw
		-5: prototype array :1
		*/

		// Pop value
		lua_pop(L, 2);
	}

	/*
	Stack state:
	-1: settings.raw
	-2: prototype array
	*/

	lua_pop(L, 2);

	return 0;
}

void LuaStorage::register_storage(Storage* s)
{
	storage = s;
}

std::string LuaStorage::get_last_error()
{
	return last_error;
}
