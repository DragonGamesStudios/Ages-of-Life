#pragma once

#include "lua_.hpp"
#include "builtins/builtins.h"

#include <map>
#include <string>
#include <set>
#include <type_traits>

class Storage;

class LuaStorage
{
private:
	std::map<std::string, std::uint64_t> builtin_prototypes;
	Storage* storage;

	std::string last_error;

	bool load_prototype(lua_State* L);

public:
	LuaStorage();

	void prepare_state(lua_State* L, const std::vector<std::string>& fields);
	void prepare_reader(lua_State* L);

	bool load_prototypes(lua_State* L, const std::vector<std::string>& allowed_prototypes);
	int register_prototypes(lua_State* L);

	void register_storage(Storage* s);

	std::string get_last_error();

};
