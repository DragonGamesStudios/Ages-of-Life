#pragma once

#include "LuaSettings.h"

#include <art/FileSystem.h>

enum class LoaderStage {
	STAGE_NONE = 0,
	STAGE_SETTINGS = 1,
	STAGE_DATA = 2
};

class LuaModLoader
{
private:
	lua_State* settings_state;
	lua_State* data_state;

	LuaSettings* l_settings;

	LoaderStage current_stage;

	std::string last_error;
	art::FileSystem* fs;

	void prepare_state(lua_State* L);
	int require(lua_State* L);
	std::filesystem::path correct_path(const std::string& lua_path);

	bool move_lua_value(lua_State* from, lua_State* to, int from_idx);

public:
	LuaModLoader();
	~LuaModLoader();

	void register_filesystem(art::FileSystem* fs_);

	void register_l_settings(LuaSettings* ls);
	void begin_stage(LoaderStage stage);
	void end_stage();

	bool load_mod(const std::string& mod_name, const std::filesystem::path& file_to_run);
	std::string get_last_error();
};