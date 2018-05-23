#-----------------------------------------------------------------------------
#
#  CMake config
#
#  tgd-header-lib examples
#
#-----------------------------------------------------------------------------

include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/include-external")

add_executable(tgd-cat tgd-cat.cpp)
target_link_libraries(tgd-cat ${ZLIB_LIBRARIES})

add_executable(tgd-export tgd-export.cpp)
target_link_libraries(tgd-export ${ZLIB_LIBRARIES})

add_executable(tgd-filter tgd-filter.cpp)
target_link_libraries(tgd-filter ${ZLIB_LIBRARIES})

add_executable(tgd-layer tgd-layer.cpp)
target_link_libraries(tgd-layer ${ZLIB_LIBRARIES})

add_executable(tgd-list tgd-list.cpp)
target_link_libraries(tgd-list ${ZLIB_LIBRARIES})

