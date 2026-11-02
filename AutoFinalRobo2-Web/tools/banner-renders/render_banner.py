"""Render de banners AFR2 (AFR2-20261102-01). Uso:
blender -b --factory-startup -P render_banner.py -- <modelo auto|brazo|conjunto> <models_dir> <out_dir> [samples] [escala %] [tomas]
AFR2-20261102-03: albedo de blancos limitado a 0,8, luces más suaves y bajas, exposición -0,3,
render a 150 % para reducir después (compose_banner.py). Mismas cámaras que AFR2-20261102-01.
Importa los GLB de Web/public/models, fondo transparente + piso atrapa-sombras, luz de estudio.
Salida: <out>/<modelo>-wide.png (1600x667, robot a la derecha) y <modelo>-narrow.png (800x500)."""
import bpy, sys, os, math
from mathutils import Vector

argv = sys.argv[sys.argv.index('--') + 1:]
MODEL, MODELS, OUT = argv[0], argv[1], argv[2]
SAMPLES = int(argv[3]) if len(argv) > 3 else 256
SCALE = int(argv[4]) if len(argv) > 4 else 150
SHOTS = argv[5].split(',') if len(argv) > 5 else ['wide', 'narrow']
os.makedirs(OUT, exist_ok=True)

bpy.ops.wm.read_factory_settings(use_empty=True)
scene = bpy.context.scene

def import_glb(name):
    before = set(bpy.data.objects)
    bpy.ops.import_scene.gltf(filepath=os.path.join(MODELS, name))
    new = [o for o in bpy.data.objects if o not in before]
    roots = [o for o in new if o.parent is None or o.parent not in new]
    return new, roots

def bounds(objs):
    pts = [o.matrix_world @ Vector(c) for o in objs if o.type == 'MESH' for c in o.bound_box]
    lo = Vector((min(p.x for p in pts), min(p.y for p in pts), min(p.z for p in pts)))
    hi = Vector((max(p.x for p in pts), max(p.y for p in pts), max(p.z for p in pts)))
    return lo, hi

def group(roots, name):
    empty = bpy.data.objects.new(name, None); scene.collection.objects.link(empty)
    for r in roots:
        mw = r.matrix_world.copy(); r.parent = empty; r.matrix_world = mw
    return empty

def place_on_ground(objs, empty):
    bpy.context.view_layer.update()
    lo, hi = bounds(objs)
    empty.location = Vector((-(lo.x + hi.x) / 2, -(lo.y + hi.y) / 2, -lo.z))
    bpy.context.view_layer.update()

objs = []
if MODEL in ('auto', 'conjunto'):
    car, car_roots = import_glb('AutoFinalRobo2.glb')
    place_on_ground(car, group(car_roots, 'auto'))
    objs += car
if MODEL in ('brazo', 'conjunto'):
    arm, arm_roots = import_glb('AFR2_Brazo_v17_Blanco.glb')
    arm_group = group(arm_roots, 'brazo')
    if MODEL == 'brazo':
        place_on_ground(arm, arm_group)
    else:
        # Montaje de la simulación conjunta: en Three.js CAD_TO_THREE·montaje·restToMount es la
        # traslación (0, 0.079041, -0.027819) relativa al marco del auto; en Blender (Z arriba): (0, 0.027819, 0.079041).
        arm_group.location = Vector((0, 0.027819, 0.079041))
    objs += arm
bpy.context.view_layer.update()

# Los GLB traen blancos con albedo 1,0: con luz de estudio se recortan. Un plástico blanco real ronda 0,8.
for mat in bpy.data.materials:
    if not mat.use_nodes: continue
    for node in mat.node_tree.nodes:
        if node.type != 'BSDF_PRINCIPLED': continue
        color = node.inputs['Base Color'].default_value
        peak = max(color[0], color[1], color[2])
        if peak > 0.8:
            k = 0.8 / peak
            node.inputs['Base Color'].default_value = (color[0] * k, color[1] * k, color[2] * k, color[3])
        node.inputs['Roughness'].default_value = max(node.inputs['Roughness'].default_value, 0.35)
