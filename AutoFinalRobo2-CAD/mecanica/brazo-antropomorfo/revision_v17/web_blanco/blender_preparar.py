"""Importacion por piezas y entrega articulada. Ejecutar dentro de Blender."""
import bpy,json,math
from pathlib import Path
from mathutils import Matrix,Vector

HERE=Path('D:/ProyectoRobotica2/AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/web_blanco')
EXPORT=Path('D:/ProyectoRobotica2/AutoFinalRobo2-CAD/exports/brazo-v17-blanco-20261031')
MAN=json.loads((EXPORT/'manifest.json').read_text(encoding='utf-8'))
GROUPS={'01_Giro_base':'j1','02_Brazo_PVC':'j2','03_Antebrazo_PVC':'j3','04_Giro_muneca':'j4','05_Inclinacion_muneca':'j5','06_Herramienta':'j6','07_Mandibula_izquierda':'jaw-left','08_Mandibula_derecha':'jaw-right','00_Base_control':'base-fixed','09_Pinion_hombro':'drive-j2','11_Pinion_J4':'drive-j4','12_Polea_entrada_J3':'drive-j3','10_Pinion_pinza':'drive-grip'}
JOINTS=['J1_Giro_base','J2_Hombro','J3_Codo','J4_Giro_muneca','J5_Inclinacion_muneca','J6_Giro_herramienta']
ROOTS=['01_Giro_base','02_Brazo_PVC','03_Antebrazo_PVC','04_Giro_muneca','05_Inclinacion_muneca','06_Herramienta']

def matrix_cm(values):
 m=Matrix([values[i:i+4] for i in range(0,16,4)]);m.translation*=.01;return m

def srgb(v):return v/12.92 if v<=.04045 else ((v+.055)/1.055)**2.4

def collection(name):
 c=bpy.data.collections.new(name);bpy.context.scene.collection.children.link(c);return c

def relink(o,c):
 for old in list(o.users_collection):old.objects.unlink(o)
 c.objects.link(o)

def transform(m):
 p=m.translation;q=m.to_quaternion().normalized()
 def clean(x):return 0 if abs(x)<1e-9 else round(x,12)
 return dict(position=[clean(v) for v in p],quaternion=[clean(q.x),clean(q.y),clean(q.z),clean(q.w)])

def import_parts():
 if bpy.data.collections.get('AFR2_Brazo_v17_Piezas_Origen'):raise RuntimeError('Piezas ya importadas; no duplicar escena')
 previous=list(bpy.context.scene.objects)
 source=collection('AFR2_Brazo_v17_Piezas_Origen')
 before=set(bpy.data.objects)
 bpy.ops.wm.obj_import(directory=str(EXPORT/'parts')+'/',files=[{'name':Path(d['file']).name} for d in MAN['definitions']],forward_axis='Y',up_axis='Z',use_split_objects=True,use_split_groups=False,validate_meshes=False)
 imported=set(bpy.data.objects)-before
 lookup={o.name:o for o in imported}
 for key,mat in MAN['materials'].items():
  m=bpy.data.materials.get(key)
  if not m:continue
  m.name=mat['name'];m.use_nodes=True
  node=next(n for n in m.node_tree.nodes if n.type=='BSDF_PRINCIPLED')
  rgba=tuple(srgb(v) for v in mat['srgb'])+(1,)
  node.inputs['Base Color'].default_value=rgba;node.inputs['Metallic'].default_value=mat['metallic'];node.inputs['Roughness'].default_value=mat['roughness'];m.diffuse_color=rgba
 used=set()
 for i,row in enumerate(MAN['instances']):
  seed=lookup[row['id']]
  if row['id'] not in used:o=seed;used.add(row['id']);relink(o,source)
  else:o=seed.copy();source.objects.link(o)
  o.name='P%04d_%s'%(i,row['component']);o['cad_path']=row['path'];o['cad_body']=row['body'];o['cad_root']=row['root'];o['source_part']=row['id']
  o.matrix_world=matrix_cm(row['matrix_cm'])
 for o in previous:o.hide_set(True);o.hide_render=True
 bpy.context.scene.unit_settings.system='METRIC';bpy.context.scene.unit_settings.scale_length=1
 print(json.dumps(dict(source_parts=len(source.objects),unique_meshes=len({o.data for o in source.objects}),materials=len(MAN['materials']))))

