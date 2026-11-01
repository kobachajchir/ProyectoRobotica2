"""Vista de entrega desde el GLB comprimido, no desde las mallas originales."""
import bpy,json
from pathlib import Path
HERE=Path('D:/ProyectoRobotica2/AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/web_blanco')
if bpy.data.collections.get('AFR2_Preview_GLB_600k'):raise RuntimeError('Preview ya importado; no duplicar')
before=set(bpy.data.objects)
bpy.ops.import_scene.gltf(filepath=str(HERE/'AFR2_Brazo_v17_Blanco.glb'))
preview=bpy.data.collections.new('AFR2_Preview_GLB_600k');bpy.context.scene.collection.children.link(preview)
items=set(bpy.data.objects)-before
for o in items:
 for c in list(o.users_collection):c.objects.unlink(o)
 preview.objects.link(o)
for o in items:o['preview_delivery']=True
bpy.data.collections['AFR2_Brazo_v17_Web'].hide_render=True
bpy.data.collections['AFR2_Brazo_v17_Web'].hide_viewport=True
meshes=[o for o in items if o.type=='MESH']
print(json.dumps(dict(imported_objects=len(items),mesh_objects=len(meshes),materials=sorted({m.name for o in meshes for m in o.data.materials if m}),file=str(HERE/'AFR2_Brazo_v17_Blanco.glb'))))
