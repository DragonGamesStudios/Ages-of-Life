#pragma once
#include <array>
#include <map>
#include <string>
#include <fstream>

typedef std::array<char, 3> version_t;

const version_t AOL_VERSION = { 0, 0, 5 };

void save_configuration(std::ofstream& of, const std::map<std::string, version_t>& config);

void save_base_data(std::ofstream& of, const std::string& scenario_name, const std::map<std::string, version_t>& mod_config);
void load_base_data(std::ifstream& f, version_t& aol_version, std::string& scenario_name, std::map<std::string, version_t>& mod_config);