#pragma once

/// @file WTextLocalization.h
/// @brief BaseTextLocalization specialization with std::wstring

#ifndef __LINUX__

#include "TextLocalization.h"

namespace localization
{
	/// @brief TextLocalization with std::wstring
	using WTextLocalization = localization::BaseTextLocalization<wchar_t>;

	/// @brief TextLocalization specialization with wchar_t
	template<>
	class LOCALIZATION_API BaseTextLocalization<wchar_t> final
	{
	private:
		std::unordered_map<std::string, std::unordered_map<std::string, std::wstring>> dictionaries;
		std::string originalLanguage;
		std::string language;
		std::filesystem::path pathToModule;
		HMODULE handle;

	private:
		void convertLocalization(const TextLocalization& localizationModule);

	private:
		BaseTextLocalization(const std::string& localizationModule);

		BaseTextLocalization(const TextLocalization& localizationModule);

		BaseTextLocalization(const WTextLocalization&) = delete;

		BaseTextLocalization(WTextLocalization&& other) noexcept;

		WTextLocalization& operator = (const WTextLocalization&) = delete;

		WTextLocalization& operator = (WTextLocalization&& other) noexcept;

		~BaseTextLocalization() = default;

	public:
		/// @brief Exception can be thrown on first call
		/// @return Singleton instance
		/// @exception std::runtime_error Can't find Localization.dll or something inside Localization.dll
		static BaseTextLocalization& get();

		/// @brief Change localization
		/// @param language Language key
		/// @exception std::runtime_error Wrong language
		void changeLanguage(const std::string& language);

		/// @brief Get original language
		/// @return originalLanguage
		std::string_view getOriginalLanguage() const;

		/// @brief Get current language
		/// @return language
		const std::string& getCurrentLanguage() const;

		/// @brief Get path to used module
		const std::filesystem::path& getPathToModule() const;

		/// @brief Get localized text
		/// @param key Localization key
		/// @param language Specific language
		/// @param allowOriginal If can't find text for specific language try to find in original language
		/// @return Localized value
		/// @exception std::runtime_error Wrong key
		/// @exception std::out_of_range
		std::wstring_view getString(const std::string& key, const std::string& language, bool allowOriginal = true) const;

		/// @brief Get localized text
		/// @param key Localization key
		/// @return Localized value
		/// @exception std::runtime_error Wrong key
		std::wstring_view operator [] (const std::string& key) const;

		friend class MultiLocalizationManager;
		friend struct LocalizationHolder;
		friend std::unique_ptr<WTextLocalization>::deleter_type;
	};
}

#endif
