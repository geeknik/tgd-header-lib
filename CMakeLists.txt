#-----------------------------------------------------------------------------
#
#  CMake config
#
#  tgd-header-lib
#
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

#-----------------------------------------------------------------------------

project(tgd-header-lib)

set(TGD_HEADER_LIB_VERSION_MAJOR 0)
set(TGD_HEADER_LIB_VERSION_MINOR 0)
set(TGD_HEADER_LIB_VERSION_PATCH 1)

set(TGD_HEADER_LIB_VERSION
    "${TGD_HEADER_LIB_VERSION_MAJOR}.${TGD_HEADER_LIB_VERSION_MINOR}.${TGD_HEADER_LIB_VERSION_PATCH}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#-----------------------------------------------------------------------------

option(WERROR "Add -Werror flag to build (turns warnings into errors)" ON)

if(MSVC)
    add_definitions(-std=c++11 /W3)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
else()
    add_definitions(-std=c++11 -Wall -Wextra -pedantic -Wsign-compare -Wconversion)
#    add_definitions(-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-documentation -Wno-switch-enum -Wno-weak-vtables -Wno-padded -Wno-documentation-unknown-command -Wno-exit-time-destructors -Wno-missing-prototypes)
    if(WERROR)
        add_definitions(-Werror)
    endif()
endif()

include_directories("${CMAKE_SOURCE_DIR}/include")


#-----------------------------------------------------------------------------
#
#  Find dependencies
#
#-----------------------------------------------------------------------------

find_package(ZLIB REQUIRED)

include_directories(${ZLIB_INCLUDE_DIR})


#-----------------------------------------------------------------------------
#
#  Optional "clang-tidy" target
#
#-----------------------------------------------------------------------------
message(STATUS "Looking for clang-tidy")
find_program(CLANG_TIDY NAMES clang-tidy clang-tidy-6.0 clang-tidy-5.0)

if(CLANG_TIDY)
    message(STATUS "Looking for clang-tidy - found ${CLANG_TIDY}")
    add_custom_target(clang-tidy
        ${CLANG_TIDY}
        -p ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/examples/*.cpp
        ${CMAKE_SOURCE_DIR}/test/*.cpp
        ${CMAKE_SOURCE_DIR}/test/t/*.cpp
    )
else()
    message(STATUS "Looking for clang-tidy - not found")
    message(STATUS "  Build target 'clang-tidy' will not be available.")
endif()


#-----------------------------------------------------------------------------
#
#  Optional "cppcheck" target
#
#-----------------------------------------------------------------------------
message(STATUS "Looking for cppcheck")
find_program(CPPCHECK NAMES cppcheck)

if(CPPCHECK)
    message(STATUS "Looking for cppcheck - found")
    add_custom_target(cppcheck
        ${CPPCHECK}
        -Uassert --std=c++11 --enable=all
        ${CMAKE_SOURCE_DIR}/examples/*.cpp
        ${CMAKE_SOURCE_DIR}/test/*.cpp
        ${CMAKE_SOURCE_DIR}/test/t/*.cpp
    )
else()
    message(STATUS "Looking for cppcheck - not found")
    message(STATUS "  Build target 'cppcheck' will not be available.")
endif()


#-----------------------------------------------------------------------------
#
#  Include what you use
#
#-----------------------------------------------------------------------------
message(STATUS "Looking for iwyu")
find_program(IWYU_TOOL NAMES iwyu_tool)

if(IWYU_TOOL)
    message(STATUS "Looking for iwyu - found")
    add_custom_target(iwyu
        ${IWYU_TOOL} -p ${CMAKE_BINARY_DIR}
    )
else()
    message(STATUS "Looking for iwyu - not found")
    message(STATUS "  Build target 'iwyu' will not be available.")
endif()


#-----------------------------------------------------------------------------
#
#  Installation
#
#-----------------------------------------------------------------------------

install(DIRECTORY include/tgd_header DESTINATION include)


#-----------------------------------------------------------------------------

enable_testing()

add_subdirectory(examples)

add_subdirectory(test)


#-----------------------------------------------------------------------------
