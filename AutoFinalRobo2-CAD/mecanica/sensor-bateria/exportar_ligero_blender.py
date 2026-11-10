"""AFR2-20261109-10. Exporta SOLO R1/R4/C3/J3 y cables, sin duplicar el auto.
Panel, voltímetro y portapilas se reutilizan desde el GLB existente en el navegador.
"""
import bpy, math, json, hashlib, gzip, contextlib, io
from pathlib import Path
from mathutils import Vector
ROOT=Path('D:/ProyectoRobotica2');HERE=ROOT/'AutoFinalRobo2-CAD/mecanica/sensor-bateria'
scene=bpy.data.scenes['AFR2_Agregados_20261109'];bpy.context.window.scene=scene
light=bpy.data.collections.new('AFR2_Circuito4_Cables_Compacto_WEB');scene.collection.children.link(light)
copies=[];focus=[]
for name,target in [('R1-10k',320),('R4-10k',320),('C3-104-100nF',320),('J3-A5',120),('rojo-R1-a-nodo',100),('rojo-nodo-a-C3',100),('negro-C3-a-GND',100),('rojo-nodo-a-pin-A5',100)]:
    original=bpy.data.objects[name];mesh=original.data.copy();mesh.transform(original.matrix_world)
    mesh.transform(__import__('mathutils').Matrix.Translation((.049,-.042,-.080)))
    ob=bpy.data.objects.new('CIRCUITO_'+name,mesh);light.objects.link(ob);copies.append(ob)
    if name in ['R1-10k','R4-10k','C3-104-100nF','J3-A5']:focus.extend(v.co.copy() for v in mesh.vertices)
    bpy.context.view_layer.objects.active=ob
    count=sum(len(p.vertices)-2 for p in mesh.polygons)
    if count>target:
        mod=ob.modifiers.new('ESP_compacto','DECIMATE');mod.ratio=target/count;mod.use_collapse_triangulate=True;bpy.ops.object.modifier_apply(modifier=mod.name)
red=bpy.data.materials['AFR2_Cable_Rojo'];black=bpy.data.materials['AFR2_Negro']
def wire(name,coords,mat):
    curve=bpy.data.curves.new(name,'CURVE');curve.dimensions='3D';curve.resolution_u=2;curve.bevel_depth=.00038;curve.bevel_resolution=0;curve.use_fill_caps=True
    spline=curve.splines.new('POLY');spline.points.add(len(coords)-1)
    for point,co in zip(spline.points,coords):point.co=(*co,1)
    ob=bpy.data.objects.new(name,curve);light.objects.link(ob);curve.materials.append(mat)
    bpy.ops.object.select_all(action='DESELECT');ob.select_set(True);bpy.context.view_layer.objects.active=ob;bpy.ops.object.convert(target='MESH');copies.append(ob)
# Compact vertical arrangement: voltmeter above holder, divider below.
# Straight, parallel side rails; positive on the left, negative on the right.
wire('rojo-BATT-izquierda', [(-.010,-.037,.001),(-.042,-.037,.001),(-.042,0,.010),(-.0381,0,.010)],red)
wire('rojo-derivacion-voltimetro', [(-.042,0,.010),(-.042,.034,.008),(-.0175,.034,.008)],red)
wire('negro-GND-derecha', [(.010,-.037,.001),(.042,-.037,.001),(.042,0,.010),(.0381,0,.010)],black)
wire('negro-derivacion-voltimetro', [(.042,0,.010),(.042,.034,.008),(.0175,.034,.008)],black)
bpy.ops.object.select_all(action='DESELECT')
for ob in copies:ob.select_set(True)
bpy.context.view_layer.objects.active=copies[0];bpy.ops.object.join()
merged=bpy.context.object;merged.name='battery-divider-wires'
slots=list(merged.data.materials);unique=list(dict.fromkeys(slots));mapping=[unique.index(m) for m in slots];indices=[mapping[p.material_index] for p in merged.data.polygons]
merged.data.materials.clear()
for m in unique:merged.data.materials.append(m)
for p,index in zip(merged.data.polygons,indices):p.material_index=index
for ob in scene.objects:
    if ob.name not in light.objects:ob.hide_render=True;ob.hide_set(True)
bpy.context.view_layer.update()
blend=HERE/'AFR2_Sensor_Bateria_Cableado_20261109_v2.blend'
bpy.ops.wm.save_as_mainfile(filepath=str(blend),compress=True)
out=ROOT/'AutoFinalRobo2-Web/public/models/AFR2_Sensor_Bateria.glb'
with contextlib.redirect_stdout(io.StringIO()):
    bpy.ops.export_scene.gltf(filepath=str(out),export_format='GLB',use_selection=True,use_active_scene=True,export_materials='EXPORT',export_cameras=False,export_lights=False,export_animations=False,export_normals=True,export_texcoords=False,export_draco_mesh_compression_enable=True,export_draco_mesh_compression_level=10,export_draco_position_quantization=14,export_draco_normal_quantization=6)
raw=out.read_bytes();report=json.loads((HERE/'entrega_agregados.json').read_text())
report.update(blend=str(blend),web_asset=str(out),bytes=len(raw),gzip_bytes=len(gzip.compress(raw,compresslevel=9)),sha256=hashlib.sha256(raw).hexdigest(),triangles=sum(len(p.vertices)-2 for p in merged.data.polygons),objects=1,export_meshes=1,materials=len(unique),blend_bytes=blend.stat().st_size,export_scope='R1,R4,C3,J3 and red/black wires ONLY; panel/voltmeter/holder reused in browser',wire_routes='short straight parallel rails: positive left, GND right; holder center, voltmeter above, divider below',focus_bounds_three=[min(v.x for v in focus),min(v.z for v in focus),min(-v.y for v in focus),max(v.x for v in focus),max(v.z for v in focus),max(-v.y for v in focus)])
report['reuse_layout_three']={'holder_translation':[0,-.07905133813619614,-.07199974171817303],'voltmeter_translation':[.05,-.07995112371444702,-.001997555620968344],'compact_center':[0,.010,0]}
(HERE/'entrega_agregados.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps({k:report[k] for k in ['bytes','gzip_bytes','triangles','export_meshes','focus_bounds_three']}))
