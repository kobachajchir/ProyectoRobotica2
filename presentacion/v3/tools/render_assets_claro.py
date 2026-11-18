# Renders de la presentación v4 (tema claro) desde la geometría real del proyecto.
# Uso: blender -b --factory-startup --python render_assets_claro.py -- <salida_renders> [ids...]
#   ids = nombres de archivo sin "_v001.png" (p. ej. F05_mega2560_iso). Sin ids: todos.
# Fuentes, SOLO LECTURA (nunca se guarda ningún .blend ni se toca el GLB):
#   GLB  AutoFinalRobo2-Web/public/models/AutoFinalRobo2.glb        (auto y componentes en posición de montaje)
#   BLEND AutoFinalRobo2-CAD/.../AutoFinalRobo2-Final-20261025.blend  colección AFR2_Final_Detallado_20261025
#         (objetos FINAL_RAW_*, malla de Fusion sin simplificar) para placas con primeros planos de chips.
# Escribe <salida>/manifest_renders.json con cámara, objetos y resolución de cada asset.
import bpy, bmesh, sys, os, math, json
import numpy as np
from mathutils import Vector

ARGS = sys.argv[sys.argv.index('--') + 1:]
OUT = os.path.abspath(ARGS[0]); os.makedirs(OUT, exist_ok=True)
ONLY = set(ARGS[1:])
ROOT = 'D:/ProyectoRobotica2/'
GLB = ROOT + 'AutoFinalRobo2-Web/public/models/AutoFinalRobo2.glb'
BLEND = ROOT + 'AutoFinalRobo2-CAD/mecanica/auto-completo/AutoFinalRobo2-Final-20261025.blend'
SAMPLES = int(os.environ.get('AFR2_SAMPLES', '128'))
VIEW = os.environ.get('AFR2_VIEW', 'Standard')     # AgX lavaba los colores del CAD: Standard en todos
BG_LIGHT = (0xF4, 0xF6, 0xFB)                       # fondo de escenas con fondo
FLOOR = (0xE9, 0xED, 0xF4)                          # piso claro
MANIFEST = []


def lin(c8):
    c = c8 / 255.0
    return c / 12.92 if c <= 0.04045 else ((c + 0.055) / 1.055) ** 2.4


# ---------------------------------------------------------------- escena
def load(source):
    if source == 'glb':
        bpy.ops.wm.read_factory_settings(use_empty=True)
        bpy.ops.import_scene.gltf(filepath=GLB)
    else:
        bpy.ops.wm.open_mainfile(filepath=BLEND, load_ui=False)   # solo lectura: nunca se llama a save
        keep = bpy.data.collections['AFR2_Final_Detallado_20261025']
        for o in list(bpy.data.objects):
            if keep not in o.users_collection:
                bpy.data.objects.remove(o, do_unlink=True)
        keep.hide_render = False; keep.hide_viewport = False
        for o in keep.objects:
            o.hide_render = False; o.hide_set(False)
    bpy.context.view_layer.update()   # en -b la matrix_world queda en identidad hasta actualizar
    sc = bpy.context.scene
    sc.render.engine = 'CYCLES'
    sc.cycles.samples = SAMPLES
    sc.cycles.use_denoising = True
    try:
        sc.cycles.device = 'GPU'
    except Exception:
        pass
    sc.view_settings.view_transform = VIEW
    sc.view_settings.look = 'AgX - Medium High Contrast' if VIEW == 'AgX' else 'None'
    sc.render.film_transparent = True
    sc.render.image_settings.file_format = 'PNG'
    sc.render.image_settings.color_mode = 'RGBA'
    w = bpy.data.worlds.new('W'); sc.world = w; w.use_nodes = True
    bg = w.node_tree.nodes['Background']
    bg.inputs[0].default_value = (0.95, 0.96, 0.98, 1); bg.inputs[1].default_value = 0.12   # relleno ambiente claro
    sc.view_settings.exposure = -0.4   # pedido del usuario: la primera tanda quedó sobreexpuesta
    cam_d = bpy.data.cameras.new('C'); cam = bpy.data.objects.new('C', cam_d)
    sc.collection.objects.link(cam); sc.camera = cam
    return sc, cam, cam_d


def meshes():
    return [o for o in bpy.data.objects if o.type == 'MESH' and not o.name.startswith('SCN_')]