lo, hi = bounds(objs)
center = (lo + hi) / 2
size = hi - lo
radius = size.length / 2

# Piso atrapa-sombras y entorno
bpy.ops.mesh.primitive_plane_add(size=max(size.x, size.y) * 12, location=(center.x, center.y, 0))
floor = bpy.context.active_object
floor.is_shadow_catcher = True
world = bpy.data.worlds.new('afr2'); scene.world = world; world.use_nodes = True
bg = world.node_tree.nodes['Background']; bg.inputs['Color'].default_value = (0.32, 0.38, 0.46, 1); bg.inputs['Strength'].default_value = 0.28

def area(name, loc, energy, size_m, color=(1, 1, 1)):
    data = bpy.data.lights.new(name, 'AREA'); data.energy = energy; data.size = size_m; data.color = color
    light = bpy.data.objects.new(name, data); scene.collection.objects.link(light)
    light.location = loc
    direction = center - Vector(loc); light.rotation_euler = direction.to_track_quat('-Z', 'Y').to_euler()
r = radius
# Key grande y suave arriba a la izquierda, relleno frío tenue y contraluz cálido para separar del fondo.
area('key', (center.x - 2.2 * r, center.y + 2.6 * r, center.z + 3.2 * r), 62 * r * r / 0.09, 3.2 * r, (1, 0.97, 0.93))
area('fill', (center.x + 2.8 * r, center.y + 1.6 * r, center.z + 1.4 * r), 18 * r * r / 0.09, 3.6 * r, (0.84, 0.9, 1))
area('rim', (center.x + 0.6 * r, center.y - 3.0 * r, center.z + 2.6 * r), 55 * r * r / 0.09, 1.8 * r, (1, 0.9, 0.84))
area('top', (center.x, center.y, center.z + 4.0 * r), 14 * r * r / 0.09, 4.0 * r)

# Cámara 3/4 desde el frente del robot (frente = +Y en Blender) y algo elevada
cam_data = bpy.data.cameras.new('cam'); cam_data.lens = 50
cam = bpy.data.objects.new('cam', cam_data); scene.collection.objects.link(cam); scene.camera = cam
direction = Vector((-0.62, 1.0, 0.42)).normalized()

scene.render.engine = 'CYCLES'
scene.cycles.samples = SAMPLES
scene.cycles.adaptive_threshold = 0.006
scene.cycles.sample_clamp_indirect = 6
scene.cycles.max_bounces = 10
scene.cycles.glossy_bounces = 6
scene.cycles.use_denoising = True
scene.cycles.device = 'CPU'
tmp = os.path.join(OUT, 'tmp'); os.makedirs(tmp, exist_ok=True)
bpy.context.preferences.filepaths.temporary_directory = tmp
scene.cycles.use_auto_tile = True
scene.cycles.tile_size = 256
scene.cycles.denoiser = 'OPENIMAGEDENOISE'
scene.render.film_transparent = True
scene.view_settings.view_transform = 'AgX'
scene.view_settings.look = 'AgX - Medium High Contrast'
scene.view_settings.exposure = -0.3
scene.render.filter_size = 1.2
scene.render.image_settings.file_format = 'PNG'
scene.render.image_settings.color_mode = 'RGBA'

def shoot(width, height, fill, shift_x, name):
    scene.render.resolution_x, scene.render.resolution_y = width, height
    scene.render.resolution_percentage = SCALE
    cam_data.sensor_fit = 'VERTICAL'
    fov = 2 * math.atan(cam_data.sensor_height / 2 / cam_data.lens)
    dist = radius / math.sin(fov / 2) / fill
    cam.location = center + direction * dist
    cam.rotation_euler = (center - cam.location).to_track_quat('-Z', 'Y').to_euler()
    cam_data.shift_x = shift_x
    scene.render.filepath = os.path.join(OUT, f'{MODEL}-{name}.png')
    bpy.ops.render.render(write_still=True)
    print('RENDERED', scene.render.filepath, scene.cycles.device)

if 'wide' in SHOTS: shoot(1600, 667, 1.15, -0.40, 'wide')
if 'narrow' in SHOTS: shoot(800, 500, 1.05, 0.0, 'narrow')
