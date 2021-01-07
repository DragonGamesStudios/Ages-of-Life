#include "Settings.h"

Settings::Settings()
{
	l_settings = 0;
}

void Settings::register_l_settings(LuaSettings* settings)
{
	l_settings = settings;
}
