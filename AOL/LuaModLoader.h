#pragma once

#include "LuaStorage.h"
#include "LuaSaveSystem.h"
#include "LoaderStage.h"

#include "lua_fn.h"

class LuaModLoader
{
private:
	lua_State* executed_state;
	lua_State* mod_state;

	LuaStorage* l_storage;
	LuaSaveSystem* l_savesystem;

	LoaderStage current_stage;

	LuaError last_error;
	art::FileSystem* fs;

	std::map<LoaderStage, std::vector<std::string>> allowed_prototypes;

	std::map<std::string, std::map<std::string, std::vector<std::string>>> prototype_histories;

	int require(lua_State* L);

public:
	LuaModLoader();
	~LuaModLoader();

	void register_filesystem(art::FileSystem* fs_);

	void register_l_storage(LuaStorage* ls);
	void register_l_savesystem(LuaSaveSystem* ls);

	void begin_stage(LoaderStage stage);
	bool end_stage();
	LoaderStage get_current_stage() const;

	bool load_mod(const std::string& mod_name, const std::filesystem::path& file_to_run);
	LuaError get_last_error();
};