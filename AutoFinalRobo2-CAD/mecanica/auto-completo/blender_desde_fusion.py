"""Importa exports/2026-10-24/parts conservando poses y materiales de Fusion.
Ejecutar dentro de Blender mediante Agent MCP, no con Python externo.
Fusion: X derecha, Y arriba, frente -Z; Blender: X derecha, Z arriba, frente +Y.
"""
import bpy, json, math
from pathlib import Path
from mathutils import Matrix, Vector, Color

BASE = Path('D:/ProyectoRobotica2/AutoFinalRobo2-CAD')
PARTS = BASE / 'exports/2026-10-24/parts'
C = Matrix(((1,0,0,0),(0,0,-1,0),(0,1,0,0),(0,0,0,1)))

def import_assembly():
    manifest = json.loads((PARTS / 'manifest.json').read_text(encoding='utf-8'))
    # Solo se retira la importación de prueba identificada previamente en scene_overview.
    for name in ('Body1', 'GM25_370'):
        obj=bpy.data.objects.get(name)
        if obj: bpy.data.objects.remove(obj, do_unlink=True)
    model=bpy.data.collections.new('AFR2_Modelo_Detallado')
    bpy.context.scene.collection.children.link(model)
    templates={}
    for entry in manifest['instances']:
        key=entry['component']
        if key in templates: continue
        before=set(bpy.data.objects)
        filepath=str(PARTS / entry['file'])
        if filepath.endswith('.usdz'):
            bpy.ops.wm.usd_import(filepath=filepath)
        else:
            # Importador OBJ: Y adelante y Z arriba deja los ejes de archivo sin giro.
            bpy.ops.wm.obj_import(filepath=filepath, forward_axis='Y', up_axis='Z')
        imported=set(bpy.data.objects)-before
        bpy.context.view_layer.update()
        meshes=[]
        for obj in imported:
            if obj.type!='MESH' or obj.hide_render or obj.hide_get(): continue
            mat=obj.matrix_world.copy()
            data=obj.data.copy()
            if filepath.endswith('.obj'): mat=C @ Matrix.Scale(.01,4) @ mat
            data.transform(mat)
            data.update()
            copy=bpy.data.objects.new(key+'__'+obj.name,data)
            model.objects.link(copy)
            meshes.append(copy)
        for obj in imported: bpy.data.objects.remove(obj,do_unlink=True)
        bpy.ops.object.select_all(action='DESELECT')
        for obj in meshes: obj.select_set(True)
        bpy.context.view_layer.objects.active=meshes[0]
        bpy.ops.object.join()
        template=bpy.context.object
        template.name=key
        templates[key]=template
        print('IMPORTED',key,len(template.data.vertices),len(template.data.polygons))
    counts={}
    for entry in manifest['instances']:
        key=entry['component']; template=templates[key]
        count=counts.get(key,0); counts[key]=count+1
        obj=template if count==0 else bpy.data.objects.new(entry['name'],template.data)
        if count: model.objects.link(obj)
        obj.name=entry['name']
        values=entry['matrix_cm']
        pose=Matrix([values[i:i+4] for i in range(0,16,4)])
        pose.translation*=.01
        obj.matrix_world=C @ pose @ C.inverted()
        obj['source_component']=key
        obj['fusion_instance']=entry['name']
    bpy.context.view_layer.update()
    floor=min((obj.matrix_world@v.co).z for obj in model.objects for v in obj.data.vertices)
    for obj in model.objects: obj.location.z-=floor
    model['floor_offset_m']=-floor
    # El exportador de Fusion entrega RGB de pantalla sin convertir a luz lineal.
    for m in {m for obj in model.objects for m in obj.data.materials if m}:
        if not m.use_nodes: continue
        bs=next((n for n in m.node_tree.nodes if n.type=='BSDF_PRINCIPLED'),None)
        if not bs: continue
        rgba=bs.inputs['Base Color'].default_value
        rgb=Color(rgba[:3]).from_srgb_to_scene_linear()
        bs.inputs['Base Color'].default_value=(*rgb,rgba[3])
        m.diffuse_color=(*rgb,rgba[3])
        bs.inputs['Roughness'].default_value=max(.32,bs.inputs['Roughness'].default_value)
    bpy.context.scene.unit_settings.system='METRIC'
    bpy.context.scene.unit_settings.length_unit='MILLIMETERS'
    print('ASSEMBLY',len(model.objects),'floor',floor)
    print('IMAGES',[(i.name,i.size[:]) for i in bpy.data.images])
    return model

if __name__=='__main__': import_assembly()
