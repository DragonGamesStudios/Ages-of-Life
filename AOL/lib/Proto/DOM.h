#pragma once
#include "Proto.h"

// DOM GUI

struct DOM_element;

struct DOM_path : public std::vector<DOM_element*>
{
	DOM_path operator&(DOM_path path1);
};

struct DOM_ruleset
{
	DOM_ruleset(json _info);
	json info;
	DOM_ruleset operator|(DOM_ruleset ruleset);
	void operator|=(DOM_ruleset ruleset);
	const json get_rule(std::string rule_name);
	void set_rule(std::string rule_name, std::string value);
};

extern DOM_ruleset DOM_default_ruleset;

struct DOM_computed
{
	ALLEGRO_COLOR background_color = { 0, 0, 0, 0 };
	int width = 0;
	int height = 0;
	int x = 0;
	int y = 0;

	int box_content_width = 0;
	int box_content_height = 0;

	int padding_left = 0;
	int padding_top = 0;
	int padding_right = 0;
	int padding_bottom = 0;

	int margin_top = 0;
	int margin_right = 0;
	int margin_bottom = 0;
	int margin_left = 0;

	int border_width_left = 0;
	int border_width_right = 0;
	int border_width_top = 0;
	int border_width_bottom = 0;

	std::string border_style_left = "none";
	std::string border_style_right = "none";
	std::string border_style_top = "none";
	std::string border_style_bottom = "none";

	ALLEGRO_COLOR border_color_left = { 0, 0, 0, 0 };
	ALLEGRO_COLOR border_color_right = { 0, 0, 0, 0 };
	ALLEGRO_COLOR border_color_top = { 0, 0, 0, 0 };
	ALLEGRO_COLOR border_color_bottom = { 0, 0, 0, 0 };
};

struct DOM_dependent_unit_value
{
	bool is = false;
	std::string unit = "";
	float value = 0;
};

struct DOM_dependent
{
	bool background_color = false;
	bool border_style_left = false;
	bool border_style_top = false;
	bool border_style_right = false;
	bool border_style_bottom = false;

	bool border_color_left = false;
	bool border_color_top = false;
	bool border_color_right = false;
	bool border_color_bottom = false;

	DOM_dependent_unit_value width;
	DOM_dependent_unit_value height;

	DOM_dependent_unit_value margin_top;
	DOM_dependent_unit_value margin_bottom;
	DOM_dependent_unit_value margin_left;
	DOM_dependent_unit_value margin_right;

	DOM_dependent_unit_value padding_top;
	DOM_dependent_unit_value padding_bottom;
	DOM_dependent_unit_value padding_left;
	DOM_dependent_unit_value padding_right;

	DOM_dependent_unit_value border_width_top;
	DOM_dependent_unit_value border_width_bottom;
	DOM_dependent_unit_value border_width_left;
	DOM_dependent_unit_value border_width_right;
};

struct DOM_element
{
	std::vector<DOM_element*> children = {};
	DOM_element* parent = nullptr;
	DOM_path path = {};
	bool initialized = false;
	DOM_computed computed = {};
	json state = {};
	std::string id = "";
	std::vector<std::string> classes = {};
	std::map<std::string, std::vector<std::vector<std::string>>> parsed_ruleset = {};
	DOM_dependent dependent = {};
	DOM_element* document_base = nullptr;
	std::tuple<std::vector<DOM_dependent_unit_value*>, std::vector<int*>, std::vector<bool>, std::vector<int>> box_model_deps = { {}, {}, {}, {} };

	void set_rulesets(std::vector<DOM_ruleset> rulesets);
	void add_child(DOM_element* child);
	void calculate();
	int get_width(std::string boxes = "cpbm");
	int get_height(std::string boxes = "cpbm");
	void draw(int x, int y);
	void update(double dt);
};

class DOM_document
{
public:
	DOM_document(DOM_element* root, int width = 1000, int height = 1000);
	~DOM_document();

	DOM_element* get_root();
	DOM_element* get_element_by_id(std::string id);
	std::vector<DOM_element> get_elements_of_class(std::string classname);
	void add_event_listener(std::string, DOM_element* element, std::function<void()>);

	std::map<std::string, std::pair<DOM_element*, std::function<void()>>> event_listeners;

	void draw(int x, int y);
	void update(double dt);
	void calculate();

	void add_element_with_children(DOM_element* element);

private:
	std::vector<DOM_element*> elements;
	DOM_element* root;
	DOM_element* base;
};