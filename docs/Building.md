# Building tetrad-engine
### Requirements
##### Windows
  * Any C++11-compliant compiler
    * If using Visual Studio, must be 2012 or later
  * [CMake](https://cmake.org/download/) (version 3.0 or greater)
  * [Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html) (optional, needed only for building documentation)

##### Linux
  * Any C++11-compliant compiler
  * [CMake](https://cmake.org/download/) (version 3.0 or greater)
  * [Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html) (optional, needed only for building documentation)
  * Developer packages for OpenGL and Google perftools
     * On Debian-based distributions, the following command can be used to install these dependencies:
     `sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev xorg-dev libgoogle-perftools-dev`

### Building the code for the first time
  1. Clone the repo with `git clone --recurse-submodules https://github.com/akhouderchah/tetrad-engine.git`.
  If the repo was already cloned without `--recurse-submodules`, `git submodule update --init --recursive`
  can be run to fully initialize the repo.
  1. Run CMake, specifying `${PROJECT_DIR}/src` as the source directory, and `${PROJECT_DIR}/bin/${BUILD_NAME}`
  as the build directory (where `${PROJECT_DIR}` is the path to the top tetrad-engine directory and `${BUILD_NAME}` is the
  desired name of the build folder, such as 'debug' or 'release')

  1. Run "configure", set the desired options&mdash;the most common option being `CMAKE_BUILD_TYPE` for doing `DEBUG`
  or `RELEASE` builds&mdash;and then run "generate"

  1. Build the code using the build files generated in `${PROJECT_DIR}/bin/${BUILD_NAME}`
  >##### Note for Visual Studio builds
  >After opening the .sln file, set "tetrad-game" as the startup project in order for F5 to launch tetrad-game after building.

  >##### Note for Makefile-based builds
  >Take advantage of multi-core systems by using the -j flag. An 8-core system would use `make -j9` for optimal build speeds.

### Building the code after the first time
  1. Simply perform Step 3 of the above process. CMake will be triggered on its own if it needs to run
  > ##### Note
  > The one exception here is when new files are added. In this case, the `${PROJECT_DIR}/src/CMakeLists.txt` file must be
  > touched before performing Step 3.

### Building the documentation
  1. Ensure your system has Doxygen installed

  1. If not already done, generate the build files with CMake, as detailed in
  [Building the code for the first time](#building-the-code-for-the-first-time)

  1. Build the "doc" target
     * With Makefile-based builds, this is done with `make doc`. Optionally, one can also go into the 'latex' folder and
     run `make` to generate a pdf of the documentation
     * With Visual Studio, this is done by building the "doc" project
