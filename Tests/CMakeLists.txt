cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/bin)
set(GTEST_VERSION 1.17.0)

project(Tests)

include(FetchContent)

FetchContent_Declare(
	gtest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG v${GTEST_VERSION}
)

FetchContent_MakeAvailable(gtest)

set(LOCALIZATION_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/../Localization)

if (UNIX)
	add_definitions(-D__LINUX__)

	set(DLL ${LOCALIZATION_LIBRARY_DIR}/lib/libLocalization.so)
else ()
	set(DLL ${LOCALIZATION_LIBRARY_DIR}/dll/Localization.dll)
endif()

add_executable(
	${PROJECT_NAME}
	main.cpp
)

target_include_directories(
	${PROJECT_NAME} PRIVATE
	${LOCALIZATION_LIBRARY_DIR}/include/
)

target_link_directories(
	${PROJECT_NAME} PRIVATE
	${LOCALIZATION_LIBRARY_DIR}/lib/
)

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	JSON
	Localization
	gtest
	gtest_main
)

install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(FILES ${DLL} DESTINATION .)
install(FILES first.txt DESTINATION .)
install(FILES second.txt DESTINATION .)
