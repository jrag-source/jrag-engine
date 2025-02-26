# JRAG Engine
Hobbyist C++ game engine/framework made with the [Simple and Fast Multimedia Library (SFML)](https://www.sfml-dev.org/).

## Getting Started
Clone the git repo:
```
git clone https://github.com/jrag-source/jrag-engine.git
```

The project has the following prerequisites:
* (Partially) C++23 compliant compiler.
* CMake v3.31 or higher. 
* SFML 2.6

Optionally, you can use [Conan](https://conan.io/) to manage library dependencies instead of manually installing SFML.

### Development Environment (recommended)
A docker container with a pre-configured environment with all the necessary dependencies to build and run the engine along with examples. It also provides extra tooling such as linters, formatters, and an IDE.

Prerequisites:
* Docker
* Bash

Simply run the environment launch script:
```
./dev_env/launch.sh
```

The first run will build the docker image. Subsequent runs will be much faster to launch as they just create and enter a container using that previously built image.

Inside the container you will have access to:
* GCC compiler
* CMake
* SFML
* Clang-Tidy
* Clang-Format
* VSCodium
* Git

### Build
Once the necessary tooling is installed you can configure the project like so:
```
cmake --preset <configurePreset>
```

And then build:
```
cmake --build --preset <buildPreset> --parallel
```

You can see a list of available configure presets with:
```
cmake --list-presets
```

And build presets:
```
cmake --build --list-presets
```

You can create a custom preset by creating a CMakeUserPresets.json file. Refer to [CMake's documentation](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html) for more information.

If you are using the development environment container you can use the **devenv-debug** or **devenv-release** presets.

### Run
The example binary can be found under the build/${presetName}/example directory.

Here is how you would run the example program built with the **base** configuration:
```
./build/base/example/example
```