#pragma once

#include "LuaStorage.h"

class Storage
{
private:
	LuaStorage* l_storage;

public:
	Storage();

	void register_l_storage(LuaStorage* storage);

	//void initialize_prototype(const SettingPrototype& prototype);
	void initialize_l_prototypes();
};