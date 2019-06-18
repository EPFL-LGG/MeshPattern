import compas
from compas.rpc import Proxy

MeshPattern = Proxy('MeshPattern')
mesh = MeshPattern.iglMesh()