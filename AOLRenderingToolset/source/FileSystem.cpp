#include "..\include\art\FileSystem.h"

namespace art
{
	art::FileSystem::FileSystem(bool appdata)
	{
		if (appdata)
		{
			char* appbuff;
			size_t len;
			_dupenv_s(&appbuff, &len, "APPDATA");

			current_path = appbuff;
		}
		else
		{
			current_path = fs::current_path();
		}
	}

	std::ifstream FileSystem::open_file(std::string path)
	{
		return std::ifstream(current_path / path);
	}

	std::ifstream FileSystem::open_file(fs::path path)
	{
		return std::ifstream(current_path / path);
	}

}