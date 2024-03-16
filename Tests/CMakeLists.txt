cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/bin)

if (UNIX)
	add_definitions(-D__LINUX__)

	set(DLL ../Localization/lib/libLocalization.so)
else ()
	set(DLL ../Localization/dll/Localization.dll)
endif (UNIX)

project(Tests)

add_executable(
	${PROJECT_NAME}
	main.cpp
)

target_include_directories(
	${PROJECT_NAME} PRIVATE
	../src/
	gtest/include/
)

target_link_directories(
	${PROJECT_NAME} PRIVATE
	gtest/lib
)

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	gtest
	gtest_main
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES ${DLL} DESTINATION ${CMAKE_INSTALL_PREFIX})
