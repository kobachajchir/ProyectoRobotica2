"""Crea un auxiliar separado; no incorpora brazo ni auto al GLB/Blend."""
import bpy,json,hashlib,shutil
from pathlib import Path
HERE=Path('D:/ProyectoRobotica2/AutoFinalRobo2-CAD/mecanica/modelos-auxiliares/cubo-50mm')
ROOT=HERE.parents[3]
data=json.loads((HERE/'cubo_fusion.json').read_text(encoding='utf-8'))
name='AFR2_Auxiliar_Cubo50'
if bpy.data.collections.get(name):raise RuntimeError('El auxiliar ya existe; no duplicar')
selected=list(bpy.context.selected_objects);active=bpy.context.view_layer.objects.active
collection=bpy.data.collections.new(name);bpy.context.scene.collection.children.link(collection)
mesh=bpy.data.meshes.new(name);ii=data['indices'];mesh.from_pydata(data['vertices'],[],[ii[k:k+3] for k in range(0,len(ii),3)]);mesh.update()
o=bpy.data.objects.new(name,mesh);collection.objects.link(o)
material=bpy.data.materials.new('AFR2_Auxiliar_Cubo_Dorado');material.use_nodes=True
def linear(v):return v/12.92 if v<=.04045 else ((v+.055)/1.055)**2.4
rgba=tuple(linear(v) for v in data['material']['srgb'])+(1,)
node=next(n for n in material.node_tree.nodes if n.type=='BSDF_PRINCIPLED');node.inputs['Base Color'].default_value=rgba;node.inputs['Roughness'].default_value=data['material']['roughness'];node.inputs['Metallic'].default_value=0;material.diffuse_color=rgba;mesh.materials.append(material)
try:
 bpy.ops.object.select_all(action='DESELECT');o.select_set(True);bpy.context.view_layer.objects.active=o
 target=HERE/'AFR2_Cubo_50mm.glb'
 bpy.ops.export_scene.gltf(filepath=str(target),export_format='GLB',use_selection=True,export_yup=True,export_animations=False,export_cameras=False,export_lights=False,export_texcoords=False,export_tangents=False,export_draco_mesh_compression_enable=False)
 bpy.data.libraries.write(str(HERE/'AFR2_Cubo_50mm.blend'),{collection},fake_user=True,compress=True)
 dest=ROOT/'AutoFinalRobo2-Web/public/models/AFR2_Cubo_50mm.glb';shutil.copy2(target,dest)
 report=dict(file=target.name,bytes=target.stat().st_size,sha256=hashlib.sha256(target.read_bytes()).hexdigest(),size_m=data['size_m'],triangles=len(mesh.polygons),materials=1,web_path=str(dest.relative_to(ROOT)),excluded='Sin brazo, auto ni transformacion de agarre',build=False)
 (HERE/'entrega_cubo.json').write_text(json.dumps(report,indent=2),encoding='utf-8');print(json.dumps(report))
finally:
 collection.hide_viewport=True;collection.hide_render=True
 o.select_set(False)
 for prev in selected:prev.select_set(True)
 bpy.context.view_layer.objects.active=active
