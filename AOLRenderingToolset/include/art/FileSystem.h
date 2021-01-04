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

		bool create_dir(const std::string& dirname);
		bool delete_dir_recursively(const std::string& dirname);
		bool enter_dir(const std::string& path);

		void exit();
		void exit_to(const fs::path& dirname);

		bool create_dir_if_necessary(const std::string& dirname);

		std::ifstream open_file(const std::string& path) const;
		std::ifstream open_file(const fs::path& path) const;

		bool create_file(const std::string& filename);
		bool create_file_if_necessary(const std::string& filename);

		bool exists(fs::path path) const;

		fs::directory_iterator get_files_in_directory(const fs::path& dirname);
	};
}