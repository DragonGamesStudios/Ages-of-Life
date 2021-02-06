#pragma once
#include "lua_.hpp"

#include <nlohmann/json.hpp>

#include <set>
#include <deque>
#include <functional>

using json = nlohmann::json;

typedef std::function<int(lua_State*)> lua_fn;

int on_call(lua_State* L);
int on_garbage_collection(lua_State* L);
int on_tostring(lua_State* L);

void push_function(lua_State* L, const lua_fn& fn);

extern const char* const fn_mt;

// Move & compare
bool move_lua_value(lua_State* from, lua_State* to, int from_idx, std::set<const void*> cyclic_data_memory = {});

bool are_same(lua_State* L1, lua_State* L2, int idx1, int idx2, std::set<const void*> cyclic_data_memory = {});

// Json conversion
json lua_table_to_json(lua_State* L, int idx, std::set<const void*> cyclic_data_memory = {});
json json_from_lua(lua_State* L, int idx, std::set<const void*> cyclic_data_memory = {});
void lua_from_json(lua_State* L, const json& json_object);
json get_json_value(const json& object, std::deque<std::pair<std::string, int>>& keys);