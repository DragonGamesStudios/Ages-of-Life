#include "Storage.h"

Storage::Storage()
{
	l_storage = 0;
}

void Storage::register_l_storage(LuaStorage* settings)
{
	l_storage = settings;
}
