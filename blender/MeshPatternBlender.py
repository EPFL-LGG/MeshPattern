import numpy as np
import os
import sys
import compas
from compas.datastructures import Mesh
from compas.plotters import MeshPlotter
from compas_blender.utilities import xdraw_mesh, xdraw_cylinders
from compas.geometry import *
from tessagon.types.hex_tessagon import HexTessagon
from tessagon.adaptors.list_adaptor import ListAdaptor

# get exact directory of current blender file
file_dir = os.path.dirname(os.path.realpath(__file__))
dir_path = os.path.dirname(file_dir) 
sys.path.append(file_dir)
sys.path.append(file_dir + "/../lib")
sys.path.append(dir_path + "/../lib")

from MeshPattern import *

# Draw Mesh with vertices and faces
def drawMeshVF(v, f):
	xdraw_mesh(v, None, f)

# Create a 2D Hexagon pattern in [0, 1]x[0, 1]
# return a list of [vertices, faces]
def create2DPattern():
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
	return [vertices, faces]

if __name__ == "__main__":
    # you can change here to import your own file, like :
    # surfaceOBJ = Mesh.from_obj(dir_path + '/../data/igloo.obj')
    surfaceOBJ = Mesh.from_obj(dir_path + '/../data/minimal_surface.obj')

    #getting the vertices and faces list from compas mesh
    vertices, faces = surfaceOBJ.to_vertices_and_faces()

    #draw the mesh in the blender scene
    drawMeshVF(vertices, faces)
