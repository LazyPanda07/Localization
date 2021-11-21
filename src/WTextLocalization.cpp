#include "WTextLocalization.h"

using namespace std;

wstring to_wstring(const string& source);

namespace localization
{
	BaseTextLocalization<wchar_t>::BaseTextLocalization()
	{
		TextLocalization& instance = TextLocalization::get();

		originalLanguage = instance.getOriginalLanguage();
		language = instance.language;

		for (const auto& [language, dictionary] : (*instance.dictionaries))
		{
			unordered_map<string, wstring> convertedDictionary;

			convertedDictionary.reserve(dictionary->size());

			for (const auto& [key, value] : *dictionary)
			{
				convertedDictionary[key] = to_wstring(value);
			}

			dictionaries[language] = move(convertedDictionary);
		}
	}

	void BaseTextLocalization<wchar_t>::changeLanguage(const string& language)
	{
		this->language = language;
	}

	const string& BaseTextLocalization<wchar_t>::getOriginalLanguage() const
	{
		return originalLanguage;
	}

	const string& BaseTextLocalization<wchar_t>::getCurrentLanguage() const
	{
		return language;
	}

	const wstring& BaseTextLocalization<wchar_t>::operator [] (const string& key) const
	{
		try
		{
			const wstring& result = dictionaries.at(language).at(key);

			return result.empty() ? dictionaries.at(originalLanguage).at(key) : result;
		}
		catch (const out_of_range&)
		{
			try
			{
				return dictionaries.at(originalLanguage).at(key);
			}
			catch (const out_of_range&)
			{
				throw runtime_error(format(R"(Can't find key "{}")"sv, key));
			}
		}
	}
}

wstring to_wstring(const string& stringToConvert)
{
	wstring result;

	int size = MultiByteToWideChar
	(
		CP_UTF8,
		NULL,
		stringToConvert.data(),
		-1,
		nullptr,
		NULL
	);

	if (!size)
	{
		throw runtime_error("Can't convert to wstring");
	}

	result.resize(static_cast<size_t>(size) - 1);

	if (!MultiByteToWideChar
	(
		CP_UTF8,
		NULL,
		stringToConvert.data(),
		-1,
		result.data(),
		size
	))
	{
		throw runtime_error("Can't convert to wstring");
	}

	return result;
}
