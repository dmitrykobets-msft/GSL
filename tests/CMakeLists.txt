cmake_minimum_required(VERSION 3.0.2)

project(GSLTests CXX)
enable_testing()  # again, for support standalone testing

include(FindPkgConfig)
include(ExternalProject)

# will make visual studio generated project group files
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(CI_TESTING AND GSL_CXX_STANDARD EQUAL 20)
    add_compile_definitions(FORCE_STD_SPAN_TESTS=1)
endif()

if(IOS)
    add_compile_definitions(GTEST_HAS_DEATH_TEST=1 IOS_PROCESS_DELAY_WORKAROUND=1)
endif()

pkg_search_module(GTestMain gtest_main)
if (NOT GTestMain_FOUND)
    # No pre-installed GTest is available, try to download it using Git.
    find_package(Git REQUIRED QUIET)

    configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download
    )
    if(result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download
    )
    if(result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(GTestMain_LIBRARIES gtest_main)

    add_subdirectory(
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL
    )
endif()

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    # CMake has been started independently in this directory with tests. Do
    # import the globally installed Guidelines Support Library and test it
    # instead of the current version from the include/ folder.
    find_package(Microsoft.GSL REQUIRED)
endif()

if (MSVC AND (GSL_CXX_STANDARD GREATER_EQUAL 17))
    set(GSL_CPLUSPLUS_OPT -Zc:__cplusplus -permissive-)
endif()

# this interface adds compile options to how the tests are run
# please try to keep entries ordered =)
add_library(gsl_tests_config INTERFACE)
if(MSVC) # MSVC or simulating MSVC
    target_compile_options(gsl_tests_config INTERFACE
        ${GSL_CPLUSPLUS_OPT}
        /EHsc
        /W4
        /WX
        $<$<CXX_COMPILER_ID:MSVC>:
          /wd4996  # Use of function or classes marked [[deprecated]]
          /wd26409 # CppCoreCheck - GTest
          /wd26426 # CppCoreCheck - GTest
          /wd26440 # CppCoreCheck - GTest
          /wd26446 # CppCoreCheck - prefer gsl::at()
          /wd26472 # CppCoreCheck - use gsl::narrow(_cast)
          /wd26481 # CppCoreCheck - use span instead of pointer arithmetic
          $<$<VERSION_LESS:$<CXX_COMPILER_VERSION>,1920>: # VS2015
            /wd4189 # variable is initialized but not referenced
            $<$<NOT:$<CONFIG:Debug>>: # Release, RelWithDebInfo
              /wd4702 # Unreachable code
            >
          >
        >
        $<$<CXX_COMPILER_ID:Clang>:
          -Weverything
          -Wno-c++98-compat
          -Wno-c++98-compat-pedantic
          -Wno-covered-switch-default # GTest
          -Wno-deprecated-declarations # Allow tests for [[deprecated]] elements
          -Wno-global-constructors # GTest
          -Wno-language-extension-token # GTest gtest-port.h
          -Wno-missing-braces
          -Wno-missing-prototypes
          -Wno-shift-sign-overflow # GTest gtest-port.h
          -Wno-undef # GTest
          -Wno-used-but-marked-unused # GTest EXPECT_DEATH
          $<$<EQUAL:${GSL_CXX_STANDARD},14>: # no support for [[maybe_unused]]
            -Wno-unused-member-function
            -Wno-unused-variable
          >
        >
    )
else()
    target_compile_options(gsl_tests_config INTERFACE
        -fno-strict-aliasing
        -Wall
        -Wcast-align
        -Wconversion
        -Wctor-dtor-privacy
        -Werror
        -Wextra
        -Wpedantic
        -Wshadow
        -Wsign-conversion
        -Wno-deprecated-declarations # Allow tests for [[deprecated]] elements
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
          -Weverything
          -Wno-c++98-compat
          -Wno-c++98-compat-pedantic
          -Wno-missing-braces
          -Wno-covered-switch-default # GTest
          -Wno-global-constructors # GTest
          -Wno-missing-prototypes
          -Wno-padded
          -Wno-unknown-attributes
          -Wno-used-but-marked-unused # GTest EXPECT_DEATH
          -Wno-weak-vtables
          $<$<EQUAL:${GSL_CXX_STANDARD},14>: # no support for [[maybe_unused]]
            -Wno-unused-member-function
            -Wno-unused-variable
          >
        >
        $<$<CXX_COMPILER_ID:Clang>:
          $<$<AND:$<VERSION_GREATER:$<CXX_COMPILER_VERSION>,4.99>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,6>>:
            $<$<EQUAL:${GSL_CXX_STANDARD},17>:-Wno-undefined-func-template>
          >
          $<$<AND:$<EQUAL:${GSL_CXX_STANDARD},20>,$<OR:$<CXX_COMPILER_VERSION:11.0.0>,$<CXX_COMPILER_VERSION:10.0.0>>>:
              -Wno-zero-as-null-pointer-constant  # failing Clang Ubuntu 20.04 tests, seems to be a bug with clang 10.0.0
                                                  # and clang 11.0.0. (operator< is being re-written by the compiler
                                                  # as operator<=> and raising the warning)
          >
        >
        $<$<CXX_COMPILER_ID:AppleClang>:
          $<$<AND:$<VERSION_GREATER:$<CXX_COMPILER_VERSION>,9.1>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,10>>:
            $<$<EQUAL:${GSL_CXX_STANDARD},17>:-Wno-undefined-func-template>
          >
        >
        $<$<CXX_COMPILER_ID:GNU>:
          -Wdouble-promotion # float implicit to double
          -Wlogical-op # suspicious uses of logical operators
          $<$<NOT:$<VERSION_LESS:$<CXX_COMPILER_VERSION>,6>>:
            -Wduplicated-cond # duplicated if-else conditions
            -Wmisleading-indentation
            -Wnull-dereference
            $<$<EQUAL:${GSL_CXX_STANDARD},14>: # no support for [[maybe_unused]]
              -Wno-unused-variable
            >
          >
          $<$<NOT:$<VERSION_LESS:$<CXX_COMPILER_VERSION>,7>>:
            -Wduplicated-branches # identical if-else branches
          >
        >
    )
endif(MSVC)

# for tests to find the gtest header
target_include_directories(gsl_tests_config SYSTEM INTERFACE
    googletest/googletest/include
)

add_executable(gsl_tests
    algorithm_tests.cpp
    assertion_tests.cpp
    at_tests.cpp
    byte_tests.cpp
    notnull_tests.cpp
    owner_tests.cpp
    span_compatibility_tests.cpp
    span_ext_tests.cpp
    span_tests.cpp
    strict_notnull_tests.cpp
    string_span_tests.cpp
    utils_tests.cpp
)

target_link_libraries(gsl_tests
    Microsoft.GSL::GSL
    gsl_tests_config
    ${GTestMain_LIBRARIES}
)
add_test(gsl_tests gsl_tests)

# No exception tests

foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    STRING (REGEX REPLACE "/EHsc" "" ${flag_var} "${${flag_var}}")
endforeach(flag_var)

# this interface adds compile options to how the tests are run
# please try to keep entries ordered =)
add_library(gsl_tests_config_noexcept INTERFACE)
if(MSVC) # MSVC or simulating MSVC
    target_compile_definitions(gsl_tests_config_noexcept INTERFACE
        _HAS_EXCEPTIONS=0 # disable exceptions in the Microsoft STL
    )
    target_compile_options(gsl_tests_config_noexcept INTERFACE
        ${GSL_CPLUSPLUS_OPT}
        /W4
        /WX
        $<$<CXX_COMPILER_ID:MSVC>:
          /wd4577
          /wd4702
          /wd26440 # CppCoreCheck - GTest
          /wd26446 # CppCoreCheck - prefer gsl::at()
        >
        $<$<CXX_COMPILER_ID:Clang>:
          -Weverything
          -Wno-c++98-compat
          -Wno-c++98-compat-pedantic
          -Wno-missing-prototypes
          -Wno-unknown-attributes
        >
    )
else()
    target_compile_options(gsl_tests_config_noexcept INTERFACE
        -fno-exceptions
        -fno-strict-aliasing
        -Wall
        -Wcast-align
        -Wconversion
        -Wctor-dtor-privacy
        -Werror
        -Wextra
        -Wpedantic
        -Wshadow
        -Wsign-conversion
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
          -Weverything
          -Wno-c++98-compat
          -Wno-c++98-compat-pedantic
          -Wno-missing-prototypes
          -Wno-unknown-attributes
          -Wno-weak-vtables
        >
        $<$<CXX_COMPILER_ID:GNU>:
          -Wdouble-promotion # float implicit to double
          -Wlogical-op # suspicious uses of logical operators
          -Wuseless-cast # casting to its own type
          $<$<NOT:$<VERSION_LESS:$<CXX_COMPILER_VERSION>,6>>:
            -Wduplicated-cond # duplicated if-else conditions
            -Wmisleading-indentation
            -Wnull-dereference
          >
          $<$<NOT:$<VERSION_LESS:$<CXX_COMPILER_VERSION>,7>>:
            -Wduplicated-branches # identical if-else branches
          >
          $<$<NOT:$<VERSION_LESS:$<CXX_COMPILER_VERSION>,8>>:
            -Wcast-align=strict # increase alignment (i.e. char* to int*)
          >
        >
    )
endif(MSVC)

add_executable(gsl_noexcept_tests no_exception_ensure_tests.cpp)
target_link_libraries(gsl_noexcept_tests
    Microsoft.GSL::GSL
    gsl_tests_config_noexcept
)
add_test(gsl_noexcept_tests gsl_noexcept_tests)




function(add_compile_tests)
    cmake_parse_arguments(
        PARSED_ARGS # prefix of output variables
        "" # list of names of the boolean arguments (only defined ones will be true)
        "NAME;SRC" # list of names of mono-valued arguments
        "TEST_CASES" # list of names of multi-valued arguments (output variables are lists)
        ${ARGN} # arguments of the function to parse, here we take the all original ones
    )
    if(NOT PARSED_ARGS_NAME)
        message(FATAL_ERROR "Missing test name")
    endif()
    if(NOT PARSED_ARGS_SRC)
        message(FATAL_ERROR "Missing source file")
    endif()
    if(NOT PARSED_ARGS_TEST_CASES)
        message(FATAL_ERROR "Missing testcases")
    endif()

    foreach(TEST_CASE ${PARSED_ARGS_TEST_CASES})
        set(TARGET_NAME ${PARSED_ARGS_NAME}_${TEST_CASE})
        add_library(${TARGET_NAME}
            ${PARSED_ARGS_SRC})
        set_target_properties(${TARGET_NAME} PROPERTIES
            EXCLUDE_FROM_ALL true
            EXCLUDE_FROM_DEFAULT_BUILD true)
        target_link_libraries(${TARGET_NAME} Microsoft.GSL::GSL)

        target_compile_definitions(${TARGET_NAME} PUBLIC ${TEST_CASE})

        file(STRINGS ${PARSED_ARGS_SRC} LINE REGEX ${TEST_CASE})
        if (NOT LINE)
            message(FATAL_ERROR "${TEST_CASE} does not exist in file ${PARSED_ARGS_SRC}")
        endif()
        # message("${LINE}")
        string(REGEX MATCH "\"(.*)\"" REGEX_WITH_QUOTES ${LINE})
        # message("${CMAKE_MATCH_1}")

        add_test(NAME ${TARGET_NAME}
            COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --target ${TARGET_NAME} --config $<CONFIGURATION>)
        # set_tests_properties(${TARGET_NAME} PROPERTIES PASS_REGULAR_EXPRESSION ${CMAKE_MATCH_1})
        # message("${CMAKE_CXX_COMPILER_ID}")
        if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") AND (CMAKE_MATCH_1))
            set_tests_properties(${TARGET_NAME} PROPERTIES PASS_REGULAR_EXPRESSION ${CMAKE_MATCH_1})
        else()
            set_tests_properties(${TARGET_NAME} PROPERTIES WILL_FAIL true)
        endif()
    endforeach()
endfunction()


add_compile_tests(NAME span_compile_tests SRC compile_tests/span.cpp TEST_CASES
    from_array_constructor_TEST1
    from_array_constructor_TEST2
    from_array_constructor_TEST3
    from_array_constructor_TEST4
    from_array_constructor_TEST5
    from_array_constructor_TEST6
    from_array_constructor_TEST7
    from_array_constructor_TEST8
    from_array_constructor_TEST9
    from_std_array_constructor_TEST1
    from_std_array_constructor_TEST2
    from_std_array_constructor_TEST3
    from_std_array_constructor_TEST4
    from_std_array_constructor_TEST5
    from_std_array_constructor_TEST6
    from_const_std_array_constructor_TEST1
    from_const_std_array_constructor_TEST2
    from_const_std_array_constructor_TEST3
    from_std_array_const_constructor_TEST1
    from_std_array_const_constructor_TEST2
    from_std_array_const_constructor_TEST3
    from_std_array_const_constructor_TEST4
    from_container_constructor_TEST1
    from_container_constructor_TEST2
    from_container_constructor_TEST3
    from_container_constructor_TEST4
    from_container_constructor_TEST5
    from_container_constructor_TEST6
    from_convertible_span_constructor_TEST1
    from_convertible_span_constructor_TEST2
    from_convertible_span_constructor_TEST3
    from_convertible_span_constructor_TEST4
    )
