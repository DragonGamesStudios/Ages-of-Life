#include "LuaEventHandler.h"
#include "lua_fn.h"

#include <stdexcept>
#include <vector>
#include <string>

const int event_amount = 1;

int LuaEventHandler::register_function(lua_State* L, std::map<lua_State*, int>* fn_map)
{
	fn_typecheck(L, 1, LUA_TFUNCTION, "Function");

	fn_map->operator[](L) = luaL_ref(L, LUA_REGISTRYINDEX);

	return 0;
}

int LuaEventHandler::reg_on_event(lua_State* L)
{
	fn_typecheck(L, 1, LUA_TNUMBER, "Number");
	fn_typecheck(L, 2, LUA_TFUNCTION, "Function");

	lua_settop(L, 2);

	int e_type = (int)lua_tointeger(L, 1);

	if (e_type >= event_amount)
		luaL_error(L, "Invalid event type - event does not exist.");

	on_event[(GameEventType)e_type][L] = luaL_ref(L, LUA_REGISTRYINDEX);

	return 0;
}

LuaEventHandler::LuaEventHandler() : LuaModule()
{
	on_event = {
		{ GameEventType::on_tick, {} }
	};
}

LuaEventHandler::~LuaEventHandler()
{
	// Unref the functions and free the registry
	for (const auto& [L, fn] : on_load)
		luaL_unref(L, LUA_REGISTRYINDEX, fn);

	for (const auto& [L, fn] : on_configuration_changed)
		luaL_unref(L, LUA_REGISTRYINDEX, fn);

	for (const auto& [L, fn] : on_ready)
		luaL_unref(L, LUA_REGISTRYINDEX, fn);

	for (const auto& [L, fn] : on_save)
		luaL_unref(L, LUA_REGISTRYINDEX, fn);

	for (const auto& [e, functions] : on_event)
		for (const auto& [L, fn] : functions)
			luaL_unref(L, LUA_REGISTRYINDEX, fn);

	on_load.clear();
	on_configuration_changed.clear();
	on_ready.clear();
	on_save.clear();
	on_event.clear();
}

void LuaEventHandler::prepare_state(lua_State* L)
{
	lua_newtable(L);

	push_function(L, std::bind(&LuaEventHandler::register_function, this, std::placeholders::_1, &on_load));
	lua_setfield(L, -2, "on_load");

	push_function(L, std::bind(&LuaEventHandler::register_function, this, std::placeholders::_1, &on_configuration_changed));
	lua_setfield(L, -2, "on_configuration_changed");

	push_function(L, std::bind(&LuaEventHandler::register_function, this, std::placeholders::_1, &on_ready));
	lua_setfield(L, -2, "on_ready");

	push_function(L, std::bind(&LuaEventHandler::register_function, this, std::placeholders::_1, &on_save));
	lua_setfield(L, -2, "on_save");

	push_function(L, std::bind(&LuaEventHandler::reg_on_event, this, std::placeholders::_1));
	lua_setfield(L, -2, "on_event");

	lua_setglobal(L, "script");
}

void LuaEventHandler::run_arg0_ret0(const std::map<lua_State*, int>& fn_map, lua_State* L)
{
	auto fn_it = fn_map.find(L);

	if (fn_it != fn_map.end())
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, fn_it->second);

		if (lua_pcall(L, 0, 0, 0) != 0)
			throw std::runtime_error(lua_tostring(L, -1));

		lua_pop(L, 1);
	}
}

void LuaEventHandler::run_on_load(lua_State* L)
{
	run_arg0_ret0(on_load, L);
}

void LuaEventHandler::run_on_configuration_changed(const std::map<std::string, std::string>& configuration, bool was_changed, lua_State* L)
{
	auto fn_it = on_configuration_changed.find(L);

	if (fn_it != on_configuration_changed.end())
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, fn_it->second);

		lua_newtable(L);

		for (const auto& [key, value] : configuration)
		{
			lua_pushstring(L, value.c_str());
			lua_setfield(L, -2, key.c_str());
		}

		lua_pushboolean(L, was_changed);

		if (lua_pcall(L, 2, 0, 0) != 0)
			throw std::runtime_error(lua_tostring(L, -1));

		lua_settop(L, 0);
	}
}

void LuaEventHandler::run_on_ready(lua_State* L)
{
	run_arg0_ret0(on_ready, L);
}

void LuaEventHandler::run_on_save(lua_State* L)
{
	run_arg0_ret0(on_save, L);
}

void LuaEventHandler::prepare_event(const GameEvent& e)
{
	prepared_event = e;
	event_it = on_event.find(e.type);
}

void LuaEventHandler::raise_event(lua_State* L)
{
	auto fn_it = event_it->second.find(L);

	if (fn_it != event_it->second.end())
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX,fn_it->second);
		prepared_event.to_lua(L);

		if (lua_pcall(L, 1, 0, 0) != 0)
			throw std::runtime_error(lua_tostring(L, -1));

		lua_settop(L, 0);
	}
}
