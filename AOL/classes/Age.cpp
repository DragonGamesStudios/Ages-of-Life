#include "Age.h"
#include "Technology.h"
#include "..\independent-globals.h"


void Age::get_dumped()
{
	std::map<std::string, Dumpable> buff;
	std::map<std::string, Dumpable> fcol, mcol;

	fcol.insert(std::make_pair("r", std::make_pair("float", get_string_representation(this->font_color.r))));
	fcol.insert(std::make_pair("g", std::make_pair("float", get_string_representation(this->font_color.g))));
	fcol.insert(std::make_pair("b", std::make_pair("float", get_string_representation(this->font_color.b))));
	fcol.insert(std::make_pair("a", std::make_pair("float", get_string_representation(this->font_color.a))));

	mcol.insert(std::make_pair("r", std::make_pair("float", get_string_representation(this->master_color.r))));
	mcol.insert(std::make_pair("g", std::make_pair("float", get_string_representation(this->master_color.g))));
	mcol.insert(std::make_pair("b", std::make_pair("float", get_string_representation(this->master_color.b))));
	mcol.insert(std::make_pair("a", std::make_pair("float", get_string_representation(this->master_color.a))));

	buff.insert(std::make_pair("name",              std::make_pair("string", get_string_representation(this->name))                ));
	buff.insert(std::make_pair("translated",        std::make_pair("string", get_string_representation(this->translated))          ));
	buff.insert(std::make_pair("neigbor_age_name",  std::make_pair("string", get_string_representation(this->neighbor_age_name))   ));
	buff.insert(std::make_pair("is_neighbor_next",  std::make_pair("bool",   get_string_representation(this->is_neighbor_next))    ));
	buff.insert(std::make_pair("next_age",          std::make_pair("Age*",   get_string_representation(this->next_age, "Age"))     ));
	buff.insert(std::make_pair("previous_age",      std::make_pair("Age*",   get_string_representation(this->previous_age, "Age")) ));
	buff.insert(std::make_pair("font_color",        std::make_pair("color",  fcol)                                                 ));
	buff.insert(std::make_pair("master_color",      std::make_pair("color",  mcol)                                                 ));


	dumped = std::make_pair("Age", buff);
}

Age::Age(AgePrototype* prototype) : GameObject(prototype->name)
{
	assert(prototype->name.length() > 0);

	this->prototype = prototype;

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
	std::string correct_icon_path = parse_path(prototype->icon_path);
	std::string correct_master_image_path = parse_path(prototype->master_image_path);
	this->icon = new Image(correct_icon_path, DrawData{}, PROTO_IMAGE_IGNORE_ERRORS);
	this->master_image = new Image(correct_master_image_path, DrawData{}, PROTO_IMAGE_IGNORE_ERRORS);

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
	if (!this->master_image->image) {
		al_show_native_message_box(
			NULL,
			"Prototype error",
			("Could not load initialize prototype of '" + prototype->name + "'").c_str(),
			("Could not find image on path '" + correct_master_image_path + "'").c_str(),
			"Ok",
			ALLEGRO_MESSAGEBOX_ERROR
		);
	}

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
		if (!this->next_age) {
			if (obj_tree->at("age").find(this->neighbor_age_name) != obj_tree->at("age").end()) {
				Age* fixed = (Age*)obj_tree->at("age")[this->neighbor_age_name];
				this->next_age = fixed;
				fixed->previous_age = this;
			}
			else {
				//run error
				std::cout << "Could not find age with name " << this->neighbor_age_name << "\n";
				return;
			}
		}

		/*if (!this->previous_age) {
			auto found = std::find_if(obj_tree->at("age").begin(), obj_tree->at("age").end(), [&searched](const std::pair<std::string, GameObject*>& v)->bool {
				return ((Age*)v.second)->is_neighbor_next && (((Age*)v.second)->neighbor_age_name == searched);
				});

			if (found != obj_tree->at("age").end()) {
				this->previous_age = (Age*)found->second;
			}
		}*/
	}
	else {
		if (!this->previous_age) {
			if (obj_tree->at("age").find(this->neighbor_age_name) != obj_tree->at("age").end()) {
				Age* fixed = (Age*)obj_tree->at("age")[this->neighbor_age_name];
				this->previous_age = fixed;
				fixed->next_age = this;
			}
			else {
				//run error
				std::cout << "Could not find age with name \"" << this->neighbor_age_name << "\"\n";
				return;
			}
		}

		/*if (!this->next_age) {
			auto found = std::find_if(obj_tree->at("age").begin(), obj_tree->at("age").end(), [&searched](const std::pair<std::string, GameObject*>& v)->bool {
				return (!((Age*)v.second)->is_neighbor_next) && (((Age*)v.second)->neighbor_age_name == searched);
			});
			if (found != obj_tree->at("age").end()) {
				this->next_age = (Age*)found->second;
			}
		}*/
	}

	// fill technologies
	for (const auto& [name, technology] : obj_tree->at("technology"))
	{
		Technology* fixed = (Technology*)technology;
		if (fixed->age_name == this->name)
		{
			this->technologies.push_back(fixed);
			fixed->age = this;
		}
	}

	get_dumped();
}

AgePrototype::AgePrototype(std::string _name, std::string _neighbor_age, std::string _icon_path, std::string _master_image_path, color _font_color, color _master_color, std::string _neighbor_specifier)
	: GameObjectPrototype(_name),
	neighbor_age(_neighbor_age), icon_path(_icon_path), master_image_path(_master_image_path), font_color(_font_color), master_color(_master_color),
	neighbor_specifier(_neighbor_specifier)
{
}
