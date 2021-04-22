#pragma once
#include "LuaModule.h"
#include "events/GameEvent.h"
#include "confvers.h"

#include <map>
#include <string>

class LuaEventHandler : public LuaModule
{
private:
	std::map<lua_State*, int> on_load;
	std::map<lua_State*, int> on_configuration_changed;
	std::map<lua_State*, int> on_ready;
	std::map<lua_State*, int> on_save;

	std::map<GameEventType, std::map<lua_State*, int>> on_event;

	GameEvent prepared_event;
	std::map<GameEventType, std::map<lua_State*, int>>::iterator event_it;

	int register_function(lua_State* L, std::map<lua_State*, int>* fn_map);
	int reg_on_event(lua_State* L);

	void run_arg0_ret0(const std::map<lua_State*, int>& fn_map, lua_State* L);

public:
	LuaEventHandler();
	~LuaEventHandler();

	void prepare_state(lua_State* L);

	void run_on_load(lua_State* L);
	void run_on_configuration_changed(const std::map<std::string, version_t>& configuration, bool was_changed, lua_State* L);
	void run_on_ready(lua_State* L);
	void run_on_save(lua_State* L);

	void prepare_event(const GameEvent& e);
	void raise_event(lua_State* L);
};