#pragma once

#include <filesystem>
#include <fstream>
#include <map>

namespace art
{
	namespace fs = std::filesystem;

	class FileSystem
	{
	private:
		fs::path current_path;
		std::map<std::string, fs::path> path_templates;

	public:
		FileSystem(bool appdata = false);

		bool create_dir(const std::string& dirname);
		bool delete_dir_recursively(const std::string& dirname);
		bool enter_dir(const std::string& path);
		bool enter_dir(const fs::path& path);
		std::string get_directory() const;
		fs::path get_current_path() const;

		void exit();
		void exit_to(const fs::path& dirname);

		bool create_dir_if_necessary(const std::string& dirname);

		std::ifstream open_file(const std::string& path) const;
		std::ifstream open_file(const fs::path& path) const;

		std::ofstream open_ofile(const std::string& path) const;
		std::ofstream open_ofile(const fs::path& path) const;

		bool create_file(const std::string& filename);
		bool create_file_if_necessary(const std::string& filename);

		bool delete_file(const std::string& filename);
		bool delete_file(const fs::path& filename);
		bool delete_file_if_exists(const std::string& filename);
		bool delete_file_if_exists(const fs::path& filename);

		bool exists(const fs::path& path) const;

		bool add_path_template(const std::string& temp, const fs::path& target);
		static bool is_template(const std::string& name);

		fs::path get_correct_path(const fs::path& path) const;

		fs::directory_iterator get_files_in_directory(const fs::path& dirname);
	};
}