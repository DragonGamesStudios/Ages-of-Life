#pragma once

#include "GameObject.h"
#include <vector>

const std::uint_fast8_t setting_boolean = 0;
const std::uint_fast8_t setting_integer = 1;
const std::uint_fast8_t setting_string = 2;
const std::uint_fast8_t setting_double = 3;

union SettingValue
{
	long double d_value = 0;
	std::int_fast64_t i_value;
	bool b_value;
	std::string s_value;
	~SettingValue();
};

class LuaSettingPrototype : public LuaGameObjectPrototype
{
private:
	std::string setting_type;
	SettingValue default_value;
	std::vector<SettingValue> possible_values;

public:
	static const std::uint_fast64_t class_id = 1;
};