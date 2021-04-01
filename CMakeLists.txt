# Copyright (c) 2019-2020 VORtech b.v.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.9)

project(vt_timers CXX)



### User options
option(
    VT_TIMERS_BUILD_Fortran_LIBS
    "Build vt_fortran_timers library (.so/.dll) to use vt-timers from Fortran"
    ON)

option(
    VT_TIMERS_BUILD_SHARED_LIBS
    "Build shared vt-timers libraries (.so/.dll); overwrites CMake's BUILD_SHARED_LIBS"
    ON)


### Compile options

set(VT_TIMERS_GNU_COMPILE_OPTIONS
    -std=c++11
    -pedantic-errors
    -Wall
    -Wextra
    -Wcast-align
    -Wcast-qual
    -Wfloat-equal
    -Wlogical-op
    -Wmissing-include-dirs
    -Wold-style-cast
    -Woverloaded-virtual
    -Wsign-conversion
    -Wsuggest-override
    -Wundef
    -Wuseless-cast)

set(VT_TIMERS_Clang_COMPILE_OPTIONS
    -pedantic-errors
    -Weverything
    -Wno-c++98-compat
    -Wno-c++98-compat-pedantic
    -Wno-missing-braces
    -Wno-padded)

set(VT_TIMERS_MSVC_COMPILE_OPTIONS
    /W4
    /volatile:iso
    /Zc:strictStrings
    /Zc:rvalueCast)


### vt_timers shared library

if(MSVC AND VT_TIMERS_BUILD_SHARED_LIBS)
    set(VT_TIMERS_IMPORT_DEFINES "-DVT_TIMERS_ATTR=__declspec(dllimport)")
    set(VT_TIMERS_EXPORT_DEFINES "-DVT_TIMERS_ATTR=__declspec(dllexport)")
endif()

if(VT_TIMERS_BUILD_SHARED_LIBS)
    set(VT_TIMERS_LIB_TYPE SHARED)
else()
    set(VT_TIMERS_LIB_TYPE STATIC)
endif()

add_library(vt_timers ${VT_TIMERS_LIB_TYPE}
    "src/vt_timers.cpp"
    "src/error_handling.cpp")
set_target_properties(vt_timers PROPERTIES DEBUG_POSTFIX "d")
target_compile_definitions(
    vt_timers
    PRIVATE ${VT_TIMERS_EXPORT_DEFINES}
    INTERFACE ${VT_TIMERS_IMPORT_DEFINES})
target_compile_options(
    vt_timers
    PRIVATE ${VT_TIMERS_${CMAKE_CXX_COMPILER_ID}_COMPILE_OPTIONS})
target_include_directories(vt_timers
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")

if  (VT_TIMERS_BUILD_Fortran_LIBS) 
    enable_language(Fortran)
endif()

find_package(OpenMP)
if(OPENMP_FOUND)
    target_link_libraries(vt_timers OpenMP::OpenMP_CXX)
endif()

if (VT_TIMERS_BUILD_Fortran_LIBS) 
    add_library(vt_fortran_timers ${VT_TIMERS_LIB_TYPE} "src/vt_timers.f90")
    target_link_libraries(vt_fortran_timers vt_timers)
    if(OPENMP_FOUND)
        target_link_libraries(vt_fortran_timers OpenMP::OpenMP_Fortran)
    endif()
endif()


### Install target that can be used by projects that use add_subdirectory to include vt_timers

install(TARGETS vt_timers
        RUNTIME DESTINATION "bin" COMPONENT vt_timers-basic   # (dll's and exe's go to bin, .so's go to lib, .lib's to to lib)
        LIBRARY DESTINATION "lib" COMPONENT vt_timers-basic
        ARCHIVE DESTINATION "lib" COMPONENT vt_timers-basic)
add_custom_target( install.vt_timers
    ${CMAKE_COMMAND} -DCOMPONENT=vt_timers-basic -DCMAKE_INSTALL_CONFIG_NAME=$<CONFIG>
                     -P cmake_install.cmake)


### Tests

option(VT_TIMERS_ENABLE_TESTS "Enable the compilation of tests for timers library." OFF)

if (VT_TIMERS_ENABLE_TESTS)

    if (VT_TIMERS_BUILD_Fortran_LIBS) 
        # Test executable
        add_library(vt_fortran_test STATIC
            "test/vt_fortran_test.f90")
        target_link_libraries(vt_fortran_test vt_fortran_timers )
    endif()

    add_executable(vt_timers_test
        "test/sleep.cpp"
        "test/test_vt_timers.cpp"
        "test/test_main.cpp")
    target_link_libraries(vt_timers_test vt_timers gtest)
    if (VT_TIMERS_BUILD_Fortran_LIBS) 
        target_compile_definitions(vt_timers_test PRIVATE -DVT_TIMERS_BUILD_FORTRAN_LIBS)  
        target_link_libraries(vt_timers_test vt_fortran_test)
    endif()

    # Download and unpack googletest at configure time
    configure_file(CMakeLists.txt.gtest googletest-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
      RESULT_VARIABLE result
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
    if(result)
      message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
      RESULT_VARIABLE result
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
    if(result)
      message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif()

    # Prevent overriding the parent project's compiler/linker settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest directly to our build. This defines the gtest and gtest_main targets.
    add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                     ${CMAKE_BINARY_DIR}/googletest-build
                     EXCLUDE_FROM_ALL)
endif()
