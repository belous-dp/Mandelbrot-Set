# Mandelbrot

A multi-threaded C++ application for drawing and exploring [Mandelbrot set](https://en.m.wikipedia.org/wiki/Mandelbrot_set). Offers multiple color palettes and responsive UI with real-time rendering. Uses Qt for developing GUI.

Short demo:

https://user-images.githubusercontent.com/46475907/218579722-5c3443ac-3657-4ca4-80b7-eab5c96a72b1.mp4

## Installation

### Prerequisites
* Qt 6
* CMake
* Git
* Your favorite IDE

### Using QtCreator
Open folder with the cloned project.

### Windows. vcpkg + Visual Studio 2022
1. Add vcpkg installation directory to system variables
    1. Search -> "variables"
    2. Edit the system environment variables
    3. Environment variables
    4. New user variable
    5. Variable name: "VCPKG_DIR" (without quotes)
    5. Variable value: <path to vcpkg installation directory>
2. Locate to `$VCPKG_DIR$`
3. `git pull`
4. `./bootstrap-vcpkg.bat`
5. `./vcpkg.exe install qtbase[core,gui,widgets]:x64-windows-static`
6. Locate to the directory which you want clone project to
7. Clone this repo
8. Open Visual Studio 2022 (make sure "C++ CMake tools for Windows" marked in Visual Studio installer -> Visual Studio 2022 -> modify -> Desktop Development with C++)
9. Choose "Open folder"
10. Navigate to the clonned repo
11. Choose configuration type ("Release" is the best in terms of performance)
12. Project -> Configure mandelbrot
13. Build -> Build all
14. Debug -> Start without debugging

### Linux. vcpkg + CLion
1. Add vcpkg installation directory to system environment variables as "VCPKG_DIR" (without quotes)
2. Locate to `$VCPKG_DIR$`
3. `git pull`
4. `./bootstrap-vcpkg.sh`
5. `./vcpkg install qtbase[core,gui,widgets]:x64-linux`
6. Locate to the directory which you want clone project to
7. Clone this repo
8. In CLion open this directory
9. Choose configuration type ("Release" is the best in terms of performance)
10. Run

## Project structure

* [main.cpp](./main.cpp) starts application
* [main_window](./main_window.cpp) configures menus, status bar and updated them when needed
* [picture](./picture.cpp) is responsible for communication with worker thread and displaying image
* [worker thread(s)](./workers.cpp) does all hard computations using multi-threading and colors the image

// todo
### Moving (shifting) image workflow
### Zooming image workflow
### Coloring
### Dynamic scaling

