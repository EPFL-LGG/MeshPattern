# MeshPattern
Created for compas c++ workshop. Solution for using C++ code in blender with the help of pybind11 and compas library. 

## Prerequisite
### 1. CMake
#### Mac
in terminal type: `brew install cmake`

#### Windows
1. Download Visual Studio 2017
2. Open Visual Studio 2017. Click "Tools" - "Get Tools and Feature" to open the Visual Studio Installer. 
3. Choose Use C++ and Linux Developement options which would download necessary cmake component.

### 2. Blender
Download [Blender 2.8.0 beta](https://builder.blender.org/download/)

### 3. Git Submodule
The following library are located in folder `ext/`, they all automatically dowload. 

0. **eigen**
1. **libigl**
2. **pybind**
3. **shapeop**

## Installation
### Before Compile
Before using cmake to compile the file, make sure followings are correct

* `cmake` version >= 3.12
* `python` version >= 3.7
* confirm your anaconda python location by typing `which python` in terminal
* change the `PYTHON_EXECUTABLE` as well as `PYBIND11_PYTHON_VERSION` in `compas_libigl/CMakeLists.txt` if it does not match your system settings.

### Compile C++ Source Code
#### Mac
in terimal, `cd` to the project folder and

```console
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
```

#### Windows