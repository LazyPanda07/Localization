#include "StringViewUtils.h"

using namespace std;

namespace localization
{
	namespace utility
	{
		size_t StringViewHash::operator ()(string_view value) const
		{
			return hash<string_view>()(value);
		}

		bool StringViewEqual::operator ()(string_view left, string_view right) const
		{
			return left == right;
		}
	}
}
