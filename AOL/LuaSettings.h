#pragma once

#include "lua_.hpp"
#include "SettingPrototype.h"

#include <map>
#include <string>

class Settings;

class LuaSettings
{
private:
	std::map<std::string, SettingPrototype> prototypes;
	Settings* settings;

public:
	LuaSettings();

	void prepare_state(lua_State* L);
	void prepare_reader(lua_State* L);

	void load_prototypes(lua_State* L);
	int register_prototypes(lua_State* L);

	void register_settings(Settings* s);

};