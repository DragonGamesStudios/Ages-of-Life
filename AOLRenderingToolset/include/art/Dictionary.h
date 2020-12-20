#pragma once
#include "nlohmann/json.hpp"
#include "FileSystem.h"
#include <agl/builtins/Label.h>

namespace art
{
	using json = nlohmann::json;

	class Dictionary
	{
	private:
		json dict;
		std::vector<std::string> format_args;
		fs::path dict_path;
		std::string language;
		FileSystem* filesys;

		std::set<agl::builtins::Label*> registered_labels;
		std::map<agl::builtins::Label*, std::pair<std::string, std::vector<std::string>>> keys;
		void on_new_text_translate(agl::Event e);

	public:
		Dictionary();

		void set_translation_dir_path(std::string path);
		void set_active_language(std::string _language);
		std::string get(std::string key);
		void set_filesystem(FileSystem* fsys);
		void to_string(int val);
		void to_string(double val, int precision);
		void to_string(float val, int precision);
		std::string format(std::string original, std::vector<std::string>& args);
		std::string format(std::string original, std::map<std::string, std::string>& args);
		
		void register_label(agl::builtins::Label* lbl);
		void set_format_args(std::vector<std::string> args);
	};
}