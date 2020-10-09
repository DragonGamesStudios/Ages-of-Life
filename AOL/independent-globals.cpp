#include "independent-globals.h"
#include <sstream>
#include <iostream>

std::map<std::string, std::string> path_templates = {};

std::string get_string_representation(int v)
{
	return std::to_string(v);
}

std::string get_string_representation(std::string v)
{
	return "\"" + v + "\"";
}

std::string get_string_representation(bool v)
{
	return v ? "true" : "false";
}

std::string get_string_representation(float v)
{
	return std::to_string(v);
}

std::string get_string_representation(void* v)
{
	std::stringstream ss;
	ss << static_cast<const void*>(v);
	return "Object at " + ss.str();
}

std::string get_string_representation(void* v, std::string object_name)
{
	std::stringstream ss;
	ss << static_cast<const void*>(v);
	return object_name + " at " + ss.str();
}

std::string parse_path(std::string path)
{
	std::string registered = "";
	std::string result = "";
	int underscores = 0;
	bool waiting = false;
	for (auto c = path.begin(); c != path.end(); c++) {
		if (*c == '_') {
			underscores++;
			if (waiting) {
				registered.push_back(*c);
			}
			else {
				result.push_back(*c);
			}
			if (underscores == 2) {
				waiting = !waiting;
				underscores = 0;
				if (!waiting) {
					for (int i = 0; i < 2; i++) registered.pop_back();
					auto res = path_templates.find(registered);
					if (res != path_templates.end()) {
						for (int i = 0; i < 2; i++) result.pop_back();
						result += res->second;
					}
					registered = "";
				}
			}
		}
		else {
			underscores = 0;
			if (waiting) {
				registered.push_back(*c);
			}
			else {
				result.push_back(*c);
			}
		}
	}
	return result;
}

void print_dumpable(Dumpable dumping, int indent, std::string key)
{
	if (dumping.index() == 1) {
		auto fixed = std::get<std::pair<std::string, std::string>>(dumping);
		std::cout << std::string(indent, ' ') << fixed.first << " " << key << " = " << fixed.second << ",\n";
	}
	else if (dumping.index() == 0) {
		auto held = std::get<std::pair<std::string, std::map<std::string, Dumpable>>>(dumping);
		std::cout << std::string(indent, ' ') << held.first << " " << key << " = " << "{\n";
		for (const auto& [_key, value] : held.second) {
			print_dumpable(value, indent + 4, _key);
		}
		std::cout << std::string(indent, ' ') << "},\n";
	}
}