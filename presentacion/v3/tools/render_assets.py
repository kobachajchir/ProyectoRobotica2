# Renders de la presentación v3 desde el GLB que sirve la Web (mismo modelo que el dashboard).
# Uso: blender -b --factory-startup --python render_assets.py -- <salida_dir> [ids...]
# No modifica ningún .blend del proyecto: arranca vacío e importa el GLB.
import bpy, sys, math, json
from mathutils import Vector

ARGS = sys.argv[sys.argv.index('--') + 1:]
OUT = ARGS[0].rstrip('/\\') + '/'
ONLY = set(ARGS[1:])
CAR = 'D:/ProyectoRobotica2/AutoFinalRobo2-Web/public/models/AutoFinalRobo2.glb'
BG = (0x0B / 255, 0x12 / 255, 0x20 / 255)


def srgb2lin(c):
    return c / 12.92 if c <= 0.04045 else ((c + 0.055) / 1.055) ** 2.4


bpy.ops.wm.read_factory_settings(use_empty=True)
sc = bpy.context.scene
bpy.ops.import_scene.gltf(filepath=CAR)
MESH = [o for o in bpy.data.objects if o.type == 'MESH']
ORIG = {o.name: o.matrix_world.copy() for o in MESH}

sc.render.engine = 'CYCLES'
sc.cycles.samples = int(__import__("os").environ.get("AFR2_SAMPLES","96"))
sc.cycles.use_denoising = True
try:
    sc.cycles.device = 'GPU'
except Exception:
    pass
sc.view_settings.view_transform = 'Standard'
sc.view_settings.look = 'None'
world = bpy.data.worlds.new('W')
sc.world = world
world.use_nodes = True
bgn = world.node_tree.nodes['Background']
bgn.inputs[0].default_value = (*[srgb2lin(c) for c in BG], 1)
bgn.inputs[1].default_value = 1.0


def light(name, loc, energy, size, color=(1, 1, 1)):
    d = bpy.data.lights.new(name, 'AREA')
    d.energy, d.size, d.color = energy, size, color
    o = bpy.data.objects.new(name, d)
    sc.collection.objects.link(o)
    o.location = loc
    c = o.constraints.new('TRACK_TO')
    c.target = None
    return o


tgt = bpy.data.objects.new('LT', None)
sc.collection.objects.link(tgt)
LIGHTS = [light('Key', (0.45, -0.55, 0.6), 110, 0.5),
          light('Fill', (-0.6, -0.3, 0.35), 18, 0.8, (0.85, 0.9, 1.0)),
          light('Rim', (-0.2, 0.7, 0.5), 45, 0.5, (0.75, 0.85, 1.0))]
for L in LIGHTS:
    L.constraints[0].target = tgt

cam_d = bpy.data.cameras.new('C')
cam = bpy.data.objects.new('C', cam_d)
sc.collection.objects.link(cam)
sc.camera = cam
MANIFEST = []


def bbox(objs):
    mn = Vector((1e9,) * 3); mx = Vector((-1e9,) * 3)
    for o in objs:
        for c in o.bound_box:
            w = o.matrix_world @ Vector(c)
            mn = Vector(map(min, mn, w)); mx = Vector(map(max, mx, w))
    return mn, mx


def pick(*keys):
    return [o for o in MESH if any(k in o.name for k in keys)]


def show(objs):
    names = {o.name for o in objs}
    for o in MESH:
        o.hide_render = o.name not in names


def reset():
    for o in MESH:
        o.matrix_world = ORIG[o.name]
        o.hide_render = False
    for o in list(bpy.data.objects):
        if o.name.startswith('SCN_'):
            bpy.data.objects.remove(o)


