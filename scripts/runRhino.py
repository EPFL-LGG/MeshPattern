from compas.datastructures import Mesh
from compas_rhino.artists import MeshArtist
from compas.rpc import Proxy
import os
if __name__=="__main__":
# get exact directory of current blender file
    file_dir = os.path.dirname(os.path.realpath(__file__))
    dir_path = os.path.dirname(file_dir)
    surfaceOBJ = Mesh.from_obj(dir_path + '/data/minimal_surface.obj')

    # iterate each vertex in compas mesh and swap its Y and Z
    for key, attr in surfaceOBJ.vertices(True):
        y = attr['y']
        attr['y'] = -attr['z']
        attr['z'] =  y

    #getting the vertices and faces list from compas mesh
    vertices, faces = surfaceOBJ.to_vertices_and_faces()

    hex_tessagon = Proxy("mesh_pattern.hex_tessagon")
    cppMesh = Proxy("mesh_pattern.cppMesh")
    [pv, pf] = hex_tessagon.create2DPattern()
    [vertices, faces] = cppMesh.processMesh(vertices, faces, pv, pf)
    mesh = Mesh.from_vertices_and_faces(vertices, faces)
    artist = MeshArtist(mesh)
    #artist.draw_vertices()
    artist.draw_edges()
    artist.draw_faces()
    artist.redraw()