def pick(*keys):
    return [o for o in meshes() if any(k in o.name for k in keys)]


def show_only(objs):
    names = {o.name for o in objs}
    for o in meshes():
        o.hide_render = o.name not in names


def bbox(objs):
    pts = [o.matrix_world @ Vector(c) for o in objs for c in o.bound_box]
    mn = Vector((min(p.x for p in pts), min(p.y for p in pts), min(p.z for p in pts)))
    mx = Vector((max(p.x for p in pts), max(p.y for p in pts), max(p.z for p in pts)))
    return mn, mx


def island_box(obj, dims_mm, tol=0.6):
    """Centro y tamaño (m) de la isla de malla cuyo rectángulo XY coincide con dims_mm (geometría real)."""
    M = obj.matrix_world; bm = bmesh.new(); bm.from_mesh(obj.data); bm.verts.ensure_lookup_table()
    seen = set(); best = None
    for v in bm.verts:
        if v.index in seen:
            continue
        stack = [v]; seen.add(v.index); pts = []
        while stack:
            a = stack.pop(); pts.append(M @ a.co)
            for e in a.link_edges:
                b = e.other_vert(a)
                if b.index not in seen:
                    seen.add(b.index); stack.append(b)
        mn = Vector((min(p.x for p in pts), min(p.y for p in pts), min(p.z for p in pts)))
        mx = Vector((max(p.x for p in pts), max(p.y for p in pts), max(p.z for p in pts)))
        d = (mx - mn) * 1000
        if abs(d.x - dims_mm[0]) < tol and abs(d.y - dims_mm[1]) < tol:
            if best is None or mx.z > best[1].z:
                best = (mn, mx)
    bm.free()
    return best


def lights(sc, ctr, scale):
    def area(name, off, energy, size, color):
        d = bpy.data.lights.new(name, 'AREA'); d.energy = energy; d.size = size; d.color = color
        o = bpy.data.objects.new('SCN_' + name, d); sc.collection.objects.link(o)
        o.location = ctr + Vector(off) * scale
        o.rotation_euler = (ctr - o.location).to_track_quat('-Z', 'Y').to_euler()
    k = scale * scale   # energía proporcional al cuadrado de la distancia: misma exposición en todas las tomas
    area('Key', (0.25, 0.55, 0.85), 110 * k, 1.1 * scale, (1.0, 0.98, 0.95))   # grande arriba-adelante
    area('Fill', (-0.8, 0.1, 0.35), 28 * k, 1.4 * scale, (0.85, 0.9, 1.0))     # frío suave
    area('Rim', (0.1, -0.85, 0.55), 55 * k, 0.8 * scale, (1.0, 1.0, 1.0))      # por detrás


def catcher(sc, z, size):
    # Pedido del usuario: PNG sin fondo. La sombra de contacto quedaba como un fondo gris
    # semitransparente, así que solo se agrega con AFR2_SHADOW=1.
    if os.environ.get('AFR2_SHADOW', '0') != '1':
        return None
    bpy.ops.mesh.primitive_plane_add(size=size, location=(0, 0, z - 0.0002))
    p = bpy.context.active_object; p.name = 'SCN_ShadowCatcher'; p.is_shadow_catcher = True
    return p


def floor_mat(name, c8, rough=0.85):
    m = bpy.data.materials.new(name); m.use_nodes = True
    b = m.node_tree.nodes['Principled BSDF']
    b.inputs['Base Color'].default_value = (lin(c8[0]), lin(c8[1]), lin(c8[2]), 1)
    b.inputs['Roughness'].default_value = rough
    return m


def plane(name, loc, scale, mat):
    bpy.ops.mesh.primitive_plane_add(size=1, location=loc)
    p = bpy.context.active_object; p.name = name; p.scale = scale; p.data.materials.append(mat)
    return p


