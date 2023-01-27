cmake_minimum_required(VERSION 3.22)
project(mandelbrot)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

add_executable(mandelbrot "main.cpp" "main_window.cpp" "picture.cpp" "perf_helper.cpp" "workers.cpp" "render_layout.cpp")

file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/logs")

target_compile_options(mandelbrot PRIVATE -Wall)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(mandelbrot PRIVATE -Wno-sign-compare -Wshadow-uncaptured-local -pedantic)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(mandelbrot PRIVATE -Wno-sign-compare -Wshadow=compatible-local -pedantic)
endif ()

option(USE_SANITIZERS "Enable to build with undefined, leak and address sanitizers" OFF)
if (USE_SANITIZERS)
    target_compile_options(mandelbrot PUBLIC -fsanitize=address,undefined,leak -fno-sanitize-recover=all)
    target_link_options(mandelbrot PUBLIC -fsanitize=address,undefined,leak)
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(mandelbrot PUBLIC -stdlib=libc++)
endif ()


if (CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_options(mandelbrot PUBLIC -D_GLIBCXX_DEBUG)
endif ()

set_property(TARGET mandelbrot PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_link_libraries(mandelbrot PUBLIC Qt6::Core Qt6::Widgets)
