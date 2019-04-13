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
* change the `PYTHON_EXECUTABLE` as well as `PYBIND11_PYTHON_VERSION` in `CMakeLists.txt` if it does not match your system settings.

### Create Conda Enviroment
1. Download [anaconda3](https://www.anaconda.com/distribution/#download-section)
2. Either in Windows or Mac Terminal, type:
``` console
conda config --add channels conda-forge
conda create --name blender python=3.7 compas
conda activate blender
pip3 install tessagon
```
3. Add the new created python enviroment folder into system enviroment variables if necessary.

### Replace Blender Python
for details please refer to [Compas Blender](https://compas-dev.github.io/main/environments/blender.html)

#### On Windows
``` console
cd %PROGRAMFILES%\Blender Foundation\Blender\2.80
ren python original_python
mklink /j python %CONDA_PREFIX%
```

#### On Mac
``` consolde
cd /Applications/blender/blender.app/Contents/Resources/2.80
$ mv python original_python
$ ln -s $CONDA_PREFIX python
```

### Compile C++ Source Code
#### Mac
in terimal, `cd` to the project folder and

``` console
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
```

#### Windows
1. Open Visual Studio 2017
2. In menu, "File" - "Open" - "CMake", open CMakeList.txt in the project folder
3. In menu, "CMake" - "Build All"
4. Don't forgot to change compile mode from Debug to Release.

### Run Blender
1. Open `blender/MeshPattern.blender`
2. Click `Run Script`