def frame(objs, direction, lens=60, w=1920, h=1080, margin=1.18, shift_x=0.0, shift_y=0.0, ortho=False):
    mn, mx = bbox(objs)
    ctr = (mn + mx) / 2
    rad = (mx - mn).length / 2
    d = Vector(direction).normalized()
    sc.render.resolution_x, sc.render.resolution_y = w, h
    sc.render.resolution_percentage = 100
    cam_d.sensor_fit = 'HORIZONTAL'
    cam_d.sensor_width = 36
    aspect = w / h
    if ortho:
        cam_d.type = 'ORTHO'
        # extension proyectada real (no la esfera) sobre el plano de la camara
        up = Vector((0, 0, 1)) if abs(d.z) < 0.9 else Vector((0, 1, 0))
        right = d.cross(up).normalized(); upv = right.cross(d).normalized()
        pts = [o.matrix_world @ Vector(c) for o in objs for c in o.bound_box]
        ew = max(p.dot(right) for p in pts) - min(p.dot(right) for p in pts)
        eh = max(p.dot(upv) for p in pts) - min(p.dot(upv) for p in pts)
        cam_d.ortho_scale = max(ew, eh * aspect) * margin / (1 - 2 * abs(shift_x))
        dist = rad * 4
    else:
        cam_d.type = 'PERSP'
        cam_d.lens = lens
        fov_h = 2 * math.atan(18 / lens)
        fov_v = 2 * math.atan(math.tan(fov_h / 2) / aspect)
        usable = 1 - 2 * abs(shift_x)
        dist = max(rad * margin / math.sin(fov_v / 2), rad * margin / math.sin(fov_h * usable / 2))
    cam_d.clip_start, cam_d.clip_end = 0.001, 50
    cam_d.shift_x, cam_d.shift_y = shift_x, shift_y
    cam.location = ctr + d * dist
    cam.rotation_euler = (ctr - cam.location).to_track_quat('-Z', 'Y').to_euler()
    tgt.location = ctr
    for L, off in zip(LIGHTS, [(0.55, 0.65, 0.6), (-0.7, 0.2, 0.35), (-0.2, -0.75, 0.55)]):
        L.location = ctr + Vector(off)  # escala fija del auto: misma exposicion en todas las tomas
    return ctr, rad


def render(aid, cam_id, note, transparent=False):
    sc.render.film_transparent = transparent
    path = OUT + aid + '.png'
    sc.render.filepath = path
    bpy.ops.render.render(write_still=True)
    MANIFEST.append({'asset_id': aid, 'camera_id': cam_id, 'source_file': CAR,
                     'camera': {'type': cam_d.type, 'lens': cam_d.lens, 'loc': list(cam.location),
                                'shift': [cam_d.shift_x, cam_d.shift_y]},
                     'res': [sc.render.resolution_x, sc.render.resolution_y],
                     'visible': [o.name for o in MESH if not o.hide_render], 'note': note})
    print('RENDERED', path)


def want(i):
    return not ONLY or i in ONLY


def ground(z, size=3, color=(0.07, 0.09, 0.13), name='SCN_Ground'):
    bpy.ops.mesh.primitive_plane_add(size=size, location=(0, 0, z))
    g = bpy.context.active_object; g.name = name
    m = bpy.data.materials.new(name + 'M'); m.use_nodes = True
    m.node_tree.nodes['Principled BSDF'].inputs['Base Color'].default_value = (*color, 1)
    m.node_tree.nodes['Principled BSDF'].inputs['Roughness'].default_value = 0.8
    g.data.materials.append(m)
    return g


ALL = MESH
FRONT = Vector((0, 1, 0))  # verificado: HC-SR04 y sensor de linea en +Y  # se verifica visualmente; el HC-SR04 marca el frente

# F01 hero: tres cuartos, 38 % izquierdo libre
if want('F01'):
    reset(); show(ALL)
    frame(ALL, (1.0, 1.3, 0.62), lens=70, w=3840, h=2160, margin=0.82, shift_x=-0.17)
    render('F01_A01_RENDER_hero_v001', 'CAM_F01_HERO_V01', 'Tres cuartos, auto completo desde GLB Web')
