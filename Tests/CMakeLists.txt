cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/bin)
set(JSON_LIBRARY_PATH ../dependencies/JSON/lib)

if (UNIX)
	add_definitions(-D__LINUX__)

	set(DLL ../Localization/lib/libLocalization.so)
	set(JSON_LIBRARY_PATH ${JSON_LIBRARY_PATH}/Linux/)
else ()
	set(DLL ../Localization/dll/Localization.dll)

	if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
		set(JSON_LIBRARY_PATH ${JSON_LIBRARY_PATH}/Windows/Release-x64/)
	else ()
		set(JSON_LIBRARY_PATH ${JSON_LIBRARY_PATH}/Windows/Debug-x64/)
	endif ()
endif (UNIX)

project(Tests)

add_executable(
	${PROJECT_NAME}
	main.cpp
)

target_include_directories(
	${PROJECT_NAME} PRIVATE
	../src/
	../dependencies/JSON/include/
	gtest/include/
)

target_link_directories(
	${PROJECT_NAME} PRIVATE
	${JSON_LIBRARY_PATH}
	../Localization/lib/
	gtest/lib/
)

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	JSON
	Localization
	gtest
	gtest_main
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES ${DLL} DESTINATION ${CMAKE_INSTALL_PREFIX})
