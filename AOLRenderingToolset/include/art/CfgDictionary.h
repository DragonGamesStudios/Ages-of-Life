#include "Dictionary.h"

namespace art
{
	class CfgDictionary : public Dictionary
	{
	private:

	public:
		CfgDictionary();

		void load_dictionary_file(const fs::path& path);
	};
}