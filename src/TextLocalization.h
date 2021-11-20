#pragma once

#include <unordered_map>
#include <string>

#include <Windows.h>

namespace localization
{
	/// @brief Singleton for text localization
	class TextLocalization
	{
	private:
		const std::unordered_map<std::string, const std::unordered_map<std::string, std::string>*>* dictionaries;
		const std::string* originalLanguage;
		std::string language;
		HMODULE handle;

	private:
		TextLocalization();

		TextLocalization(const TextLocalization&) = delete;

		TextLocalization(TextLocalization&&) noexcept = delete;

		TextLocalization& operator = (const TextLocalization&) = delete;

		TextLocalization& operator = (TextLocalization&&) noexcept = delete;

		~TextLocalization();

	public:
		/// @brief Exception can be thrown on first call
		/// @return Singleton instance
		/// @exception std::runtime_error 
		static TextLocalization& get();

		/// @brief Change localization
		/// @param language Language key
		/// @exception std::runtime_error
		void changeLanguage(const std::string& language);

		/// @brief Get localized text
		/// @param key Localization key
		/// @return Localized value
		/// @exception std::runtime_error 
		const std::string& operator [] (const std::string& key) const;
	};
}
