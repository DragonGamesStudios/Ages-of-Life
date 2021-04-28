#include "LuaModExecutor.h"
#include "lua_fn.h"

#include <iostream>
#include <sstream>

LuaModExecutor::LuaModExecutor()
{
	l_savesystem = 0;
	l_remote = 0;
	l_script = 0;
	l_game = 0;

	fs = 0;
}

LuaModExecutor::~LuaModExecutor()
{
	for (const auto& [_, state] : mod_states)
	{
		lua_close(state);
	}

	mod_states.clear();
}

void LuaModExecutor::register_filesystem(art::FileSystem* filesystem)
{
	fs = filesystem;
}

void LuaModExecutor::register_l_savesystem(LuaSaveSystem* savesystem)
{
	l_savesystem = savesystem;
}

void LuaModExecutor::register_l_remote(LuaRemote* remote)
{
	l_remote = remote;
}

void LuaModExecutor::register_l_script(LuaEventHandler* script)
{
	l_script = script;
}

void LuaModExecutor::register_l_game(LuaGameScript* game)
{
	l_game = game;
}

bool LuaModExecutor::run_mod(const std::string& mod)
{
	try
	{
		fs->enter_dir(mod);

		if (!fs->exists("control.lua"))
		{
			fs->exit();
			return true;
		}

		auto mod_state = mod_states[mod];

		lua_pushcfunction(mod_state, generic_error_handler);

		if ((luaL_loadfile(mod_state, fs->get_correct_path("control.lua").string().c_str()) || lua_pcall(mod_state, 0, LUA_MULTRET, 1)) != LUA_OK)
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

		fs->exit();
	}
	catch (const std::exception& e)
	{
		last_error.message = e.what();
		return false;
	}

	return true;
}

LuaError LuaModExecutor::get_last_error() const
{
	return last_error;
}

void LuaModExecutor::set_mod_order(const std::vector<std::string>& order)
{
	mod_order = order;

	for (auto mod_it = mod_order.begin(); mod_it != mod_order.end(); mod_it++)
	{
		auto new_state = luaL_newstate();

		prepare_default_state(new_state, fs);
		l_savesystem->prepare_state(new_state, *mod_it);
		l_remote->prepare_state(new_state);
		l_script->prepare_state(new_state);
		l_game->prepare_state(new_state);

		// defines
		lua_newtable(new_state);

		// defines.events
		lua_newtable(new_state);

		//defines.events.on_tick
		lua_pushinteger(new_state, (int)GameEventType::on_tick);
		lua_setfield(new_state, -2, "on_tick");

		// set defines.events
		lua_setfield(new_state, -2, "events");

		// set defines
		lua_setglobal(new_state, "defines");

		mod_states.insert({ *mod_it, new_state });
	}
}

void LuaModExecutor::prepare_control()
{
	l_remote->set_active(true);
	l_script->set_active(true);
}

void LuaModExecutor::unprepare_control()
{
	l_remote->set_active(false);
	l_script->set_active(false);
}

void LuaModExecutor::run_on_load()
{
	l_savesystem->set_active(true);
	l_remote->set_active(true);

	for (auto mod_it = mod_order.begin(); mod_it != mod_order.end(); mod_it++)
	{
		l_script->run_on_load(mod_states[*mod_it]);
	}

	l_savesystem->set_active(false);
	l_remote->set_active(false);
}

void LuaModExecutor::run_on_configuration_changed(const std::map<std::string, version_t>& old_conf, const std::map<std::string, version_t>& new_conf)
{
	l_remote->set_active(true);
	l_savesystem->set_active(true);
	l_game->set_active(true);

	for (auto mod_it = mod_order.begin(); mod_it != mod_order.end(); mod_it++)
	{
		auto old_it = old_conf.find(*mod_it);

		l_script->run_on_configuration_changed(new_conf, old_it == old_conf.end() || (old_it->second != new_conf.at(*mod_it)), mod_states[*mod_it]);
	}

	l_remote->set_active(false);
	l_savesystem->set_active(false);
	l_game->set_active(false);
}

void LuaModExecutor::run_on_ready()
{
	l_remote->set_active(true);
	l_game->set_active(true);

	for (auto mod_it = mod_order.begin(); mod_it != mod_order.end(); mod_it++)
	{
		l_script->run_on_ready(mod_states[*mod_it]);
	}

	l_remote->set_active(false);
	l_game->set_active(false);
}

void LuaModExecutor::run_on_save()
{
	l_remote->set_active(true);
	l_savesystem->set_active(true);
	l_game->set_active(true);

	for (auto mod_it = mod_order.begin(); mod_it != mod_order.end(); mod_it++)
	{
		l_script->run_on_save(mod_states[*mod_it]);
	}

	l_remote->set_active(false);
	l_savesystem->set_active(false);
	l_game->set_active(false);
}

void LuaModExecutor::raise_event(const GameEvent& e)
{
	l_remote->set_active(true);
	l_game->set_active(true);

	l_script->prepare_event(e);
	
	for (auto mod_it = mod_order.begin(); mod_it != mod_order.end(); mod_it++)
	{
		l_script->raise_event(mod_states[*mod_it]);
	}

	l_remote->set_active(false);
	l_game->set_active(false);
}
