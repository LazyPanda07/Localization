#include "TextLocalization.h"

#include <stdexcept>
#include <format>

using namespace std;

namespace localization
{
	TextLocalization::TextLocalization()
	{
		handle = LoadLibraryA("Localization.dll");

		if (!handle)
		{
			throw runtime_error("Can't find Localization.dll");
		}

		dictionaries = reinterpret_cast<const unordered_map<string, const unordered_map<string, string>*>*>(GetProcAddress(handle, "dictionaries"));

		if (!dictionaries)
		{
			throw runtime_error("Can't find dictionaries in Localization.dll, rebuild and try again");
		}

		originalLanguage = reinterpret_cast<const string*>(GetProcAddress(handle, "originalLanguage"));

		if (!originalLanguage)
		{
			throw runtime_error("Can't find originalLanguage in Localization.dll, rebuild and try again");
		}

		language = *originalLanguage;
	}

	TextLocalization::~TextLocalization()
	{
		FreeLibrary(handle);
	}

	TextLocalization& TextLocalization::get()
	{
		static TextLocalization instance;

		return instance;
	}

	void TextLocalization::changeLanguage(const string& language)
	{
		if (dictionaries->find(language) == dictionaries->end())
		{
			throw runtime_error(format(R"(Wrong language value "{}")"sv, language));
		}

		this->language = language;
	}

	const string& TextLocalization::operator[] (const string& key) const
	{
		try
		{
			const string& result = dictionaries->at(language)->at(key);

			return result.empty() ? dictionaries->at(*originalLanguage)->at(key) : result;
		}
		catch (const out_of_range&)
		{
			try
			{
				return dictionaries->at(*originalLanguage)->at(key);
			}
			catch (const out_of_range&)
			{
				throw runtime_error(format(R"(Can't find key "{}")"sv, key));
			}
		}
	}
}
