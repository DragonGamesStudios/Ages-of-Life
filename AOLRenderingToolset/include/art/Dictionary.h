#pragma once
#include "FileSystem.h"
#include <agl/builtins/Label.h>

#include <variant>

namespace art
{
	class LocalisedString;


	class LocalisedString : public std::variant<long long, long double, std::string, std::vector<LocalisedString>>
	{
	public:
		LocalisedString(std::initializer_list<LocalisedString> initializers);
		LocalisedString(const std::string& initializers);
		LocalisedString(long long initializers);
		LocalisedString(long double initializers);

		using base = std::variant<long long, long double, std::string, std::vector<LocalisedString>>;
		using base::base;
		using base::operator=;
	};

	typedef std::function<std::vector<fs::path>(const fs::path&, const std::string&)> dict_path_function;

	class Dictionary
	{
	protected:
		std::map<std::string, std::map<std::string, std::string>> dict;
		std::string language;
		FileSystem* filesys;
		std::vector<fs::path> dict_paths;
		dict_path_function dict_func;

		std::set<agl::builtins::Label*> registered_labels;
		std::map<agl::builtins::Label*, LocalisedString> keys;
		std::string get_format_arg(std::string::iterator& c, const std::vector<std::string>& format_args) const;

	public:
		Dictionary();

		void add_locale_path(const fs::path& path);
		void clear_locale_paths();
		void set_dict_path_function(const dict_path_function& dfunc);

		void set_active_language(const std::string& _language);
		std::string get_raw(const std::string& key) const;
		void set_filesystem(FileSystem* fsys);
		std::string format(const LocalisedString& s, bool localised_expected = false) const;
		bool has_key(const std::string& key) const;
		bool has_key(const std::string& group, const std::string& key) const;

		virtual void load_dictionary_file(const fs::path& path) = 0;
		void reload_dictionary();
		void copy_dictionary(Dictionary* dictionary);
		
		void set_label_key(agl::builtins::Label* lbl, const LocalisedString& key);

		const std::map<std::string, std::map<std::string, std::string>>& get_dict() const;
	};
}