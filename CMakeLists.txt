cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install-dir CACHE STRING "")
set(LINK_LIBRARY_PATH dependencies/JSON/lib)
set(LOCALIZATION_UTILS_PATH assets)

if (UNIX)
	add_definitions(-D__LINUX__)

	set(LINK_LIBRARY_PATH ${LINK_LIBRARY_PATH}/Linux/)
	set(LOCALIZATION_UTILS_PATH ${LOCALIZATION_UTILS_PATH}/LocalizationUtils)
else ()
	if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
		set(LINK_LIBRARY_PATH ${LINK_LIBRARY_PATH}/Windows/Release-x64/)
	else ()
		set(LINK_LIBRARY_PATH ${LINK_LIBRARY_PATH}/Windows/Debug-x64/)
	endif ()

	set(LOCALIZATION_UTILS_PATH ${LOCALIZATION_UTILS_PATH}/LocalizationUtils.exe)
endif (UNIX)

project(Localization)

add_library(
	${PROJECT_NAME} SHARED
	src/MultiLocalizationManager.cpp
	src/WTextLocalization.cpp
)

target_include_directories(
	${PROJECT_NAME} PUBLIC
	src/
	dependencies/JSON/include/
)

target_link_directories(
	${PROJECT_NAME} PUBLIC
	${LINK_LIBRARY_PATH}
)

target_link_libraries(
	${PROJECT_NAME} PUBLIC
	JSON
)

install(
	TARGETS ${PROJECT_NAME}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/dll
)
install(DIRECTORY src/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${LINK_LIBRARY_PATH} DESTINATION ${CMAKE_INSTALL_PREFIX}/JSON/)
install(FILES localization_modules.json DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES ${LOCALIZATION_UTILS_PATH} DESTINATION ${CMAKE_INSTALL_PREFIX}/assets)
