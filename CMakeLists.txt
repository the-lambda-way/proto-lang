# Helpful tutorials:
# https://cliutils.gitlab.io/modern-cmake/
# https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/
# https://foonathan.net/blog/2016/03/03/cmake-install.html


cmake_minimum_required (VERSION 3.12)

project (ProtoLang
         VERSION     0.0.0
         DESCRIPTION "A fully extensible and modular language.")

add_subdirectory (pattern)
add_subdirectory (syntax)

add_executable (proto main.cpp)
target_link_libraries (proto pattern syntax)

install (TARGETS proto DESTINATION bin)



# CONFIGURE TESTING
# ----------------------------------------------------------------------------------------------------------------------
# Don't compile GoogleTest separately from your codebase:
#     https://groups.google.com/forum/#!msg/googletestframework/Zo7_HOv1MJ0/F4ZBGjh_ePcJ
include (FetchContent)

FetchContent_Declare (
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.8.1)

FetchContent_MakeAvailable (googletest)

include (GoogleTest)

# usage: add_test_package (test-name library-name include-files...)
macro (add_test_package TESTNAME LIBNAME)
    add_executable        (${TESTNAME} ${ARGN})
    target_link_libraries (${TESTNAME} ${LIBNAME} gmock gtest_main)
    install               (TARGETS ${TESTNAME} DESTINATION test)
    gtest_discover_tests  (${TESTNAME})
endmacro ()


# ADD TESTS
# ----------------------------------------------------------------------------------------------------------------------
add_test_package (PatternLibrary-test PatternLibrary pattern/test.cpp)


