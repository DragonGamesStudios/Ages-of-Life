#include "Age.h"
#include "..\independent-globals.h"

void Age::get_dumped()
{
	dumped = {};
	dumped.insert({ "name",             {"string", get_string_representation(this->name)} });
	dumped.insert({ "translated",       {"string", get_string_representation(this->translated)} });
	dumped.insert({ "neigbor_age_name", {"string", get_string_representation(this->neighbor_age_name)} });
	dumped.insert({ "is_neighbor_next", {"bool",   get_string_representation(this->is_neighbor_next)} });
	dumped.insert({ "next_age",         {"Age*",   get_string_representation(this->next_age, "Age")} });
	dumped.insert({ "previous_age",     {"Age*",   get_string_representation(this->previous_age, "Age")} });
	dumped.insert({ "r",     {"float",   get_string_representation(this->font_color.r)} });
}

Age::Age(AgePrototype* prototype) : GameObject(prototype->name)
{
	assert(prototype->name.length() > 0);
	this->name = prototype->name;
	this->translated = "";
	this->neighbor_age_name = prototype->neighbor_age;
	if (prototype->neighbor_specifier == "after")
		this->is_neighbor_next = true;
	else if (prototype->neighbor_specifier == "previous")
		this->is_neighbor_next = false;
	else
		//run error
		std::cout << "Incorrect neighbor specifier\n";

	this->icon = new Image(prototype->icon_path, DrawData{});
	this->master_image = new Image(prototype->master_image_path, DrawData{});

	this->max_level = 0;

	this->font_color = al_map_rgba(prototype->font_color.r, prototype->font_color.g, prototype->font_color.b, prototype->font_color.a);
	this->master_color = al_map_rgba(prototype->master_color.r, prototype->master_color.g, prototype->master_color.b, prototype->master_color.a);

	this->technologies = {};

	this->next_age = NULL;
	this->previous_age = NULL;

	this->get_dumped();
}

Age::~Age()
{
	delete
		this->icon,
		this->master_image;
}

void Age::fill_dependencies(std::map<std::string, std::map<std::string, GameObject*>>* obj_tree)
{
		std::string searched = this->name;
		bool valid = false;
	if (this->is_neighbor_next) {
		if (obj_tree->at("age").find(this->neighbor_age_name) != obj_tree->at("age").end()) {
			this->next_age = (Age*)obj_tree->at("age")[this->neighbor_age_name];
		}
		else {
			//run error
			std::cout << "Could not find age with name " << this->neighbor_age_name << "\n";
			return;
		}
		auto found = std::find_if(obj_tree->at("age").begin(), obj_tree->at("age").end(), [&searched](const std::pair<std::string, GameObject*>& v)->bool {
			return ((Age*)v.second)->is_neighbor_next && (((Age*)v.second)->neighbor_age_name == searched);
		});
		if (found != obj_tree->at("age").end()) {
			this->previous_age = (Age*)found->second;
		}
	}
	else {
		//for (obj_tree->at("ages").begin(), obj_tree->at("ages").end(), )
		if (obj_tree->at("age").find(this->neighbor_age_name) != obj_tree->at("age").end()) {
			this->previous_age = (Age*)obj_tree->at("age")[this->neighbor_age_name];
		}
		else {
			//run error
			std::cout << "Could not find age with name \"" << this->neighbor_age_name << "\"\n";
			return;
		}
		auto found = std::find_if(obj_tree->at("age").begin(), obj_tree->at("age").end(), [&searched](const std::pair<std::string, GameObject*>& v)->bool {
			return (!((Age*)v.second)->is_neighbor_next) && (((Age*)v.second)->neighbor_age_name == searched);
		});
		if (found != obj_tree->at("age").end()) {
			this->next_age = (Age*)found->second;
		}
	}

	get_dumped();
}

AgePrototype::AgePrototype(std::string _name, std::string _neighbor_age, std::string _icon_path, std::string _master_image_path, color _font_color, color _master_color, std::string _neighbor_specifier)
	: GameObjectPrototype(_name),
	neighbor_age(_neighbor_age), icon_path(_icon_path), master_image_path(_master_image_path), font_color(_font_color), master_color(_master_color), neighbor_specifier(_neighbor_specifier)
{
}
