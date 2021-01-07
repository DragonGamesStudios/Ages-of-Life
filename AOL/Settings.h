#pragma once

#include "LuaSettings.h"
#include "SettingPrototype.h"

class Settings
{
private:
	LuaSettings* l_settings;

public:
	Settings();

	void register_l_settings(LuaSettings* settings);

	void initialize_prototype(const SettingPrototype& prototype);
	void initialize_l_prototypes();
};