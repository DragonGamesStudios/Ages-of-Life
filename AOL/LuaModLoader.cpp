#include "LuaModLoader.h"
#include "lua_fn.h"

#include <iostream>

void LuaModLoader::prepare_state(lua_State* L)
{luaL_requiref(L, "_G", luaopen_base, 1);
	luaL_requiref(L, "math", luaopen_math, 1);
	luaL_requiref(L, "string", luaopen_string, 1);
	luaL_requiref(L, "table", luaopen_table, 1);
	luaL_requiref(L, "debug", luaopen_debug, 1);

	// Cpp function metatable
	luaL_newmetatable(L, fn_mt);

	// __call field (func())
	lua_pushcfunction(L, on_call);
	lua_setfield(L, -2, "__call");

	// __tostring field (tostring(func))
	lua_pushcfunction(L, on_tostring);
	lua_setfield(L, -2, "__tostring");

	// __gc field (garbage collection)
	lua_pushcfunction(L, on_garbage_collection);
	lua_setfield(L, -2, "__gc");

	// Globalize
	lua_setglobal(L, fn_mt);

	// Disable unsafe functions
	lua_getglobal(L, "_G");

	lua_pushnil(L);
	lua_setfield(L, -2, "dofile");

	lua_pushnil(L);
	lua_setfield(L, -2, "loadfile");

	lua_pop(L, 1);

	// Load require function
	push_function(L, std::bind(&LuaModLoader::require, this, std::placeholders::_1));
	lua_setglobal(L, "require");
}

int LuaModLoader::require(lua_State* L)
{
	std::string err;

	if (!lua_isstring(L, -1))
		err = (std::string)"Invalid argument to 'require'. String expected, got " + lua_typename(L, lua_type(L, -1)) + ".\n";

	if (!err.empty())
	{
		lua_pushstring(L, err.c_str());
		lua_error(L);
	}

	std::filesystem::path executed_file = correct_path(lua_tostring(L, -1));
	lua_State* executed_state = 0;

	switch (current_stage)
	{
	case LoaderStage::STAGE_SETTINGS:
		executed_state = settings_state;
		break;
	}

	if (!fs->exists(executed_file) )
		err = "File on path " + executed_file.string() + " not found.\n";

	if (!err.empty())
	{
		lua_pushstring(L, err.c_str());
		lua_error(L);
	}

	int beginning_stack_size = lua_gettop(executed_state);

	// Execute the file
	if (luaL_dofile(executed_state, (fs->get_correct_path(executed_file)).string().c_str()) != LUA_OK)
	{
		lua_pushstring(L, lua_tostring(executed_state, -1));
		lua_error(L);
	}

	int end_stack_size = lua_gettop(executed_state);

	// If file returned any variables, return them with stack
	for (int i = beginning_stack_size + 1; i < end_stack_size + 1; i++)
	{
		if (!move_lua_value(executed_state, L, i))
		{
			lua_pushstring(L, "Invalid key/value in returned value.");
			lua_error(L);
		}
	}

	return end_stack_size - beginning_stack_size;
}

std::filesystem::path LuaModLoader::correct_path(const std::string& lua_path)
{
	std::string word;
	std::filesystem::path result;

	for (const char c : lua_path)
	{
		if (c != '.')
			word.push_back(c);
		else if (!word.empty())
		{
			result /= word;
			word.clear();
		}
	}

	word += ".lua";

	if (!word.empty())
	{
		result /= word;
		word.clear();
	}

	return result;
}

bool LuaModLoader::move_lua_value(lua_State* from, lua_State* to, int from_idx)
{
	switch (lua_type(from, from_idx))
	{
	case LUA_TNIL:
		lua_pushnil(to);
		break;

	case LUA_TNUMBER:
		lua_pushnumber(to, lua_tonumber(from, from_idx));
		break;

	case LUA_TBOOLEAN:
		lua_pushboolean(to, lua_toboolean(from, from_idx));
		break;

	case LUA_TFUNCTION:
		lua_pushcfunction(to, lua_tocfunction(from, from_idx));
		break;

	case LUA_TLIGHTUSERDATA:
		lua_pushlightuserdata(to, lua_touserdata(from, from_idx));
		break;

	case LUA_TSTRING:
		lua_pushstring(to, lua_tostring(from, from_idx));
		break;

	case LUA_TTABLE:
		lua_newtable(to);

		lua_pushvalue(from, from_idx);

		lua_pushnil(from);
		while (lua_next(from, -2))
		{
			move_lua_value(from, to, -2);
			move_lua_value(from, to, -1);

			if (lua_isnil(to, -2))
			{
				return false;
			}

			lua_settable(to, -3);
			lua_pop(from, 1);
		}

		lua_pop(from, 2);
		break;

	default:
		lua_pushnil(to);
		return false;
		break;
	}

	return true;
}

LuaModLoader::LuaModLoader()
{
	l_settings = 0;
	settings_state = 0;
	data_state = 0;
	current_stage = LoaderStage::STAGE_NONE;
	fs = 0;
}

LuaModLoader::~LuaModLoader()
{
	if (settings_state)
	{
		settings_state = 0;
		lua_close(settings_state);
	}

	if (data_state)
	{
		lua_close(data_state);
		data_state = 0;
	}
}

void LuaModLoader::register_filesystem(art::FileSystem* fs_)
{
	fs = fs_;
}

void LuaModLoader::register_l_settings(LuaSettings* ls)
{
	l_settings = ls;
}

void LuaModLoader::begin_stage(LoaderStage stage)
{
	current_stage = stage;

	switch (stage)
	{
	case LoaderStage::STAGE_SETTINGS:
		settings_state = luaL_newstate();
		prepare_state(settings_state);
		l_settings->prepare_state(settings_state);
		break;
	}
}

void LuaModLoader::end_stage()
{
	current_stage = LoaderStage::STAGE_NONE;

	l_settings->load_prototypes(settings_state);

	lua_close(settings_state);
	settings_state = 0;
}

bool LuaModLoader::load_mod(const std::string& mod_name, const std::filesystem::path& file_to_run)
{
	std::cout << "Loading mod " << mod_name << "...\n";

	fs->enter_dir(mod_name);

	lua_State* executed_state = 0;

	switch (current_stage)
	{
	case LoaderStage::STAGE_SETTINGS:
		executed_state = settings_state;
		break;
	}

	if (!fs->exists(file_to_run))
	{
		std::cout << "Skipping\n";
		return true;
	}

	// Execute the file
	if (luaL_dofile(executed_state, (fs->get_correct_path(file_to_run)).string().c_str()) != LUA_OK)
	{
		last_error = lua_tostring(executed_state, -1);
		return false;
	}

	fs->exit();

	return true;
}

std::string LuaModLoader::get_last_error()
{
	return last_error;
}
