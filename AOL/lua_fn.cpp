#include "lua_fn.h"

#include <iostream>
#include <sstream>

extern const char* const fn_mt;
const char* const fn_mt = "LuaCPPFunctionMetatable";

int on_call(lua_State* L)
{
	lua_fn* f = static_cast<lua_fn*>(luaL_checkudata(L, 1, fn_mt));
	lua_remove(L, 1);
	return (*f)(L);
}

int on_garbage_collection(lua_State* L)
{
	lua_fn* f = static_cast<lua_fn*>(luaL_checkudata(L, 1, fn_mt));
	if (!f)
		luaL_error(L, "C++ error: problem with function deleteion");
	else
	{
		f->~lua_fn();
		//delete f;
	}

	return 0;
}

int on_tostring(lua_State* L)
{
	lua_fn* f = static_cast<lua_fn*>(luaL_checkudata(L, 1, fn_mt));

	std::stringstream ss;
	ss << "C++ function on " << std::hex << f;
	lua_pushstring(L, ss.str().c_str());

	return 1;
}

void push_function(lua_State* L, const lua_fn& fn)
{
	// Allocate memory
	void * new_fn = lua_newuserdata(L, sizeof(lua_fn));

	// Set metatable to func
	luaL_getmetatable(L, fn_mt);
	lua_setmetatable(L, -2);

	// Set variable
	new (new_fn) lua_fn(fn);
}

bool are_same(lua_State* L1, lua_State* L2, int idx1, int idx2, std::set<const void*> cyclic_data_memory)
{
	const void* ptr = 0;
	std::set<const void*>::iterator table_ptr;
	bool same = false;
	int tab1_len = 0;
	int tab2_len = 0;

	// Push values
	lua_pushvalue(L1, idx1);
	lua_pushvalue(L2, idx2);

	// Typecheck
	if (lua_type(L1, -1) != lua_type(L2, -1))
		same = false;
	else
	{
		switch (lua_type(L1, -1))
		{
		case LUA_TNIL:
			same = true;
			break;

		case LUA_TNUMBER:
			same = lua_tonumber(L1, -1) == lua_tonumber(L2, -1);
			break;

		case LUA_TSTRING:
			same = std::string(lua_tostring(L1, -1)) == lua_tostring(L2, -1);
			break;

		case LUA_TBOOLEAN:
			same = lua_toboolean(L1, -1) == lua_toboolean(L2, -1);
			break;

		case LUA_TLIGHTUSERDATA:
			same = lua_topointer(L1, -1) == lua_topointer(L2, -1);
			break;

		case LUA_TFUNCTION:
			same = lua_topointer(L1, -1) == lua_topointer(L2, -1);

			break;

		case LUA_TUSERDATA:
			same = lua_topointer(L1, -1) == lua_topointer(L2, -1);
			break;

		case LUA_TTHREAD:
			same = lua_tothread(L1, -1) == lua_tothread(L2, -1);
			break;

		case LUA_TTABLE:
			// Table comparison
			ptr = lua_topointer(L1, -1);
			table_ptr = cyclic_data_memory.find(ptr);

			if (table_ptr == cyclic_data_memory.end())
			{
				lua_pushnil(L1);
				while (lua_next(L1, -2))
				{
					move_lua_value(L1, L2, -2);
					lua_rawget(L2, -2);

					same = are_same(L1, L2, -1, -1, cyclic_data_memory);

					lua_pop(L2, 1);

					// Clear
					tab1_len++;
					lua_pop(L1, 1);

					if (!same)
						break;
				}
			}
			else
			{
				throw std::runtime_error("Recursive data detected");
			}

			lua_pushnil(L2);
			while (lua_next(L2, -2))
			{
				tab2_len++;
				lua_pop(L2, 1);
			}

			if (tab1_len != tab2_len)
				same = false;

			break;

		default:
			break;
		}
	}

	// Clear stack
	lua_pop(L1, 1);
	lua_pop(L2, 1);

	return same;
}

