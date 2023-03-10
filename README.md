# Mandelbrot

A multithreaded C++ application for drawing and exploring [Mandelbrot set](https://en.m.wikipedia.org/wiki/Mandelbrot_set). Offers multiple color palettes and responsive UI with real-time rendering. Uses Qt for developing GUI.

Short demo:

https://user-images.githubusercontent.com/46475907/218579722-5c3443ac-3657-4ca4-80b7-eab5c96a72b1.mp4

## Installation

### Prerequisites
* Qt 6
* CMake
* Git
* Your favorite IDE

### Using QtCreator
Open folder with the cloned project. Compile and run the project.

### Windows. vcpkg + Visual Studio 2022
1. Add vcpkg installation directory to system variables
    1. Search -> "variables"
    2. Edit the system environment variables
    3. Environment variables
    4. New user variable
    5. Variable name: "VCPKG_DIR" (without quotes)
    5. Variable value: <path to vcpkg installation directory>
2. Locate to `$VCPKG_DIR`
3. `git pull`
4. `.\bootstrap-vcpkg.bat`
5. `.\vcpkg.exe install qtbase[core,gui,widgets]:x64-windows-static`
6. Locate to the directory which you want clone project to
7. Clone this repo
8. Open Visual Studio 2022 (make sure "C++ CMake tools for Windows" marked in Visual Studio installer -> Visual Studio 2022 -> Modify -> Desktop Development with C++)
9. Choose "Open folder"
10. Navigate to the clonned repo
11. Choose configuration type ("Release" is the best in terms of performance)
12. Project -> Configure mandelbrot
13. Build -> Build all
14. Select Startup Item -> mandelbrot.exe
15. Debug -> Start without debugging

### Linux. vcpkg + CLion
1. Add vcpkg installation directory to system environment variables as "VCPKG_DIR" (without quotes)
2. Locate to `$VCPKG_DIR`
3. `git pull`
4. `./bootstrap-vcpkg.sh`
5. `./vcpkg install qtbase[core,gui,widgets]:x64-linux`
6. Locate to the directory which you want clone project to
7. Clone this repo
8. In CLion open this directory
9. Choose configuration type ("Release" is the best in terms of performance)
10. Run

## Project structure

### Overall workflow

* [main.cpp](./main.cpp) starts application
* [main_window](./main_window.cpp) configures menus, status bar and updated them when needed
* [picture](./picture.cpp) is responsible for communication with worker thread and displaying image
* [worker thread(s)](./workers.cpp) does all hard computations using multithreading and colors the image

The central widget of the window is [class picture](./picture.h). User interacts with this widget and rendered images are shown there. As soon as Qt recieves a signal from a user (e.g. mouse move, wheel scroll), Qt calls the appropriate function in the [picture](./picture.h#L21). In such functions the necessary data (for example, the image layout) is updated, the signal about new rendering task [is sent](./picture.cpp#L33) to the [worker thread](./workers.h), and, while the worker thread is rendering new image, the old image is changed according to the user's request and is shown to the user. When the new image is rendered, the worker thread [sends](./workers.cpp#L197) it back to the [picture](./picture.cpp#L27), which is [shows](./picture.cpp#L69) it.

### Design decisions

Almost all interaction between different modules of the application, such as the picture and the worker thread, is implemented using the [signals and slots](https://doc.qt.io/qt-6/signalsandslots.html) mechanism.

To prevent application 'freezing', the program's logic is devided into multiple threads:

1. The main thread maintains the information about the image, shows temporary dummy images (while the real one is rendering), processes events from the event loop.
2. The worker thread only renders images that are requested by the main thread.
    1. To accelerate the rendering, the worker thread [creates](./workers.cpp#L197) additional threads and splits a work into equal parts (height of an image is divided by the number of threads).
    2. Relaxed atomics are used for synchronization between worker threads.

> Before reading the next paragraph, make sure you understand [escape-time algorithm](https://en.wikipedia.org/wiki/Mandelbrot_set#Computer_drawings) that is used for computating the Mandelbrot set.

Among the other things, the following optimization is used. The rendering is done progressively: initially, the number of steps in escape-time algorithm is 100. The image is rendered really quickly and is sent to the user. Then the number of steps is increased by 100. It takes longer time to render an image with nsteps=200, but it has higher quality. And so on the number of steps is increased with each iteration. This workflow gives the user the feeling of a progress. In sectuion [dynamic scaling](todo) this process is described in more details.

Another optimization is to frequently check for new render tasks in the render queue. When such task is discovered, all worker threads stops the current task as soon as possible. This is done to make sure no resources are wasted rendering the image that is no longer needed.

In the result of the above design decisions, the application is responsive and has almost no freezes, and the rendering is done effectively.

### Moving (shifting) image details
todo
### Zooming image details
### Coloring
### Dynamic scaling

