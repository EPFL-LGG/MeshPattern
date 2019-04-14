#include <vector>
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "iglMesh.h"
#include "ShapeOpt.h"

namespace py = pybind11;
using namespace pybind11::literals;

PYBIND11_MODULE(MeshPattern, m)
{
    py::class_<iglMesh>(m, "iglMesh")
            .def(py::init())
            .def("loadMesh", &iglMesh::loadMesh)
            .def("getUVs", &iglMesh::getUVs)
            .def("getVertices",  &iglMesh::getVertices)
            .def("getFaces",  &iglMesh::getFaces);


    py::class_<ShapeOpt>(m, "ShapeOpt")
            .def(py::init())
            .def("loadMesh", &ShapeOpt::loadMesh)
            .def("getMesh", &ShapeOpt::getMesh)
            .def("addPlanarConstraint", &ShapeOpt::addPlanarConstraint)
            .def("addRegularConstraint", &ShapeOpt::addRegularConstraint)
            .def("addBoundaryConstraint", &ShapeOpt::addBoundaryConstraint)
            .def("runShapeOp", &ShapeOpt::runShapeOp);

}
