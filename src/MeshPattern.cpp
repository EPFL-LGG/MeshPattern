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
            .def("paramertization_simple", &iglMesh::paramertization_simple)
            .def("paramertization_lscm", &iglMesh::paramertization_lscm)
            .def("mapMesh3D_simple", &iglMesh::mapMesh3D_simple)
            .def("mapMesh3D_AABB", &iglMesh::mapMesh3D_AABB)
            .def("saveWireFrame", &iglMesh::saveWireFrame)
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
