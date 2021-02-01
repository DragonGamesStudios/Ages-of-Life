#pragma once
#include <memory>
#include <string>
#include <functional>

class GameObject
{
private:
	std::string name;

public:
	static const std::uint_fast64_t class_id = 0;
};