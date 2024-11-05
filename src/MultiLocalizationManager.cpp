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
			throw std::runtime_error(format("Can't find {}", localizationModulesFile));
		}

		TextLocalization::get();

#ifndef __LINUX__
		WTextLocalization::get();
#endif

		settings.setJSONData(ifstream(localizationModulesFile.data()));

		defaultModuleName = settings.getString(settings::defaultModuleSetting);

		if (settings.begin() != settings.end())
		{
			for (const string& module : json::utility::JSONArrayWrapper(settings.getArray(settings::modulesSetting)).getAsStringArray())
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
		string version = "1.1.4";

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

		return localizations.emplace
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

	bool MultiLocalizationManager::removeModule(const string& localizationModuleName)
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

	MultiLocalizationManager::LocalizationHolder* MultiLocalizationManager::getModule(const string& localizationModuleName) const
	{
		if (localizationModuleName == defaultModuleName)
		{
			throw runtime_error(format("pathToLocalizationModule can't be {}", defaultModuleName));
		}

		shared_lock<shared_mutex> lock(mapMutex);

		return localizations.at(localizationModuleName);
	}

	string_view MultiLocalizationManager::getLocalizedString(const string& localizationModuleName, const string& key, const string& language) const
	{
		if (localizationModuleName == defaultModuleName)
		{
			return TextLocalization::get().getString(key, language);
		}

		shared_lock<shared_mutex> lock(mapMutex);

		TextLocalization& text = localizations.at(localizationModuleName)->localization;

		return language.empty() ? text[key] : text.getString(key, language);
	}

#ifndef __LINUX__
	wstring_view MultiLocalizationManager::getLocalizedWideString(const string& localizationModuleName, const string& key, const string& language) const
	{
		if (localizationModuleName == defaultModuleName)
		{
			return WTextLocalization::get().getString(key, language);
		}

		shared_lock<shared_mutex> lock(mapMutex);

		WTextLocalization& text = localizations.at(localizationModuleName)->wlocalization;

		return language.empty() ? text[key] : text.getString(key, language);
	}
#endif
}
