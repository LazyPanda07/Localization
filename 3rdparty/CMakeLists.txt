cmake_minimum_required(VERSION 3.27.0)

project(3rdparty)

set(JSON_TAG v2.7.1)
set(LOCALIZATION_JSON_PATH ${CMAKE_BINARY_DIR}/Localization/JSON)
set(ANDROID_API_VERSION 35)

if (NOT EXISTS ${LOCALIZATION_JSON_PATH})
	execute_process(
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMAND git clone https://github.com/LazyPanda07/JSON.git -b ${JSON_TAG} ${LOCALIZATION_JSON_PATH}
	)

	if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
		if (UNIX)
			execute_process(
				WORKING_DIRECTORY ${LOCALIZATION_JSON_PATH}
				COMMAND ${CMAKE_COMMAND} -E make_directory build
			)

			execute_process(
				WORKING_DIRECTORY ${LOCALIZATION_JSON_PATH}/build
				COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-${ANDROID_API_VERSION} -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/JSON ..
			)

			execute_process(
				WORKING_DIRECTORY ${LOCALIZATION_JSON_PATH}/build
				COMMAND make install
			)
		else()
			execute_process(
				WORKING_DIRECTORY ${LOCALIZATION_JSON_PATH}
				COMMAND ${CMAKE_COMMAND} -E make_directory build
			)

			execute_process(
				WORKING_DIRECTORY ${LOCALIZATION_JSON_PATH}/build
				COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-${ANDROID_API_VERSION} -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/JSON -G "NMake Makefiles" ..
			)

			execute_process(
				WORKING_DIRECTORY ${LOCALIZATION_JSON_PATH}/build
				COMMAND nmake install
			)
		endif()
	else()
		if (UNIX)
			execute_process(
				WORKING_DIRECTORY ${LOCALIZATION_JSON_PATH}
				COMMAND ${CMAKE_COMMAND} -E make_directory build
			)

			execute_process(
				WORKING_DIRECTORY ${LOCALIZATION_JSON_PATH}/build
				COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/JSON ..
			)

			execute_process(
				WORKING_DIRECTORY ${LOCALIZATION_JSON_PATH}/build
				COMMAND make install
			)
		else()
			execute_process(
				WORKING_DIRECTORY ${LOCALIZATION_JSON_PATH}
				COMMAND ${CMAKE_COMMAND} -E make_directory build
			)

			execute_process(
				WORKING_DIRECTORY ${LOCALIZATION_JSON_PATH}/build
				COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/JSON -G "NMake Makefiles" ..
			)

			execute_process(
				WORKING_DIRECTORY ${LOCALIZATION_JSON_PATH}/build
				COMMAND nmake install
			)
		endif()
	endif()
endif()
