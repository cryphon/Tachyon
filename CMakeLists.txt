cmake_minimum_required(VERSION 3.10)
project(Tachyon)



option(TACHYON_BUILD_TESTS "Build tests" ON)
option(TACHYON_BUILD_BENCHMARKS "Build benchmarks" ON)
option(TACHYON_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)



set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(tachyon INTERFACE)
target_include_directories(tachyon
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

# Warnings
set(_warn_flags -Wall -Wextra -Wpedantic)
if(MSVC)
    set(_warn_flags /W4)
endif()
target_compile_options(tachyon INTERFACE ${_warn_flags})
if(TACHYON_WARNINGS_AS_ERRORS)
    if(MSVC)
        target_compile_options(tachyon INTERFACE /WX)
    else()
        target_compile_options(tachyon INTERFACE -Werror)
    endif()
endif()


# Tachyon tests

if(TACHYON_BUILD_TESTS)
    enable_testing()

    # Find all .cpp files in tests/
    file(GLOB TEST_SOURCES "${CMAKE_SOURCE_DIR}/tests/*.cpp")

    foreach(test_src ${TEST_SOURCES})
        get_filename_component(test_name ${test_src} NAME_WE)

        add_executable(${test_name} ${test_src})
        target_link_libraries(${test_name} PRIVATE tachyon)

        # Add to CTest
        add_test(NAME ${test_name} COMMAND ${test_name})
    endforeach()
endif()


# Tachyon benchmarks
if(TACHYON_BUILD_BENCHMARKS)
    # Find all .cpp files in benchmarks/
    file(GLOB BENCH_SOURCES "${CMAKE_SOURCE_DIR}/benchmarks/*.cpp")

    foreach(bench_src ${BENCH_SOURCES})
        get_filename_component(bench_name ${bench_src} NAME_WE)

        add_executable(${bench_name} ${bench_src})
        target_link_libraries(${bench_name} PRIVATE tachyon)

        # (Optional) Set higher optimization for benchmarks even in Debug mode
        target_compile_options(${bench_name} PRIVATE -O3 -march=native)
    endforeach()
endif()


set(MAIN_TEST_FILES
    tests/test_ringbuffer.cpp
)


install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)



