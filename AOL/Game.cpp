#include "Game.h"
#include <allegro5/allegro_native_dialog.h>

Game::Game()
{
	storage = 0;
	save_fs = 0;

	mod_executor = new LuaModExecutor;

	l_savesystem = new LuaSaveSystem;
	l_eventhandler = new LuaEventHandler;
	l_remote = new LuaRemote;
	l_game = new LuaGameScript;

	mod_executor->register_l_savesystem(l_savesystem);
	mod_executor->register_l_remote(l_remote);
	mod_executor->register_l_script(l_eventhandler);
	mod_executor->register_l_game(l_game);
}

Game::~Game()
{
	delete l_savesystem;
	delete l_eventhandler;
	delete l_remote;
	delete l_game;
	delete mod_executor;
}

bool Game::load(art::FileSystem* appdata_fs, art::FileSystem* local_fs, const std::vector<std::string>* mod_order, const std::map<std::string, std::string>& conf)
{
	mod_executor->set_mod_order(mod_order);

	// control.lua
	mod_executor->prepare_control();

	for (auto mod_it = mod_order->begin(); mod_it != mod_order->end(); mod_it++)
	{
		auto mod = *mod_it;
		if (mod == "core" || mod == "base")
			mod_executor->register_filesystem(local_fs);
		if (!mod_executor->run_mod(mod))
		{
			mod_error.first = mod;
			mod_error.second = mod_executor->get_last_error();
			return false;
		}
		if (mod == "core" || mod == "base")
			mod_executor->register_filesystem(appdata_fs);
	}

	mod_executor->unprepare_control();

	// on_load
	mod_executor->run_on_load();

	// Dispatch configuration
	auto saved_conf = conf;
	bool should_raise = false;

	if (saved_conf.size() != conf.size())
		should_raise = true;
	else
	{
		for (const auto& [mod, version] : conf)
		{
			auto conf_it = saved_conf.find(mod);

			if (conf_it == saved_conf.end() || conf_it->second != version)
			{
				should_raise = true;
				break;
			}
		}
	}

	if (should_raise)
		mod_executor->run_on_configuration_changed(saved_conf, conf);

	// on_ready
	mod_executor->run_on_ready();

	return true;
}

void Game::register_filesystem(art::FileSystem* fs)
{
	save_fs = new art::FileSystem;

	save_fs->enter_dir(fs->get_current_path().string());

	l_savesystem->register_filesystem(save_fs);
}

std::pair<std::string, LuaError> Game::get_mod_error() const
{
	return mod_error;
}

void Game::update(double dt)
{
	tick++;

	GameEvent e;
	e.type = GameEventType::on_tick;
	e.tick = tick;

	mod_executor->raise_event(e);
}
