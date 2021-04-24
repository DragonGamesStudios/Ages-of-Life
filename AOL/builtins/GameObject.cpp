#include "GameObject.h"

LuaGameObjectPrototype::LuaGameObjectPrototype(lua_State* L)
{
	is_valid = false;

	switch (luatab_get_string(L, "name", &name))
	{
	case 1:
		error = "Field 'name' must be a string.";
		return;

	case 2:
		error = "Field 'name' was not found.";
		return;
	}

	is_valid = true;
}