# F33 hero final: lado opuesto, centrado con espacio radial
if want('F33'):
    reset(); show(ALL)
    frame(ALL, (-1.1, 1.05, 0.55), lens=70, w=3840, h=2160, margin=1.35)
    render('F33_A01_RENDER_hero_final_v001', 'CAM_F33_HERO_FINAL_V01', 'Tres cuartos opuesto, espacio radial')
# F04 arquitectura: vista superior tres cuartos, mitad derecha libre
if want('F04'):
    reset(); show(ALL)
    frame(ALL, (0.55, 0.8, 1.15), lens=60, w=2560, h=1440, margin=0.85, shift_x=-0.22)
    render('F04_A01_RENDER_arch_iso_v001', 'CAM_F04_ARCH_ISO_V01', 'Tres cuartos superior')
# F05 rueda aislada
if want('F05'):
    reset(); wh = pick('Rueda80_ManoA:1'); show(wh)
    frame(wh, (1, -0.9, 0.45), lens=85, w=2000, h=2000, margin=1.1)
    render('F05_A01_RENDER_wheel_iso_v001', 'CAM_F05_WHEEL_ISO_V01', 'Rueda Mecanum 80 mm mano A, aislada', transparent=True)
    mn, mx = bbox(wh); ax = mx - mn
    axis = min(range(3), key=lambda i: ax[i])  # eje de la rueda = dimension menor
    d = [0, 0, 0]; d[axis] = 1
    frame(wh, d, lens=200, w=2000, h=2000, margin=1.1)
    render('F05_A01_RENDER_wheel_side_v001', 'CAM_F05_WHEEL_SIDE_V01', 'Rueda vista lateral (eje)', transparent=True)
    print('WHEEL_DIMS_MM', [round(v * 1000, 1) for v in ax])
# F07 explosionado vertical (solo visual)
if want('F07'):
    reset(); show(ALL)
    layers = [('Rueda', 0), ('Casquillo', 0), ('Prisionero', 0), ('GM25', 0), ('Soportes_Motores', 0),
              ('CapaInferior', 0), ('Portabaterias', 0.03), ('Celda', 0.03), ('Separadores', 0.06), ('Fijaciones', 0.06),
              ('CapaSuperior', 0.06), ('Mega2560', 0.10), ('MotorDriver', 0.14), ('WifiShield', 0.18),
              ('Voltimetro', 0.10), ('SensorLinea', -0.035), ('HC_SR04', 0.10), ('Soporte_HCSR04', 0.10),
              ('SG90', 0.10), ('UI_B2', 0.22)]
    for o in MESH:
        dz = next((z for k, z in layers if k in o.name), 0)
        o.location.z += dz
    bpy.context.view_layer.update()
    frame(ALL, (1.0, 1.25, 0.55), lens=60, w=3840, h=2160, margin=0.95)
    render('F07_A01_RENDER_exploded_v001', 'CAM_F07_EXPLODED_V01', 'Explosion vertical solo para visualizacion; desplazamientos no representan montaje real')
# F08 close-up panel UI OLED/encoder
if want('F08'):
    reset(); show(ALL); ui = pick('UI_B2')
    frame(ui, (0.35, -1.0, 0.75), lens=85, w=3840, h=2160, margin=1.35)
    render('F08_A01_RENDER_ui_mount_v001', 'CAM_F08_UI_MOUNT_V01', 'Close-up panel OLED + encoder')
# F09/F10 lateral ortografica del auto
if want('F09'):
    reset(); show(ALL); ground(bbox(ALL)[0].z)
    frame(ALL, (1, 0, 0), w=2560, h=1440, margin=1.25, ortho=True)
    render('F09_A01_RENDER_side_ortho_v001', 'CAM_F09_SIDE_V01', 'Lateral ortografica, suelo en z minimo')
    frame(ALL, (0, 0, 1), w=2560, h=1440, margin=1.2, ortho=True)
    render('F10_A01_RENDER_top_ortho_v001', 'CAM_F10_TOP_V01', 'Planta ortografica para poligono de apoyo')