def frame(sc, cam, cam_d, objs, direction, lens=60, res=(2400, 2400), ortho=False, margin=1.08, target=None):
    mn, mx = target or bbox(objs)
    ctr = (mn + mx) / 2; rad = max((mx - mn).length / 2, 0.004)
    d = Vector(direction).normalized()
    sc.render.resolution_x, sc.render.resolution_y = res; sc.render.resolution_percentage = 100
    cam_d.sensor_fit = 'HORIZONTAL'; cam_d.sensor_width = 36
    aspect = res[0] / res[1]
    up = Vector((0, 0, 1)) if abs(d.z) < 0.95 else Vector((0, 1, 0))
    right = d.cross(up).normalized(); upv = right.cross(d).normalized()
    pts = [Vector((x, y, z)) for x in (mn.x, mx.x) for y in (mn.y, mx.y) for z in (mn.z, mx.z)]
    ew = max(p.dot(right) for p in pts) - min(p.dot(right) for p in pts)
    eh = max(p.dot(upv) for p in pts) - min(p.dot(upv) for p in pts)
    if ortho:
        cam_d.type = 'ORTHO'; cam_d.ortho_scale = max(ew, eh * aspect) * margin; dist = rad * 6
    else:
        cam_d.type = 'PERSP'; cam_d.lens = lens
        fov_h = 2 * math.atan(18 / lens); fov_v = 2 * math.atan(math.tan(fov_h / 2) / aspect)
        dist = max((ew / 2) / math.tan(fov_h / 2), (eh / 2) / math.tan(fov_v / 2)) * margin + rad
    cam_d.clip_start = 0.0005; cam_d.clip_end = 100
    cam.location = ctr + d * dist
    cam.rotation_euler = (ctr - cam.location).to_track_quat('-Z', 'Y' if abs(d.z) < 0.95 else 'Z').to_euler()
    if abs(d.z) >= 0.95:   # planta: arriba de la imagen = +Y (frente del auto)
        cam.rotation_euler = (0, 0, 0) if d.z > 0 else (math.pi, 0, 0)
    lights(sc, ctr, max(dist, rad * 2.5))
    return ctr, rad, dist


def crop_and_finish(path, background=None, margin=0.04):
    """Recorta al contenido (alfa > 0.02, incluye la sombra) con margen del 4 %; opcional: compone sobre color."""
    img = bpy.data.images.load(path); w, h = img.size
    px = np.array(img.pixels[:], dtype=np.float32).reshape(h, w, 4)
    a = px[:, :, 3]; ys, xs = np.where(a > 0.22)   # pieza + sombra de contacto; descarta la penumbra lejana
    if background is None and len(xs):
        x0, x1, y0, y1 = xs.min(), xs.max(), ys.min(), ys.max()
        mw, mh = int((x1 - x0) * margin), int((y1 - y0) * margin)
        x0, y0 = max(0, x0 - mw), max(0, y0 - mh); x1, y1 = min(w - 1, x1 + mw), min(h - 1, y1 + mh)
        px = px[y0:y1 + 1, x0:x1 + 1]
    if background is not None:
        bgc = np.array([c / 255.0 for c in background], dtype=np.float32)   # pixels de PNG ya en sRGB
        al = px[:, :, 3:4]
        px = np.concatenate([px[:, :, :3] * al + bgc * (1 - al), np.ones_like(al)], axis=2)
    nh, nw = px.shape[:2]
    out = bpy.data.images.new('crop', nw, nh, alpha=True)
    out.pixels = px.ravel().tolist(); out.filepath_raw = path; out.file_format = 'PNG'; out.save()
    bpy.data.images.remove(img); bpy.data.images.remove(out)
    return nw, nh


def render(sc, cam, cam_d, aid, source, objs, note, background=None, crop=True):
    background = None   # pedido del usuario: todos los PNG sin fondo (transparentes)
    path = os.path.join(OUT, aid + '_v001.png')
    sc.render.filepath = path
    bpy.ops.render.render(write_still=True)
    size = crop_and_finish(path, background) if (crop or background) else (sc.render.resolution_x, sc.render.resolution_y)
    MANIFEST.append({
        'archivo': 'renders/' + aid + '_v001.png',
        'origen': {'fuente': GLB if source == 'glb' else BLEND + ' (colección AFR2_Final_Detallado_20261025)',
                   'objetos': [o.name for o in objs]},
        'camara': {'tipo': cam_d.type, 'lente_mm': round(cam_d.lens, 1) if cam_d.type == 'PERSP' else None,
                   'ortho_scale_m': round(cam_d.ortho_scale, 4) if cam_d.type == 'ORTHO' else None,
                   'posicion_m': [round(v, 4) for v in cam.location],
                   'direccion': [round(v, 4) for v in (cam.matrix_world.to_quaternion() @ Vector((0, 0, -1)))]},
        'resolucion_render': [sc.render.resolution_x, sc.render.resolution_y], 'resolucion_final': list(size),
        'fondo': ('#%02X%02X%02X' % background) if background else 'transparente', 'render': {
            'motor': 'Cycles', 'muestras': SAMPLES, 'denoise': True, 'view_transform': VIEW,
            'look': sc.view_settings.look}, 'nota': note, 'estado': 'hecho'})
    print('RENDERED', path, size)


