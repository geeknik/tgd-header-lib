#-----------------------------------------------------------------------------
#
#  CMake config
#
#  tgd-header-lib tests
#
#-----------------------------------------------------------------------------

include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/catch")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

set(TEST_SOURCES buffer
                 encoding
                 file_io
                 layer
                 memory_io
                 stream
                 tile)

string(REGEX REPLACE "([^;]+)" "t/test_\\1.cpp" _test_sources "${TEST_SOURCES}")

add_executable(unit-tests test_main.cpp ${_test_sources})
target_link_libraries(unit-tests ${ZLIB_LIBRARIES})

add_test(NAME unit-tests
         COMMAND unit-tests)

#-----------------------------------------------------------------------------
