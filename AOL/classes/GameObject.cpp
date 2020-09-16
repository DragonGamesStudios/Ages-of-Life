#include "GameObject.h"

GameObject::GameObject(std::string _name) : name(_name), translated("") {}

GameObjectFinder::GameObjectFinder(std::string name) : _name(name) {}

const bool GameObjectFinder::operator()(const std::pair<std::string, GameObject*>& v)
{
	return v.first == _name;
}

GameObjectPrototype::GameObjectPrototype(std::string _name) : name(_name)
{
}
