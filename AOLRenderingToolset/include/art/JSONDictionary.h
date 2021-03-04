#include "Dictionary.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace art
{
	class JSONDictionary : public Dictionary
	{
	private:

	public:
		JSONDictionary();

		void load_dictionary_file(const fs::path& path);
	};
}