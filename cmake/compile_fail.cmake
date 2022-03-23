# # Copyright Louis Dionne 2017
# # Distributed under the Boost Software License, Version 1.0.
# # See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt
# #
# # This file defines the `compile_fail` function, which creates a compilation
# # failure test. A compilation failure test is a test that passes if the source
# # file fails to compile.
#
# function(getthem)
# endfunction()
#
# fucntion(add_compile_tests)
#     if (ARGC < 2)
#         fail()
#     endif()
#
#     set(TEST_NAME ARGV0)
#     set(SOURCES ARGV1..N)
#     set(TEST_CASE TEST_CASE) # default dummy value
#
#     add_library(${TEST_NAME} ${SOURCES})
#     target_compile_definitions(${TEST_NAME} ${TEST_CASE} 1)
#
#     foreach source in sources:  #fix
#        set (extractedtests extracttests(source)) # extraction
#         for each testcase in extractedtests: #fix
#         add_test(NAME ${TEST_NAME}_${TEST_CASE}
#             COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --target ${TEST_NAME} --config $<CONFIGURATION> --DTEST_CASE=${testcase})
#             if MSVC
#                 set_tests_properties(${testname} PROPERTIES PASS_REGULAR_EXPRESSION "error: no matching constructor for initialization of 'span<int, 6>'")
#             elseif CLANG OR APPLE_CLANG
#                 set_tests_properties(${testname} PROPERTIES PASS_REGULAR_EXPRESSION "error: no matching constructors for initialization of 'span<int, 6>'")
#             elseif GNU
#                 set_tests_properties(${testname} PROPERTIES PASS_REGULAR_EXPRESSION "error: no matching constructors for initialization of 'span<int, 6>'")
# endfunction()
#
# #if TEST1
#
# function(add_test_target target sources)
# endfunction()
#
# # add_test_target(all_compilation_tests span.cpp algos.cpp)
# add_compile_tests(all_comiplation_tests)



function(compile_fail testname sourcefile)
  add_executable(${testname} "${sourcefile}")
  set_target_properties(${testname} PROPERTIES EXCLUDE_FROM_ALL true
                                               EXCLUDE_FROM_DEFAULT_BUILD true)
  add_test(NAME ${testname}
      COMMAND ${CMAKE_COMMAND} -DTEST=1 --build "${CMAKE_BINARY_DIR}" --target ${testname} --config $<CONFIGURATION>)
# set_tests_properties(${testname} PROPERTIES PASS_REGULAR_EXPRESSION "error: no matching constructor for initialization of 'span<int, 6>'")
# message("??? ${failurePattern}")
# set_tests_properties(${testname} PROPERTIES PASS_REGULAR_EXPRESSION ${failurePattern})
endfunction()
