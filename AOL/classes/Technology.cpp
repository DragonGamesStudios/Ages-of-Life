#include "Technology.h"
#include <algorithm>

#ifndef min
#define min std::min
#endif // !min


struct MyStruct
{
	int x = 3;
	int y = 5;
};

TechnologyPrototype::TechnologyPrototype(
	std::string _name,
	std::string _age,
	std::string _icon_path,
	int _level,
	int _research_time,
	std::vector<std::string> _parents,
	std::string _order,
	int _force_offset,
	int _group_order,
	bool _pass,
	bool _count_to_width,
	bool _is_root,
	bool _master,
	std::string _arrow_turn
) :
	GameObjectPrototype(_name), age(_age), arrow_turn(_arrow_turn), icon_path(_icon_path), level(_level), research_time(_research_time), parents(_parents), order(_order),
	force_offset(_force_offset), group_order(_group_order), pass(_pass), count_to_width(_count_to_width), is_root(_is_root), master(_master)
{
}

Technology::Technology(TechnologyPrototype* prototype) : GameObject(prototype->name)
{
	this->name = prototype->name;
	this->age_name = prototype->age;
	this->arrows = {};
	this->arrow_turn = prototype->arrow_turn;
	this->blit = { 0,0 };
	this->children = {};
	this->count_to_width = prototype->count_to_width;
	this->force_offset = prototype->force_offset;
	this->group_order = prototype->group_order;
	std::string correct_icon_path = parse_path(prototype->icon_path);
	this->icon = new Image(correct_icon_path, DrawData{}, PROTO_IMAGE_IGNORE_ERRORS);
	if (!this->icon->image) {
		al_show_native_message_box(
			NULL,
			"Prototype error",
			("Could not load initialize prototype of '" + prototype->name + "'").c_str(),
			("Could not find image on path '" + correct_icon_path + "'").c_str(),
			"Ok",
			ALLEGRO_MESSAGEBOX_ERROR
		);
	}
	this->is_root = prototype->is_root;
	this->level = prototype->level;
	this->master = prototype->master;
	this->order = prototype->order;
	this->parents = {};
	this->parents_names = prototype->parents;
	this->pass = prototype->pass;
	this->research_effect = {};
	this->research_time = prototype->research_time;
	this->translated_description = "";
	this->translated_name = "";
	this->path = {};

	this->get_dumped();
}

Technology::~Technology()
{
	delete this->icon;
}

void Technology::fill_dependencies(std::map<std::string, std::map<std::string, GameObject*>>* obj_tree)
{
	// We don't fill age - it's already filled
	for (auto technology_name : this->parents_names)
	{
		if (obj_tree->at("technology").find(technology_name) != obj_tree->at("technology").end())
		{
			Technology* fixed = (Technology*)obj_tree->at("technology")[technology_name];
			this->parents.push_back(fixed);
			fixed->children.push_back(this);
		}
		else
		{
			al_show_native_message_box(
				NULL,
				"Prototype error",
				("Could not fill dependencies of " + this->name).c_str(),
				("Could not find technology with name " + technology_name).c_str(),
				"Ok",
				ALLEGRO_MESSAGEBOX_ERROR
			);
		}
	}

	this->get_dumped();
}

void Technology::get_arrows()
{
}

void Technology::get_dumped()
{
	std::map<std::string, Dumpable> buff = {
		{"name", std::make_pair("string", get_string_representation(name))},
		{"age_name", std::make_pair("string", get_string_representation(age_name))},
		{"arrow_turn", std::make_pair("string", get_string_representation(arrow_turn))},
		{"order", std::make_pair("string", get_string_representation(order))},
		{"translated_description", std::make_pair("string", get_string_representation(translated_description))},
		{"translated_name", std::make_pair("string", get_string_representation(translated_name))},
		{"count_to_width", std::make_pair("bool", get_string_representation(count_to_width))},
		{"force_offset", std::make_pair("int", get_string_representation(force_offset))},
		{"group_order", std::make_pair("int", get_string_representation(group_order))},
		{"research_time", std::make_pair("int", get_string_representation(research_time))},
		{"icon", std::make_pair("Image*", get_string_representation(icon, "Image"))},
		{"age", std::make_pair("Age*", get_string_representation(age, "Age"))},
		{"is_root", std::make_pair("bool", get_string_representation(is_root))},
		{"level", std::make_pair("int", get_string_representation(level))},
		{"master", std::make_pair("bool", get_string_representation(master))},
		{"pass", std::make_pair("bool", get_string_representation(pass))},
		{"group", std::make_pair("Technology*", get_string_representation(group ? group->name : get_string_representation(group)))},
		{"path", std::make_pair(
			"TechnologyPath",
			std::get<std::pair<std::string, std::map<std::string, Dumpable>>>
			(vector_to_dumpable<Technology*>(path, [](Technology* t)->std::string {return t->name; }, "Technology*")).second
		)},
	};
	this->dumped = std::make_pair("Technology", buff);
}

TechnologyPath Technology::get_path()
{
	if (this->path.size() == 0) {
		for (auto parent : this->parents) {
			parent->get_path();
			this->path += parent->path;
		}

		this->path.push_back(this);
	}

	return this->path;
}

Technology* Technology::get_group()
{
	// Skip if group filled
	if (!this->group)
	{
		// If the path doesn't exist, get it
		this->get_path();

		int current_level = 0;
		// Pre-set sure name to master technology
		Technology* sure_tech = NULL;

		for (auto technology = this->path.begin(); technology != this->path.end(); technology++)
		{
			if ((*technology)->level == current_level)
			{
				// If the level repeats, break iteration
				break;
			}

			// If the level doesn't repeat, update the name to previous technology, if it exists
			if (technology != this->path.begin())
			{
				sure_tech = *(technology - 1);
			}

			// Update the currently processed level
			current_level = (*technology)->level;
		}

		// Use sure technology
		this->group = sure_tech;

		this->get_dumped();
	}

	return this->group;
}

const TechnologyPath TechnologyPath::operator+(TechnologyPath& tech)
{
	TechnologyPath newpath = {};
	auto iter1 = tech.begin();
	auto iter2 = this->begin();
	
	// Iterate over paths
	while (iter1 != tech.end() && iter2 != this->end())
	{
		// If technologies are identical, skip one
		if ((*iter1) == (*iter2))
		{
			iter1++;
		}

		if (iter1 == tech.end() || iter2 == this->end())
			break;

		// If technology level is lower, it goes first
		if ((*iter1)->level > (*iter2)->level)
		{
			newpath.push_back(*iter2);
			iter2++;
		}
		else if ((*iter1)->level < (*iter2)->level)
		{
			newpath.push_back(*iter1);
			iter1++;
		}
		// If both technologies are the same level, they are sorted alphabetically
		else
		{
			if ((*iter1)->name < (*iter2)->name)
			{
				newpath.push_back(*iter1);
				iter1++;
			}
			else
			{
				newpath.push_back(*iter2);
				iter2++;
			}
		}
	}

	for (auto fill = iter1; fill != tech.end(); fill++)
	{
		newpath.push_back(*fill);
	}

	for (auto fill = iter2; fill != this->end(); fill++)
	{
		newpath.push_back(*fill);
	}

	return newpath;
}

void TechnologyPath::operator+=(TechnologyPath& tech)
{
	*this = this->operator+(tech);
}
