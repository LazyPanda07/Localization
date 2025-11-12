#pragma once

#include <string_view>

#include "LocalizationConstants.h"

namespace localization::utility
{
	struct LOCALIZATION_API StringViewHash
	{
		using is_transparent = void;

		size_t operator ()(std::string_view value) const;
	};

	struct LOCALIZATION_API StringViewEqual
	{
		using is_transparent = void;

		bool operator ()(std::string_view left, std::string_view right) const;
	};
}
