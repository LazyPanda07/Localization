cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install-dir CACHE STRING "")
set(LINK_LIBRARY_PATH dependencies/JSON/lib)

if (UNIX)
	add_definitions(-D__LINUX__)

	set(LINK_LIBRARY_PATH ${LINK_LIBRARY_PATH}/Linux/)
else ()
	if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
		set(LINK_LIBRARY_PATH ${LINK_LIBRARY_PATH}/Windows/Release-x64/)
	else ()
		set(LINK_LIBRARY_PATH ${LINK_LIBRARY_PATH}/Windows/Debug-x64/)
	endif ()
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
install(DIRECTORY dependencies/JSON/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/JSON/include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${LINK_LIBRARY_PATH} DESTINATION ${CMAKE_INSTALL_PREFIX}/JSON/lib)
install(FILES localization_modules.json DESTINATION ${CMAKE_INSTALL_PREFIX})
