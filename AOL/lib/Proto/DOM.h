#pragma once
#include "Proto.h"
#include <set>

#define DOM_EVENT_ELEMENT_HOVER_GAINED -2
#define DOM_EVENT_ELEMENT_HOVER_LOST -1
#define DOM_EVENT_MOUSE_PRESSED 0
#define DOM_EVENT_MOUSE_RELEASED 1
#define DOM_EVENT_MOUSE_MOTION 2
#define DOM_EVENT_MOUSE_SCROLL 3
#define DOM_EVENT_MOUSE_DRAG 4
#define DOM_EVENT_KEY_PRESSED 5
#define DOM_EVENT_KEY_RELEASED 6
#define DOM_EVENT_KEY_INPUT 7
//#define DOM_EVENT_COUNT 5

// DOM GUI
extern std::map<std::string, ALLEGRO_BITMAP*> DOM_loaded_images;

struct DOM_element;
class DOM_document;

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

	std::string border_style_left = "";
	std::string border_style_right = "";
	std::string border_style_top = "";
	std::string border_style_bottom = "";

	ALLEGRO_COLOR border_color_left = { 0, 0, 0, 0 };
	ALLEGRO_COLOR border_color_right = { 0, 0, 0, 0 };
	ALLEGRO_COLOR border_color_top = { 0, 0, 0, 0 };
	ALLEGRO_COLOR border_color_bottom = { 0, 0, 0, 0 };

	bool hide_overflow = true;
	bool scrollable_x = false;
	bool scrollable_y = false;

	int scroll_x = 0;
	int scroll_y = 0;
};

struct DOM_computed_background
{
	ALLEGRO_COLOR background_color = { 0, 0, 0, 0 };

	std::vector<ALLEGRO_BITMAP*> background_images = {};

	std::string background_clip = "";
	std::string background_origin = "";

	int background_size_width = 0;
	int background_size_height = 0;

	float background_size_sx = 1;
	float background_size_sy = 1;

	int background_position_x = 0;
	int background_position_y = 0;

	bool background_repeat_x = false;
	bool background_repeat_y = false;
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
	bool background_image = false;
	bool background_clip = false;
	bool background_origin = false;
	bool background_repeat_x = false;
	bool background_repeat_y = false;

	bool background_size = false;
	std::string background_size_type = "";

	DOM_dependent_unit_value background_size_width;
	DOM_dependent_unit_value background_size_height;
	DOM_dependent_unit_value background_position_x;
	DOM_dependent_unit_value background_position_y;

	bool border_style_left = false;
	bool border_style_top = false;
	bool border_style_right = false;
	bool border_style_bottom = false;

	bool border_color_left = false;
	bool border_color_top = false;
	bool border_color_right = false;
	bool border_color_bottom = false;

	bool overflow = false;

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

struct DOM_event
{
	char type;
	int x;
	int y;
	int dx;
	int dy;
	unsigned int button;
	int keycode;
	char character;
	unsigned int modifiers;
	DOM_element* element;
};

struct DOM_element
{
	DOM_element();
	std::vector<DOM_element*> children = {};
	DOM_element* parent = nullptr;
	DOM_path path = {};
	bool initialized = false;
	DOM_computed* computed;
	DOM_computed_background* computed_background;
	json state = {};
	std::string id = "";
	std::vector<std::string> classes = {};
	std::map<std::string, std::vector<std::vector<std::string>>> parsed_ruleset = {};
	DOM_dependent dependent = {};
	DOM_element* document_base = nullptr;
	DOM_document* document = nullptr; 
	std::tuple<std::vector<DOM_dependent_unit_value*>, std::vector<int*>, std::vector<bool>, std::vector<int>> box_model_deps = { {}, {}, {}, {} };
	bool hover = false;
	std::map<char, std::function<void(DOM_event e)>> event_functions = {};

	bool hover_loss_event = false;
	bool hover_gain_event = false;

	void set_rulesets(std::vector<DOM_ruleset> rulesets);
	void add_child(DOM_element* child);
	void calculate();
	int get_width(std::string boxes = "cpbm");
	int get_height(std::string boxes = "cpbm");
	void draw(int x, int y);
	void update(double dt, int x, int y, bool force_fail = false);
	void on(char event, std::function<void(DOM_event e)> fun);
	void dispatch_scroll_event(DOM_event e);
};

class DOM_document
{
public:
	DOM_document(DOM_element* root, int width = 1000, int height = 1000);
	~DOM_document();

	DOM_element* get_root();
	DOM_element* get_element_by_id(std::string id);
	std::vector<DOM_element*> get_elements_of_class(std::string classname);

	std::map<char, std::set<DOM_element*>> event_listeners = {};
	void dispatch_event(ALLEGRO_EVENT e);
	std::vector<DOM_event> events_to_dispatch = {};
	std::vector<DOM_event> element_related_events = {};

	void draw(int x, int y);
	void update(double dt, int x, int y);
	void calculate();

	void add_element_with_children(DOM_element* element);
	ALLEGRO_MOUSE_STATE mouse_state = {};

	// No longer makes sense
	// std::vector<DOM_element*> hovered_elements = {};

private:
	std::vector<DOM_element*> elements = {};
	DOM_element* root = nullptr;
	DOM_element* base = nullptr;
};

void DOM_quit();