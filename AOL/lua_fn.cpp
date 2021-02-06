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
	{
		lua_pushstring(L, "C++ error: problem with function deleteion");
		lua_error(L);
	}
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
			same = lua_tocfunction(L1, -1) == lua_tocfunction(L2, -1);
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
		lua_pushcfunction(to, lua_tocfunction(from, -1));
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