def want(i):
    return not ONLY or i in ONLY


# ================================================================ assets
COMP = (2400, 2400); CAR = (3840, 2160)
JOBS = []
def job(aid, source):
    def deco(f):
        JOBS.append((aid, source, f)); return f
    return deco


def comp(aid, keys, direction, lens=85, ortho=False, note='', shadow=True, source='glb', exclude=()):
    @job(aid, source)
    def _(sc, cam, cam_d):
        objs = [o for o in pick(*keys) if not any(x in o.name for x in exclude)]
        show_only(objs)
        mn, mx = bbox(objs)
        if shadow:
            catcher(sc, mn.z, (mx - mn).length * 3)
        frame(sc, cam, cam_d, objs, direction, lens=lens, res=COMP, ortho=ortho)
        render(sc, cam, cam_d, aid, source, objs, note)


# 1 · componentes aislados (GLB: orientación de montaje; frente del auto en +Y)
comp('F04_bateria_superior', ('Celda_18650', 'Portabaterias'), (0, -0.28, 1), note='2 celdas 18650 + portabaterías, planta con leve perspectiva')
comp('F04_oled_encoder_superior', ('UI_B2_Fondo_OLED',), (0, -1, 0.9), note='Panel UI (OLED 0,96 + encoder + PCB + soporte) desde arriba y atrás: el OLED mira hacia -Y')
comp('F04_sensor_linea_inferior', ('SensorLinea_5IR',), (0, 0.15, -1), shadow=False, note='Vista inferior: cinco pares emisor/receptor; sin shadow catcher para no tapar la vista')
comp('F04_hcsr04_frente', ('AFR2_HC_SR04', 'Soporte_HCSR04'), (0, 1, 0.12), note='Frente (+Y), transductores a cámara; sin servo')
comp('F04_driver_superior', ('OSOYOOMotorDriver',), (0, -0.15, 1), note='OSOYOO Model Y V2 desde arriba')
comp('F04_motor_gm25_iso', ('GM25_370:1',), (1, -1, 0.6), note='GM25-370 con encoder Hall, isométrica 3/4')
comp('F04_servo_sg90_iso', ('SG90',), (1, 1, 0.6), note='SG90 con su salida, isométrica 3/4')
comp('F05_stack_mega_driver_wifi', ('Mega2560', 'OSOYOOMotorDriver', 'OSOYOOWifiShield'), (0.8, -1, 0.9), lens=70, note='Mega + Driver + WiFi en su posición de montaje del GLB')
# placas con primer plano: malla detallada de Fusion (.blend final abierto sin guardar)
comp('F05_mega2560_iso', ('FINAL_RAW_Mega2560',), (0.9, -1, 0.95), source='blend', note='Malla detallada de Fusion; chip ATmega2560 visible')
comp('F05_mega2560_superior', ('FINAL_RAW_Mega2560',), (0, 0, 1), ortho=True, source='blend', note='Planta ortográfica, malla detallada')
comp('F05_wifi_shield_iso', ('FINAL_RAW_OSOYOOWifiShield',), (0.9, -1, 0.95), source='blend', note='Malla detallada; módulo ESP-12 visible')
comp('F05_wifi_shield_superior', ('FINAL_RAW_OSOYOOWifiShield',), (0, 0, 1), ortho=True, source='blend', note='Planta ortográfica, malla detallada')


