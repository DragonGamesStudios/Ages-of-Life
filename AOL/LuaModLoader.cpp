#include "LuaModLoader.h"
#include "lua_fn.h"

#include <iostream>
#include <sstream>

void LuaModLoader::prepare_state(lua_State* L)
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
	push_function(L, std::bind(&LuaModLoader::require, this, std::placeholders::_1));
	lua_setglobal(L, "require");
}

int LuaModLoader::require(lua_State* L)
{
	std::string err;

	if (!lua_isstring(L, -1))
		err = (std::string)"Invalid argument to 'require'. String expected, got " + lua_typename(L, lua_type(L, -1)) + ".\n";

	if (!err.empty())
	{
		lua_pushstring(L, err.c_str());
		lua_error(L);
	}

	std::filesystem::path executed_file = correct_path(lua_tostring(L, -1));

	if (!fs->exists(executed_file) )
		err = "File on path " + executed_file.string() + " not found.\n";

	if (!err.empty())
	{
		lua_pushstring(L, err.c_str());
		lua_error(L);
	}

	int beginning_stack_size = lua_gettop(mod_state);

	// Execute the file
	if (luaL_dofile(mod_state, (fs->get_correct_path(executed_file)).string().c_str()) != LUA_OK)
	{
		lua_pushstring(L, lua_tostring(mod_state, -1));
		lua_error(L);
	}

	int end_stack_size = lua_gettop(mod_state);

	return end_stack_size - beginning_stack_size;
}

std::filesystem::path LuaModLoader::correct_path(const std::string& lua_path)
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

bool LuaModLoader::move_lua_value(lua_State* from, lua_State* to, int from_idx, bool first_call)
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
				move_lua_value(from, to, -2, false);
				move_lua_value(from, to, -1, false);

				if (lua_isnil(to, -2))
				{
					if (first_call)
						cyclic_data_memory.clear();
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
		if (first_call)
			cyclic_data_memory.clear();
		return false;
		break;
	}

	lua_pop(from, 1);

	if (first_call)
		cyclic_data_memory.clear();

	return true;
}