def delivery():
 if bpy.data.collections.get('AFR2_Brazo_v17_Web'):raise RuntimeError('Version Web ya preparada')
 source=bpy.data.collections['AFR2_Brazo_v17_Piezas_Origen'];dest=collection('AFR2_Brazo_v17_Web')
 mount=Matrix.Translation((0,MAN['parameters']['v05_axis_y']*.01,0));to_mount=mount.inverted()
 roots={k:to_mount@matrix_cm(v['matrix_cm']) for k,v in MAN['roots'].items()}
 angles=[(MAN['joint_positions_rad'][n]+math.pi)%(2*math.pi)-math.pi for n in JOINTS]
 origins=[];parent=Matrix.Identity(4)
 for i,name in enumerate(ROOTS):
  frame=roots[name];axis='Z' if i in [0,3,5] else 'Y'
  origin=parent.inverted()@frame@Matrix.Rotation(-angles[i],4,axis)
  origins.append(transform(origin));parent=frame
 followers=[]
 # Motor shafts follow the same kinematic parent as their physical supports.
 for node,root,parent_idx,driver,axis,ratio in [('drive-j2','09_Pinion_hombro',0,1,'Y',-4),('drive-j3','12_Polea_entrada_J3',1,2,'Y',2.5),('drive-j4','11_Pinion_J4',2,3,'Z',-1),('drive-grip','10_Pinion_pinza',5,5,'Z',0)]:
  origin=roots[ROOTS[parent_idx]].inverted()@roots[root]@Matrix.Rotation(-ratio*angles[driver],4,axis)
  followers.append(dict(visualNode=node,parentIndex=parent_idx,driverIndex=driver,axis=[0,1,0] if axis=='Y' else [0,0,1],ratio=ratio,origin=transform(origin),gripperRadiansPerMm=1/(2*MAN['parameters']['grip_r']*10) if node=='drive-grip' else 0))
 groups={}
 for root,node in GROUPS.items():
  e=bpy.data.objects.new(node,None);dest.objects.link(e);e['cad_root']=root;groups[root]=e
 bpy.ops.object.select_all(action='DESELECT')
 for root,e in groups.items():
  items=[]
  for seed in source.objects:
   if seed['cad_root']!=root:continue
   o=seed.copy();o.data=seed.data.copy();dest.objects.link(o);o.matrix_world=to_mount@seed.matrix_world;items.append(o)
  if not items:raise RuntimeError('Grupo sin geometria '+root)
  for o in items:o.select_set(True)
  bpy.context.view_layer.objects.active=items[0];bpy.ops.object.join();joined=items[0]
  joined.name='mesh_'+GROUPS[root];joined.parent=e
  # Remove CAD tessellation subdivisions on coplanar faces, without moving boundaries.
  mod=joined.modifiers.new('Disolver_coplanar_CAD','DECIMATE');mod.decimate_type='DISSOLVE';mod.angle_limit=math.radians(.3);mod.use_dissolve_boundaries=False
  bpy.ops.object.modifier_apply(modifier=mod.name)
  joined.select_set(False)
 source.hide_viewport=True;source.hide_render=True
 # Source jaw locations are the actual openings, rather than servo command assumptions.
 palm=roots['06_Herramienta'];inv=palm.inverted()
 left=inv@roots['07_Mandibula_izquierda'];right=inv@roots['08_Mandibula_derecha']
 opening=(right.translation.x-left.translation.x)*1000-18
 # Pads/fingers thicknesses are measured from the inherited V7 parameter definitions.
 opening=(right.translation.x-left.translation.x)*1000-(MAN['parameters']['finger_t']+2*MAN['parameters']['pad_t'])*10
 metadata=dict(roots={k:transform(v) for k,v in roots.items()},restAngles=[math.degrees(v) for v in angles],jointOrigins=origins,followers=followers,restOpening=opening,mount_native_m=list(mount.translation),parameters=MAN['parameters'],groups=GROUPS,
  gltf_yup=True,restToMount=dict(position=[0,0,0],quaternion=[math.sqrt(.5),0,0,math.sqrt(.5)]))
 (HERE/'rig_v17.json').write_text(json.dumps(metadata,indent=2),encoding='utf-8')
 print(json.dumps(dict(meshes=len([o for o in dest.objects if o.type=='MESH']),restAngles=metadata['restAngles'],opening=opening,origins=origins)))

