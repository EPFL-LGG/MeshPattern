cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(MSVC)
	add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE=1)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/")
endif()

message(${CMAKE_MODULE_PATH})
set(EIGEN3_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext/eigen")
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

set(PYBIND11_CPP_STANDARD -std=c++11)
set(PYBIND11_PYTHON_VERSION 3.7)
set(PYTHON_EXECUTABLE "$ENV{HOME}/anaconda3/envs/blender/bin/python3.7")
#set(PYTHON_EXECUTABLE "C:/Users/ziqwang/AppData/Local/Continuum/anaconda2/envs/blender/python.exe")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ext/pybind11)

#ShapeOp
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ext/ShapeOp)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ext/ShapeOp")

#Libigl
find_package(LIBIGL REQUIRED)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)