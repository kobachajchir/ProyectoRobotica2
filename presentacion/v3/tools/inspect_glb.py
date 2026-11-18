import bpy, sys
from mathutils import Vector
bpy.ops.wm.read_factory_settings(use_empty=True)
p = sys.argv[sys.argv.index('--')+1]
bpy.ops.import_scene.gltf(filepath=p)
mn=Vector((1e9,)*3); mx=Vector((-1e9,)*3)
for o in bpy.data.objects:
    if o.type=='MESH':
        for c in o.bound_box:
            w=o.matrix_world@Vector(c); mn=Vector(map(min,mn,w)); mx=Vector(map(max,mx,w))
print("BBOX",tuple(round(v,4) for v in mn),tuple(round(v,4) for v in mx))
names=sorted(o.name for o in bpy.data.objects if o.type=='MESH')
print("NMESH",len(names)); print("NAMES"," | ".join(names[:200]))
print("MATS"," | ".join(m.name for m in bpy.data.materials))
