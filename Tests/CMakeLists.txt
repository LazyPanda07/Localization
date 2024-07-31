cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/bin)

project(Tests)

set(LOCALIZATION_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/../Localization)

if (UNIX)
	add_definitions(-D__LINUX__)

	set(DLL ${LOCALIZATION_LIBRARY_DIR}/lib/libLocalization.so)
else ()
	set(DLL ${LOCALIZATION_LIBRARY_DIR}/dll/Localization.dll)
endif (UNIX)

add_executable(
	${PROJECT_NAME}
	main.cpp
)

target_include_directories(
	${PROJECT_NAME} PRIVATE
	${LOCALIZATION_LIBRARY_DIR}/include/
	gtest/include/
)

target_link_directories(
	${PROJECT_NAME} PRIVATE
	${LOCALIZATION_LIBRARY_DIR}/lib/
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
