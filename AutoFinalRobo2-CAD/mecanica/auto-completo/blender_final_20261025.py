"""Importa Fusion final, conserva la escena anterior y exporta GLB <900000 B.

Ejecutar dentro del Blender abierto mediante Agent MCP. No toca el CAD fuente.
"""
import bpy,bmesh,math,json,hashlib,struct
from pathlib import Path
from mathutils import Matrix,Color
BASE=Path('D:/ProyectoRobotica2/AutoFinalRobo2-CAD')
PARTS=BASE/'exports/2026-10-25-final/parts'
OUT=BASE/'exports/2026-10-25-final/AutoFinalRobo2-final.glb'
C=Matrix(((1,0,0,0),(0,0,-1,0),(0,1,0,0),(0,0,0,1)))
RAW='AFR2_Final_Detallado_20261025'
LIGHT='AFR2_Final_Web_20261025'

def import_final():
    assert bpy.data.collections.get(RAW) is None,'La importacion final ya existe'
    previous=bpy.context.scene
    scene=bpy.data.scenes.new('AFR2_Final_20261025');bpy.context.window.scene=scene
    scene.unit_settings.system='METRIC';scene.unit_settings.length_unit='MILLIMETERS'
    scene.render.engine='CYCLES';scene.cycles.samples=32
    scene.render.resolution_x=1400;scene.render.resolution_y=1000;scene.render.resolution_percentage=100
    scene.world=previous.world
    for ob in previous.objects:
        if ob.type in {'CAMERA','LIGHT'} and ob.name.startswith('AFR2_') or ob.name=='AFR2_Suelo_Estudio':scene.collection.objects.link(ob)
    scene.camera=previous.camera
    target=bpy.data.collections.new(RAW);scene.collection.children.link(target)
    manifest=json.loads((PARTS/'manifest.json').read_text(encoding='utf-8'))
    templates={}
    for entry in manifest['instances']:
        key=entry['file']
        if key in templates:continue
        before=set(bpy.data.objects);path=PARTS/key
        if path.suffix=='.usdz':bpy.ops.wm.usd_import(filepath=str(path))
        else:bpy.ops.wm.obj_import(filepath=str(path),forward_axis='Y',up_axis='Z')
        imported=set(bpy.data.objects)-before;bpy.context.view_layer.update();meshes=[]
        for ob in imported:
            if ob.type!='MESH' or ob.hide_render or ob.hide_get():continue
            data=ob.data.copy();matrix=ob.matrix_world.copy()
            if path.suffix=='.obj':matrix=C@Matrix.Scale(.01,4)@matrix
            data.transform(matrix);data.update()
            copy=bpy.data.objects.new('FINAL_RAW_'+ob.name,data);target.objects.link(copy);meshes.append(copy)
        for ob in imported:bpy.data.objects.remove(ob,do_unlink=True)
        assert meshes,'No hay mallas visibles en '+key
        bpy.ops.object.select_all(action='DESELECT')
        for ob in meshes:ob.select_set(True)
        bpy.context.view_layer.objects.active=meshes[0];bpy.ops.object.join()
        templates[key]=bpy.context.object
        print('IMPORTED',entry['component'],len(bpy.context.object.data.vertices))
    counts={}
    for entry in manifest['instances']:
        key=entry['file'];template=templates[key];count=counts.get(key,0);counts[key]=count+1
        ob=template if count==0 else bpy.data.objects.new(entry['name'],template.data)
        if count:target.objects.link(ob)
        ob.name='FINAL_RAW_'+entry['name']
        values=entry['matrix_cm'];pose=Matrix([values[i:i+4] for i in range(0,16,4)]);pose.translation*=.01
        ob.matrix_world=C@pose@C.inverted();ob['source_component']=entry['component'];ob['fusion_instance']=entry['name']
    bpy.context.view_layer.update()
    floor=min((ob.matrix_world@v.co).z for ob in target.objects for v in ob.data.vertices)
    for ob in target.objects:ob.location.z-=floor
    target['floor_offset_m']=-floor
    for material in {m for ob in target.objects for m in ob.data.materials if m}:
        if not material.use_nodes:continue
        bs=next((n for n in material.node_tree.nodes if n.type=='BSDF_PRINCIPLED'),None)
        if bs:
            rgba=bs.inputs['Base Color'].default_value;rgb=Color(rgba[:3]).from_srgb_to_scene_linear()
            bs.inputs['Base Color'].default_value=(*rgb,rgba[3]);material.diffuse_color=(*rgb,rgba[3])
    print('FINAL_IMPORTED',len(target.objects),'floor_offset_m',-floor)

