#include "globals.h"
#include "classes/Age.h"
#include "classes/Technology.h"

#ifndef __LUA_INC_H__

#define __LUA_INC_H__

extern "C"
{
#include <lualib.h>
#include <lua.h>
#include <lauxlib.h>
}

#endif // __LUA_INC_H__

std::vector<std::pair<std::string, GameObjectPrototype*>> loaded_prototypes = {};

bool check_lua(lua_State* L, int r)
{
	if (r != LUA_OK) {
		std::cout << "Lua error: " << lua_tostring(L, -1) << '\n';
		return false;
	}
	return true;
}

bool get_lua_value(lua_State* L, std::string key, std::string* res, int pos=-1)
{
	lua_pushstring(L, key.c_str());
	lua_gettable(L, pos-1);
	if (!lua_isstring(L, -1)) {
		return false;
	}
	*res = lua_tostring(L, -1);
	lua_pop(L, 1);
	return true;
}

bool get_lua_value(lua_State* L, std::string key, int* res, int pos = -1)
{
	lua_pushstring(L, key.c_str());
	lua_gettable(L, pos - 1);
	if (!lua_isnumber(L, -1)) {
		return false;
	}
	*res = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return true;
}

bool get_lua_value(lua_State* L, std::string key, bool* res, int pos = -1)
{
	lua_pushstring(L, key.c_str());
	lua_gettable(L, pos - 1);
	if (!lua_isboolean(L, -1)) {
		return false;
	}
	*res = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return true;
}

