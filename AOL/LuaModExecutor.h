#pragma once
#include "LuaSaveSystem.h"
#include "LuaRemote.h"
#include "LuaGameScript.h"
#include "LuaEventHandler.h"
#include "lua_fn.h"

#include <map>
#include <string>

class LuaModExecutor
{
private:
	std::map<std::string, lua_State*> mod_states;

	LuaError last_error;
	const std::vector<std::string>* mod_order;

	art::FileSystem* fs;

	// Modding classes
	LuaSaveSystem* l_savesystem;
	LuaRemote* l_remote;
	LuaEventHandler* l_script;
	LuaGameScript* l_game;

public:
	LuaModExecutor();
	~LuaModExecutor();

	void register_filesystem(art::FileSystem* filesystem);
	void register_l_savesystem(LuaSaveSystem* savesystem);
	void register_l_remote(LuaRemote* remote);
	void register_l_script(LuaEventHandler* script);
	void register_l_game(LuaGameScript* game);

	bool run_mod(const std::string& mod);

	LuaError get_last_error() const;

	void set_mod_order(const std::vector<std::string>* order);

	void prepare_control();
	void unprepare_control();

	void run_on_load();
	void run_on_configuration_changed(const std::map<std::string, std::string>& old_conf, const std::map<std::string, std::string>& new_conf);
	void run_on_ready();
	void run_on_save();

	void raise_event(const GameEvent& e);
};