#include "..\include\art\CfgDictionary.h"

namespace art
{
	CfgDictionary::CfgDictionary() : Dictionary()
	{
		dict_func = [](const fs::path& d_path, const std::string& language) -> std::vector<fs::path> {
			return { d_path / (language + ".cfg") };
		};
	}

	void CfgDictionary::load_dictionary_file(const fs::path& path)
	{
		std::string active_group;
		std::string key;
		std::string value;

		auto f = filesys->open_file(path);

		std::string line;

		auto insert_into = dict.find("");

		bool reading_group = false, reading_key = false, reading_value = false;

		while (std::getline(f, line))
		{
			for (const char c : line)
			{
				if (!reading_value)
				{
					if (c == '\r' || c == '\t' || c == ' ')
						continue;

					if (!(reading_group || reading_key))
					{
						if (c == '[')
							reading_group = true;
						else
						{
							key.push_back(c);
							reading_key = true;
						}
					}
					else if (reading_group)
					{
						if (c != ']')
							active_group.push_back(c);
						else
						{
							reading_group = false;
							insert_into = dict.insert({ active_group, {} }).first;
							active_group.clear();
						}
					}
					else
					{
						if (c != '=')
							key.push_back(c);
						else
						{
							reading_value = true;
							reading_key = false;
						}
					}
				}
				else
				{
					value.push_back(c);
				}
			}

			reading_group = reading_key = reading_value = false;

			if (!(key.empty() || value.empty()))
			{
				insert_into->second.insert({ key, value });
			}

			key.clear();
			value.clear();
			active_group.clear();
		}
	}

}