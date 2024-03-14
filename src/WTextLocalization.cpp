#include "WTextLocalization.h"

using namespace std;

static wstring to_wstring(const string& source);

namespace localization
{
	void BaseTextLocalization<wchar_t>::convertLocalization(const TextLocalization& localizationModule)
	{
		originalLanguage = localizationModule.getOriginalLanguage();
		language = localizationModule.language;

		for (const auto& [language, dictionary] : (*localizationModule.dictionaries))
		{
			unordered_map<string, wstring> convertedDictionary;

			convertedDictionary.reserve(dictionary->size());

			for (const auto& [key, value] : *dictionary)
			{
				convertedDictionary[key] = to_wstring(value);
			}

			dictionaries[language] = move(convertedDictionary);
		}

		pathToModule = localizationModule.getPathToModule();
	}

	BaseTextLocalization<wchar_t>::BaseTextLocalization(const string& localizationModule)
	{
		if (localizationModule == TextLocalization::get().getPathToModule())
		{
			this->convertLocalization(TextLocalization::get());
		}
		else
		{
			this->convertLocalization(TextLocalization(localizationModule));
		}
	}

	BaseTextLocalization<wchar_t>::BaseTextLocalization(const TextLocalization& localizationModule)
	{
		this->convertLocalization(localizationModule);
	}

	BaseTextLocalization<wchar_t>::BaseTextLocalization(WTextLocalization&& other) noexcept
	{
		(*this) = move(other);
	}

	WTextLocalization& BaseTextLocalization<wchar_t>::operator = (WTextLocalization&& other) noexcept
	{
		dictionaries = move(other.dictionaries);
		originalLanguage = move(other.originalLanguage);
		language = move(other.language);
		pathToModule = move(other.pathToModule);

		return *this;
	}

	BaseTextLocalization<wchar_t>& BaseTextLocalization<wchar_t>::get()
	{
		static unique_ptr<BaseTextLocalization<wchar_t>> instance;

		if (!instance)
		{
			json::JSONParser settings(ifstream(localizationModulesFile.data()));

			instance = unique_ptr<WTextLocalization>(new WTextLocalization(settings.getString(settings::defaultModuleSetting)));
		}

		return *instance;
	}

	void BaseTextLocalization<wchar_t>::changeLanguage(const string& language)
	{
		if (dictionaries.find(language) == dictionaries.end())
		{
			throw runtime_error(format(R"(Wrong language value "{}")"sv, language));
		}

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

	const string& BaseTextLocalization<wchar_t>::getPathToModule() const
	{
		return pathToModule;
	}

	const wstring& BaseTextLocalization<wchar_t>::getString(const string& key, const string& language) const
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

	const wstring& BaseTextLocalization<wchar_t>::operator [] (const string& key) const
	{
		return this->getString(key, language);
	}
}

wstring to_wstring(const string& stringToConvert)
{
	wstring result;

#ifdef __LINUX__

#else
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
#endif

	return result;
}
