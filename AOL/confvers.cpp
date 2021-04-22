#include "confvers.h"

void save_configuration(std::ofstream& of, const std::map<std::string, version_t>& config)
{
	if (!of.is_open())
		return;

	for (const auto& [name, version] : config)
	{
		of << name << (char)1;

		for (int i = 0; i < 3; i++)
			of << version[i];
	}
}

void save_base_data(std::ofstream& of, const std::string& scenario_name, const std::map<std::string, version_t>& mod_config)
{
	for (int i = 0; i < 3; i++)
		of << AOL_VERSION[i];

	of << scenario_name << (char)0;

	save_configuration(of, mod_config);

	of << (char)0;

	// Future: options saving
}

void load_base_data(std::ifstream& f, version_t& aol_version, std::string& scenario_name, std::map<std::string, version_t>& mod_config)
{
	for (int i = 0; i < 3; i++)
		f.read(&aol_version[0], 1);

	char c = 0;
	f.get(c);

	while (c)
	{
		scenario_name.push_back(c);
		f.get(c);
	}

	f.get(c);

	while (c)
	{
		std::string mod_name;
		version_t mod_version;

		while (c != (char)1)
		{
			mod_name.push_back(c);
			f.get(c);
		}

		f.get(c);

		for (int i = 0; i < 3; i++)
		{
			mod_version[i] = c;
			f.get(c);
		}

		mod_config.insert({ mod_name, mod_version });
	}

	// Future: options loading
}
