cmake_minimum_required(VERSION 3.27.0)

project(3rdparty)

set(JSON_TAG v2.6.1)
set(JSON_PATH ${PROJECT_SOURCE_DIR}/JSON)
set(JSON_INCLUDE ${PROJECT_SOURCE_DIR}/JSON/src/ PARENT_SCOPE)

if (EXISTS ${JSON_PATH})
	execute_process(
		WORKING_DIRECTORY ${JSON_PATH}
		COMMAND git pull
	)

	execute_process(
		WORKING_DIRECTORY ${JSON_PATH}
		COMMAND git checkout ${JSON_TAG}
	)
else()
	execute_process(
		COMMAND git clone https://github.com/LazyPanda07/JSON -b ${JSON_TAG} ${JSON_PATH}
	)
endif()

add_subdirectory(JSON)
