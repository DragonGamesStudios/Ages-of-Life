#pragma once
#include "LuaModule.h"
#include "lua_fn.h"

#include <map>
#include <string>
#include <vector>

typedef std::map<std::string, LuaFunction> lua_interface;

class LuaRemote : public LuaModule
{
private:
	std::map<std::string, lua_interface> interfaces;

	int add_interface(lua_State* L);
	int remove_interface(lua_State* L);
	int call(lua_State* L);

	void add_interface_to_state(lua_State* L, const std::string& name, const lua_interface& iface, int level = 0);

	std::vector<lua_State*> states;

public:
	LuaRemote();

	void prepare_state(lua_State* L);
};