bool move_lua_value(lua_State* from, lua_State* to, int from_idx, std::set<const void*> cyclic_data_memory)
{
	lua_pushvalue(from, from_idx);

	std::set<const void*>::iterator table_ptr;
	const void* ptr = 0;
	LuaFunction f;

	switch (lua_type(from, -1))
	{
	case LUA_TNIL:
		lua_pushnil(to);
		break;

	case LUA_TNUMBER:
		lua_pushnumber(to, lua_tonumber(from, -1));
		break;

	case LUA_TBOOLEAN:
		lua_pushboolean(to, lua_toboolean(from, -1));
		break;

	case LUA_TFUNCTION:
		if (lua_dump(from, lf_writer, &f, false) != 0)
			throw std::runtime_error("Something went wrong when copying Lua function.");
		
		if (lua_load(to, lf_reader, &f, (std::stringstream() << "Function: " << (void*)&f.data).str().c_str(), 0) != LUA_OK)
			throw std::runtime_error("Something went wrong when copying Lua function.");

		break;

	case LUA_TLIGHTUSERDATA:
		lua_pushlightuserdata(to, lua_touserdata(from, -1));
		break;

	case LUA_TSTRING:
		lua_pushstring(to, lua_tostring(from, -1));
		break;

	case LUA_TTABLE:
		ptr = lua_topointer(from, -1);
		table_ptr = cyclic_data_memory.find(ptr);

		if (table_ptr == cyclic_data_memory.end())
		{
			cyclic_data_memory.insert(ptr);

			lua_newtable(to);

			lua_pushnil(from);
			while (lua_next(from, -2))
			{
				move_lua_value(from, to, -2, cyclic_data_memory);
				move_lua_value(from, to, -1, cyclic_data_memory);

				if (lua_isnil(to, -2))
				{
					return false;
				}

				lua_settable(to, -3);

				lua_pop(from, 1);
			}
		}
		else
		{
			throw std::runtime_error("Recursive data detected");
		}

		break;

	default:
		lua_pushnil(to);
		return false;
		break;
	}

	lua_pop(from, 1);

	return true;
}

json json_from_lua(lua_State* L, int idx, std::set<const void*> cyclic_data_memory)
{
	json value;

	switch (lua_type(L, idx))
	{
	case LUA_TSTRING:
		value = lua_tostring(L, idx);
		break;

	case LUA_TNUMBER:
		value = lua_tonumber(L, idx);
		break;

	case LUA_TBOOLEAN:
		value = lua_toboolean(L, idx);
		break;

	case LUA_TTABLE:
		value = lua_table_to_json(L, idx, cyclic_data_memory);
		break;

	default:
		throw std::runtime_error((std::string)"Unsupported value type. String, number or table expected, got " + lua_typename(L, lua_type(L, idx)));
		break;
	}

	return value;
}

void lua_from_json(lua_State* L, const json& json_object)
{
	switch (json_object.type())
	{
	case json::value_t::boolean:
		lua_pushboolean(L, (bool)json_object);
		break;

	case json::value_t::string:
		lua_pushstring(L, ((std::string)json_object).c_str());
		break;

	case json::value_t::number_float:
		lua_pushnumber(L, (double)json_object);
		break;

	case json::value_t::number_integer:
	case json::value_t::number_unsigned:
		lua_pushinteger(L, (long long)json_object);
		break;

	case json::value_t::object:
		lua_newtable(L);

		for (const auto& value : json_object.items())
		{
			lua_pushstring(L, value.key().c_str());
			lua_from_json(L, value.value());

			lua_rawset(L, -3);
		}

		break;

	case json::value_t::array:
		lua_newtable(L);

		for (size_t i = 1; i <= json_object.size(); i++)
		{
			lua_pushinteger(L, i);
			lua_from_json(L, json_object[i-1]);

			lua_rawset(L, -3);
		}

		break;
	}
}

