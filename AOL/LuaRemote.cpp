#include "LuaRemote.h"

int LuaRemote::add_interface(lua_State* L)
{
	check_active(L);

	// remote.add_interface(string, table[string -> function])
	fn_typecheck(L, 1, LUA_TSTRING, "String");
	fn_typecheck(L, 2, LUA_TTABLE, "Table");

	if (lua_type(L, 2) != LUA_TTABLE)
		luaL_error(L, ((std::string)"Invalid argument 2 type. Table expected, got " + lua_typename(L, lua_type(L, 2))).c_str());

	std::string interface_name = lua_tostring(L, 1);

	// Create new interface
	lua_interface new_interface;

	lua_pushvalue(L, 2);

	lua_pushnil(L);
	while (lua_next(L, -2))
	{
		if (lua_type(L, -2) != LUA_TSTRING)
			luaL_error(L, ((std::string)"Invalid interface key type. String expected, got " + lua_typename(L, lua_type(L, 1))).c_str());

		if (lua_type(L, -1) != LUA_TFUNCTION)
			luaL_error(L, ((std::string)"Invalid interface value type. Function expected, got " + lua_typename(L, lua_type(L, 1))).c_str());

		// Add function to interface
		std::string func_name = lua_tostring(L, -2);
		LuaFunction func;

		if (lua_dump(L, lf_writer, &func, false))
			luaL_error(L, "Something went wrong when dumping given function.");

		new_interface.insert({ func_name, func });

		// Pop the value
		lua_pop(L, 1);
	}

	// Add interface
	if (!interfaces.insert({ interface_name, new_interface }).second)
		luaL_error(L, "Interface with the given name already exists.");

	// Update other states
	for (auto state : states)
		add_interface_to_state(state, interface_name, new_interface);

	return 0;
}

int LuaRemote::remove_interface(lua_State* L)
{
	check_active(L);

	fn_typecheck(L, 1, LUA_TSTRING, "String");

	auto interface_it = interfaces.find(lua_tostring(L, 1));

	if (interface_it == interfaces.end())
	{
		lua_pushboolean(L, false);
	}
	else
	{

		// Update states
		for (auto state : states)
		{
			// Get remote.interfaces
			lua_getglobal(state, "remote");

			lua_pushstring(state, "interfaces");

			lua_rawget(state, -2);

			// Remove interface
			lua_pushstring(state, interface_it->first.c_str());

			lua_pushnil(state);

			lua_rawset(state, -2);

			// Pop out of existence
			lua_settop(state, 0);
		}

		// Delete functions in memory
		for (auto [name, fn] : interface_it->second)
		{
			fn.data.clear();
		}

		// Erase interface
		interfaces.erase(interface_it);

		lua_pushboolean(L, true);
	}

	return 1;
}

int LuaRemote::call(lua_State* L)
{
	check_active(L);

	// remote.call(string, string, ...)
	fn_typecheck(L, 1, LUA_TSTRING, "String");
	fn_typecheck(L, 2, LUA_TSTRING, "String");

	int arg_count = lua_gettop(L) - 2;

	std::string iname = lua_tostring(L, 1);
	std::string fname = lua_tostring(L, 2);

	// Push errhandler
	lua_pushcfunction(L, generic_error_handler);
	int fpos = lua_gettop(L);

	// Get remote.interfaces[iname][fname]
	lua_getglobal(L, "remote");

	lua_pushstring(L, "interfaces");
	lua_rawget(L, -2);

	lua_pushstring(L, iname.c_str());
	lua_rawget(L, -2);

	if (lua_isnil(L, -1))
		luaL_error(L, "Interface does not exist");

	lua_pushstring(L, fname.c_str());
	lua_rawget(L, -2);

	// Stack:
	// -1: remote.interfaces[iname][fname]

	// Delete the tables
	for (int i = 0; i < 3; i++)
		lua_remove(L, -2);

	for (int i = -arg_count - 2; i < -2; i++)
	{
		lua_pushvalue(L, i);
	}

	int stack_size = lua_gettop(L);

	if (lua_pcall(L, arg_count, LUA_MULTRET, fpos) != 0)
	{
		luaL_error(L, "Something went wrong when running the function");
	}

	return lua_gettop(L) - stack_size;
}

void LuaRemote::add_interface_to_state(lua_State* L, const std::string& name, const lua_interface& iface, int level)
{
	switch (level)
	{
	case 0:
		lua_getglobal(L, "remote");

		[[fallthrough]];

	case 1:
		lua_pushstring(L, "interfaces");
		lua_rawget(L, -2);

		break;
	}

	// Push interface name and create its table
	lua_pushstring(L, name.c_str());

	lua_newtable(L);

	for (const auto& [f_name, func] : iface)
	{
		// Push function name
		lua_pushstring(L, f_name.c_str());

		// Push function
		lua_load(L, lf_reader, (void*)&func, f_name.c_str(), 0);

		lua_rawset(L, -3);
	}

	lua_rawset(L, -3);

	lua_pop(L, 2 - level);
}

LuaRemote::LuaRemote() : LuaModule()
{
}

void LuaRemote::prepare_state(lua_State* L)
{
	lua_newtable(L);

	push_function(L, std::bind(&LuaRemote::add_interface, this, std::placeholders::_1));
	lua_setfield(L, -2, "add_interface");

	push_function(L, std::bind(&LuaRemote::remove_interface, this, std::placeholders::_1));
	lua_setfield(L, -2, "remove_interface");

	push_function(L, std::bind(&LuaRemote::call, this, std::placeholders::_1));
	lua_setfield(L, -2, "call");

	lua_newtable(L);

	for (const auto& [iname, iface] : interfaces)
		add_interface_to_state(L, iname, iface, 2);

	lua_setfield(L, -2, "interfaces");

	lua_setglobal(L, "remote");

	states.push_back(L);
}
