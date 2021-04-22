#pragma once
#include "Storage.h"
#include "LuaModExecutor.h"

#include <art/FileSystem.h>
#include <art/Dictionary.h>
#include <agl/GraphicsHandler.h>

struct Scenario
{
	std::string name;
	bool is_local = true;
	std::filesystem::path path;
	art::Dictionary* dict;
	agl::Image* preview;
	bool has_preview = true;
};

struct GameData
{
	version_t aol_version;
	Scenario* scenario;
	std::map<std::string, version_t> mod_config;
};

class Game
{
public:
	Game();
	~Game();

	bool load(art::FileSystem* appdata_fs, art::FileSystem* local_fs, const std::vector<std::string>& mod_order, const std::map<std::string, version_t>& conf, const GameData& g_data);
	void register_filesystem(art::FileSystem* fs);

	std::pair<std::string, LuaError> get_mod_error() const;

	GameData base_data;

	void update(double dt);

private:
	art::FileSystem* save_fs;

	Storage* storage;

	LuaModExecutor* mod_executor;

	LuaSaveSystem* l_savesystem;
	LuaEventHandler* l_eventhandler;
	LuaRemote* l_remote;
	LuaGameScript* l_game;

	std::pair<std::string, LuaError> mod_error;

	std::map<std::string, version_t> configuration;

	std::uint_fast64_t tick;
};