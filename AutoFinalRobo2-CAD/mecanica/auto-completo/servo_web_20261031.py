"""Blender: separar salida del SG90 conservando mallas/materiales del auto.

Usa una coleccion propia; no guarda ni reemplaza la escena abierta del brazo.
Fuente original 25/10 preservada. No ejecuta comprobaciones de fabricacion.
"""
import bpy,bmesh,json,re,struct,hashlib
from pathlib import Path
from mathutils import Matrix

ROOT=Path('D:/ProyectoRobotica2')
SOURCE=ROOT/'AutoFinalRobo2-CAD/exports/2026-10-25-final/AutoFinalRobo2-final.glb'
HERE=ROOT/'AutoFinalRobo2-CAD/mecanica/auto-completo/servo_web_20261031'
HERE.mkdir(exist_ok=True)
# Always import the immutable source into a fresh owned scene. Re-running does
# not split an already processed servo or modify existing objects.
previous=bpy.context.window.scene
scene=bpy.data.scenes.new('AFR2_Auto_Servo_CAD_20261031');bpy.context.window.scene=scene
before=set(bpy.data.objects);bpy.ops.import_scene.gltf(filepath=str(SOURCE))
col=bpy.data.collections.new('AFR2_Auto_Servo_CAD_Export');scene.collection.children.link(col)
for ob in set(bpy.data.objects)-before:
    for c in list(ob.users_collection):c.objects.unlink(ob)
    col.objects.link(ob)
col.hide_viewport=False;col.hide_render=False
servo=next(ob for ob in col.objects if ob.name.startswith('AFR2_SG90:'))
bm=bmesh.new();bm.from_mesh(servo.data)
bmesh.ops.remove_doubles(bm,verts=list(bm.verts),dist=1e-7)
bm.verts.ensure_lookup_table();bm.verts.index_update()
C=Matrix.Rotation(-1.5707963267948966,4,'X')
seen=set();mobile=set();shells=[]
for vertex in bm.verts:
    if vertex in seen:continue
    stack=[vertex];seen.add(vertex);island=[]
    while stack:
        v=stack.pop();island.append(v)
        for e in v.link_edges:
            other=e.other_vert(v)
            if other not in seen:seen.add(other);stack.append(other)
    pts=[C@servo.matrix_world@v.co for v in island]
    lo=min(p.y for p in pts)-.07905134505;hi=max(p.y for p in pts)-.07905134505
    # Whole disconnected shells, not a cut through the housing: output shaft,
    # horn and central screw lie above the fixed housing's lower surfaces.
    moving=lo>.0055 and hi<.0155
    if moving:mobile.update(v.index for v in island)
    shells.append(dict(vertices=len(island),min_arm_z_m=lo,max_arm_z_m=hi,moving=moving))
moving=bm.copy();fixed=bm.copy();bm.free()
for part,keep in [(moving,True),(fixed,False)]:
    part.verts.ensure_lookup_table()
    bmesh.ops.delete(part,geom=[v for v in part.verts if (v.index in mobile)!=keep],context='VERTS')
out=servo.copy();out.data=servo.data.copy();out.name='AFR2_SG90_Salida';col.objects.link(out)
moving.to_mesh(out.data);moving.free();out.data.update()
servo.data=servo.data.copy();fixed.to_mesh(servo.data);fixed.free();servo.data.update()
out['cad_motion']='SG90 output, horn and screw; rotate with HC-SR04 about native CAD shaft'
raw=SOURCE.read_bytes();n=struct.unpack_from('<I',raw,12)[0]
names={n.get('name') for n in json.loads(raw[20:20+n])['nodes']}
renamed=[]
for ob in list(col.objects):
    name=re.sub(r'\.\d{3}$','',ob.name)
    if name not in names:continue
    existing=bpy.data.objects.get(name)
    if existing and existing!=ob:
        renamed.append((existing,existing.name));existing.name=existing.name+'__temporarily_reserved'
    ob.name=name
target=HERE/'AutoFinalRobo2.glb'
try:
    bpy.ops.object.select_all(action='DESELECT')
    for ob in col.objects:ob.select_set(True)
    bpy.context.view_layer.objects.active=servo
    bpy.ops.export_scene.gltf(filepath=str(target),export_format='GLB',use_selection=True,use_active_scene=True,
        export_materials='EXPORT',export_cameras=False,export_lights=False,export_animations=False,
        export_normals=True,export_draco_mesh_compression_enable=True,export_draco_mesh_compression_level=10,
        export_draco_position_quantization=11,export_draco_normal_quantization=8,export_draco_texcoord_quantization=12)
    bpy.data.libraries.write(str(HERE/'AutoFinalRobo2_Servo_CAD.blend'),{scene},compress=True,fake_user=True)
finally:
    bpy.context.window.scene=previous
    for ob,name in renamed:
        current=bpy.data.objects.get(name)
        if current and current!=ob:current.name=name+'__servo_export_copy'
        ob.name=name
    col.hide_viewport=True;col.hide_render=True
report=dict(source=str(SOURCE.relative_to(ROOT)),output=str(target.relative_to(ROOT)),bytes=target.stat().st_size,
    sha256=hashlib.sha256(target.read_bytes()).hexdigest(),shells=shells,
    axis_arm_mm=[-5.15002937,42.999972322,13.149956902],native_document_edited=False,
    tests=False,build=False,runtime_verified=False)
(HERE/'origen.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
print(json.dumps({k:v for k,v in report.items() if k!='shells'}))