json get_json_value(const json& object, std::deque<std::pair<std::string, int>>& keys)
{
	auto front = keys.front();
	int key_num = front.second;
	std::string key_str = front.first;

	if (key_str.empty())
	{
		if (key_num >= 0 && key_num < object.size())
		{
			keys.pop_front();
			if (!keys.empty())
				return get_json_value(object.at(key_num), keys);
			else
				return object.at(key_num);
		}
		else
			return false;
	}
	else
	{
		if (object.find(key_str) != object.end())
		{
			keys.pop_front();
			if (!keys.empty())
				return get_json_value(object.at(key_str), keys);
			else
				return object.at(key_str);
		}
		else
			return false;
	}
}

std::filesystem::path correct_path(const std::string& lua_path)
{
	std::string word;
	std::filesystem::path result;

	for (const char c : lua_path)
	{
		if (c != '.')
			word.push_back(c);
		else if (!word.empty())
		{
			result /= word;
			word.clear();
		}
	}

	word += ".lua";

	if (!word.empty())
	{
		result /= word;
		word.clear();
	}

	return result;
}

int lua_require(lua_State* L, art::FileSystem* fs)
{
	std::string err;

	if (!lua_isstring(L, -1))
		err = (std::string)"Invalid argument to 'require'. String expected, got " + lua_typename(L, lua_type(L, -1)) + ".\n";

	if (!err.empty())
		luaL_error(L, err.c_str());

	std::filesystem::path executed_file = correct_path(lua_tostring(L, -1));

	if (!fs->exists(executed_file))
		err = "File on path " + executed_file.string() + " not found.\n";

	if (!err.empty())
		luaL_error(L, err.c_str());

	int beginning_stack_size = lua_gettop(L);

	// Execute the file
	if (luaL_dofile(L, (fs->get_correct_path(executed_file)).string().c_str()) != LUA_OK)
		luaL_error(L, lua_tostring(L, -1));

	int end_stack_size = lua_gettop(L);

	return end_stack_size - beginning_stack_size;
}

json lua_table_to_json(lua_State* L, int idx, std::set<const void*> cyclic_data_memory)
{
	json ret;

	lua_pushvalue(L, idx);

	const void* ptr = lua_topointer(L, -1);

	auto table_ptr = cyclic_data_memory.find(ptr);

	bool is_numeric = 0;

	lua_rawgeti(L, -1, 1);
	if (lua_isnil(L, -1))
		is_numeric = false;
	else
	{
		is_numeric = true;
		ret = json::array();
	}

	lua_pop(L, 1);

	if (!is_numeric)
	{
		if (table_ptr == cyclic_data_memory.end())
		{
			lua_pushnil(L);
			while (lua_next(L, -2))
			{
				if (!lua_isstring(L, -2))
				{
					throw std::runtime_error((std::string)"Invalid key type. String expected, got " + lua_typename(L, lua_type(L, -2)));
				}

				ret[lua_tostring(L, -2)] = json_from_lua(L, -1, cyclic_data_memory);

				lua_pop(L, 1);
			}
		}
		else
		{
			throw std::runtime_error("Recursive data detected");
		}
	}
	else
	{
		size_t size = lua_rawlen(L, -1);

		for (size_t i = 1; i <= size; i++)
		{
			lua_rawgeti(L, -1, i);

			ret.push_back(json_from_lua(L, -1));

			lua_pop(L, 1);
		}
	}

	lua_pop(L, 1);

	return ret;
}

void prepare_default_state(lua_State* L, art::FileSystem* fs)
{
	luaL_requiref(L, "_G", luaopen_base, 1);
	luaL_requiref(L, "math", luaopen_math, 1);
	luaL_requiref(L, "string", luaopen_string, 1);
	luaL_requiref(L, "table", luaopen_table, 1);
	luaL_requiref(L, "debug", luaopen_debug, 1);

	lua_settop(L, 0);

	// Cpp function metatable
	luaL_newmetatable(L, fn_mt);

	// __call field (func())
	lua_pushcfunction(L, on_call);
	lua_setfield(L, -2, "__call");

	// __tostring field (tostring(func))
	lua_pushcfunction(L, on_tostring);
	lua_setfield(L, -2, "__tostring");

	// __gc field (garbage collection)
	lua_pushcfunction(L, on_garbage_collection);
	lua_setfield(L, -2, "__gc");

	// Pop
	lua_pop(L, 1);

	// Disable unsafe functions
	lua_getglobal(L, "_G");

	lua_pushnil(L);
	lua_setfield(L, -2, "dofile");

	lua_pushnil(L);
	lua_setfield(L, -2, "loadfile");

	lua_pop(L, 1);

	// Load require function
	push_function(L, std::bind(lua_require, std::placeholders::_1, fs));
	lua_setglobal(L, "require");
}

