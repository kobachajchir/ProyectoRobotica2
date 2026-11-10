"""AFR2-20261109-10. Ensamble de presentación desde STEP reales y CAD del auto.
Ejecutar dentro del Blender abierto. No hace renders, validaciones ni cargas.
"""
import bpy, bmesh, json, math, hashlib, gzip, contextlib, io
from pathlib import Path
from mathutils import Vector, Matrix

ROOT = Path('D:/ProyectoRobotica2')
HERE = ROOT / 'AutoFinalRobo2-CAD/mecanica/sensor-bateria'
CACHE = ROOT / 'AutoFinalRobo2-CAD/exports/20261109-sensor-bateria'
DATA = json.loads((CACHE / 'step_meshes.json').read_text(encoding='utf-8'))
SCENE = bpy.data.scenes.get('AFR2_Agregados_20261109')
if SCENE is None:
    SCENE = bpy.data.scenes.new('AFR2_Agregados_20261109'); bpy.context.window.scene = SCENE
    with contextlib.redirect_stdout(io.StringIO()):
        bpy.ops.import_scene.gltf(filepath=str(ROOT/'AutoFinalRobo2-Web/public/models/AutoFinalRobo2.glb'))
bpy.context.window.scene = SCENE
if bpy.data.collections.get('AFR2_Agregados_Ligeros_20261109'):
    raise RuntimeError('La preparación ya existe; usar el .blend guardado, no duplicarla.')
COLLECTION = bpy.data.collections.new('AFR2_Agregados_Ligeros_20261109')
SCENE.collection.children.link(COLLECTION)

def material(name, color, metallic=0, roughness=.5, emission=0):
    mat = bpy.data.materials.new(name); mat.diffuse_color = (*color, 1); mat.use_nodes = True
    node = mat.node_tree.nodes.get('Principled BSDF')
    node.inputs['Base Color'].default_value = (*color, 1)
    node.inputs['Metallic'].default_value = metallic; node.inputs['Roughness'].default_value = roughness
    if emission:
        node.inputs['Emission Color'].default_value = (*color, 1); node.inputs['Emission Strength'].default_value = emission
    return mat

M = {
    'tan': material('AFR2_Resistencia_Crema', (.72,.53,.25)),
    'orange': material('AFR2_Capacitor_Naranja', (.8,.19,.025)),
    'metal': material('AFR2_Terminal_Estanado', (.58,.62,.66), .8, .3),
    'gold': material('AFR2_Pin_Dorado', (.68,.42,.08), .7, .3),
    'black': material('AFR2_Negro', (.012,.015,.02)),
    'red': material('AFR2_Cable_Rojo', (.72,.018,.03)),
    'white': material('AFR2_PLA_Blanco', (.78,.8,.82)),
    'blue': material('AFR2_PCB_Azul', (.025,.11,.3)),
    'green': material('AFR2_PCB_Verde', (.025,.19,.085)),
    'led': material('AFR2_LED_Rojo', (.8,.025,.015), emission=1.2),
}

def empty(name, parent=None):
    obj = bpy.data.objects.new(name, None); COLLECTION.objects.link(obj); obj.parent = parent; return obj

ASSEMBLY = empty('AFR2_Nuestros_Agregados')
PANEL = empty('addon-panel', ASSEMBLY)
POWER = empty('addon-battery-sensor', ASSEMBLY)
CIRCUIT = empty('battery-divider', POWER)
CIRCUIT.location = (-.049, .005, .084)

def simplify(obj, target):
    bpy.context.view_layer.objects.active = obj
    obj.select_set(True)
    bm = bmesh.new(); bm.from_mesh(obj.data)
    bmesh.ops.remove_doubles(bm, verts=list(bm.verts), dist=.0000001)
    bmesh.ops.dissolve_limit(bm, angle_limit=math.radians(2), verts=list(bm.verts), edges=list(bm.edges), delimit={'MATERIAL'})
    bm.to_mesh(obj.data); bm.free(); obj.data.update()
    triangles = sum(max(0,len(p.vertices)-2) for p in obj.data.polygons)
    if triangles > target:
        mod = obj.modifiers.new('Ligero_para_ESP', 'DECIMATE'); mod.ratio = target/triangles
        mod.use_collapse_triangulate = True; bpy.ops.object.modifier_apply(modifier=mod.name)
    obj.data.set_sharp_from_angle(angle=math.radians(35))
    for poly in obj.data.polygons: poly.use_smooth = True
    obj.select_set(False)

def source_part(prefix, name, parent, target):
    original = next(ob for ob in SCENE.objects if ob.name.startswith(prefix))
    obj = bpy.data.objects.new(name, original.data.copy()); COLLECTION.objects.link(obj)
    obj.data.transform(original.matrix_world); obj.parent = parent
    for index, mat in enumerate(obj.data.materials):
        if not mat or not mat.use_nodes: continue
        node = mat.node_tree.nodes.get('Principled BSDF')
        if not node: continue
        r,g,b,_ = node.inputs['Base Color'].default_value
        key = 'led' if 'LED' in mat.name else 'white' if min(r,g,b)>.58 else 'black' if max(r,g,b)<.15 else 'blue' if b>r*1.5 else 'green' if g>r*1.5 else 'gold' if r>g*1.2 and g>b*1.3 else 'metal'
        obj.data.materials[index] = M[key]
    simplify(obj,target)
    return obj

