#pragma once

#include "LuaStorage.h"
#include "LuaSaveSystem.h"

enum class LoaderStage {
	STAGE_NONE = 0,
	STAGE_SETTINGS = 1,
	STAGE_DATA = 2,
	STAGE_SETTINGS_MIGRATIONS = 4,
	STAGE_PROTOTYPES = 8,
	STAGE_RUNTIME = 16,
	STAGE_NEW_GAME = 32
};



class LuaModLoader
{
private:
	lua_State* executed_state;
	lua_State* mod_state;

	LuaStorage* l_storage;
	LuaSaveSystem* l_savesystem;

	LoaderStage current_stage;

	std::string last_error;
	art::FileSystem* fs;

	std::map<LoaderStage, std::vector<std::string>> allowed_prototypes;

	std::map<std::string, std::map<std::string, std::vector<std::string>>> prototype_histories;

	void prepare_state(lua_State* L);
	int require(lua_State* L);
	std::filesystem::path correct_path(const std::string& lua_path);

	void print_stack(lua_State* stack);
	std::string get_string(lua_State* L, int idx);

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
	std::string get_last_error();
};