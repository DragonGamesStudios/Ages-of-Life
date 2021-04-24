#include "Base.h"

LuaBasePrototype::LuaBasePrototype(lua_State* L)
	: LuaGameObjectPrototype(L)
{
	bool was_valid = is_valid;
	is_valid = false;

	if (luatab_get_string(L, "order", &order) == 1)
	{
		error = "Field 'order' must be a string";
		return;
	}

	localised_name = art::LocalisedString({ "name." + name });
	localised_description = art::LocalisedString({ "description." + name });

	if (luatab_get_localised_string(L, "localised_name", &localised_name) == 1)
	{
		error = "Invalid localised string in field 'localised_name'";
		return;
	}

	if (luatab_get_localised_string(L, "localised_description", &localised_description) == 1)
	{
		error = "Invalid localised string in field 'localised_description'";
		return;
	}

	is_valid = was_valid;
}
