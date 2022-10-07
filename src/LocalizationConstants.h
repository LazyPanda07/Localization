#pragma once

#include <string>

#define LOCALIZATION_API __declspec(dllexport)

#pragma warning(disable: 4251)

namespace localization
{
	inline constexpr std::string_view localizationModulesFile = "localization_modules.json";

	namespace settings
	{
		inline const std::string defaultModuleSetting = "defaultModule";
		inline const std::string modulesSetting = "modules";
	}
}
