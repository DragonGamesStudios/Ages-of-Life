#include "independent-globals.h"
#include <sstream>

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
