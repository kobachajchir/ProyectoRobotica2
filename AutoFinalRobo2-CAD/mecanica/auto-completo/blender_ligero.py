"""Copia ligera con instancias, normales suaves, materiales compartidos y Draco.
Ejecutar en Blender tras importar el ensamble. Conserva el detallado oculto.
"""
import bpy, bmesh, math, json
from pathlib import Path

BASE=Path('D:/ProyectoRobotica2/AutoFinalRobo2-CAD')
OUT=BASE/'exports/2026-10-24/AutoFinalRobo2-ligero.glb'

def export_light():
    source=bpy.data.collections['AFR2_Modelo_Detallado']
    target=bpy.data.collections.new('AFR2_Modelo_Ligero')
    bpy.context.scene.collection.children.link(target)
    meshes={}; materials={}
    for original in source.objects:
        obj=original.copy()
        if original.data not in meshes:
            data=original.data.copy()
            bm=bmesh.new();bm.from_mesh(data)
            bmesh.ops.remove_doubles(bm,verts=list(bm.verts),dist=1e-7)
            bmesh.ops.dissolve_limit(bm,angle_limit=math.radians(.15),verts=list(bm.verts),edges=list(bm.edges),delimit={'MATERIAL','UV','NORMAL'})
            bm.to_mesh(data);bm.free()
            # No se cambia la paleta: se unifican materiales con propiedades iguales.
            for i,m in enumerate(data.materials):
                if not m or not m.use_nodes: continue
                bs=next((n for n in m.node_tree.nodes if n.type=='BSDF_PRINCIPLED'),None)
                if not bs:continue
                key=tuple(round(v,5) for v in bs.inputs['Base Color'].default_value)+(round(bs.inputs['Metallic'].default_value,4),round(bs.inputs['Roughness'].default_value,4))
                if key not in materials: materials[key]=m
                data.materials[i]=materials[key]
            for polygon in data.polygons:polygon.use_smooth=True
            data.set_sharp_from_angle(angle=math.radians(35))
            meshes[original.data]=data
        obj.data=meshes[original.data]
        obj.name='AFR2_'+original.name
        target.objects.link(obj)
    source.hide_render=True;source.hide_viewport=True
    ratios={'OSOYOOWifiShield':.35,'Mega2560_CAD':.45,'SG90':.35,
            'SensorLinea_5IR':.12,'HC_SR04':.4,'Portabaterias_SinCableExterior':.2,
            'Rueda80_ManoA':.55,'Rueda80_ManoB':.55}
    seen=set()
    for obj in target.objects:
        if obj.data in seen: continue
        shared=[x for x in target.objects if x.data==obj.data]
        ratio=ratios.get(obj.get('source_component'),1)
        if ratio!=1:
            mod=obj.modifiers.new('Reduccion_visual','DECIMATE')
            mod.ratio=ratio;mod.use_collapse_triangulate=True
            dg=bpy.context.evaluated_depsgraph_get();dg.update()
            data=bpy.data.meshes.new_from_object(obj.evaluated_get(dg),depsgraph=dg)
            obj.modifiers.clear()
            for other in shared: other.data=data
        if len(obj.data.polygons)>6000 and 'Rueda' not in obj.name:
            mod=obj.modifiers.new('Ajuste_presupuesto','DECIMATE')
            mod.ratio=.65;mod.use_collapse_triangulate=True
            dg=bpy.context.evaluated_depsgraph_get();dg.update()
            data=bpy.data.meshes.new_from_object(obj.evaluated_get(dg),depsgraph=dg)
            obj.modifiers.clear()
            for other in shared: other.data=data
        if 'SensorLinea' in obj.name:
            bm=bmesh.new();bm.from_mesh(obj.data)
            bmesh.ops.dissolve_degenerate(bm,dist=1e-8,edges=list(bm.edges))
            bmesh.ops.triangulate(bm,faces=list(bm.faces))
            bm.to_mesh(obj.data);bm.free();obj.data.update()
        seen.add(obj.data)
    bpy.ops.object.select_all(action='DESELECT')
    for obj in target.objects:obj.select_set(True)
    bpy.context.view_layer.objects.active=next(iter(target.objects))
    bpy.ops.export_scene.gltf(filepath=str(OUT),export_format='GLB',use_selection=True,export_materials='EXPORT',export_cameras=False,export_lights=False,export_animations=False,export_texcoords=True,export_normals=True,export_draco_mesh_compression_enable=True,export_draco_mesh_compression_level=10,export_draco_position_quantization=13,export_draco_normal_quantization=8,export_draco_texcoord_quantization=12)
    unique={obj.data for obj in target.objects}
    stats={'file':str(OUT),'bytes':OUT.stat().st_size,'objects':len(target.objects),'unique_meshes':len(unique),'unique_materials':len(materials),'triangles_unique':sum(len(p.vertices)-2 for d in unique for p in d.polygons),'compression':'KHR_draco_mesh_compression','textures_exported':[]}
    (BASE/'docs/blender-exportacion-20261024.json').write_text(json.dumps(stats,indent=2),encoding='utf-8')
    print(json.dumps(stats))

if __name__=='__main__':export_light()
