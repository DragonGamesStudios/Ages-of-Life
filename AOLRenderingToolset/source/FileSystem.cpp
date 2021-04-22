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
			current_path = get_correct_path(path);
			return true;
		}
		return false;
	}

	bool FileSystem::enter_dir(const fs::path& path)
	{
		if (exists(path))
		{
			current_path = get_correct_path(path);
			return true;
		}
		return false;
	}

	std::string FileSystem::get_directory() const
	{
		return current_path.filename().string();
	}

	fs::path FileSystem::get_current_path() const
	{
		return current_path;
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

	std::ofstream FileSystem::open_ofile(const std::string& path) const
	{
		return std::ofstream(current_path / path);
	}

	std::ofstream FileSystem::open_ofile(const fs::path& path) const
	{
		return std::ofstream(current_path / path);
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

	bool FileSystem::delete_file(const std::string& filename)
	{
		return fs::remove(filename);
	}

	bool FileSystem::delete_file(const fs::path& filename)
	{
		return fs::remove(filename);
	}

	bool FileSystem::delete_file_if_exists(const std::string& filename)
	{
		if (exists(filename))
			return fs::remove(filename);

		return true;
	}

	bool FileSystem::delete_file_if_exists(const fs::path& filename)
	{
		if (exists(filename))
			return fs::remove(filename);

		return true;
	}

	bool FileSystem::exists(const fs::path& path) const
	{
		return fs::exists(get_correct_path(path));
	}

	bool FileSystem::add_path_template(const std::string& temp, const fs::path& target)
	{
		fs::path correct = target;
		if (!correct.is_absolute()) correct = current_path / correct;

		size_t s = temp.size();
		if (s > 4 && temp[0] == temp[1] && temp[1] == temp[s - 1] && temp[s - 1] == temp[s - 2] && temp[s - 2] == '_')
		{
			path_templates.insert(std::make_pair(temp, correct));
			return true;
		}

		return false;
	}

	bool FileSystem::is_template(const std::string& name)
	{
		size_t s = name.size();
		return (s > 4 && name[0] == name[1] && name[1] == name[s - 1] && name[s - 1] == name[s - 2] && name[s - 2] == '_');
	}

	fs::path FileSystem::get_correct_path(const fs::path& path) const
	{
		if (path.empty())
			return current_path;

		if (path.has_root_path())
			return path;

		std::string root = (*path.begin()).string();

		fs::path rest;
		for (auto it = ++path.begin(); it != path.end(); it++)
			rest /= *it;

		if (is_template(root) && path_templates.find(root) != path_templates.end())
			return path_templates.at(root) / rest;

		return current_path / path;
	}

	fs::directory_iterator FileSystem::get_files_in_directory(const fs::path& dirname)
	{
		return fs::directory_iterator(get_correct_path(dirname));
	}

}