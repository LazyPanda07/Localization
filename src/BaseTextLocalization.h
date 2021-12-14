#pragma once

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <format>

#include <Windows.h>

namespace localization
{
	using namespace std::string_view_literals;

	/// @brief Singleton for text localization
	template<typename T>
	class BaseTextLocalization final
	{
	private:
		const std::unordered_map<std::string, const std::unordered_map<std::string, std::string>*>* dictionaries;
		const std::string* originalLanguage;
		std::string language;
		HMODULE handle;

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
		const std::basic_string<T>& operator [] (const std::string& key) const;

		friend class BaseTextLocalization<wchar_t>;
	};

	template<typename T>
	BaseTextLocalization<T>::BaseTextLocalization()
	{
		handle = LoadLibraryA("Localization.dll");

		if (!handle)
		{
			throw std::runtime_error("Can't find Localization.dll");
		}

		dictionaries = reinterpret_cast<const std::unordered_map<std::string, const std::unordered_map<std::string, std::string>*>*>(GetProcAddress(handle, "dictionaries"));

		if (!dictionaries)
		{
			throw std::runtime_error("Can't find dictionaries in Localization.dll, rebuild and try again");
		}

		originalLanguage = reinterpret_cast<const std::string*>(GetProcAddress(handle, "originalLanguage"));

		if (!originalLanguage)
		{
			throw std::runtime_error("Can't find originalLanguage in Localization.dll, rebuild and try again");
		}

		language = *originalLanguage;
	}

	template<typename T>
	BaseTextLocalization<T>::~BaseTextLocalization()
	{
		FreeLibrary(handle);
	}

	template<typename T>
	inline BaseTextLocalization<T>& BaseTextLocalization<T>::get()
	{
		static BaseTextLocalization<T> instance;

		return instance;
	}

	template<typename T>
	void BaseTextLocalization<T>::changeLanguage(const std::string& language)
	{
		if (dictionaries->find(language) == dictionaries->end())
		{
			throw std::runtime_error(std::format(R"(Wrong language value "{}")"sv, language));
		}

		this->language = language;
	}

	template<typename T>
	const std::string& BaseTextLocalization<T>::getOriginalLanguage() const
	{
		return *originalLanguage;
	}

	template<typename T>
	const std::string& BaseTextLocalization<T>::getCurrentLanguage() const
	{
		return language;
	}

	template<typename T>
	const std::basic_string<T>& BaseTextLocalization<T>::operator[] (const std::string& key) const
	{
		try
		{
			const std::string& result = dictionaries->at(language)->at(key);

			return result.empty() ? dictionaries->at(*originalLanguage)->at(key) : result;
		}
		catch (const std::out_of_range&)
		{
			try
			{
				return dictionaries->at(*originalLanguage)->at(key);
			}
			catch (const std::out_of_range&)
			{
				throw std::runtime_error(format(R"(Can't find key "{}")"sv, key));
			}
		}
	}
}
