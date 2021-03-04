#pragma once
#include "Storage.h"
#include "LuaModExecutor.h"

#include <art/FileSystem.h>

class Game
{
private:
	art::FileSystem* save_fs;

	Storage* storage;

	LuaModExecutor* mod_executor;

	LuaSaveSystem* l_savesystem;
	LuaEventHandler* l_eventhandler;
	LuaRemote* l_remote;
	LuaGameScript* l_game;

	std::pair<std::string, LuaError> mod_error;

	std::map<std::string, std::string> configuration;

	std::uint_fast64_t tick;

public:
	Game();
	~Game();

	bool load(art::FileSystem* appdata_fs, art::FileSystem* local_fs, const std::vector<std::string>* mod_order, const std::map<std::string, std::string>& conf);
	void register_filesystem(art::FileSystem* fs);

	std::pair<std::string, LuaError> get_mod_error() const;

	void update(double dt);
};