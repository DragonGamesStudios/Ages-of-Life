#pragma once
#include "GameObject.h"
#include "..\independent-globals.h"

class Technology;

struct AgePrototype : GameObjectPrototype {
	//std::string name;
	std::string neighbor_age;
	std::string icon_path;
	std::string master_image_path;
	color font_color;
	color master_color;
	std::string neighbor_specifier;

	AgePrototype(
		std::string _name = "",
		std::string _neighbor_age = "",
		std::string _icon_path = "",
		std::string _master_image_path = "",
		color _font_color = color{ 255, 255, 255 },
		color _master_color = color{ 100, 100, 100 },
		std::string _neighbor_specifier = "previous"
	);
};

class Age : public GameObject
{
public:
	std::string name;
	std::string translated;
	std::string neighbor_age_name;
	bool is_neighbor_next;

	Age* next_age;
	Age* previous_age;

	Image* icon;
	int max_level;
	std::vector<Technology*> technologies;
	ALLEGRO_COLOR font_color;
	ALLEGRO_COLOR master_color;
	Image* master_image;

	Dumpable dumped;

	AgePrototype* prototype;

	void get_dumped();

	Age(AgePrototype* prototype);
	~Age();

	void fill_dependencies(std::map<std::string, std::map<std::string, GameObject*>>* obj_tree);
};

struct AgeReference {
	Age* age = NULL;
	bool unlocked = false;
};

