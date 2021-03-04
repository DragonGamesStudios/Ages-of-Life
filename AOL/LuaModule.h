#pragma once
#include "lua_.hpp"

class LuaModule
{
protected:
	bool active;
	void check_active(lua_State* L);

public:
	LuaModule();

	virtual void prepare_state(lua_State* L) = 0;
	void set_active(bool v);
};