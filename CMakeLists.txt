# vim: noexpandtab:ts=4:sts=4:sw=4

cmake_minimum_required (VERSION 3.0.2)
project (roshi)

set (SRC
	src/main.c
	src/sql.c
	src/commands.c
	src/cmd_newlog.c
	src/cmd_add.c
	src/cmd_add_readline_completers.c
	src/cmd_add_readline_generators.c
	src/cmd_add_readline_hooks.c
	src/options.c
	src/cmd_analyze.c
	)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Debug build")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wextra")
endif()

find_package(Git)
if(Git_FOUND)
	if(EXISTS "${CMAKE_SOURCE_DIR}/.git")

		execute_process(
			COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
			WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
			OUTPUT_VARIABLE _git_rev
			ERROR_QUIET
			OUTPUT_STRIP_TRAILING_WHITESPACE
			)

		message("Adding git rev ${_git_rev} to version number")

		add_definitions(-DGIT_REV="${_git_rev}")
	else()
		message("Not adding git rev to version number")
	endif()
endif()

add_executable (roshi ${SRC})

target_link_libraries (roshi sqlite3 readline confuse)

install(TARGETS
	roshi
	DESTINATION
	bin
)

install(FILES
	roshi.1
	DESTINATION
	man
)

enable_testing()
add_test( newlog ../tests/run_test.py newlog ${CMAKE_BINARY_DIR} )
add_test( add ../tests/run_test.py add ${CMAKE_BINARY_DIR} )

