#include "MultiLocalizationManager.h"

#include <fstream>
#include <mutex>

#include <JsonArrayWrapper.h>

#include "LocalizationConstants.h"

namespace localization
{
#ifdef __LINUX__
	MultiLocalizationManager::LocalizationHolder::LocalizationHolder(TextLocalization&& localization) noexcept :
		localization(std::move(localization))
	{

	}
#else
	MultiLocalizationManager::LocalizationHolder::LocalizationHolder(TextLocalization&& localization, WTextLocalization&& wlocalization) noexcept :
		localization(std::move(localization)),
		wlocalization(std::move(wlocalization))
	{

	}
#endif

	MultiLocalizationManager::MultiLocalizationManager()
	{
		if (!std::filesystem::exists(localizationModulesFile))
		{
			throw std::runtime_error(std::format("Can't find {}", localizationModulesFile));
		}

		TextLocalization::get();

#ifndef __LINUX__
		WTextLocalization::get();
#endif

		settings.setJSONData(std::ifstream(localizationModulesFile.data()));

		defaultModuleName = settings.get<std::string>(settings::defaultModuleSetting);

		if (settings.begin() != settings.end())
		{
			std::vector<std::string> modules = json::utility::JsonArrayWrapper(settings.get<std::vector<json::JsonObject>>(settings::modulesSetting)).as<std::string>();

			for (const std::string& module : modules)
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

	std::string MultiLocalizationManager::getVersion()
	{
		std::string version = "1.4.2";

		return version;
	}

	MultiLocalizationManager& MultiLocalizationManager::getManager()
	{
		static MultiLocalizationManager instance;

		return instance;
	}

	MultiLocalizationManager::LocalizationHolder* MultiLocalizationManager::addModule(const std::string& localizationModuleName, const std::filesystem::path& pathToLocalizationModule)
	{
		if (pathToLocalizationModule == defaultModuleName)
		{
			throw std::runtime_error(format("pathToLocalizationModule can't be {}", defaultModuleName));
		}

		std::lock_guard<std::shared_mutex> lock(mapMutex);

		TextLocalization textLocalizationModule(pathToLocalizationModule.string());

#ifndef __LINUX__
		WTextLocalization wtextLocalizationModule(textLocalizationModule);
#endif

		return localizations.try_emplace
		(
			localizationModuleName,
			new LocalizationHolder
			(
				std::move(textLocalizationModule)
#ifndef __LINUX__
				, std::move(wtextLocalizationModule)
#endif
			)
		).first->second;
	}

	bool MultiLocalizationManager::removeModule(std::string_view localizationModuleName)
	{
		std::lock_guard<std::shared_mutex> lock(mapMutex);

		auto it = localizations.find(localizationModuleName);

		if (it == localizations.end())
		{
			return false;
		}

		localizations.erase(it);

		delete it->second;

		return true;
	}

	MultiLocalizationManager::LocalizationHolder* MultiLocalizationManager::getModule(std::string_view localizationModuleName) const
	{
		if (localizationModuleName == defaultModuleName)
		{
			throw std::runtime_error(std::format("pathToLocalizationModule can't be {}", defaultModuleName));
		}

		std::shared_lock<std::shared_mutex> lock(mapMutex);

		if (auto it = localizations.find(localizationModuleName); it != localizations.end())
		{
			return it->second;
		}

		throw std::runtime_error(format("Can't find Localization holder with module name: {}", localizationModuleName));

		return nullptr;
	}

	std::string_view MultiLocalizationManager::getLocalizedString(std::string_view localizationModuleName, std::string_view key, std::string_view language) const
	{
		if (localizationModuleName == defaultModuleName)
		{
			return TextLocalization::get().getString(key, language);
		}

		std::shared_lock<std::shared_mutex> lock(mapMutex);
		auto it = localizations.find(localizationModuleName);

		if (it == localizations.end())
		{
			throw std::runtime_error(std::format("Can't find Localization holder with module name: {}", localizationModuleName));
		}

		TextLocalization& text = it->second->localization;

		return language.empty() ? text[key] : text.getString(key, language);
	}

#ifndef __LINUX__
	std::wstring_view MultiLocalizationManager::getLocalizedWideString(std::string_view localizationModuleName, std::string_view key, std::string_view language) const
	{
		if (localizationModuleName == defaultModuleName)
		{
			return WTextLocalization::get().getString(key, language);
		}

		std::shared_lock<std::shared_mutex> lock(mapMutex);
		auto it = localizations.find(localizationModuleName);

		if (it == localizations.end())
		{
			throw std::runtime_error(std::format("Can't find Localization holder with module name: {}", localizationModuleName));
		}

		WTextLocalization& text = it->second->wlocalization;

		return language.empty() ? text[key] : text.getString(key, language);
	}
#endif
}
