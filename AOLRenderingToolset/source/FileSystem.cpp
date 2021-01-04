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

	bool FileSystem::create_dir(const std::string& dirname)
	{
		return fs::create_directory(current_path / dirname);
	}

	bool FileSystem::delete_dir_recursively(const std::string& dirname)
	{
		return fs::remove_all(current_path / dirname) > 0;
	}

	bool FileSystem::enter_dir(const std::string& path)
	{
		if (exists(path))
		{
			current_path /= path;
			return true;
		}
		return false;
	}

	void FileSystem::exit()
	{
		current_path = current_path.parent_path();
	}

	void FileSystem::exit_to(const fs::path& dirname)
	{
		while (
			current_path.filename() != dirname && fs::absolute(current_path) != fs::absolute(dirname)
			&& !current_path.empty()
			)
		{
			this->exit();
		}
	}

	bool FileSystem::create_dir_if_necessary(const std::string& dirname)
	{
		if (!exists(dirname))
			return create_dir(dirname);

		return true;
	}

	std::ifstream FileSystem::open_file(const std::string& path) const
	{
		return std::ifstream(current_path / path);
	}

	std::ifstream FileSystem::open_file(const fs::path& path) const
	{
		return std::ifstream(current_path / path);
	}

	bool FileSystem::create_file(const std::string& filename)
	{
		std::ofstream f(filename);

		bool worked = f.is_open();

		f.close();

		return worked;
	}

	bool FileSystem::create_file_if_necessary(const std::string& filename)
	{
		if (!exists(filename))
			return create_file(filename);

		return true;
	}

	bool FileSystem::exists(fs::path path) const
	{
		return fs::exists(current_path / path);
	}

	fs::directory_iterator FileSystem::get_files_in_directory(const fs::path& dirname)
	{
		return fs::directory_iterator(current_path / dirname);
	}

}