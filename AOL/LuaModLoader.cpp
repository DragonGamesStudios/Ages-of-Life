#include "LuaModLoader.h"
#include "lua_fn.h"

#include <iostream>
#include <sstream>

void LuaModLoader::prepare_state(lua_State* L)
{
	luaL_requiref(L, "_G", luaopen_base, 1);
	luaL_requiref(L, "math", luaopen_math, 1);
	luaL_requiref(L, "string", luaopen_string, 1);
	luaL_requiref(L, "table", luaopen_table, 1);
	luaL_requiref(L, "debug", luaopen_debug, 1);

	lua_settop(L, 0);

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

	// Pop
	lua_pop(L, 1);

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

	if (!fs->exists(executed_file) )
		err = "File on path " + executed_file.string() + " not found.\n";

	if (!err.empty())
	{
		lua_pushstring(L, err.c_str());
		lua_error(L);
	}

	int beginning_stack_size = lua_gettop(mod_state);

	// Execute the file
	if (luaL_dofile(mod_state, (fs->get_correct_path(executed_file)).string().c_str()) != LUA_OK)
	{
		lua_pushstring(L, lua_tostring(mod_state, -1));
		lua_error(L);
	}

	int end_stack_size = lua_gettop(mod_state);

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

void LuaModLoader::print_stack(lua_State* stack)
{
	for (int i = 1; i <= lua_gettop(stack); i++)
	{
		std::cout << i << ". "<< lua_typename(stack, lua_type(stack, i)) << "\t" << get_string(stack, i) << "\n";
	}
}

std::string LuaModLoader::get_string(lua_State* L, int idx)
{
	switch (lua_type(L, idx))
	{
	case LUA_TTABLE:
		return ((std::stringstream)"table " << lua_topointer(L, idx)).str();
		break;

	case LUA_TNUMBER:
		return std::to_string(lua_tonumber(L, idx));
		break;

	case LUA_TSTRING:
		return lua_tostring(L, idx);
		break;

	default:
		return "nil";
		break;
	}
}

LuaModLoader::LuaModLoader()
{
	l_storage = 0;
	l_savesystem = 0;
	executed_state = 0;
	mod_state = 0;
	current_stage = LoaderStage::STAGE_NONE;
	fs = 0;

	allowed_prototypes = {
		{LoaderStage::STAGE_SETTINGS, {"setting"}},
		{LoaderStage::STAGE_PROTOTYPES, {}},
		{LoaderStage::STAGE_DATA, {}},
		{LoaderStage::STAGE_NEW_GAME, {}},
	};
}

LuaModLoader::~LuaModLoader()
{
	if (executed_state)
	{
		lua_close(executed_state);
		executed_state = 0;
	}

	if (mod_state)
	{
		lua_close(mod_state);
		mod_state = 0;
	}
}

void LuaModLoader::register_filesystem(art::FileSystem* fs_)
{
	fs = fs_;
}

void LuaModLoader::register_l_storage(LuaStorage* ls)
{
	l_storage = ls;
}

void LuaModLoader::register_l_savesystem(LuaSaveSystem* ls)
{
	l_savesystem = ls;
}

void LuaModLoader::begin_stage(LoaderStage stage)
{
	current_stage = stage;

	if (current_stage != LoaderStage::STAGE_NEW_GAME)
	{
		executed_state = luaL_newstate();
		prepare_state(executed_state);
		l_storage->prepare_state(executed_state, allowed_prototypes[current_stage]);
	}
}

bool LuaModLoader::end_stage()
{
	if ((int)current_stage & ((int)LoaderStage::STAGE_SETTINGS | (int)LoaderStage::STAGE_DATA | (int)LoaderStage::STAGE_PROTOTYPES))
	{
		if (!l_storage->load_prototypes(executed_state, allowed_prototypes[current_stage]))
			return false;
	}
	else if (current_stage == LoaderStage::STAGE_NEW_GAME)
		l_savesystem->save_mods();

	current_stage = LoaderStage::STAGE_NONE;

	if (executed_state)
	{
		lua_close(executed_state);
		executed_state = 0;
	}

	return true;
}

LoaderStage LuaModLoader::get_current_stage() const
{
	return current_stage;
}

bool LuaModLoader::load_mod(const std::string& mod_name, const std::filesystem::path& file_to_run)
{
	try
	{
		std::cout << "Loading mod " << mod_name << "...\n";

		fs->enter_dir(mod_name);

		// Create a new state
		mod_state = luaL_newstate();
		prepare_state(mod_state);
		l_storage->prepare_state(mod_state, allowed_prototypes[current_stage]);

		if (current_stage == LoaderStage::STAGE_NEW_GAME)
			l_savesystem->prepare_state(mod_state, mod_name);

		if (!fs->exists(file_to_run))
		{
			std::cout << "Skipping\n";
			return true;
		}

		// Migrating data
		if ((int)current_stage & ((int)LoaderStage::STAGE_SETTINGS | (int)LoaderStage::STAGE_DATA | (int)LoaderStage::STAGE_PROTOTYPES))
		{
			// Move data
			lua_getglobal(executed_state, "data");
			lua_getfield(executed_state, -1, "raw");

			lua_getglobal(mod_state, "data");
			move_lua_value(executed_state, mod_state, -1);
			lua_setfield(mod_state, -2, "raw");

			lua_pop(mod_state, 1);
			lua_pop(executed_state, 2);
		}

		// Execute the file
		if (luaL_dofile(mod_state, (fs->get_correct_path(file_to_run)).string().c_str()) != LUA_OK)
		{
			last_error = lua_tostring(mod_state, -1);
			return false;
		}

		// Loading data
		if ((int)current_stage & ((int)LoaderStage::STAGE_SETTINGS | (int)LoaderStage::STAGE_DATA | (int)LoaderStage::STAGE_PROTOTYPES))
		{
			// Getting the tables
			lua_getglobal(mod_state, "data");
			if (lua_isnil(mod_state, -1))
			{
				lua_close(mod_state);
				return true;
			}

			lua_pushstring(mod_state, "raw");
			lua_rawget(mod_state, -2);
			if (lua_isnil(mod_state, -1))
			{
				lua_close(mod_state);
				return true;
			}

			lua_getglobal(executed_state, "data");
			if (lua_isnil(executed_state, -1))
			{
				lua_close(mod_state);
				lua_pop(executed_state, 1);
				last_error = "For some reason table `data` in the executed state was nil.";
				return false;
			}

			lua_getfield(executed_state, -1, "raw");
			if (lua_isnil(executed_state, -1))
			{
				lua_close(mod_state);
				lua_pop(executed_state, 2);
				last_error = "For some reason table `data.raw` in the executed state was nil.";
				return false;
			}

			// Comparison (history loading)
			for (const auto& prototype_type : allowed_prototypes[current_stage])
			{
				lua_pushstring(mod_state, prototype_type.c_str());
				lua_rawget(mod_state, -2);
				if (lua_isnil(mod_state, -1))
				{
					lua_pop(mod_state, -1);
					continue;
				}

				lua_getfield(executed_state, -1, prototype_type.c_str());

				lua_pushnil(mod_state);
				while (lua_next(mod_state, -2))
				{
					std::string p_name = lua_tostring(mod_state, -2);

					// Name check
					lua_pushstring(mod_state, "name");
					lua_rawget(mod_state, -2);
					std::string actual_name = lua_tostring(mod_state, -1);
					if (actual_name != p_name)
					{
						last_error = "Prototype name: " + actual_name + " is not the same as expected " + p_name + ". Avoid editing prototype's name.";
						lua_close(mod_state);
						lua_settop(executed_state, 0);
						return false;
					}
					lua_pop(mod_state, 1);

					// Type check
					lua_pushstring(mod_state, "type");
					lua_rawget(mod_state, -2);
					std::string actual_type = lua_tostring(mod_state, -1);
					if (actual_type != prototype_type)
					{
						last_error = "Prototype type: " + actual_name + " is not the same as expected " + prototype_type + ". Avoid editing prototype's type.";
						lua_close(mod_state);
						lua_settop(executed_state, 0);
						return false;
					}
					lua_pop(mod_state, 1);

					lua_getfield(executed_state, -1, p_name.c_str());

					if (!lua_isnil(executed_state, -1))
					{
						// This prototype was edited or left as it was
						if (!are_same(mod_state, executed_state, -1, -1))
							prototype_histories[prototype_type][p_name].push_back(mod_name);
					}
					else
					{
						// This is a new prototype: create new history
						prototype_histories[prototype_type].insert({ p_name, {mod_name} });
					}

					lua_pop(executed_state, 1);
					move_lua_value(mod_state, executed_state, -1);
					lua_setfield(executed_state, -2, p_name.c_str());
					lua_pop(executed_state, 1);

					lua_pop(mod_state, 1);
				}
			}

			// Finalizing
			lua_pop(executed_state, 2);
		}


		lua_close(mod_state);
		fs->exit();
	}
	catch (std::exception& e)
	{
		last_error = e.what();
		return false;
	}

	return true;
}

std::string LuaModLoader::get_last_error()
{
	return last_error;
}
