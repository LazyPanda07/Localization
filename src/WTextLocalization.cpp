#ifndef __LINUX__

#include "WTextLocalization.h"

using namespace std;

static wstring to_wstring(string_view source);

namespace localization
{
	void BaseTextLocalization<wchar_t>::convertLocalization(const TextLocalization& localizationModule)
	{
		using getDictionariesLanguages = const char** (*)(uint64_t* size);
		using getDictionary = const char* (*)(const char* language, uint64_t* size, const char*** key, const char*** values);
		using freeDictionary = void(*)(const char** keys, const char** values);

		auto load = [this](HMODULE handle, const char* name)
			{
#ifdef __LINUX__
				return dlsym(handle, name);
#else
				return GetProcAddress(handle, name);
#endif
			};

		originalLanguage = localizationModule.getOriginalLanguage();
		language = localizationModule.language;
		pathToModule = localizationModule.getPathToModule();

		getDictionariesLanguages dictionariesLanguagesFunction = reinterpret_cast<getDictionariesLanguages>(load(localizationModule.handle, "getDictionariesLanguages"));
		getDictionary dictionaryFunction = reinterpret_cast<getDictionary>(load(localizationModule.handle, "getDictionary"));
		freeDictionary freeDictionaryFunction = reinterpret_cast<freeDictionary>(load(localizationModule.handle, "freeDictionary"));

		uint64_t languagesSize = 0;
		const char** languages = dictionariesLanguagesFunction(&languagesSize);

		for (uint64_t i = 0; i < languagesSize; i++)
		{
			const char* language = languages[i];
			unordered_map<string, wstring, utility::StringViewHash, utility::StringViewEqual> convertedDictionary;
			uint64_t dictionarySize = 0;
			const char** keys;
			const char** values;

			dictionaryFunction(language, &dictionarySize, &keys, &values);

			convertedDictionary.reserve(dictionarySize);

			for (uint64_t j = 0; j < dictionarySize; j++)
			{
				convertedDictionary[keys[j]] = to_wstring(values[j]);	
			}

			dictionaries[language] = move(convertedDictionary);

			freeDictionaryFunction(keys, values);
		}

		reinterpret_cast<void(*)(const char**)>(load(localizationModule.handle, "freeDictionariesLanguages"))(languages);
	}

	BaseTextLocalization<wchar_t>::BaseTextLocalization(string_view localizationModule) :
		handle(nullptr)
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

	BaseTextLocalization<wchar_t>::BaseTextLocalization(const TextLocalization& localizationModule) :
		handle(nullptr)
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

	void BaseTextLocalization<wchar_t>::changeLanguage(string_view language)
	{
		if (dictionaries.find(language) == dictionaries.end())
		{
			throw runtime_error(format(R"(Wrong language value "{}")", language));
		}

		this->language = language;
	}

	string_view BaseTextLocalization<wchar_t>::getOriginalLanguage() const
	{
		return originalLanguage;
	}

	string_view BaseTextLocalization<wchar_t>::getCurrentLanguage() const
	{
		return language;
	}

	const filesystem::path& BaseTextLocalization<wchar_t>::getPathToModule() const
	{
		return pathToModule;
	}

	wstring_view BaseTextLocalization<wchar_t>::getString(string_view key, string_view language, bool allowOriginal) const
	{
		if (auto languageIterator = dictionaries.find(language); languageIterator != dictionaries.end())
		{
			if (auto keyIterator = languageIterator->second.find(key); keyIterator != languageIterator->second.end())
			{
				if (keyIterator->second.size())
				{
					return keyIterator->second;
				}

				if (allowOriginal)
				{
					return this->getString(key, originalLanguage, false);
				}throw runtime_error(format("Can't find localized string with key: {}", key));

				throw runtime_error(format("Can't find localized string with key: {}", key));
			}
			else if (allowOriginal)
			{
				return this->getString(key, originalLanguage, false);
			}
			else
			{
				throw runtime_error(format("Can't find localized string with key: {}", key));
			}
		}
		else
		{
			throw runtime_error(format("Can't find language: {}", language));
		}

		return {};
	}

	wstring_view BaseTextLocalization<wchar_t>::operator [] (string_view key) const
	{
		return this->getString(key, language);
	}
}

wstring to_wstring(string_view stringToConvert)
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

#endif