# Existing meshes keep their original positions in the assembled auto.
source_part('AFR2_AFR2_UI_B2_Fondo_OLED15_Auto5', 'panel-oled-encoder-soporte', PANEL, 4800)
source_part('AFR2_Portabaterias', 'portapilas', POWER, 950)
source_part('AFR2_Celda_18650:1', 'celda-1', POWER, 250)
source_part('AFR2_Celda_18650:2', 'celda-2', POWER, 250)
source_part('AFR2_Voltimetro', 'voltimetro', POWER, 430)

def step_part(key, name, position, angle=0, target=600):
    entries = DATA['parts'][key]['meshes']; vertices=[]; faces=[]
    for entry in entries:
        offset=len(vertices); vertices.extend([[v*.001 for v in p] for p in entry['vertices']])
        faces.extend([[i+offset for i in face] for face in entry['faces']])
    mesh=bpy.data.meshes.new(name); mesh.from_pydata(vertices,[],faces); mesh.update()
    obj=bpy.data.objects.new(name,mesh);COLLECTION.objects.link(obj);obj.parent=CIRCUIT
    obj.location=position;obj.rotation_euler.z=angle
    base = 'tan' if key=='resistor' else 'orange' if key=='capacitor' else 'black'
    mesh.materials.append(M[base]);mesh.materials.append(M['gold' if key=='pin' else 'metal'])
    for p in mesh.polygons:
        c=p.center*1000
        body = (.55<c.x<7.05 and c.z>0) if key=='resistor' else c.z>1.3 if key=='capacitor' else abs(c.x)<.5 and abs(c.y)>.15 and c.z>=0
        p.material_index=0 if body else 1
    simplify(obj,target);obj['source_step']=DATA['parts'][key]['source']
    return obj

step_part('resistor','R1-10k',(-.010,0,0),target=540)
step_part('resistor','R4-10k',(.00238,0,0),target=540)
step_part('capacitor','C3-104-100nF',(0,.007,0),target=480)
step_part('pin','J3-A5',(0,-.006,0),angle=-math.pi/2,target=160)

def wire(name, points, color, parent, radius=.00042):
    curve=bpy.data.curves.new(name,'CURVE');curve.dimensions='3D';curve.resolution_u=2
    curve.bevel_depth=radius;curve.bevel_resolution=1;curve.use_fill_caps=True
    spline=curve.splines.new('BEZIER');spline.bezier_points.add(len(points)-1)
    for point,coordinate in zip(spline.bezier_points,points):
        point.co=coordinate;point.handle_left_type='AUTO';point.handle_right_type='AUTO'
    obj=bpy.data.objects.new(name,curve);COLLECTION.objects.link(obj);obj.parent=parent;curve.materials.append(M[color])
    return obj

# External rails are created only by exportar_ligero_blender.py (compact final layout).
MID=(0,0,-.003)
wire('rojo-R1-a-nodo',[(-.00238,0,-.003),MID,(.00238,0,-.003)],'red',CIRCUIT,.00028)
wire('rojo-nodo-a-C3',[MID,(-.001,.003,-.003),(0,.007,-.002)],'red',CIRCUIT,.00028)
wire('negro-C3-a-GND',[(.0025,.007,-.002),(.010,.006,-.003),(.010,0,-.003)],'black',CIRCUIT,.00028)
wire('rojo-nodo-a-pin-A5',[MID,(-.001,-.003,-.003),(0,-.006,-.0025)],'red',CIRCUIT,.00028)

# Convert cables to small mesh tubes; retain their descriptive net names.
bpy.ops.object.select_all(action='DESELECT')
for obj in list(COLLECTION.objects):
    if obj.type=='CURVE':
        bpy.context.view_layer.objects.active=obj;obj.select_set(True)
        bpy.ops.object.convert(target='MESH');obj.select_set(False)

# Imported auto is an untouched reference in this new scene, not an export member.
for ob in list(SCENE.objects):
    if ob.name not in COLLECTION.objects:
        ob.hide_render=True;ob.hide_set(True)
SCENE['AFR2_change']='AFR2-20261109-10'
SCENE['circuit']='BATT+ -> R1 10k -> A5 -> R4 10k -> GND; C3 100nF in parallel with R4'
SCENE['presentation_only']='Routed wires and mounting position are illustrative; no electrical/mechanical approval.'
bpy.context.view_layer.update()
for area in bpy.context.screen.areas:
    if area.type=='VIEW_3D':
        area.spaces.active.region_3d.view_location=Vector((-.025,-.035,.09))
        area.spaces.active.region_3d.view_distance=.24

HERE.mkdir(parents=True,exist_ok=True)
blend=CACHE/'AFR2_Preparacion_Agregados.blend'
bpy.ops.wm.save_as_mainfile(filepath=str(blend))
report={'change_id':'AFR2-20261109-10','step_sources':{k:{a:b for a,b in v.items() if a!='meshes'} for k,v in DATA['parts'].items()},'wire_colors':{'BATT+':'red','GND':'black','A5':'red'},'adc':'8V battery ->4V A5 nominal; divider10k/10k; C3=100nF','state':'prepared for compact export; no rendering, electrical checks, build or flash'}
(HERE/'entrega_agregados.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print('Preparación editable guardada; ejecutar exportar_ligero_blender.py para el único GLB final de cuatro componentes y cables.')
