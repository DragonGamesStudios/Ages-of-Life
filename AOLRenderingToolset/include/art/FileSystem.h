#pragma once

#include <filesystem>
#include <fstream>

namespace art
{
	namespace fs = std::filesystem;

	class FileSystem
	{
	private:
		fs::path current_path;

	public:
		FileSystem(bool appdata = false);

		bool create_dir(std::string dirname);
		bool enter_dir(std::string path);
		std::ifstream open_file(std::string path);
		std::ifstream open_file(fs::path path);
	};
}