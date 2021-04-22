#include "LuaModLoader.h"
#include "lua_fn.h"

#include <iostream>
#include <sstream>

int LuaModLoader::require(lua_State* L)
{
	return lua_require(L, fs);
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
	ls->set_active(true);
}

void LuaModLoader::begin_stage(LoaderStage stage)
{
	current_stage = stage;

	if (current_stage != LoaderStage::STAGE_NEW_GAME)
	{
		executed_state = luaL_newstate();
		prepare_default_state(executed_state, fs);
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

		fs->enter_dir(mod_name);

		if (!fs->exists(file_to_run))
		{
			fs->exit();
			return true;
		}

		// Create a new state
		mod_state = luaL_newstate();
		prepare_default_state(mod_state, fs);
		l_storage->prepare_state(mod_state, allowed_prototypes[current_stage]);

		if (current_stage == LoaderStage::STAGE_NEW_GAME)
			l_savesystem->prepare_state(mod_state, mod_name);

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

		// Prepare error handler
		lua_pushcfunction(mod_state, generic_error_handler);

		// Execute the file
		if ((luaL_loadfile(mod_state, (fs->get_correct_path(file_to_run)).string().c_str()) || lua_pcall(mod_state, 0, LUA_MULTRET, 1)) != LUA_OK)
		{
			lua_getfield(mod_state, -1, "message");

			if (lua_type(mod_state, -1) == LUA_TTABLE)
				last_error.message = ((std::stringstream)"table 0x" << lua_topointer(mod_state, -1)).str();
			else
				last_error.message = lua_tostring(mod_state, -1);

			lua_pop(mod_state, 1);

			lua_getfield(mod_state, -1, "traceback");
			last_error.traceback = lua_tostring(mod_state, -1);

			lua_pop(mod_state, 1);

			return false;
		}

		lua_settop(mod_state, 0);

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
				last_error.message = "For some reason table `data` in the executed state was nil.";
				return false;
			}

			lua_getfield(executed_state, -1, "raw");
			if (lua_isnil(executed_state, -1))
			{
				lua_close(mod_state);
				lua_pop(executed_state, 2);
				last_error.message = "For some reason table `data.raw` in the executed state was nil.";
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
						last_error.message = "Prototype name: " + actual_name + " is not the same as expected " + p_name + ". Avoid editing prototype's name.";
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
						last_error.message = "Prototype type: " + actual_name + " is not the same as expected " + prototype_type + ". Avoid editing prototype's type.";
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
		mod_state = 0;

		fs->exit();
	}
	catch (const std::exception& e)
	{
		last_error.message = e.what();
		return false;
	}

	return true;
}

LuaError LuaModLoader::get_last_error()
{
	return last_error;
}
