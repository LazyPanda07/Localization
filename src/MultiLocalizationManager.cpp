#include "MultiLocalizationManager.h"

#include <fstream>
#include <mutex>

#include "LocalizationConstants.h"
#include "JSONArrayWrapper.h"

using namespace std;

namespace localization
{
#ifdef __LINUX__
	MultiLocalizationManager::LocalizationHolder::LocalizationHolder(TextLocalization&& localization) noexcept :
		localization(move(localization))
	{

	}
#else
	MultiLocalizationManager::LocalizationHolder::LocalizationHolder(TextLocalization&& localization, WTextLocalization&& wlocalization) noexcept :
		localization(move(localization)),
		wlocalization(move(wlocalization))
	{

	}
#endif

	MultiLocalizationManager::MultiLocalizationManager()
	{
		if (!filesystem::exists(localizationModulesFile))
		{
			throw runtime_error(format("Can't find {}", localizationModulesFile));
		}

		TextLocalization::get();

#ifndef __LINUX__
		WTextLocalization::get();
#endif

		settings.setJSONData(ifstream(localizationModulesFile.data()));

		defaultModuleName = settings.getString(settings::defaultModuleSetting);

		if (settings.begin() != settings.end())
		{
			vector<string> modules = json::utility::JSONArrayWrapper(settings.getArray(settings::modulesSetting)).getAsStringArray();

			for (const string& module : modules)
			{
				this->addModule(module);
			}
		}
	}

	MultiLocalizationManager::~MultiLocalizationManager()
	{
		for (const auto& [_, localization] : localizations)
		{
			delete localization;
		}

		localizations.clear();
	}

	string MultiLocalizationManager::getVersion()
	{
		string version = "1.1.5";

		return version;
	}

	MultiLocalizationManager& MultiLocalizationManager::getManager()
	{
		static MultiLocalizationManager instance;

		return instance;
	}

	MultiLocalizationManager::LocalizationHolder* MultiLocalizationManager::addModule(const string& localizationModuleName, const filesystem::path& pathToLocalizationModule)
	{
		if (pathToLocalizationModule == defaultModuleName)
		{
			throw runtime_error(format("pathToLocalizationModule can't be {}", defaultModuleName));
		}

		unique_lock<shared_mutex> lock(mapMutex);

		TextLocalization textLocalizationModule(pathToLocalizationModule.string());

#ifndef __LINUX__
		WTextLocalization wtextLocalizationModule(textLocalizationModule);
#endif

		return localizations.try_emplace
		(
			localizationModuleName,
			new LocalizationHolder
			(
				move(textLocalizationModule)
#ifndef __LINUX__
				, move(wtextLocalizationModule)
#endif
			)
		).first->second;
	}

	bool MultiLocalizationManager::removeModule(string_view localizationModuleName)
	{
		unique_lock<shared_mutex> lock(mapMutex);

		auto it = localizations.find(localizationModuleName);

		if (it == localizations.end())
		{
			return false;
		}

		localizations.erase(it);

		delete it->second;

		return true;
	}

	MultiLocalizationManager::LocalizationHolder* MultiLocalizationManager::getModule(string_view localizationModuleName) const
	{
		if (localizationModuleName == defaultModuleName)
		{
			throw runtime_error(format("pathToLocalizationModule can't be {}", defaultModuleName));
		}

		shared_lock<shared_mutex> lock(mapMutex);

		if (auto it = localizations.find(localizationModuleName); it != localizations.end())
		{
			return it->second;
		}

		throw runtime_error(format("Can't find Localizatio holder with module name: {}", localizationModuleName));

		return nullptr;
	}

	string_view MultiLocalizationManager::getLocalizedString(string_view localizationModuleName, string_view key, string_view language) const
	{
		if (localizationModuleName == defaultModuleName)
		{
			return TextLocalization::get().getString(key, language);
		}

		shared_lock<shared_mutex> lock(mapMutex);
		auto it = localizations.find(localizationModuleName);

		if (it == localizations.end())
		{
			throw runtime_error(format("Can't find Localizatio holder with module name: {}", localizationModuleName));
		}

		TextLocalization& text = it->second->localization;

		return language.empty() ? text[key] : text.getString(key, language);
	}

#ifndef __LINUX__
	wstring_view MultiLocalizationManager::getLocalizedWideString(string_view localizationModuleName, string_view key, string_view language) const
	{
		if (localizationModuleName == defaultModuleName)
		{
			return WTextLocalization::get().getString(key, language);
		}

		shared_lock<shared_mutex> lock(mapMutex);
		auto it = localizations.find(localizationModuleName);

		if (it == localizations.end())
		{
			throw runtime_error(format("Can't find Localizatio holder with module name: {}", localizationModuleName));
		}

		WTextLocalization& text = it->second->wlocalization;

		return language.empty() ? text[key] : text.getString(key, language);
	}
#endif
}
