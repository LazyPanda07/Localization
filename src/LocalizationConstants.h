#pragma once

#include <string>

namespace localization
{
	inline constexpr std::string_view localizationModulesFile = "localization_modules.json";

	namespace settings
	{
		inline const std::string defaultModuleSetting = "defaultModule";
		inline const std::string modulesSetting = "modules";
	}
}