// Typecheck
bool fn_typecheck(lua_State* L, int idx, int l_type, const std::string& expected, bool error)
{
	if (lua_type(L, idx) != l_type)
	{
		if (!error) return false;

		std::stringstream err;
		err << "Invalid argument " << idx << " type. " << expected << "expected, got " << lua_typename(L, lua_type(L, idx)) << '.';
		luaL_error(L, err.str().c_str());

		return false;
	}

	return true;
}

// Print stack
void print_stack(lua_State* stack)
{
	for (int i = 1; i <= lua_gettop(stack); i++)
	{
		std::cout << i << ". " << lua_typename(stack, lua_type(stack, i)) << "\t" << get_string(stack, i) << "\n";
	}
}

std::string get_string(lua_State* L, int idx)
{
	switch (lua_type(L, idx))
	{
	case LUA_TTABLE:
		return ((std::stringstream)"table " << lua_topointer(L, idx)).str();
		break;

	case LUA_TNUMBER:
		return std::to_string(lua_tonumber(L, idx));
		break;

	case LUA_TSTRING:
		return lua_tostring(L, idx);
		break;

	default:
		return "nil";
		break;
	}
}

// Lua function storage
const char* lf_reader(lua_State* L, void* ud, size_t* size)
{
	LuaFunction* lf = (LuaFunction*)ud;

	*size = lf->data.size();
	return lf->data.size() ? lf->data.c_str() : 0;
}

int lf_writer(lua_State* L, const void* b, size_t size, void* ud)
{
	LuaFunction* lf = (LuaFunction*)ud;

	lf->data += std::string((const char*)b, size);

	return 0;
}

// Error handling
int generic_error_handler(lua_State* L)
{
	// Ensure stack size
	lua_settop(L, 1);

	// Create new error table
	lua_newtable(L);

	// Get the message
	lua_pushvalue(L, -2);
	lua_setfield(L, -2, "message");

	// Get the traceback
	luaL_traceback(L, L, 0, 0);
	lua_setfield(L, -2, "traceback");

	return 1;
}

int luatab_get_string(lua_State* L, const std::string& key, std::string* value)
{
	// Get the value
	lua_pushstring(L, key.c_str());
	lua_rawget(L, -2);
	
	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		return 2;
	}
	else if (lua_type(L, -1) != LUA_TSTRING)
	{
		lua_pop(L, 1);
		return 1;
	}

	*value = lua_tostring(L, -1);
	lua_pop(L, 1);

	return 0;
}

int luatab_get_localised_string(lua_State* L, const std::string& key, art::LocalisedString* value)
{
	// Get the value
	lua_pushstring(L, key.c_str());
	lua_rawget(L, -2);

	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		return 2;
	}
	
	bool valid = true;

	*value = lua_get_localised_string(L, -1, valid);

	lua_pop(L, 1);

	if (!valid)
		return 1;

	return 0;
}

int luatab_convert_boolean(lua_State* L, const std::string& key, bool* value)
{
	// Get the value
	lua_pushstring(L, key.c_str());
	lua_rawget(L, -2);

	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		return 2;
	}

	*value = lua_toboolean(L, -1);
	lua_pop(L, 1);

	return 0;
}

int luatab_get_int64(lua_State* L, const std::string& key, std::int_fast64_t* value)
{
	// Get the value
	lua_pushstring(L, key.c_str());
	lua_rawget(L, -2);

	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		return 2;
	}
	else if (lua_type(L, -1) != LUA_TNUMBER)
	{
		lua_pop(L, 1);
		return 1;
	}

	*value = lua_tointeger(L, -1);

	// Pop
	lua_pop(L, 1);

	return 0;
}

