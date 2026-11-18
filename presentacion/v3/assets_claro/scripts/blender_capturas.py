# Instancia propia de Blender (no la sesión del usuario). Abre el .blend final, prepara vistas y captura
# la ventana con bpy.ops.screen.screenshot. Nunca guarda: sale con os._exit.
import bpy, os, sys
OUT = 'D:/ProyectoRobotica2/presentacion/v3/assets_claro/capturas/'
DET, WEB = 'AFR2_Final_Detallado_20261025', 'AFR2_Final_Web_20261025'
steps = []


def areas(t):
    w = bpy.context.window_manager.windows[0]
    return w, [a for a in w.screen.areas if a.type == t]


def ctx(t):
    w, a = areas(t)
    if not a:
        return None
    reg = [r for r in a[0].regions if r.type == 'WINDOW'][0]
    return dict(window=w, screen=w.screen, area=a[0], region=reg)


def show(col_name):
    vl = bpy.context.view_layer
    for lc in vl.layer_collection.children:
        lc.exclude = lc.name != col_name and lc.name in (DET, WEB)
    c = bpy.data.collections[col_name]; c.hide_viewport = False
    vl.update()
    vis = {o.name for o in vl.objects}
    for o in bpy.data.objects:
        if c in o.users_collection:
            o.hide_viewport = False
            if o.name in vis:
                vl.objects[o.name].hide_set(False)


def prep_view(sel=None):
    o = ctx('VIEW_3D')
    sp = o['area'].spaces.active
    sp.overlay.show_stats = True
    sp.shading.type = 'SOLID'; sp.shading.color_type = 'MATERIAL'
    for ob in bpy.context.view_layer.objects:
        ob.select_set(False)
    if sel:
        sel.select_set(True); bpy.context.view_layer.objects.active = sel
    with bpy.context.temp_override(**o):
        if sel:
            bpy.ops.view3d.view_selected()
        else:
            bpy.ops.view3d.view_all()
    oc = ctx('OUTLINER')
    if oc:
        with bpy.context.temp_override(**oc):
            bpy.ops.outliner.show_one_level(open=True)
            bpy.ops.outliner.show_one_level(open=True)


def shot(name):
    w = bpy.context.window_manager.windows[0]
    with bpy.context.temp_override(window=w, screen=w.screen, area=w.screen.areas[0]):
        bpy.ops.screen.screenshot(filepath=OUT + name, check_existing=False)
    print('SHOT', name)


def frame_col(name):
    col = bpy.data.collections[name]
    vl = bpy.context.view_layer
    objs = [vl.objects[o.name] for o in bpy.data.objects if col in o.users_collection and o.name in vl.objects]
    for o in vl.objects:
        o.select_set(False)
    for o in objs:
        o.select_set(True)
    vl.objects.active = objs[0]
    o = ctx('VIEW_3D')
    with bpy.context.temp_override(**o):
        bpy.ops.view3d.view_selected()
    for ob in objs:
        ob.select_set(False)


def s1():
    show(DET); prep_view()
def s1b():
    frame_col(DET)
def s2():
    shot('F09_blender_importado_v001.png'); show(WEB); prep_view()
def s2b():
    frame_col(WEB)
def s3():
    shot('F09_blender_ligero_v001.png')
    show(DET)
    det = bpy.data.collections[DET]
    ob = next(o for o in bpy.data.objects if det in o.users_collection and 'SensorLinea' in o.name)
    m = ob.modifiers.new('Reduccion_solo_web', 'DECIMATE'); m.ratio = 0.12; m.use_collapse_triangulate = True
    prep_view(ob)
def s3b():
    pc = ctx('PROPERTIES')
    if pc:
        pc['area'].spaces.active.context = 'MODIFIER'
def s4():
    shot('F09_blender_decimate_v001.png')
    os._exit(0)   # sin guardar


SEQ = [s1, s1b, s2, s2b, s3, s3b, s4]


def tick():
    import traceback
    if SEQ:
        try:
            SEQ.pop(0)()
        except Exception:
            traceback.print_exc(); sys.stdout.flush(); os._exit(1)
        return 4.0
    return None


bpy.app.timers.register(tick, first_interval=6.0)
