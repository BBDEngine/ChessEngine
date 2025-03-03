cmake_minimum_required(VERSION 3.15...3.31)

project(bbd VERSION 1.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE) # enables LTO
set(EVAL_PATH ./drill/nnue_v1-100/quantised.bin)

############################################################################
find_program(CLANG_FORMAT "clang-format")
# setting up formatting

file(GLOB_RECURSE ALL_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/src/*.cpp
        ${PROJECT_SOURCE_DIR}/src/*.h
        ${PROJECT_SOURCE_DIR}/tests/*.cpp
        ${PROJECT_SOURCE_DIR}/tests/*.h
)

add_custom_target(format-check
        COMMAND ${CLANG_FORMAT} -n --style=Microsoft -Werror ${ALL_SOURCE_FILES}
        COMMENT "Checking code formatting"
        VERBATIM
)

add_custom_target(format-apply
        COMMAND ${CLANG_FORMAT} -i --style=Microsoft ${ALL_SOURCE_FILES}
        COMMENT "Formatting the code"
        VERBATIM
)
############################################################################

set(SOURCES
        src/main.cpp
        src/board.cpp
        src/search.cpp
        src/network.cpp
)

add_executable(bbd ${SOURCES})

target_compile_options(bbd PRIVATE
        -Wall
        -Werror
        -g
        -O3
)

target_compile_definitions(bbd PRIVATE EVALFILE=\"${CMAKE_SOURCE_DIR}/${EVAL_PATH}\")

############################################################################
# include format checking
IF (CLANG_FORMAT)
    add_dependencies(bbd format-check)
    add_dependencies(format-check format-apply)
ENDIF ()


############################################################################
# could be tested with "ctest"
enable_testing()
add_subdirectory(tests)

add_custom_target(run-tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMENT "Testing the project"
        VERBATIM
)

get_property(TEST_TARGETS DIRECTORY tests PROPERTY BUILDSYSTEM_TARGETS)
if (TEST_TARGETS)
    add_dependencies(run-tests ${TEST_TARGETS})
endif ()

add_dependencies(bbd run-tests)
############################################################################