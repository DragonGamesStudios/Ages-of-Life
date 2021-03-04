#include "..\include\art\JSONDictionary.h"

namespace art
{
	JSONDictionary::JSONDictionary() : Dictionary()
	{
		dict_func = [](const fs::path& d_path, const std::string& language) -> std::vector<fs::path> {
			return { d_path / (language + ".json") };
		};
	}

	void JSONDictionary::load_dictionary_file(const fs::path& path)
	{
		json buff;
		filesys->open_file(path) >> buff;

		for (const auto& item : buff.items())
		{
			switch (item.value().type())
			{
			case json::value_t::string:
				dict.at("").insert({ item.key(), item.value() });

				break;

			case json::value_t::object:
				auto it = dict.find(item.key());

				if (it == dict.end())
					dict.insert({ item.key(), {} });
				
				auto insert_into = dict.at(item.key());

				for (const auto& keyval : item.value().items())
				{
					if (keyval.value().type() == json::value_t::string)
						insert_into.insert({ keyval.key(), keyval.value() });
				}

				break;
			}
		}
	}
}