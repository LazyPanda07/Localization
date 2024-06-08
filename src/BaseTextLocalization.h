#pragma once

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <format>
#include <algorithm>
#include <fstream>
#include <filesystem>

#ifdef __LINUX__
#include <dlfcn.h>
#else
#include <Windows.h>
#endif

#include "JSONParser.h"
#include "LocalizationConstants.h"

#ifdef __LINUX__
using HMODULE = void*;
#endif

namespace localization
{
	/// @brief Singleton for text localization
	template<typename T>
	class LOCALIZATION_API BaseTextLocalization final
	{
	private:
		using dictionariesFunction = const char* (*)(const char* key, const char* language);
		using findLanguageFunction = bool (*)(const char* language);
		using originalLanguageFunction = const char* (*)();

	private:
		dictionariesFunction dictionaries;
		findLanguageFunction findLanguage;
		originalLanguageFunction originalLanguage;
		std::string language;
		std::filesystem::path pathToModule;
		HMODULE handle;

	private:
		BaseTextLocalization(std::string_view localizationModule);

		BaseTextLocalization(const BaseTextLocalization<T>&) = delete;

		BaseTextLocalization(BaseTextLocalization<T>&& other) noexcept;

		BaseTextLocalization<T>& operator = (const BaseTextLocalization<T>&) = delete;

		BaseTextLocalization<T>& operator = (BaseTextLocalization<T>&& other) noexcept;

		~BaseTextLocalization();

	public:
		/// @brief Exception can be thrown on first call. Warnings outputs in std::cerr
		/// @return Singleton instance of default localization module(Localization.dll)
		/// @exception std::runtime_error Can't find localization module or something inside localization module
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
		std::basic_string_view<T> getString(const std::string& key, const std::string& language, bool allowOriginal = true) const;

		/// @brief Get localized text
		/// @param key Localization key
		/// @return Localized value
		/// @exception std::runtime_error Wrong key
		std::basic_string_view<T> operator [] (const std::string& key) const;

		friend class BaseTextLocalization<wchar_t>;
		friend class MultiLocalizationManager;
		friend struct LocalizationHolder;
		friend std::unique_ptr<BaseTextLocalization<T>>::deleter_type;
	};

	template<typename T>
	BaseTextLocalization<T>::BaseTextLocalization(std::string_view localizationModule)
	{
#ifdef __LINUX__
		pathToModule = std::format("lib{}.so", localizationModule);
#else
		pathToModule = std::format("{}.dll", localizationModule);
#endif

		if (!std::filesystem::exists(pathToModule))
		{
			throw std::runtime_error(std::format("Can't find {}", pathToModule.string()));
		}

#ifdef __LINUX__
		handle = dlopen(pathToModule.string().data(), RTLD_LAZY);
#else
		handle = LoadLibraryA(pathToModule.string().data());
#endif
		auto load = [this](const char* name)
			{
#ifdef __LINUX__
				return dlsym(handle, name);
#else
				return GetProcAddress(handle, name);
#endif
			};

		if (!handle)
		{
			throw std::runtime_error(std::format("Can't load {}", pathToModule.string()));
		}

		dictionaries = reinterpret_cast<dictionariesFunction>(load("getLocalizedString"));

		if (!dictionaries)
		{
			throw std::runtime_error(std::format("Can't find getLocalizedString function in {}, rebuild and try again", pathToModule.string()));
		}

		originalLanguage = reinterpret_cast<originalLanguageFunction>(load("getOriginalLanguage"));

		if (!originalLanguage)
		{
			throw std::runtime_error(std::format("Can't find getOriginalLanguage function in {}, rebuild and try again", pathToModule.string()));
		}

		findLanguage = reinterpret_cast<findLanguageFunction>(load("findLanguage"));

		if (!findLanguage)
		{
			throw std::runtime_error(std::format("Can't find findLanguage function in {}, rebuild and try again", pathToModule.string()));
		}

		language = originalLanguage();
	}

	template<typename T>
	BaseTextLocalization<T>::BaseTextLocalization(BaseTextLocalization<T>&& other) noexcept
	{
		(*this) = std::move(other);
	}

	template<typename T>
	BaseTextLocalization<T>& BaseTextLocalization<T>::operator = (BaseTextLocalization<T>&& other) noexcept
	{
		dictionaries = other.dictionaries;
		originalLanguage = other.originalLanguage;
		language = std::move(other.language);
		pathToModule = std::move(other.pathToModule);
		handle = other.handle;

		other.handle = nullptr;

		return *this;
	}

	template<typename T>
	BaseTextLocalization<T>::~BaseTextLocalization()
	{
#ifdef __LINUX__
		dlclose(handle);
#else
		FreeLibrary(handle);
#endif

		handle = nullptr;
	}

	template<typename T>
	inline BaseTextLocalization<T>& BaseTextLocalization<T>::get()
	{
		static std::unique_ptr<BaseTextLocalization<T>> instance;

		if (!instance)
		{
			json::JSONParser settings(std::ifstream(localizationModulesFile.data()));

			instance = std::unique_ptr<BaseTextLocalization<T>>(new BaseTextLocalization<T>(settings.getString(settings::defaultModuleSetting)));
		}

		return *instance;
	}

	template<typename T>
	void BaseTextLocalization<T>::changeLanguage(const std::string& language)
	{
		if (!findLanguage(language.data()))
		{
			throw std::runtime_error(std::format(R"(Wrong language value "{}")", language));
		}

		this->language = language;
	}

	template<typename T>
	std::string_view BaseTextLocalization<T>::getOriginalLanguage() const
	{
		return originalLanguage();
	}

	template<typename T>
	const std::string& BaseTextLocalization<T>::getCurrentLanguage() const
	{
		return language;
	}

	template<typename T>
	const std::filesystem::path& BaseTextLocalization<T>::getPathToModule() const
	{
		return pathToModule;
	}

	template<typename T>
	std::basic_string_view<T> BaseTextLocalization<T>::getString(const std::string& key, const std::string& language, bool allowOriginal) const
	{
		const char* result = dictionaries(key.data(), language.data());

		if (!result)
		{
			if (!allowOriginal)
			{
				throw std::runtime_error(std::format(R"(Can't find key "{}" for {})", key, language));
			}

			std::string_view originalLanguageView = this->getOriginalLanguage();

			result = dictionaries(key.data(), originalLanguageView.data());

			if (!result)
			{
				throw std::runtime_error(std::format(R"(Can't find key "{}" for {}, also can't find in original language {})", key, language, originalLanguageView));
			}
		}

		return std::string_view(result);
	}

	template<typename T>
	std::basic_string_view<T> BaseTextLocalization<T>::operator [] (const std::string& key) const
	{
		return this->getString(key, language);
	}
}
