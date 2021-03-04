#include "..\include\art\Dictionary.h"

#include <agl/events.h>

#include <sstream>

#define LOCALISED_LL 0
#define LOCALISED_LD 1
#define LOCALISED_STR 2
#define	LOCALISED_VEC 3

namespace art
{

	std::string Dictionary::get_format_arg(std::string::iterator& c, const std::vector<std::string>& format_args) const
	{
		std::string passed;
		std::string number_str;
		int number_int = 0;
		int underscores = 0;
		bool read_number = false;

		while (number_str.empty() && underscores != 2)
		{
			passed.push_back(*c);

			if (!read_number)
			{
				if (*c == '_')
					underscores++;
				else
					return passed;

				if (underscores == 2)
				{
					read_number = true;
				}
			}
			else
			{
				underscores = 0;

				if (*c >= '0' && *c <= '9')
					number_str.push_back(*c);
				else if (*c == '_')
				{
					read_number = false;
					continue;
				}
				else
					return passed;

			}
			c++;
		}

		number_int = std::stoi(number_str);

		if (number_int < 0 || number_int >= format_args.size())
			return "Undefined argument: __" + number_str + "__";

		return format_args.at(number_int);
	}

	Dictionary::Dictionary()
	{
		filesys = 0;
		dict.insert({ "", {} });
		dict_func = [](const fs::path& d_path, const std::string& language) -> std::vector<fs::path> {
			return { d_path / language };
		};
	}

	void Dictionary::add_locale_path(const fs::path& path)
	{
		dict_paths.push_back(path);
	}

	void Dictionary::clear_locale_paths()
	{
		dict_paths.clear();
	}

	void Dictionary::set_dict_path_function(const dict_path_function& dfunc)
	{
		dict_func = dfunc;
	}

	void Dictionary::set_active_language(const std::string& _language)
	{
		language = _language;
		dict.clear();
		if (filesys)
		{
			reload_dictionary();
		}

		for (auto& lbl : registered_labels)
		{
			lbl->set_text(format(keys[lbl]));
		}
	}

	std::string Dictionary::get_raw(const std::string& key) const
	{
		std::string group;
		std::string value;

		std::string::const_iterator c = key.begin();

		while (c != key.end() && *c != '.')
		{
			group.push_back(*c);
			c++;
		}

		if (c != key.end())
			c++;

		for (; c != key.end(); c++)
		{
			value.push_back(*c);
		}

		if (!group.empty())
		{
			auto group_it = dict.find(group);

			if (group_it != dict.end())
			{
				auto value_it = group_it->second.find(value);

				if (value_it != group_it->second.end())
					return value_it->second;
			}
		}
		else
		{
			auto& group_dict = dict.at("");
			auto value_it = group_dict.find(value);

			if (value_it != group_dict.end())
				return value_it->second;
		}

		return "Translation for key \"" + key + "\" not found.";
	}

	void Dictionary::set_filesystem(FileSystem* fsys)
	{
		filesys = fsys;
	}

	std::string Dictionary::format(const LocalisedString& s, bool localised_expected) const
	{
		std::stringstream formatted;

		if (localised_expected && s.index() != LOCALISED_STR)
			throw std::runtime_error("Incorrect localised string. First element of vector should be a string.");
		
		switch (s.index())
		{
		case LOCALISED_LL:
			formatted << std::get<long long>(s);
			break;

		case LOCALISED_LD:
			formatted << std::get<long double>(s);
			break;

		case LOCALISED_STR:
			if (localised_expected && !std::get<std::string>(s).empty())
				formatted << get_raw(std::get<std::string>(s));
			else
				formatted << std::get<std::string>(s);

			break;

		case LOCALISED_VEC:
			std::vector<std::string> to_format;

			bool expect_string = true;

			for (const auto& arg : std::get<std::vector<LocalisedString>>(s))
			{
				to_format.push_back(format(arg, expect_string));

				expect_string = false;
			}

			// Compose result

			if (to_format[0].empty())
			{
				auto f_it = to_format.begin();
				std::advance(f_it, 1);

				for (; f_it != to_format.end(); f_it++)
					formatted << *f_it;
			}
			else
			{
				std::string::iterator c = to_format[0].begin();
				bool ignore_next = false;

				while (c != to_format[0].end())
				{
					if (!ignore_next)
					{
						if (*c == '\\')
							ignore_next = true;
						else if (*c == '_')
						{
							formatted << get_format_arg(c, to_format);
							c--;
						}
						else
							formatted << *c;
					}
					else
					{
						if (*c == 'n')
							formatted << '\n';
						else
							formatted << *c;
					}

					c++;
				}
			}
		}

		return formatted.str();
	}

	void Dictionary::reload_dictionary()
	{
		dict.clear();
		dict.insert({ "", {} });

		for (const auto& d_path : dict_paths)
		{
			for (const auto& f_path : dict_func(d_path, language))
			{
				load_dictionary_file(f_path);
			}
		}
	}

	void Dictionary::set_label_key(agl::builtins::Label* lbl, const LocalisedString& key)
	{
		registered_labels.insert(lbl);

		keys[lbl] = key;

		lbl->set_text(format(key), false);
	}

	LocalisedString::LocalisedString(std::initializer_list<LocalisedString> initializers)
	{
		*this = std::vector<LocalisedString>(initializers);
	}

	LocalisedString::LocalisedString(const std::string& initializers)
	{
		*this = initializers;
	}

	LocalisedString::LocalisedString(long long initializers)
	{
		*this = initializers;
	}

	LocalisedString::LocalisedString(long double initializers)
	{
		*this = initializers;
	}

}