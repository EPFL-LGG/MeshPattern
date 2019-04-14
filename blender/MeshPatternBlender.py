import numpy as np
import os
import sys
import compas
from compas.datastructures import Mesh
from compas.plotters import MeshPlotter
from compas_blender.utilities import xdraw_mesh, xdraw_cylinders
from compas.geometry import *

file_dir = os.path.dirname(os.path.realpath(__file__))
dir_path = os.path.dirname(file_dir) 
sys.path.append(file_dir)
sys.path.append(file_dir + "/../lib")
sys.path.append(dir_path + "/../lib")

import MeshPattern
from MeshPattern import *

from tessagon.types.hex_tessagon import HexTessagon
from tessagon.adaptors.list_adaptor import ListAdaptor

def drawMeshVF(v, f):
	xdraw_mesh(v, None, f)

def drawMeshVEF(v, e, f):
	xdraw_mesh(v, e, f)

def create2DPattern():
	#Create 2D pattern in [-1, 1]x[-1,1]
	options = {
    	'function': lambda u, v: [u, v, 0],
    	'u_range': [0, 1.0],
    	'v_range': [0, 1.0],
    	'u_num': 16,
    	'v_num': 10,
    	'u_cyclic': False,
    	'v_cyclic': False,
    	'adaptor_class' : ListAdaptor
  	}
	tessagon = HexTessagon(**options)
	vertices = tessagon.create_mesh()['vert_list']
	faces = tessagon.create_mesh()['face_list']
	mesh = iglMesh()
	mesh.loadMesh(vertices, faces)
	return mesh


if __name__ == "__main__":
	#Use compas to read obj file 
	surfaceOBJ = Mesh.from_obj(dir_path + '/../data/minimal_surface.obj')
	#surfaceOBJ = Mesh.from_obj(dir_path + '/data/minimal_surface.obj')
	
	vertices, faces = surfaceOBJ.to_vertices_and_faces()
	for key, attr in surfaceOBJ.vertices(True):
		attr['x'] = vertices[key][0]
		attr['y'] = -vertices[key][2]
		attr['z'] = vertices[key][1]

	vertices, faces = surfaceOBJ.to_vertices_and_faces()

	#Create a C++ iglMesh class and enter input OBJ mesh
	mesh = iglMesh()
	mesh.loadMesh(vertices, faces)
	mesh.paramertization_lscm()

	#Process mesh
	pattern2D = create2DPattern()
	pattern2D.paramertization_simple()
	mesh.mapMesh3D_AABB(pattern2D)

    #ShapeOp
	solver = ShapeOpt()
	solver.loadMesh(pattern2D)
	solver.addPlanarConstraint(1.0)
	solver.addBoundaryConstraint(0.5)
	solver.addRegularConstraint(1.0)
	solver.runShapeOp(10)
	pattern3D = solver.getMesh()

	#Return proccessd mesh and get its vertices and faces

	
	wire = pattern3D.saveWireFrame(0.01, 10)
	vertices = wire.getVertices()
	faces = wire.getFaces()
	drawMeshVF(vertices, faces)