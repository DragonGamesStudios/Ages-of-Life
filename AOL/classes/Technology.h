#pragma once
#include "Age.h"

struct TechnologyPrototype : GameObjectPrototype {
	std::string age;
	std::string order;
	std::string arrow_turn;
	std::string icon_path;
	int level;
	int research_time;
	int force_offset;
	int group_order;
	bool pass;
	bool count_to_width;
	bool master;
	bool is_root;
	std::vector<std::string> parents;

	TechnologyPrototype(
		std::string _name = "",
		std::string _age = "",
		std::string _icon_path = "__base__/graphics/age/icons/stone-age.png",
		int _level = 0,
		int _research_time = 0,
		std::vector<std::string> _parents = {},
		std::string _order  = "0",
		int _force_offset = 0,
		int _group_order = 0,
		bool _pass = false,
		bool _count_to_width = true,
		bool _is_root = false,
		bool _master = false,
		std::string _arrow_turn = "top"
	);
};

struct ResearchEffectPrototype {

};

struct ResearchEffect {

};

class Technology;

class TechnologyPath : public std::vector<Technology*> {
public:
	using std::vector<Technology*>::operator=;
	using std::vector<Technology*>::operator[];
	const TechnologyPath operator+(TechnologyPath& tech);
	void operator+=(TechnologyPath& tech);
};

class Technology : public GameObject
{
public:
	std::string age_name;
	std::string order;
	std::string name;
	std::string translated_name;
	std::string translated_description;
	std::string arrow_turn;

	ResearchEffect research_effect;

	int level;
	int research_time;
	int force_offset;
	int group_order;

	bool pass;
	bool count_to_width;
	bool master;
	bool is_root;

	std::pair<int, int> blit;

	std::vector<std::string> parents_names;
	std::vector<std::vector<std::pair<int, int>>> arrows;
	std::vector<Technology*> parents;
	std::vector<Technology*> children;

	Image* icon;
	Age* age;

	Dumpable dumped;
	TechnologyPath path;
	Technology* group;

	Technology(TechnologyPrototype* prototype);
	~Technology();

	void fill_dependencies(std::map<std::string, std::map<std::string, GameObject*>>* obj_tree);
	void get_arrows();
	void get_dumped();

	TechnologyPath get_path();
	Technology* get_group();
};

struct TechnologyReference {
	Technology* technology = NULL;
	bool researched = false;
	bool visible = false;
};

struct TechnologyGroupData {
	int x = 0;
	int y = 0;
	int width = 0;
	int level_height = 0;
	int level_y = 0;
};

const int tech_interval_x = 20, tech_interval_y = 100, tech_label_width = 300, tech_label_height = 150;