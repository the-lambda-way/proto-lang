# good doc: https://cliutils.gitlab.io/modern-cmake/

cmake_minimum_required (VERSION 3.15)

project (ProtoLang
         VERSION     0.0.0
         DESCRIPTION "A fully extensible and modular language."
         LANGUAGES   CXX)

add_executable (proto main.cpp)
install (TARGETS proto DESTINATION bin)


# CONFIGURE TESTING
# ----------------------------------------------------------------------------------------------------------------------
include (CMakeDependentOption)
cmake_dependent_option (BUILD_TESTS "Build your tests" ON "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF)

if (BUILD_TESTS)
    enable_testing ()
endif ()

# Don't compile separately from your tests: https://groups.google.com/forum/#!msg/googletestframework/Zo7_HOv1MJ0/F4ZBGjh_ePcJ
# https://cliutils.gitlab.io/modern-cmake/chapters/testing/googletest.html
include (FetchContent)

FetchContent_Declare (
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.8.1)

FetchContent_MakeAvailable (googletest)

include (GoogleTest)

# usage: add_test_package (test test.cpp)
macro (add_test_package TESTNAME)
    add_executable        (${TESTNAME} ${ARGN})
    target_link_libraries (${TESTNAME} gtest gmock gtest_main)
    set_target_properties (${TESTNAME} PROPERTIES FOLDER test)
    gtest_discover_tests  (${TESTNAME})
endmacro ()


# ADD TESTS
# ----------------------------------------------------------------------------------------------------------------------
include_directories ("${PROJECT_SOURCE_DIR}/pattern")
add_test_package    (pattern-test pattern/test.cpp)


