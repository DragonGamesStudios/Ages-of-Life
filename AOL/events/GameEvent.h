#include "..\lua_.hpp"

#include <memory>
#include <map>
#include <string>

enum class GameEventType
{
	on_tick = 0
};

struct GameEvent
{
	GameEventType type = GameEventType::on_tick;
	std::uint_fast64_t tick = 0;

	virtual void to_lua(lua_State* L) const;
};

extern std::map<GameEventType, std::string> event_names;