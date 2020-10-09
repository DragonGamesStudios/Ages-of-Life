#pragma once
#include <string>
#include <variant>
#include <map>
#include <functional>

std::string get_string_representation(int v);
std::string get_string_representation(std::string v);
std::string get_string_representation(bool v);
std::string get_string_representation(float v);
std::string get_string_representation(void* v);
std::string get_string_representation(void* v, std::string object_name);

extern std::map<std::string, std::string> path_templates;

std::string parse_path(std::string path);

class Dumpable;

class Dumpable : public std::variant<std::pair<std::string, std::map<std::string, Dumpable>>, std::pair<std::string, std::string>> {
public:
	using base = std::variant<std::pair<std::string, std::map<std::string, Dumpable>>, std::pair<std::string, std::string>>;
	using base::base;
	using base::operator=;
};

template<typename _T>
Dumpable vector_to_dumpable(std::vector<_T> vec, std::function<std::string(_T)> parser, std::string type="notype")
{
	std::map<std::string, Dumpable> buff;

	int i = 0;
	for (auto elem = vec.begin(); elem != vec.end(); elem++) {
		buff.insert({ get_string_representation(i), std::make_pair(type, parser(*elem)) });
		i++;
	}

	return std::make_pair("std::vector<" + type + ">", buff);
}

void print_dumpable(Dumpable dumping, int indent = 0, std::string key = "dumping");