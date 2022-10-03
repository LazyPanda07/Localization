#pragma once

#include <unordered_map>
#include <filesystem>

#include "TextLocalization.h"
#include "WTextLocalization.h"

namespace localization
{
	/// @brief Manage multi localization modules and multi localization itself
	class MultiLocalizationManager
	{
	public:
		struct LocalizationHolder
		{
		public:
			TextLocalization localization;
			WTextLocalization wlocalization;

		public:
			LocalizationHolder(TextLocalization&& localization, WTextLocalization&& wlocalization) noexcept;

			LocalizationHolder(const LocalizationHolder&) = delete;

			LocalizationHolder& operator = (const LocalizationHolder&) = delete;

			LocalizationHolder(LocalizationHolder&& other) noexcept = default;

			LocalizationHolder& operator = (LocalizationHolder&& other) noexcept = default;
		};

	private:
		std::unordered_map<std::string, LocalizationHolder*> localizations;

	private:
		MultiLocalizationManager();

		MultiLocalizationManager(const MultiLocalizationManager&) = delete;

		MultiLocalizationManager(MultiLocalizationManager&&) noexcept = delete;

		MultiLocalizationManager& operator = (const MultiLocalizationManager&) = delete;

		MultiLocalizationManager& operator = (MultiLocalizationManager&&) noexcept = delete;

		~MultiLocalizationManager();

	public:
		/// @brief Singleton instance
		/// @return MultiLocalizationManager
		/// @exception json::exceptions::CantFindValueException 
		/// @exception std::bad_variant_access Other type found
		static MultiLocalizationManager& getManager();

		/// @brief Add additional localization module
		/// @param pathToLocalizationModule Path to localization module
		/// @return Pointer to MultiLocalizationManager::LocalizationHolder 
		/// @exception std::runtime_error
		const LocalizationHolder* addModule(const std::filesystem::path& pathToLocalizationModule);

		/// @brief Remove localization module
		/// @param pathToLocalizationModule Path to localization module
		/// @return Module was successfully removed
		bool removeModule(const std::filesystem::path& pathToLocalizationModule);

		/// @brief Get pointer to MultiLocalizationManager::LocalizationHolder 
		/// @param pathToLocalizationModule Pat to localization module
		/// @return Pointer to MultiLocalizationManager::LocalizationHolder 
		/// @exception std::runtime_error
		const LocalizationHolder* getModule(const std::filesystem::path& pathToLocalizationModule) const;

		/// @brief Get localized text
		/// @param key Localization key
		/// @return Localized value
		/// @exception std::runtime_error Wrong key 
		const std::string& getLocalizedString(const std::filesystem::path& pathToLocalizationModule, const std::string& key) const;

		/// @brief Get localized text
		/// @param key Localization key
		/// @return Localized value
		/// @exception std::runtime_error Wrong key
		const std::wstring& getLocalizedWideString(const std::filesystem::path& pathToLocalizationModule, const std::string& key) const;
	};
}
