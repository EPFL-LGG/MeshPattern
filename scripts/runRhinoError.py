from compas.datastructures import Mesh
from compas_rhino.artists import MeshArtist
from compas.rpc import Proxy
#================================================#
from mesh_pattern import hex_tessagon
[vertices, faces] = hex_tessagon.create2DPattern()
mesh = Mesh.from_vertices_and_faces(vertices, faces)
#================================================#


# artist = MeshArtist(mesh)
# artist.draw_vertices()
# artist.draw_edges()
# artist.draw_faces()
# artist.redraw()