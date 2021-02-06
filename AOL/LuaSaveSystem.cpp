#include "LuaSaveSystem.h"
#include "lua_fn.h"

int LuaSaveSystem::get_saved(lua_State* L)
{
	std::string mod = current_mod;

	if (lua_type(L, 1) == LUA_TSTRING)
		mod = lua_tostring(L, 1);
	else if (lua_gettop(L))
	{
		lua_pushstring(L, ((std::string)"Invalid argument #1. Expected string/nil, got " + lua_typename(L, lua_type(L, 1))).c_str());
		lua_error(L);
	}

	auto it = mod_saves.find(mod);

	if (it == mod_saves.end())
	{
		lua_pushstring(L, "Mod with the given name does not exist.");
		lua_error(L);
	}

	lua_from_json(L, it->second);

	return 1;
}

int LuaSaveSystem::get_value_from_saved(lua_State* L)
{
	json ret;

	std::string mod = current_mod;
	if (lua_type(L, 1) == LUA_TTABLE)
	{
		lua_pushvalue(L, 1);
	}
	else if (lua_type(L, 1) == LUA_TSTRING && lua_type(L, 2) == LUA_TTABLE)
	{
		mod = lua_tostring(L, 1);
		lua_pushvalue(L, 2);
	}
	else
	{
		lua_pushstring(L, ((std::string)"Invalid arguments. Expected (table)/(string, table), got (" + lua_typename(L, lua_type(L, 1)) + ", " + lua_typename(L, lua_type(L, 2)) + ")").c_str());
	}

	auto it = mod_saves.find(mod);

	if (it == mod_saves.end())
	{
		lua_pushstring(L, "Mod with the given name does not exist.");
		lua_error(L);
	}

	std::deque<std::pair<std::string, int>> keys;

	size_t size = lua_rawlen(L, -1);
	for (size_t i = 1; i <= size; i++)
	{
		lua_rawgeti(L, -1, i);

		std::pair<std::string, int> key = { "", 0 };

		if (lua_type(L, -1) == LUA_TSTRING)
			key.first = lua_tostring(L, -1);
		else if (lua_type(L, -1) == LUA_TNUMBER)
			key.second = (int)lua_tointeger(L, -1) - 1;
		else
		{
			lua_pushstring(L, ((std::string)"Invalid key queue element type. String or number expected, got " + lua_typename(L, lua_type(L, -1))).c_str());
			lua_error(L);
		}

		keys.push_back(key);

		lua_pop(L, 1);
	}

	ret = get_json_value(it->second, keys);

	lua_pop(L, 1);

	if (keys.empty())
		lua_from_json(L, ret);
	else
		lua_pushnil(L);


	return 1;
}

int LuaSaveSystem::save(lua_State* L)
{
	if (lua_type(L, 1) == LUA_TTABLE)
	{
		mod_saves[current_mod] = lua_table_to_json(L, 1);
	}
	else
	{
		lua_pushstring(L, ((std::string)"Invalid argument 1. Table expected, got " + lua_typename(L, lua_type(L, 1))).c_str());
		lua_error(L);
	}

	return 0;
}

LuaSaveSystem::LuaSaveSystem()
{
	fs = 0;
}

void LuaSaveSystem::register_filesystem(art::FileSystem* filesystem)
{
	fs = filesystem;
}

void LuaSaveSystem::prepare_state(lua_State* L, const std::string& mod)
{
	current_mod = mod;

	json mod_save;

	if (fs->exists(mod + ".json"))
	{
		auto mod_file = fs->open_file(mod + ".json");
		mod_file >> mod_save;
	}

	mod_saves.insert({ mod, mod_save });

	lua_newtable(L);

	// Registering functions
	push_function(L, std::bind(&LuaSaveSystem::save, this, std::placeholders::_1));
	lua_setfield(L, -2, "save");

	push_function(L, std::bind(&LuaSaveSystem::get_value_from_saved, this, std::placeholders::_1));
	lua_setfield(L, -2, "get_value_from_saved");

	push_function(L, std::bind(&LuaSaveSystem::get_saved, this, std::placeholders::_1));
	lua_setfield(L, -2, "get_saved");

	lua_setglobal(L, "savesystem");
}

void LuaSaveSystem::save_mods()
{
	for (const auto& [mod, mod_save] : mod_saves)
	{
		if (mod_save.empty())
		{
			fs->delete_file_if_exists(mod + ".json");
		}
		else
		{
			auto mod_file = fs->open_ofile(mod + ".json");
			mod_file << std::setw(4) << mod_save << std::endl;
			mod_file.close();
		}
	}
}