bool LuaModLoader::are_same(lua_State* L1, lua_State* L2, int idx1, int idx2, bool first_call)
{
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
			std::cout << "Hi\n";
			lua_pushnil(L1);
			while (lua_next(L1, -2))
			{
				move_lua_value(L1, L2, -2);
				lua_rawget(L2, -2);

				std::cout << "L1:\n";
				print_stack(L1);
				std::cout << "L2:\n";
				print_stack(L2);

				same = are_same(L1, L2, -1, -1, false);

				lua_pop(L2, 1);

				// Clear
				tab1_len++;
				lua_pop(L1, 1);

				if (!same)
					break;
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

	if (first_call)
		cyclic_data_memory.clear();

	return same;
}

void LuaModLoader::print_stack(lua_State* stack)
{
	for (int i = 1; i <= lua_gettop(stack); i++)
	{
		std::cout << i << ". "<< lua_typename(stack, lua_type(stack, i)) << "\t" << get_string(stack, i) << "\n";
	}
}

std::string LuaModLoader::get_string(lua_State* L, int idx)
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

LuaModLoader::LuaModLoader()
{
	l_storage = 0;
	executed_state = 0;
	mod_state = 0;
	current_stage = LoaderStage::STAGE_NONE;
	fs = 0;

	allowed_prototypes = {
		{LoaderStage::STAGE_SETTINGS, {"setting"}},
		{LoaderStage::STAGE_PROTOTYPES, {}},
		{LoaderStage::STAGE_DATA, {}},
	};
}

LuaModLoader::~LuaModLoader()
{
	if (executed_state)
	{
		lua_close(executed_state);
		executed_state = 0;
	}

	if (mod_state)
	{
		lua_close(mod_state);
		mod_state = 0;
	}
}

void LuaModLoader::register_filesystem(art::FileSystem* fs_)
{
	fs = fs_;
}

void LuaModLoader::register_l_storage(LuaStorage* ls)
{
	l_storage = ls;
}

void LuaModLoader::begin_stage(LoaderStage stage)
{
	current_stage = stage;

	executed_state = luaL_newstate();
	prepare_state(executed_state);
	l_storage->prepare_state(executed_state, allowed_prototypes[current_stage]);
}

bool LuaModLoader::end_stage()
{
	if (!l_storage->load_prototypes(executed_state, allowed_prototypes[current_stage]))
		return false;

	current_stage = LoaderStage::STAGE_NONE;

	lua_close(executed_state);
	executed_state = 0;

	return true;
}

LoaderStage LuaModLoader::get_current_stage() const
{
	return current_stage;
}

bool LuaModLoader::load_mod(const std::string& mod_name, const std::filesystem::path& file_to_run)
{
	try
	{
		std::cout << "Loading mod " << mod_name << "...\n";

		fs->enter_dir(mod_name);

		// Create a new state
		mod_state = luaL_newstate();
		prepare_state(mod_state);
		l_storage->prepare_state(mod_state, allowed_prototypes[current_stage]);

		if (!fs->exists(file_to_run))
		{
			std::cout << "Skipping\n";
			return true;
		}

		// Move data
		lua_getglobal(executed_state, "data");
		lua_getfield(executed_state, -1, "raw");

		lua_getglobal(mod_state, "data");
		move_lua_value(executed_state, mod_state, -1);
		lua_setfield(mod_state, -2, "raw");

		lua_pop(mod_state, 1);
		lua_pop(executed_state, 2);

		// Execute the file
		if (luaL_dofile(mod_state, (fs->get_correct_path(file_to_run)).string().c_str()) != LUA_OK)
		{
			last_error = lua_tostring(mod_state, -1);
			return false;
		}

		// Getting the tables
		lua_getglobal(mod_state, "data");
		if (lua_isnil(mod_state, -1))
		{
			lua_close(mod_state);
			return true;
		}

		lua_pushstring(mod_state, "raw");
		lua_rawget(mod_state, -2);
		if (lua_isnil(mod_state, -1))
		{
			lua_close(mod_state);
			return true;
		}

		lua_getglobal(executed_state, "data");
		if (lua_isnil(executed_state, -1))
		{
			lua_close(mod_state);
			lua_pop(executed_state, 1);
			last_error = "For some reason table `data` in the executed state was nil.";
			return false;
		}

		lua_getfield(executed_state, -1, "raw");
		if (lua_isnil(executed_state, -1))
		{
			lua_close(mod_state);
			lua_pop(executed_state, 2);
			last_error = "For some reason table `data.raw` in the executed state was nil.";
			return false;
		}

		// Comparison (history loading)
		for (const auto& prototype_type : allowed_prototypes[current_stage])
		{
			lua_pushstring(mod_state, prototype_type.c_str());
			lua_rawget(mod_state, -2);
			if (lua_isnil(mod_state, -1))
			{
				lua_pop(mod_state, -1);
				continue;
			}

			lua_getfield(executed_state, -1, prototype_type.c_str());

			lua_pushnil(mod_state);
			while (lua_next(mod_state, -2))
			{
				std::string p_name = lua_tostring(mod_state, -2);

				// Name check
				lua_pushstring(mod_state, "name");
				lua_rawget(mod_state, -2);
				std::string actual_name = lua_tostring(mod_state, -1);
				if (actual_name != p_name)
				{
					last_error = "Prototype name: " + actual_name + " is not the same as expected " + p_name + ". Avoid editing prototype's name.";
					lua_close(mod_state);
					lua_settop(executed_state, 0);
					return false;
				}
				lua_pop(mod_state, 1);

				// Type check
				lua_pushstring(mod_state, "type");
				lua_rawget(mod_state, -2);
				std::string actual_type = lua_tostring(mod_state, -1);
				if (actual_type != prototype_type)
				{
					last_error = "Prototype type: " + actual_name + " is not the same as expected " + prototype_type + ". Avoid editing prototype's type.";
					lua_close(mod_state);
					lua_settop(executed_state, 0);
					return false;
				}
				lua_pop(mod_state, 1);

				lua_getfield(executed_state, -1, p_name.c_str());

				if (!lua_isnil(executed_state, -1))
				{
					// This prototype was edited or left as it was
					if (!are_same(mod_state, executed_state, -1, -1))
						prototype_histories[prototype_type][p_name].push_back(mod_name);
				}
				else
				{
					// This is a new prototype: create new history
					prototype_histories[prototype_type].insert({ p_name, {mod_name} });
				}

				lua_pop(executed_state, 1);
				move_lua_value(mod_state, executed_state, -1);
				lua_setfield(executed_state, -2, p_name.c_str());
				lua_pop(executed_state, 1);

				lua_pop(mod_state, 1);
			}
		}

		// Finalizing
		lua_pop(executed_state, 2);

		lua_close(mod_state);
		fs->exit();
	}
	catch (std::exception& e)
	{
		last_error = e.what();
		return false;
	}

	return true;
}

std::string LuaModLoader::get_last_error()
{
	return last_error;
}
