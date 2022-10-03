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
		static MultiLocalizationManager& getManager();

		const LocalizationHolder* addModule(const std::filesystem::path& pathToLocalizationModule);

		bool removeModule(const std::filesystem::path& pathToLocalizationModule);

		const LocalizationHolder* getModule(const std::filesystem::path& pathToLocalizationModule) const;

		const std::string& getLocalizedString(const std::filesystem::path& pathToLocalizationModule, const std::string& key) const;

		const std::wstring& getLocalizedWideString(const std::filesystem::path& pathToLocalizationModule, const std::string& key) const;
	};
}
