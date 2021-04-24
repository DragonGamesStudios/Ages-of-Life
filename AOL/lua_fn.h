#pragma once
#include "lua_.hpp"

#include <nlohmann/json.hpp>
#include <art/FileSystem.h>
#include <art/Dictionary.h>

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

// Require
std::filesystem::path correct_path(const std::string& lua_path);
int lua_require(lua_State* L, art::FileSystem* fs);
void prepare_default_state(lua_State* L, art::FileSystem* fs);

// Typecheck
bool fn_typecheck(lua_State* L, int idx, int l_type, const std::string& expected, bool error = true);

// Print stack
void print_stack(lua_State* stack);
std::string get_string(lua_State* L, int idx);

// Lua function storing
struct LuaFunction
{
	std::string data;
};

int lf_writer(lua_State* L, const void* b, size_t size, void* ud);

const char* lf_reader(lua_State* L, void* ud, size_t* size);

// Error handling
int generic_error_handler(lua_State* L);

struct LuaError
{
	std::string message;
	std::string traceback;
};

// Table operations
// Errors:
// 1 - Invalid type
// 2 - Field not found
// 3 - Array element incorrect type

int luatab_get_string(lua_State* L, const std::string& key, std::string* value);
int luatab_get_localised_string(lua_State* L, const std::string& key, art::LocalisedString* value);
int luatab_convert_boolean(lua_State* L, const std::string& key, bool* value);
int luatab_get_int64(lua_State* L, const std::string& key, std::int_fast64_t* value);
int luatab_get_ld(lua_State* L, const std::string& key, long double* value);
int luatab_get_int64_array(lua_State* L, const std::string& key, std::vector<std::int_fast64_t>* value, int* errpos);
int luatab_get_ld_array(lua_State* L, const std::string& key, std::vector<long double>* value, int* errpos);
int luatab_get_string_array(lua_State* L, const std::string& key, std::vector<std::string>* value, int* errpos);

// Localised string
art::LocalisedString lua_get_localised_string(lua_State* L, int index, bool& is_valid);