int luatab_get_ld(lua_State* L, const std::string& key, long double* value)
{
	// Get the value
	lua_pushstring(L, key.c_str());
	lua_rawget(L, -2);

	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		return 2;
	}
	else if (lua_type(L, -1) != LUA_TNUMBER)
	{
		lua_pop(L, 1);
		return 1;
	}

	*value = lua_tonumber(L, -1);

	// Pop
	lua_pop(L, 1);

	return 0;
}

int luatab_get_int64_array(lua_State* L, const std::string& key, std::vector<std::int_fast64_t>* value, int* errpos)
{
	// Get the value
	lua_pushstring(L, key.c_str());
	lua_rawget(L, -2);

	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		return 2;
	}
	else if (lua_type(L, -1) != LUA_TTABLE)
	{
		lua_pop(L, 1);
		return 1;
	}

	// Iterate
	auto length = lua_rawlen(L, -1);

	for (int i = 0; i < length; i++)
	{
		lua_rawgeti(L, -1, i);

		if (lua_type(L, -1) != LUA_TNUMBER)
		{
			lua_pop(L, 2);
			*errpos = i;
			return 3;
		}

		value->push_back(lua_tointeger(L, -1));
		lua_pop(L, 1);
	}

	lua_pop(L, 1);

	return 0;
}

int luatab_get_ld_array(lua_State* L, const std::string& key, std::vector<long double>* value, int* errpos)
{
	// Get the value
	lua_pushstring(L, key.c_str());
	lua_rawget(L, -2);

	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		return 2;
	}
	else if (lua_type(L, -1) != LUA_TTABLE)
	{
		lua_pop(L, 1);
		return 1;
	}

	// Iterate
	auto length = lua_rawlen(L, -1);

	for (int i = 0; i < length; i++)
	{
		lua_rawgeti(L, -1, i);

		if (lua_type(L, -1) != LUA_TNUMBER)
		{
			lua_pop(L, 2);
			*errpos = i;
			return 3;
		}

		value->push_back(lua_tonumber(L, -1));
		lua_pop(L, 1);
	}

	lua_pop(L, 1);

	return 0;
}

int luatab_get_string_array(lua_State* L, const std::string& key, std::vector<std::string>* value, int* errpos)
{
	// Get the value
	lua_pushstring(L, key.c_str());
	lua_rawget(L, -2);

	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		return 2;
	}
	else if (lua_type(L, -1) != LUA_TTABLE)
	{
		lua_pop(L, 1);
		return 1;
	}

	// Iterate
	auto length = lua_rawlen(L, -1);

	for (int i = 0; i < length; i++)
	{
		lua_rawgeti(L, -1, i);

		if (lua_type(L, -1) != LUA_TSTRING)
		{
			lua_pop(L, 2);
			*errpos = i;
			return 3;
		}

		value->push_back(lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	lua_pop(L, 1);

	return 0;
}

art::LocalisedString lua_get_localised_string(lua_State* L, int index, bool& is_valid)
{
	std::vector<art::LocalisedString> localised_vector;

	switch (lua_type(L, index))
	{
	case LUA_TSTRING:
		return (std::string)lua_tostring(L, index);

	case LUA_TNUMBER:
		if (lua_isinteger(L, -1))
			return (long long)lua_tointeger(L, index);
		else
			return (long double)lua_tonumber(L, index);

	case LUA_TTABLE:
		// Get length
		lua_pushvalue(L, index);

		auto length = lua_rawlen(L, -1);

		// Iterate
		for (int i = 0; i < length; i++)
		{
			lua_rawgeti(L, -1, i);

			localised_vector.push_back(lua_get_localised_string(L, -1, is_valid));

			lua_pop(L, 1);
		}

		// Pop
		lua_pop(L, 1);

		return localised_vector;

	default:
		is_valid = false;
		break;
	}

	return art::LocalisedString();
}