# F14/F18 motor GM25-370 y servo SG90
if want('F14'):
    reset(); m = pick('GM25_370:1'); show(m)
    frame(m, (1, -1, 0.6), lens=85, w=2000, h=2000, margin=1.15)
    render('F14_A01_RENDER_motor_gm25_v001', 'CAM_F14_MOTOR_V01', 'Motor GM25-370 aislado', transparent=True)
    s = pick('SG90'); show(s)
    frame(s, (1, -1, 0.6), lens=85, w=2000, h=2000, margin=1.15)
    render('F18_A01_RENDER_servo_sg90_v001', 'CAM_F18_SERVO_V01', 'Servo SG90 aislado', transparent=True)
# F15 ultrasonido con obstaculo
if want('F15'):
    reset(); show(ALL)
    us = pick('HC_SR04'); mn, mx = bbox(us); c = (mn + mx) / 2
    amn, amx = bbox(ALL)
    # el sensor esta en el extremo de y; se coloca un plano obstaculo delante
    front_sign = -1 if c.y < (amn.y + amx.y) / 2 else 1
    y_obs = (amn.y if front_sign < 0 else amx.y) + front_sign * 0.14
    bpy.ops.mesh.primitive_cube_add(size=1, location=(c.x, y_obs, 0.1))
    ob = bpy.context.active_object; ob.name = 'SCN_Obstacle'; ob.scale = (0.25, 0.01, 0.25)
    mat = bpy.data.materials.new('SCN_ObsM'); mat.use_nodes = True
    mat.node_tree.nodes['Principled BSDF'].inputs['Base Color'].default_value = (0.05, 0.06, 0.08, 1)
    ob.data.materials.append(mat)
    ground(amn.z, color=(0.035, 0.045, 0.07))
    frame(ALL + [ob], (1, 0, 0.05), w=2560, h=1440, margin=1.05, ortho=True)
    render('F15_A01_RENDER_us_tof_v001', 'CAM_F15_US_TOF_V01', 'Lateral: HC-SR04 sobre SG90 frente a obstaculo de escena (no es parte del robot)')
    print('US_FRONT_SIGN', front_sign, 'US_CENTER', list(c))
# F16 vista baja con pista
if want('F16'):
    reset(); show(ALL); amn, amx = bbox(ALL)
    g = ground(amn.z, size=2, color=(0.16, 0.17, 0.17))
    bpy.ops.mesh.primitive_plane_add(size=1, location=(0, 0, amn.z + 0.0005))
    st = bpy.context.active_object; st.name = 'SCN_Stripe'; st.scale = (0.019, 2, 1)
    mt = bpy.data.materials.new('SCN_StripeM'); mt.use_nodes = True
    mt.node_tree.nodes['Principled BSDF'].inputs['Base Color'].default_value = (0.01, 0.01, 0.01, 1)
    mt.node_tree.nodes['Principled BSDF'].inputs['Roughness'].default_value = 0.95
    st.data.materials.append(mt)
    frame(ALL, (0.9, 1.0, 0.12), lens=40, w=2560, h=1440, margin=0.9)
    render('F16_A01_RENDER_line_low_v001', 'CAM_F16_LINE_LOW_V01', 'Vista baja sobre pista clara con banda oscura (escena)')
# F22 electronica apilada
if want('F22'):
    reset(); e = pick('Mega2560', 'MotorDriver', 'WifiShield'); show(e)
    frame(e, (0.8, -1, 0.9), lens=70, w=2560, h=1440, margin=1.1)
    render('F22_A01_RENDER_stack_v001', 'CAM_F22_STACK_V01', 'Mega 2560 + OSOYOO Motor Driver + OSOYOO WiFi Shield', transparent=True)

json.dump(MANIFEST, open(OUT + 'cameras_' + ('_'.join(sorted(ONLY)) or 'all') + '.json', 'w'), indent=1)