def optimize():
 dest=bpy.data.collections['AFR2_Brazo_v17_Web'];meshes=[o for o in dest.objects if o.type=='MESH']
 # OBJ crea materiales por archivo; conservar nueve materiales reales,
 # no cientos de copias equivalentes ni sus llamadas de dibujo.
 source=bpy.data.collections['AFR2_Brazo_v17_Piezas_Origen']
 seen=set()
 for o in list(source.objects)+meshes:
  mesh=o.data
  if mesh in seen:continue
  seen.add(mesh);materials=[];remap={}
  for i,m in enumerate(mesh.materials):
   key=m.name.split('.')[0]
   canonical=bpy.data.materials[MAN['materials'][key]['name']] if key in MAN['materials'] else m
   if canonical not in materials:materials.append(canonical)
   remap[i]=materials.index(canonical)
  indices=[remap[p.material_index] for p in mesh.polygons]
  mesh.materials.clear()
  for m in materials:mesh.materials.append(m)
  for p,i in zip(mesh.polygons,indices):p.material_index=i
 bpy.ops.object.select_all(action='DESELECT')
 for o in meshes:
  o.hide_set(False)
  for mod in list(o.modifiers):
   if mod.name=='Reduccion_Web':o.modifiers.remove(mod)
  ratio=globals().get('WEB_RATIO',.128)
  mod=o.modifiers.new('Reduccion_Web','DECIMATE');mod.decimate_type='COLLAPSE';mod.ratio=ratio;mod.use_collapse_triangulate=True
 for o in dest.objects:o.select_set(True)
 target=HERE/'AFR2_Brazo_v17_Blanco.glb'
 pos_bits=globals().get('POSITION_BITS',11);normal_bits=globals().get('NORMAL_BITS',7)
 bpy.ops.export_scene.gltf(filepath=str(target),export_format='GLB',use_selection=True,export_yup=True,export_apply=True,export_extras=False,export_animations=False,export_cameras=False,export_lights=False,export_texcoords=False,export_tangents=False,export_draco_mesh_compression_enable=True,export_draco_mesh_compression_level=10,export_draco_position_quantization=pos_bits,export_draco_normal_quantization=normal_bits,export_draco_color_quantization=8)
 deps=bpy.context.evaluated_depsgraph_get();triangles=0
 for o in meshes:
  ev=o.evaluated_get(deps);m=ev.to_mesh();m.calc_loop_triangles();triangles+=len(m.loop_triangles);ev.to_mesh_clear()
 budget=globals().get('WEB_BUDGET',600000)
 result=dict(file=str(target),bytes=target.stat().st_size,budget_bytes=budget,within_budget=target.stat().st_size<=budget,ratio=ratio,position_bits=pos_bits,normal_bits=normal_bits,triangles=triangles,materials=len({m.name for o in meshes for m in o.data.materials if m}),source_parts=len(bpy.data.collections['AFR2_Brazo_v17_Piezas_Origen'].objects),exported_groups=list(GROUPS.values()),excluded=MAN['excluded_roots'])
 (HERE/'entrega_glb.json').write_text(json.dumps(result,indent=2),encoding='utf-8');print(json.dumps(result))

phase=globals().get('BLENDER_PHASE')
if phase=='import':import_parts()
elif phase=='delivery':delivery()
elif phase=='optimize':optimize()
else:raise RuntimeError('Indicar import/delivery/optimize')
