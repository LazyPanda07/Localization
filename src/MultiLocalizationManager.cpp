#include "MultiLocalizationManager.h"

#include <fstream>

#include "LocalizationConstants.h"
#include "JSONArrayWrapper.h"

using namespace std;

namespace localization
{
	MultiLocalizationManager::LocalizationHolder::LocalizationHolder(TextLocalization&& localization, WTextLocalization&& wlocalization) noexcept :
		localization(move(localization)),
		wlocalization(move(wlocalization))
	{

	}

	MultiLocalizationManager::MultiLocalizationManager()
	{
		if (!filesystem::exists(localizationModulesFile))
		{
			throw std::runtime_error(format("Can't find {}"sv, localizationModulesFile));
		}

		TextLocalization::get();
		WTextLocalization::get();

		settings.setJSONData(ifstream(localizationModulesFile.data()));

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

	MultiLocalizationManager& MultiLocalizationManager::getManager()
	{
		static MultiLocalizationManager instance;

		return instance;
	}

	MultiLocalizationManager::LocalizationHolder* MultiLocalizationManager::addModule(const string& localizationModuleName, const filesystem::path& pathToLocalizationModule)
	{
		if (pathToLocalizationModule == settings.getString(settings::defaultModuleSetting))
		{
			throw runtime_error(format("pathToLocalizationModule can't be {}"sv, settings.getString(settings::defaultModuleSetting)));
		}

		unique_lock<mutex> lock(mapMutex);

		TextLocalization textLocalizationModule(pathToLocalizationModule.string());
		WTextLocalization wtextLocalizationModule(textLocalizationModule);

		return localizations.emplace(localizationModuleName, new LocalizationHolder(move(textLocalizationModule), move(wtextLocalizationModule))).first->second;
	}

	bool MultiLocalizationManager::removeModule(const string& localizationModuleName)
	{
		unique_lock<mutex> lock(mapMutex);

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
		if (localizationModuleName == settings.getString(settings::defaultModuleSetting))
		{
			throw runtime_error(format("pathToLocalizationModule can't be {}"sv, settings.getString(settings::defaultModuleSetting)));
		}

		unique_lock<mutex> lock(mapMutex);

		return localizations.at(localizationModuleName);
	}

	const string& MultiLocalizationManager::getLocalizedString(const string& localizationModuleName, const string& key, const string& language) const
	{
		if (localizationModuleName == settings.getString(settings::defaultModuleSetting))
		{
			return TextLocalization::get()[key];
		}

		unique_lock<mutex> lock(mapMutex);

		TextLocalization& text = localizations.at(localizationModuleName)->localization;

		return language.empty() ? text[key] : text.getString(key, language);
	}

	const wstring& MultiLocalizationManager::getLocalizedWideString(const filesystem::path& pathToLocalizationModule, const string& key, const string& language) const
	{
		if (pathToLocalizationModule == settings.getString(settings::defaultModuleSetting))
		{
			return WTextLocalization::get()[key];
		}

		unique_lock<mutex> lock(mapMutex);

		WTextLocalization& text = localizations.at(pathToLocalizationModule.string())->wlocalization;

		return language.empty() ? text[key] : text.getString(key, language);
	}
}
