#include "Game.h"
#include <allegro5/allegro_native_dialog.h>

Game::Game()
{
	storage = 0;
	save_fs = 0;
	tick = 0;

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

bool Game::load(art::FileSystem* appdata_fs, art::FileSystem* local_fs, const std::vector<std::string>& mod_order, const std::map<std::string, version_t>& conf, const GameData& g_data)
{
	// Load base data
	base_data = g_data;

	// Mod order
	auto m_order = mod_order;
	m_order.push_back(base_data.scenario->path.string());

	mod_executor->set_mod_order(m_order);

	// control.lua
	mod_executor->prepare_control();

	for (auto mod_it = m_order.begin(); mod_it != m_order.end(); mod_it++)
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
	bool should_raise = false;

	if (base_data.mod_config.size() != conf.size())
		should_raise = true;
	else
	{
		for (const auto& [mod, version] : conf)
		{
			auto conf_it = base_data.mod_config.find(mod);

			if (conf_it == base_data.mod_config.end() || conf_it->second != version)
			{
				should_raise = true;
				break;
			}
		}
	}

	if (should_raise)
		mod_executor->run_on_configuration_changed(base_data.mod_config, conf);

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
