#pragma once

#include "LuaStorage.h"

#include <art/FileSystem.h>

enum class LoaderStage {
	STAGE_NONE = 0,
	STAGE_SETTINGS = 1,
	STAGE_DATA = 2,
	STAGE_RUNTIME = 3,
	STAGE_PROTOTYPES = 4,
	STAGE_SETTINGS_MIGRATIONS = 5
};

class LuaModLoader
{
private:
	lua_State* executed_state;
	lua_State* mod_state;

	LuaStorage* l_storage;

	LoaderStage current_stage;

	std::string last_error;
	art::FileSystem* fs;

	std::map<LoaderStage, std::vector<std::string>> allowed_prototypes;

	std::map<std::string, std::map<std::string, std::vector<std::string>>> prototype_histories;
	std::set<const void*> cyclic_data_memory;

	void prepare_state(lua_State* L);
	int require(lua_State* L);
	std::filesystem::path correct_path(const std::string& lua_path);

	bool move_lua_value(lua_State* from, lua_State* to, int from_idx, bool first_call = true);

	bool are_same(lua_State* L1, lua_State* L2, int idx1, int idx2, bool first_call = true);

	void print_stack(lua_State* stack);
	std::string get_string(lua_State* L, int idx);

public:
	LuaModLoader();
	~LuaModLoader();

	void register_filesystem(art::FileSystem* fs_);

	void register_l_storage(LuaStorage* ls);
	void begin_stage(LoaderStage stage);
	bool end_stage();
	LoaderStage get_current_stage() const;

	bool load_mod(const std::string& mod_name, const std::filesystem::path& file_to_run);
	std::string get_last_error();
};