#pragma once

#include "LuaModule.h"

#include <art/FileSystem.h>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class LuaSaveSystem : public LuaModule
{
private:
	art::FileSystem* fs;
	
	std::string current_mod;
	std::map<std::string, json> mod_saves;

	// Lua API functions
	int get_saved(lua_State* L);
	int get_value_from_saved(lua_State* L);
	int save(lua_State* L);

public:
	LuaSaveSystem();

	void register_filesystem(art::FileSystem* filesystem);
	void prepare_state(lua_State* L, const std::string& mod);
	// To make compiler happy
	void prepare_state(lua_State* L);
	void save_mods();

	void set_current_mod(const std::string& mod);
};