#pragma once
#include <string>

std::string get_string_representation(int v);
std::string get_string_representation(std::string v);
std::string get_string_representation(bool v);
std::string get_string_representation(float v);
std::string get_string_representation(void* v);
std::string get_string_representation(void* v, std::string object_name);