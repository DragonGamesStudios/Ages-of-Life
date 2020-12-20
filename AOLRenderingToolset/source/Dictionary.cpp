#include "..\include\art\Dictionary.h"
#include <fmt/format.h>

#include <agl/events.h>

namespace art
{
	void Dictionary::on_new_text_translate(agl::Event e)
	{
		if (
			registered_labels.find((agl::builtins::Label*)e.source) == registered_labels.end() &&
			e.type == AGL_EVENT_TEXT_CHANGED
			)
		{
			auto lbl = (agl::builtins::Label*) e.source;
			keys[lbl] = { lbl->get_text(), format_args };
			lbl->set_text(get(lbl->get_text()), false);
		}
	}

	Dictionary::Dictionary()
	{
		filesys = 0;
	}

	void Dictionary::set_translation_dir_path(std::string path)
	{
		dict_path = path;
	}

	void Dictionary::set_active_language(std::string _language)
	{
		language = _language;
		dict.clear();
		if (filesys)
		{
			filesys->open_file(dict_path / (language + ".json")) >> dict;
		}

		for (auto& lbl : registered_labels)
		{
			set_format_args(keys[lbl].second);
			lbl->set_text(keys[lbl].first, false);
		}
	}

	std::string Dictionary::get(std::string key)
	{
		// TODO: add support for both arg formats: vector and map
		auto it = dict.find(key);
		if (it != dict.end())
			return format(*it, format_args);

		return "Translation for key \"" + key + "\" not found.";
	}

	void Dictionary::set_filesystem(FileSystem* fsys)
	{
		filesys = fsys;
	}

	std::string Dictionary::format(std::string original, std::vector<std::string>& args)
	{
		std::string formatted;
		bool key_listening = false;
		bool ignore_next = false;
		std::string key;

		for (const char c : original)
		{
			if (c == '/')
				ignore_next = true;
			else if (!ignore_next && c == '{')
				key_listening = true;
			else if (!ignore_next && c == '}')
			{
				key_listening = false;
				int index = std::stoi(key);

				if (index > 0 && index < args.size())
					formatted += args[index];
				else
					formatted += "Arg not found at index " + key;
				key.clear();
			}
			else
			{
				if (!key_listening)
					formatted.push_back(c);
				else
					key.push_back(c);
			}
		}

		return formatted;
	}

	std::string Dictionary::format(std::string original, std::map<std::string, std::string>& args)
	{
		std::string formatted;
		bool key_listening = false;
		bool ignore_next = false;
		std::string key;

		for (const char c : original)
		{
			if (c == '/')
				ignore_next = true;
			else if (!ignore_next && c == '{')
				key_listening = true;
			else if (!ignore_next && c == '}')
			{
				key_listening = false;
				auto it = args.find(key);
				if (it != args.end())
					formatted += args[key];
				else
					formatted += "Arg not found at key " + key;
				key.clear();
			}
			else
			{
				if (!key_listening)
					key.push_back(c);
				else
					formatted.push_back(c);
			}
		}

		return formatted;
	}

	void Dictionary::register_label(agl::builtins::Label* lbl)
	{
		registered_labels.insert(lbl);

		std::string lt = lbl->get_text();

		keys.insert({ lbl, {lt, { format_args }} });

		if (!lt.empty())
			lbl->set_text(get(lt), false);
	}

	void Dictionary::set_format_args(std::vector<std::string> args)
	{
		format_args = args;
	}

}