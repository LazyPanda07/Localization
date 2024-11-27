#pragma once

#include <string_view>

namespace localization
{
	namespace utility
	{
		struct StringViewHash
		{
			using is_transparent = void;

			size_t operator ()(std::string_view value) const;
		};

		struct StringViewEqual
		{
			using is_transparent = void;

			bool operator ()(std::string_view left, std::string_view right) const;
		};
	}
}
