#pragma once

#include "Base.h"
#include <vector>

enum class SettingType
{
	startup = 0,
	runtime_global = 1,
	runtime_per_user = 2,
	invalid = 3
};

struct LuaSettingPrototype : LuaBasePrototype
{
	LuaSettingPrototype(lua_State* L);

	bool hidden;
	SettingType setting_type;

	bool connected_to_scenario;
	std::string scenario;
};

struct LuaIntSettingPrototype : LuaSettingPrototype
{
	LuaIntSettingPrototype(lua_State* L);

	std::int_fast64_t default_value;

	bool use_min;
	std::int_fast64_t min_value;

	bool use_max;
	std::int_fast64_t max_value;

	std::vector<std::int_fast64_t> allowed_values;

	static const std::uint_fast64_t class_id = 0;
	static std::string class_name;
};

struct LuaBoolSettingPrototype : LuaSettingPrototype
{
	LuaBoolSettingPrototype(lua_State* L);

	bool default_value;

	bool use_forced;
	bool forced_value;

	static const std::uint_fast64_t class_id = 1;
	static std::string class_name;
};

struct LuaDoubleSettingPrototype : LuaSettingPrototype
{
	LuaDoubleSettingPrototype(lua_State* L);

	long double default_value;

	bool use_min;
	long double min_value;

	bool use_max;
	long double max_value;

	std::vector<long double> allowed_values;

	static const std::uint_fast64_t class_id = 2;
	static std::string class_name;
};

struct LuaStringSettingPrototype : LuaSettingPrototype
{
	LuaStringSettingPrototype(lua_State* L);

	std::string default_value;

	bool allow_blank;
	bool auto_trim;

	std::vector<std::string> allowed_values;

	static const std::uint_fast64_t class_id = 3;
	static std::string class_name;
};