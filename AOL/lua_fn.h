#pragma once
#include "lua_.hpp"
#include <functional>

typedef std::function<int(lua_State*)> lua_fn;

int on_call(lua_State* L);
int on_garbage_collection(lua_State* L);
int on_tostring(lua_State* L);

void push_function(lua_State* L, const lua_fn& fn);

extern const char* const fn_mt;