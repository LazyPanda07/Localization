#include "StringViewUtils.h"

namespace localization::utility
{
	size_t StringViewHash::operator ()(std::string_view value) const
	{
		return std::hash<std::string_view>()(value);
	}

	bool StringViewEqual::operator ()(std::string_view left, std::string_view right) const
	{
		return left == right;
	}
}
