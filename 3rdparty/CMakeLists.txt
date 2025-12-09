cmake_minimum_required(VERSION 3.27.0)

set(FETCHCONTENT_QUIET OFF CACHE BOOL "")
set(JSON_VERSION 3.2.1)

project(3rdparty)

include(FetchContent)

if (NOT TARGET JSON)
	FetchContent_Declare(
		JSON
		GIT_REPOSITORY https://github.com/LazyPanda07/JSON.git
		GIT_TAG v${JSON_VERSION}
	)

	FetchContent_MakeAvailable(JSON)
endif()