int lua_HostFunction(lua_State* L)
{
	int nargs = lua_gettop(L);
	if (nargs != 1) { std::cout << "Exactly one argument must be provided\n"; return 0; }
	int type = lua_type(L, 1);
	if (type != LUA_TTABLE) { std::cout << "Argument must be a table\n"; return 0; }
	//std::cout << "[C++] called HostFunction()\n";

	size_t l = lua_rawlen(L, 1);
	lua_pushnil(L);
	bool isvalid = true;
	std::string error = "";
	GameObjectPrototype* new_prototype = NULL;
	while (lua_next(L, -2) != 0) {
		if (lua_istable(L, -1)) {

			new_prototype = new GameObjectPrototype;

			std::string object_type = "";
			if (!get_lua_value(L, "type", &object_type)) {
				isvalid = false;
				error = "Incorrect type of 'type' field. String expected\n";
				break;
			}

			if (object_type == "age") {
				AgePrototype* new_age_prototype = new AgePrototype(new_prototype->name);
				delete new_prototype;
				new_prototype = NULL;

				if (!get_lua_value(L, "name", &new_age_prototype->name)) {
					isvalid = false;
					error = "Incorrect type of 'name' field. String expected\n";
					break;
				}

				if (!get_lua_value(L, "neighbor_age", &new_age_prototype->neighbor_age)) {
					isvalid = false;
					error = "Incorrect type of 'neighbor_age' field. String expected\n";
					break;
				}

				if (!get_lua_value(L, "icon_path", &new_age_prototype->icon_path)) {
					isvalid = false;
					error = "Incorrect type of 'icon_path' field. String expected\n";
					break;
				}

				if (!get_lua_value(L, "master_image_path", &new_age_prototype->master_image_path)) {
					isvalid = false;
					error = "Incorrect type of 'master_image_path' field. String expected\n";
					break;
				}

				if (!get_lua_value(L, "neighbor_specifier", &new_age_prototype->neighbor_specifier)) {
					isvalid = false;
					error = "Incorrect type of 'neighbor_specifier' field. String expected\n";
					break;
				}

				lua_pushstring(L, "font_color");
				lua_gettable(L, -2);
				if (!lua_istable(L, -1)) {
					isvalid = false;
					error = "Incorrect type of 'font_color' field. Table expected\n";
					break;
				}

				int r = 0, g = 0, b = 0;

				if (!get_lua_value(L, "r", &r)) {
					isvalid = false;
					error = "Incorrect type of 'font_color'->'r' field. Number expected\n";
					break;
				}
				else if (r < 0 || r > 255) {
					isvalid = false;
					error = "Incorrect value of 'font_color'->'r' field. r < 0 or r > 255\n";
					break;
				}

				if (!get_lua_value(L, "g", &g)) {
					isvalid = false;
					error = "Incorrect type of 'font_color'->'g' field. Number expected\n";
					break;
				}
				else if (g < 0 || g > 255) {
					isvalid = false;
					error = "Incorrect value of 'font_color'->'g' field. g < 0 or g > 255\n";
					break;
				}

				if (!get_lua_value(L, "b", &b)) {
					isvalid = false;
					error = "Incorrect type of 'font_color'->'b' field. Number expected\n";
					break;
				}
				else if (b < 0 || b > 255) {
					isvalid = false;
					error = "Incorrect value of 'font_color'->'b' field. b < 0 or b > 255\n";
					break;
				}

				new_age_prototype->font_color.r = r;
				new_age_prototype->font_color.g = g;
				new_age_prototype->font_color.b = b;

				lua_pop(L, 1);

				lua_pushstring(L, "master_color");
				lua_gettable(L, -2);
				if (!lua_istable(L, -1)) {
					isvalid = false;
					error = "Incorrect type of 'master_color' field. Table expected\n";
					break;
				}

				r = g = b = 0;

				if (!get_lua_value(L, "r", &r)) {
					isvalid = false;
					error = "Incorrect type of 'master_color'->'r' field. Number expected\n";
					break;
				}
				else if (r < 0 || r > 255) {
					isvalid = false;
					error = "Incorrect value of 'master_color'->'r' field. r < 0 or r > 255\n";
					break;
				}

				if (!get_lua_value(L, "g", &g)) {
					isvalid = false;
					error = "Incorrect type of 'master_color'->'g' field. Number expected\n";
					break;
				}
				else if (g < 0 || g > 255) {
					isvalid = false;
					error = "Incorrect value of 'master_color'->'g' field. g < 0 or g > 255\n";
					break;
				}

				if (!get_lua_value(L, "b", &b)) {
					isvalid = false;
					error = "Incorrect type of 'master_color'->'b' field. Number expected\n";
					break;
				}
				else if (b < 0 || b > 255) {
					isvalid = false;
					error = "Incorrect value of 'master_color'->'b' field. b < 0 or b > 255\n";
					break;
				}

				new_age_prototype->master_color.r = r;
				new_age_prototype->master_color.g = g;
				new_age_prototype->master_color.b = b;

				lua_pop(L, 1);

				new_prototype = new_age_prototype;
			}

			if (object_type == "technology") {
				auto new_tech_prototype = new TechnologyPrototype();
				delete new_prototype;
				new_prototype = NULL;

				if (!get_lua_value(L, "name", &new_tech_prototype->name)) {
					isvalid = false;
					error = "Incorrect type of field 'name'. String expected";
					break;
				}

				if (!get_lua_value(L, "age", &new_tech_prototype->age)) {
					isvalid = false;
					error = "Incorrect type of field 'age'. String expected";
					break;
				}

				if (!get_lua_value(L, "level", &new_tech_prototype->level)) {
					isvalid = false;
					error = "Incorrect type of field 'level'. Number expected";
					break;
				}

				if (!get_lua_value(L, "is_root", &new_tech_prototype->is_root)) {
					new_tech_prototype->is_root = false;
					lua_pop(L, 1);
				}

				lua_pushstring(L, "parents");
				lua_gettable(L, -2);
				lua_pushnil(L);
				while (lua_next(L, -2) != 0)
				{

					new_tech_prototype->parents.push_back(lua_tostring(L, -1));
					lua_pop(L, 1);
				}
				lua_pop(L, 1);

				new_prototype = new_tech_prototype;
			}

			loaded_prototypes.push_back(std::make_pair(object_type, new_prototype));
		}
		else {
			std::cout << "Incorrect prototype. Please provide table\n";
		}
		lua_pop(L, 1);
	}
	if (!isvalid) {
		if (new_prototype) delete new_prototype;
		std::cout << "Error: " << error << '\n';
	}
	return 1;
}

void Game::api_load_datafile(std::string path)
{
	int i_err = 0;
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	lua_register(L, "aol_register", lua_HostFunction);

	if (check_lua(L, luaL_dofile(L, path.c_str()))) {
		

		/*lua_getglobal(L, "player");
		if (lua_istable(L, -1)) {
			lua_pushstring(L, "name");
			lua_gettable(L, -2);
			std::cout << "Lua effect: " << lua_tostring(L, -1) << '\n';
			lua_pop(L, 1);

			lua_pushstring(L, "surname");
			lua_gettable(L, -2);
			std::cout << "Lua effect: " << lua_tostring(L, -1) << '\n';
			lua_pop(L, 1);

			lua_pushstring(L, "family");
			lua_gettable(L, -2);
			std::cout << "Lua effect: " << lua_tostring(L, -1) << '\n';
			lua_pop(L, 1);
		}*/

		for (auto p = loaded_prototypes.begin(); p != loaded_prototypes.end(); p++) {
			this->register_prototype(p->second, p->first);
		}
	}

	lua_close(L);
}