def detail(aid, key, dims, note, k=1.6):
    @job(aid, 'blend')
    def _(sc, cam, cam_d):
        objs = pick(key); show_only(objs)
        isl = island_box(objs[0], dims)
        mn, mx = bbox(objs); catcher(sc, mn.z, (mx - mn).length * 3)
        if isl is None:
            MANIFEST.append({'archivo': 'renders/' + aid + '_v001.png', 'estado': 'pendiente',
                             'motivo': 'no se encontró la isla de malla %s mm' % (dims,)}); return
        c = (isl[0] + isl[1]) / 2; half = Vector((dims[0], dims[1], 6)) / 1000 * k
        frame(sc, cam, cam_d, objs, (0.45, -0.75, 1), lens=135, res=COMP, target=(c - half, c + half), margin=1.0)
        render(sc, cam, cam_d, aid, 'blend', objs, note + ' · isla %s mm en %s m' % (dims, [round(v, 4) for v in c]), crop=False)


detail('F05_mega2560_chip_detalle', 'FINAL_RAW_Mega2560', (13.7, 13.7), 'Primer plano del ATmega2560 (TQFP-100, cuerpo 14×14 mm)')
detail('F05_esp12_detalle', 'FINAL_RAW_OSOYOOWifiShield', (24.0, 16.0), 'Primer plano del módulo ESP-12 (24×16 mm)', k=0.75)


# 2 · auto y placas
def car(aid, direction, lens=70, ortho=False, note='', keys=None, prep=None, background=None, shadow=True, res=CAR):
    @job(aid, 'glb')
    def _(sc, cam, cam_d):
        objs = pick(*keys) if keys else meshes(); show_only(objs)
        if prep:
            prep(sc, objs)
        mn, mx = bbox(objs)
        if shadow:
            catcher(sc, mn.z, 4)
        frame(sc, cam, cam_d, objs, direction, lens=lens, res=res, ortho=ortho)
        render(sc, cam, cam_d, aid, 'glb', objs, note, background=background)


car('F01_auto_hero', (1, 1.3, 0.62), note='Auto completo 3/4 frontal (portada)')
car('F_cierre_auto_hero', (-1.1, 1.05, 0.55), note='Auto completo 3/4 desde el lado opuesto (cierre)')
car('F04_auto_iso_superior', (0.55, 0.8, 1.15), lens=60, note='Auto completo 3/4 superior (arquitectura)')
car('F07_placas_iso', (1, 1.2, 0.8), keys=('CapaSuperior', 'CapaInferior'), note='Solo placas, posición de montaje')


def separate(sc, objs):
    for o in objs:
        if 'CapaSuperior' in o.name:
            o.location.z += 0.06
    bpy.context.view_layer.update()


car('F07_placas_iso_separadas', (1, 1.2, 0.8), keys=('CapaSuperior', 'CapaInferior'), prep=separate,
    note='Placa superior elevada 60 mm SOLO para visualización; no es posición de montaje')
car('F07_placa_superior_planta', (0, 0, 1), ortho=True, keys=('CapaSuperior',), res=COMP, note='Planta ortográfica, +Y arriba')
car('F07_placa_inferior_planta', (0, 0, 1), ortho=True, keys=('CapaInferior',), res=COMP, note='Planta ortográfica, +Y arriba')


@job('F06_rueda_iso', 'glb')
def _(sc, cam, cam_d):
    objs = pick('Rueda80_ManoA:1'); show_only(objs); mn, mx = bbox(objs); catcher(sc, mn.z, 1)
    frame(sc, cam, cam_d, objs, (1, -0.9, 0.45), lens=85, res=COMP)
    render(sc, cam, cam_d, 'F06_rueda_iso', 'glb', objs, 'Rueda Mecanum mano A; bbox 47×109,2×109,2 mm (firmware usa 80 mm: discrepancia abierta)')


@job('F06_rueda_lateral', 'glb')
def _(sc, cam, cam_d):
    objs = pick('Rueda80_ManoA:1'); show_only(objs); mn, mx = bbox(objs); catcher(sc, mn.z, 1)
    ax = mx - mn; i = min(range(3), key=lambda k: ax[k]); d = [0, 0, 0]; d[i] = 1
    frame(sc, cam, cam_d, objs, d, lens=200, res=COMP)
    render(sc, cam, cam_d, 'F06_rueda_lateral', 'glb', objs, 'Rueda vista según su eje')


