import numpy as np
import os
import sys
import compas
from compas.datastructures import Mesh
from compas.plotters import MeshPlotter
from compas_blender.utilities import *
from MeshPattern import *

def processMesh(vertices, faces, pv, pf):
    #load iglMesh
    mesh = iglMesh()
    mesh.loadMesh(vertices, faces)

    #do parametrization
    mesh.parametrization_simple()
    vertices = mesh.getUVs()
    faces = mesh.getFaces()
    #drawMeshVF(vertices, faces)

    #do lift
    pattern2D = iglMesh()
    pattern2D.loadMesh(pv, pf)
    mesh.mapMesh3D_AABB(pattern2D)
    vertices = pattern2D.getVertices()
    faces = pattern2D.getFaces()
    

    #do shapeop
    #ShapeOp
    solver = ShapeOpt()
    solver.loadMesh(pattern2D)
    solver.addPlanarConstraint(1.0)
    solver.addRegularConstraint(1.0)
    solver.addBoundaryConstraint(1.0)
    solver.runShapeOp(10)
    pattern3D = solver.getMesh()
    vertices = pattern3D.getVertices()
    faces = pattern3D.getFaces()

    #output wireframe
    wire = pattern3D.saveWireFrame(0.01, 10)
    vertices = wire.getVertices()
    faces = wire.getFaces()

    return [vertices, faces]

if __name__ == "__main__":
    import hex_tessagon
    # get exact directory of current blender file
    file_dir = os.path.dirname(os.path.realpath(__file__))
    dir_path = os.path.dirname(file_dir)
    
    surfaceOBJ = Mesh.from_obj(dir_path + '/../data/minimal_surface.obj')
    # iterate each vertex in compas mesh and swap its Y and Z
    for key, attr in surfaceOBJ.vertices(True):
        y = attr['y']
        attr['y'] = -attr['z']
        attr['z'] =  y

    #getting the vertices and faces list from compas mesh
    vertices, faces = surfaceOBJ.to_vertices_and_faces()

    [pv, pf] = hex_tessagon.create2DPattern()
    [vertices, faces] = processMesh(vertices, faces, pv, pf)