def prepare_light():
    source=bpy.data.collections[RAW];target=bpy.data.collections.new(LIGHT);bpy.context.scene.collection.children.link(target)
    meshes={};materials={}
    for original in source.objects:
        ob=original.copy()
        if original.data not in meshes:
            data=original.data.copy();bm=bmesh.new();bm.from_mesh(data)
            bmesh.ops.remove_doubles(bm,verts=list(bm.verts),dist=1e-7)
            bmesh.ops.dissolve_limit(bm,angle_limit=math.radians(.15),verts=list(bm.verts),edges=list(bm.edges),delimit={'MATERIAL','UV','NORMAL'})
            bm.to_mesh(data);bm.free()
            for i,m in enumerate(data.materials):
                if not m or not m.use_nodes:continue
                bs=next((n for n in m.node_tree.nodes if n.type=='BSDF_PRINCIPLED'),None)
                if not bs:continue
                key=tuple(round(v,6) for v in bs.inputs['Base Color'].default_value)+tuple(round(bs.inputs[k].default_value,5) for k in ['Metallic','Roughness','Alpha'])
                if key not in materials:materials[key]=m
                data.materials[i]=materials[key]
            for p in data.polygons:p.use_smooth=True
            data.set_sharp_from_angle(angle=math.radians(35));meshes[original.data]=data
        ob.data=meshes[original.data];ob.name='AFR2_'+original['fusion_instance'];target.objects.link(ob)
    source.hide_render=True;source.hide_viewport=True
    ratios={'OSOYOOWifiShield':.35,'Mega2560_CAD':.45,'SG90':.35,'SensorLinea_5IR':.12,'HC_SR04':.4,'Portabaterias_SinCableExterior':.2,'Rueda80_ManoA':.55,'Rueda80_ManoB':.55,'AFR2_UI_B2_Fondo_OLED15_Auto5':.65}
    seen=set()
    for ob in target.objects:
        if ob.data in seen:continue
        shared=[x for x in target.objects if x.data==ob.data]
        ratio=ratios.get(ob.get('source_component'),1)
        if len(ob.data.polygons)>6000 and 'Rueda' not in ob.name:ratio*=.65
        if ratio<1:
            mod=ob.modifiers.new('Reduccion_solo_web','DECIMATE');mod.ratio=ratio;mod.use_collapse_triangulate=True
            dg=bpy.context.evaluated_depsgraph_get();dg.update();data=bpy.data.meshes.new_from_object(ob.evaluated_get(dg),depsgraph=dg);ob.modifiers.clear()
            for other in shared:other.data=data
        if 'SensorLinea' in ob.name:
            bm=bmesh.new();bm.from_mesh(ob.data);bmesh.ops.dissolve_degenerate(bm,dist=1e-8,edges=list(bm.edges));bmesh.ops.triangulate(bm,faces=list(bm.faces));bm.to_mesh(ob.data);bm.free()
        seen.add(ob.data)
    print('FINAL_LIGHT',len(target.objects),len({o.data for o in target.objects}))

def fit_budget():
    """Receta final aplicada: conserva ruedas y reduce más la electrónica."""
    target=bpy.data.collections[LIGHT];seen=set()
    for ob in target.objects:
        if ob.data in seen:continue
        shared=[x for x in target.objects if x.data==ob.data]
        if len(ob.data.polygons)>4000:
            ratio=.85 if 'Rueda' in ob.name else (.65 if 'AFR2_UI' in ob.name else .72)
            mod=ob.modifiers.new('Presupuesto_900k','DECIMATE');mod.ratio=ratio;mod.use_collapse_triangulate=True
            dg=bpy.context.evaluated_depsgraph_get();dg.update()
            data=bpy.data.meshes.new_from_object(ob.evaluated_get(dg),depsgraph=dg);ob.modifiers.clear()
            for other in shared:other.data=data
        ob.data.validate(verbose=False,clean_customdata=False);ob.data.update();seen.add(ob.data)
    ob=next(o for o in target.objects if 'AFR2_UI' in o.name)
    mod=ob.modifiers.new('Ajuste_panel_900k','DECIMATE');mod.ratio=.8;mod.use_collapse_triangulate=True
    dg=bpy.context.evaluated_depsgraph_get();dg.update()
    data=bpy.data.meshes.new_from_object(ob.evaluated_get(dg),depsgraph=dg);ob.modifiers.clear();ob.data=data
    data.validate(verbose=False,clean_customdata=False);data.update()

def export_final(position_bits=11):
    target=bpy.data.collections[LIGHT];bpy.ops.object.select_all(action='DESELECT')
    for ob in target.objects:ob.select_set(True)
    bpy.context.view_layer.objects.active=next(iter(target.objects))
    bpy.ops.export_scene.gltf(filepath=str(OUT),export_format='GLB',use_selection=True,use_active_scene=True,export_materials='EXPORT',export_cameras=False,export_lights=False,export_animations=False,export_normals=True,export_draco_mesh_compression_enable=True,export_draco_mesh_compression_level=10,export_draco_position_quantization=position_bits,export_draco_normal_quantization=8,export_draco_texcoord_quantization=12)
    unique={ob.data for ob in target.objects}
    raw=OUT.read_bytes();json_bytes=struct.unpack_from('<I',raw,12)[0];gltf=json.loads(raw[20:20+json_bytes])
    stats={'file':OUT.relative_to(BASE).as_posix(),'bytes':len(raw),'sha256':hashlib.sha256(raw).hexdigest(),'objects':len(target.objects),'meshes':len(gltf['meshes']),'triangles_unique':sum(len(p.vertices)-2 for m in unique for p in m.polygons),'materials':len(gltf['materials']),'compression':'KHR_draco_mesh_compression','position_bits':position_bits,'budget_bytes':900000,'under_budget':len(raw)<900000}
    (BASE/'docs/modelo-final-20261025.json').write_text(json.dumps(stats,indent=2),encoding='utf-8');print(json.dumps(stats))
