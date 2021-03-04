#include "GameEvent.h"

std::map<GameEventType, std::string> event_names = {
	{ GameEventType::on_tick, "on_tick" },
};

void GameEvent::to_lua(lua_State* L) const
{
	lua_newtable(L);
	
	// type
	lua_pushstring(L, event_names[type].c_str());
	lua_setfield(L, -2, "type");

	// tick
	lua_pushinteger(L, tick);
	lua_setfield(L, -2, "tick");
}
