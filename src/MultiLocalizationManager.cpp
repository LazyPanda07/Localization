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

	MultiLocalizationManager::LocalizationHolder* MultiLocalizationManager::addModule(const filesystem::path& pathToLocalizationModule)
	{
		if (pathToLocalizationModule == settings.getString(settings::defaultModuleSetting))
		{
			throw runtime_error(format("pathToLocalizationModule can't be {}"sv, settings.getString(settings::defaultModuleSetting)));
		}

		TextLocalization textLocalizationModule(pathToLocalizationModule.string());
		WTextLocalization wtextLocalizationModule(textLocalizationModule);

		return localizations.emplace(pathToLocalizationModule.string(), new LocalizationHolder(move(textLocalizationModule), move(wtextLocalizationModule))).first->second;
	}

	bool MultiLocalizationManager::removeModule(const filesystem::path& pathToLocalizationModule)
	{
		auto it = localizations.find(pathToLocalizationModule.string());

		if (it == localizations.end())
		{
			return false;
		}

		localizations.erase(it);

		delete it->second;

		return true;
	}

	MultiLocalizationManager::LocalizationHolder* MultiLocalizationManager::getModule(const filesystem::path& pathToLocalizationModule) const
	{
		if (pathToLocalizationModule == settings.getString(settings::defaultModuleSetting))
		{
			throw runtime_error(format("pathToLocalizationModule can't be {}"sv, settings.getString(settings::defaultModuleSetting)));
		}

		return localizations.at(pathToLocalizationModule.string());
	}

	const string& MultiLocalizationManager::getLocalizedString(const filesystem::path& pathToLocalizationModule, const string& key) const
	{
		if (pathToLocalizationModule == settings.getString(settings::defaultModuleSetting))
		{
			return TextLocalization::get()[key];
		}

		return localizations.at(pathToLocalizationModule.string())->localization[key];
	}

	const wstring& MultiLocalizationManager::getLocalizedWideString(const filesystem::path& pathToLocalizationModule, const string& key) const
	{
		if (pathToLocalizationModule == settings.getString(settings::defaultModuleSetting))
		{
			return WTextLocalization::get()[key];
		}

		return localizations.at(pathToLocalizationModule.string())->wlocalization[key];
	}
}