LAYERS = [('Rueda', 0), ('Casquillo', 0), ('Prisionero', 0), ('GM25', 0), ('Soportes_Motores', 0),
          ('CapaInferior', 0), ('Portabaterias', 0.03), ('Celda', 0.03), ('Separadores', 0.06), ('Fijaciones', 0.06),
          ('CapaSuperior', 0.06), ('Mega2560', 0.10), ('MotorDriver', 0.14), ('WifiShield', 0.18),
          ('Voltimetro', 0.10), ('SensorLinea', -0.035), ('HC_SR04', 0.10), ('Soporte_HCSR04', 0.10),
          ('SG90', 0.10), ('UI_B2', 0.22)]   # mismos desplazamientos que render_assets.py (v3)


def explode(sc, objs):
    for o in objs:
        o.location.z += next((z for k, z in LAYERS if k in o.name), 0)
    bpy.context.view_layer.update()


car('F08_explosionado', (1.0, 1.25, 0.55), lens=60, prep=explode, note='Explosión vertical solo visual (mismos desplazamientos que la v3)')


@job('F09_panel_ui_montaje', 'glb')
def _(sc, cam, cam_d):
    objs = meshes(); show_only(objs); ui = pick('UI_B2_Fondo_OLED')
    frame(sc, cam, cam_d, ui, (0.35, -1.0, 0.75), lens=85, res=CAR, margin=1.35)
    render(sc, cam, cam_d, 'F09_panel_ui_montaje', 'glb', ui, 'Primer plano del panel OLED/encoder montado (auto completo visible)', background=BG_LIGHT)


car('F_lateral_ortho', (1, 0, 0), ortho=True, note='Lateral ortográfica (+Y a la derecha)')
car('F_planta_ortho', (0, 0, 1), ortho=True, note='Planta ortográfica (+Y arriba)')


@job('F_us_tof', 'glb')
def _(sc, cam, cam_d):
    objs = meshes(); show_only(objs); amn, amx = bbox(objs)
    us = pick('AFR2_HC_SR04'); umn, umx = bbox(us); c = (umn + umx) / 2
    y_obs = amx.y + 0.14
    bpy.ops.mesh.primitive_cube_add(size=1, location=(c.x, y_obs, 0.1))
    ob = bpy.context.active_object; ob.name = 'SCN_Obstaculo'; ob.scale = (0.25, 0.01, 0.25)
    ob.data.materials.append(floor_mat('SCN_ObsM', (0xC9, 0xCF, 0xD9), 0.6))
    frame(sc, cam, cam_d, objs + [ob], (1, 0, 0.05), res=(2560, 1440), ortho=True, margin=1.05)
    render(sc, cam, cam_d, 'F_us_tof', 'glb', us, 'Lateral con obstáculo de escena gris claro (no es parte del robot); sin piso ni fondo', background=BG_LIGHT)


@job('F_linea_baja', 'glb')
def _(sc, cam, cam_d):
    objs = meshes(); show_only(objs); amn, amx = bbox(objs)
    ls = pick('SensorLinea_5IR'); lmn, lmx = bbox(ls); cx = (lmn.x + lmx.x) / 2
    plane('SCN_Piso', (cx, 0, amn.z - 0.0002), (0.6, 0.6, 1), floor_mat('SCN_PisoM', FLOOR))   # tramo de pista, resto transparente
    plane('SCN_Cinta', (cx, 0, amn.z + 0.0003), (0.019, 0.6, 1), floor_mat('SCN_CintaM', (8, 8, 8), 0.95))
    frame(sc, cam, cam_d, objs, (0.9, 1.0, 0.12), lens=40, res=(2560, 1440), margin=0.92)
    render(sc, cam, cam_d, 'F_linea_baja', 'glb', ls, 'Vista baja sobre un tramo de pista #E9EDF4 de 600 mm con banda negra de 19 mm bajo el sensor; fuera del tramo, transparente', background=BG_LIGHT)


# ---------------------------------------------------------------- ejecución
for aid, source, f in JOBS:
    if not want(aid):
        continue
    sc, cam, cam_d = load(source)
    f(sc, cam, cam_d)

mpath = os.path.join(OUT, 'manifest_renders.json')
old = json.load(open(mpath, encoding='utf8')) if os.path.exists(mpath) else []
done = {m['archivo'] for m in MANIFEST}
json.dump([m for m in old if m['archivo'] not in done] + MANIFEST, open(mpath, 'w', encoding='utf8'), ensure_ascii=False, indent=1)
print('MANIFEST', mpath, len(MANIFEST))
