#pragma once

/// @file WTextLocalization.h
/// @brief BaseTextLocalization specialization with std::wstring

#include "TextLocalization.h"

namespace localization
{
	/// @brief TextLocalization with std::wstring
	using WTextLocalization = localization::BaseTextLocalization<wchar_t>;

	/// @brief TextLocalization specialization with wchar_t
	template<>
	class BaseTextLocalization<wchar_t> final
	{
	private:
		std::unordered_map<std::string, std::unordered_map<std::string, std::wstring>> dictionaries;
		std::string originalLanguage;
		std::string language;

	private:
		BaseTextLocalization();

		BaseTextLocalization(const BaseTextLocalization&) = delete;

		BaseTextLocalization(BaseTextLocalization&&) noexcept = delete;

		BaseTextLocalization& operator = (const BaseTextLocalization&) = delete;

		BaseTextLocalization& operator = (BaseTextLocalization&&) noexcept = delete;

		~BaseTextLocalization();

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
		const std::string& getOriginalLanguage() const;

		/// @brief Get current language
		/// @return language
		const std::string& getCurrentLanguage() const;

		/// @brief Get localized text
		/// @param key Localization key
		/// @return Localized value
		/// @exception std::runtime_error Wrong key
		const std::wstring& operator [] (const std::string& key) const;
	};
}
