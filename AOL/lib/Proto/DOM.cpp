#include "Proto.h"

json DOM_default_ruleset_json = {
	{"background-color", {0, 0, 0, 0}},
	{"width", "auto"}
};

DOM_ruleset DOM_default_ruleset = DOM_default_ruleset_json;

DOM_ruleset::DOM_ruleset(json _info)
{
	this->info = _info;
}