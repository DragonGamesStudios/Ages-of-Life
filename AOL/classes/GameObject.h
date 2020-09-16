#pragma once
#include <string>
#include "..\allegrolib.h"
#include "..\lib\Proto\Proto.h"

struct color {
	unsigned char r = 255;
	unsigned char g = 255;
	unsigned char b = 255;
	unsigned char a = 255;
};

class GameObject {
public:
	std::string name;
	std::string translated;

	GameObject(std::string _name);
	virtual ~GameObject() = default;
};

struct GameObjectFinder {
	GameObjectFinder(std::string name);
	const bool operator()( const std::pair<std::string, GameObject*>& v );
private:
	std::string _name;
};

struct GameObjectPrototype {
	std::string name;
	/*std::string neighbor_age;
	std::string icon_path;
	std::string master_image_path;
	color font_color;
	color master_color;
	std::string neighbor_specifier;*/
	GameObjectPrototype(std::string _name = "");
};