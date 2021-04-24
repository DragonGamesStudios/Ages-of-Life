#include "Setting.h"

// Setting
LuaSettingPrototype::LuaSettingPrototype(lua_State* L)
	: LuaBasePrototype(L)
{
	bool was_valid = is_valid;
	is_valid = false;

	hidden = false;
	luatab_convert_boolean(L, "hidden", &hidden);

	std::string lua_setting_type;
	switch (luatab_get_string(L, "setting_type", &lua_setting_type))
	{
	case 1:
		error = "Field 'setting_type' must be a string.";
		return;

	case 2:
		error = "Field 'setting_type' was not found.";
		return;
	}

	if (lua_setting_type == "startup")
		setting_type = SettingType::startup;
	else if (lua_setting_type == "runtime-global")
		setting_type = SettingType::runtime_global;
	else if (lua_setting_type == "runtime-per-user")
		setting_type = SettingType::runtime_per_user;
	else
	{
		setting_type = SettingType::invalid;
		error = "Invalid value in field 'setting-type'. Must be one of: startup, runtime-global, runtime-per-user. Got '" + lua_setting_type + "'.";
		return;
	}

	connected_to_scenario = false;

	switch (luatab_get_string(L, "scenario", &scenario))
	{
	case 0:
		connected_to_scenario = true;
		break;

	case 1:
		error = "Field 'scenario' must be a string.";
		return;
	}

	is_valid = was_valid;
}

// Integer setting
std::string LuaIntSettingPrototype::class_name = "int-setting";

LuaIntSettingPrototype::LuaIntSettingPrototype(lua_State* L)
	: LuaSettingPrototype(L)
{
	bool was_valid = is_valid;
	is_valid = false;

	default_value = 0;
	switch (luatab_get_int64(L, "default_value", &default_value))
	{
	case 1:
		error = "Field 'default_value' must be an integer.";
		return;

	case 2:
		error = "Field 'default_value' was not found.";
		return;
	}

	use_min = false;
	min_value = 0;

	switch (luatab_get_int64(L, "min_value", &min_value))
	{
	case 0:
		use_min = true;

		if (min_value > default_value)
		{
			error = "Default value must be higher than minimum value.";
			return;
		}

		break;

	case 1:
		error = "Field 'min_value' must be an integer.";
		return;
	}

	use_max = false;
	max_value = 0;

	switch (luatab_get_int64(L, "max_value", &max_value))
	{
	case 0:
		use_max = true;

		if (max_value < default_value)
		{
			error = "Default value must be lower than maximum value.";
			return;
		}

		break;

	case 1:
		error = "Field 'max_value' must be an integer.";
		return;
	}

	int errpos = 0;
	switch (luatab_get_int64_array(L, "allowed_values", &allowed_values, &errpos))
	{
	case 1:
		error = "Field 'allowed_values' must be an array of integers.";
		return;

	case 3:
		error = "Invalid type of element of field 'allowed_values' on position " + std::to_string(errpos) + ". Must be an integer.";
		return;
	}

	is_valid = was_valid;
}

// Double setting
std::string LuaDoubleSettingPrototype::class_name = "double-setting";

LuaDoubleSettingPrototype::LuaDoubleSettingPrototype(lua_State* L)
	: LuaSettingPrototype(L)
{
	bool was_valid = is_valid;
	is_valid = false;

	default_value = 0;
	switch (luatab_get_ld(L, "default_value", &default_value))
	{
	case 1:
		error = "Field 'default_value' must be a floating point.";
		return;

	case 2:
		error = "Field 'default_value' was not found.";
		return;
	}

	use_min = false;
	min_value = 0;

	switch (luatab_get_ld(L, "min_value", &min_value))
	{
	case 0:
		use_min = true;

		if (min_value > default_value)
		{
			error = "Default value must be higher than minimum value.";
			return;
		}

		break;

	case 1:
		error = "Field 'min_value' must be a floating point.";
		return;
	}

	use_max = false;
	max_value = 0;

	switch (luatab_get_ld(L, "max_value", &max_value))
	{
	case 0:
		use_max = true;

		if (max_value < default_value)
		{
			error = "Default value must be lower than maximum value.";
			return;
		}

		break;

	case 1:
		error = "Field 'max_value' must be a floating point";
		return;
	}

	int errpos = 0;
	switch (luatab_get_ld_array(L, "allowed_values", &allowed_values, &errpos))
	{
	case 1:
		error = "Field 'allowed_values' must be an array of floating points.";
		return;

	case 3:
		error = "Invalid type of element of field 'allowed_values' on position " + std::to_string(errpos) + ". Must be a floating point.";
		return;
	}

	is_valid = was_valid;
}

// String setting
std::string LuaStringSettingPrototype::class_name = "string-setting";

LuaStringSettingPrototype::LuaStringSettingPrototype(lua_State* L)
	: LuaSettingPrototype(L)
{
	bool was_valid = is_valid;
	is_valid = false;
	
	switch (luatab_get_string(L, "default_value", &default_value))
	{
	case 1:
		error = "Field 'default_value' must be a string.";
		return;

	case 2:
		error = "Field 'default_value' was not found.";
		return;
	}

	int errpos = 0;
	switch (luatab_get_string_array(L, "allowed_values", &allowed_values, &errpos))
	{
	case 1:
		error = "Field 'allowed_values' must be an array of strings.";
		return;

	case 3:
		error = "Invalid type of element of field 'allowed_values' on position " + std::to_string(errpos) + ". Must be a string.";
		return;
	}

	allow_blank = false;
	luatab_convert_boolean(L, "allow_blank", &allow_blank);

	auto_trim = false;
	luatab_convert_boolean(L, "auto_trim", &auto_trim);

	is_valid = was_valid;
}

// Bool setting
std::string LuaBoolSettingPrototype::class_name = "bool-setting";

LuaBoolSettingPrototype::LuaBoolSettingPrototype(lua_State* L)
	: LuaSettingPrototype(L)
{
	bool was_valid = is_valid;
	is_valid = false;

	default_value = false;
	if (luatab_convert_boolean(L, "default_value", &default_value) == 2)
	{
		error = "Field 'default_value' was not found.";
		return;
	}

	forced_value = false;
	use_forced = false;
	if (!luatab_convert_boolean(L, "forced_value", &forced_value))
		use_forced = true;

	is_valid = was_valid;
}