cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
set(LOCALIZATION_UTILS_PATH assets)

project(Localization VERSION 1.4.6)

if (UNIX)
	add_definitions(-D__LINUX__)

	if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
		set(LOCALIZATION_UTILS_PATH ${LOCALIZATION_UTILS_PATH}/LinuxARM/LocalizationUtils)	
	else()
		set(LOCALIZATION_UTILS_PATH ${LOCALIZATION_UTILS_PATH}/Linux/LocalizationUtils)	
	endif()
else()
	set(LOCALIZATION_UTILS_PATH ${LOCALIZATION_UTILS_PATH}/Windows/LocalizationUtils.exe)
endif()

add_library(
	${PROJECT_NAME} SHARED
	src/MultiLocalizationManager.cpp
	src/WTextLocalization.cpp
	src/StringViewUtils.cpp
)

target_include_directories(
	${PROJECT_NAME} PUBLIC
	include
)

if (DEFINED ENV{MARCH} AND NOT "$ENV{MARCH}" STREQUAL "")
	target_compile_options(${PROJECT_NAME} PRIVATE -march=$ENV{MARCH})
endif()

add_subdirectory(3rdparty)

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	JSON
)

install(
	TARGETS ${PROJECT_NAME}
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION dll
)

install(DIRECTORY include DESTINATION .)
install(FILES localization_modules.json DESTINATION .)
install(FILES ${LOCALIZATION_UTILS_PATH} DESTINATION assets)
