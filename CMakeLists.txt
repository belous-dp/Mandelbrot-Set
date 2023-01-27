cmake_minimum_required(VERSION 3.22)
project(mandelbrot)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

add_executable(mandelbrot "main.cpp" "main_window.cpp" "picture.cpp" "perf_helper.cpp")

file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/logs")

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_options(mandelbrot PUBLIC -D_GLIBCXX_DEBUG)
endif ()

set_property(TARGET mandelbrot PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_link_libraries(mandelbrot PUBLIC Qt6::Core Qt6::Widgets)
