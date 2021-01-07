#pragma once

#include "BasePrototype.h"

enum class SettingTypeEnum {
	SETTING_FLOAT = 1,
	SETTING_INT = 2,
	SETTING_STRING = 3,
	SETTING_BOOL = 4
};

union SettingValue
{
	long double ld;
	std::int64_t lli;
	std::string s;
	bool b = 0;
	~SettingValue();
};

struct SettingPrototype
	: BasePrototype
{
	//SettingValue value;
	SettingTypeEnum v_type;
};