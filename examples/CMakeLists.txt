#-----------------------------------------------------------------------------
#
#  CMake config
#
#  tgd-header-lib examples
#
#-----------------------------------------------------------------------------

add_executable(tgd-cat tgd-cat.cpp)
target_link_libraries(tgd-cat ${ZLIB_LIBRARIES})

add_executable(tgd-export tgd-export.cpp)
target_link_libraries(tgd-export ${ZLIB_LIBRARIES})

add_executable(tgd-filter tgd-filter.cpp)
target_link_libraries(tgd-filter ${ZLIB_LIBRARIES})

add_executable(tgd-info tgd-info.cpp)
target_link_libraries(tgd-info ${ZLIB_LIBRARIES})

#-----------------------------------------------------------------------------

set(_commands cat
              export
              filter
              info)

foreach(example ${_commands})

    add_test(NAME example_${example}_help COMMAND tgd-${example} -h)

    add_test(NAME example_${example}_bare COMMAND tgd-${example})
    set_tests_properties(example_${example}_bare PROPERTIES WILL_FAIL true)

    add_test(NAME example_${example}_error COMMAND tgd-${example} --nonexistent-option)
    set_tests_properties(example_${example}_error PROPERTIES WILL_FAIL true)

endforeach()

#-----------------------------------------------------------------------------

set(TESTDATA "${CMAKE_SOURCE_DIR}/test/data")

add_test(NAME example_cat_create COMMAND tgd-cat ${TESTDATA}/test-a.png ${TESTDATA}/test-b.mvt ${TESTDATA}/test-c.jpg -o test-tile.tgd)

add_test(NAME example_info_all COMMAND tgd-info test-tile.tgd)
set_tests_properties(example_info_all PROPERTIES PASS_REGULAR_EXPRESSION "^LAYER test-a\n")
set_tests_properties(example_info_all PROPERTIES DEPENDS example_cat_create)

add_test(NAME example_info_layer_b COMMAND tgd-info test-tile.tgd -n test-b)
set_tests_properties(example_info_layer_b PROPERTIES PASS_REGULAR_EXPRESSION "^LAYER test-b\n")
set_tests_properties(example_info_layer_b PROPERTIES DEPENDS example_cat_create)

add_test(NAME example_filter_layer_c COMMAND tgd-filter test-tile.tgd -n test-c -o test-c.tgd)
set_tests_properties(example_filter_layer_c PROPERTIES DEPENDS example_cat_create)

add_test(NAME example_export_layer_c COMMAND tgd-export test-c.tgd -o test-c-generated.jpg)
set_tests_properties(example_export_layer_c PROPERTIES DEPENDS example_filter_layer_c)

