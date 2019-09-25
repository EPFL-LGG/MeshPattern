from tessagon.types.hex_tessagon import HexTessagon
from tessagon.adaptors.list_adaptor import ListAdaptor

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
    pass