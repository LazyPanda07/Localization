cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install-dir CACHE STRING "")
set(CMAKE_BUILD_TYPE Release CACHE STRING "")
set(LOCALIZATION_UTILS_PATH assets)

if (UNIX)
	add_definitions(-D__LINUX__)

	set(LOCALIZATION_UTILS_PATH ${LOCALIZATION_UTILS_PATH}/Linux/LocalizationUtils)
else()
	set(LOCALIZATION_UTILS_PATH ${LOCALIZATION_UTILS_PATH}/Windows/LocalizationUtils.exe)
endif()

project(Localization VERSION 1.3.0)

add_subdirectory(3rdparty)

add_library(
	${PROJECT_NAME} SHARED
	src/MultiLocalizationManager.cpp
	src/WTextLocalization.cpp
	src/StringViewUtils.cpp
)

target_include_directories(
	${PROJECT_NAME} PRIVATE
	src/
	3rdparty/JSON/include/
)

target_link_directories(
	${PROJECT_NAME} PRIVATE
	3rdparty/JSON/lib/
)

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	JSON
)

install(
	TARGETS ${PROJECT_NAME}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/dll
)

install(DIRECTORY src/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY 3rdparty/JSON/ DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES localization_modules.json DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES ${LOCALIZATION_UTILS_PATH} DESTINATION ${CMAKE_INSTALL_PREFIX}/assets)
