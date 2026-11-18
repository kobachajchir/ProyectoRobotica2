import bpy, os, json, sys
out = sys.argv[sys.argv.index('--')+1]
res = {}
def stats(col):
    objs=[o for o in col.all_objects if o.type=='MESH']
    uniq={o.data for o in objs}
    tris_u=sum(sum(len(p.vertices)-2 for p in m.polygons) for m in uniq)
    tris_i=sum(sum(len(p.vertices)-2 for p in o.data.polygons) for o in objs)
    mats={m for o in objs for m in o.data.materials if m}
    return {'objetos':len(objs),'mallas_unicas':len(uniq),'triangulos_unicos':tris_u,'triangulos_instanciados':tris_i,'materiales':len(mats)}
det=bpy.data.collections['AFR2_Final_Detallado_20261025']; web=bpy.data.collections['AFR2_Final_Web_20261025']
res['detallada']=stats(det); res['web_en_blend']=stats(web)
# export de la detallada sin Draco (fuera del repo)
for o in bpy.context.view_layer.objects: o.select_set(False)
det.hide_viewport=False; det.hide_render=False
lc=bpy.context.view_layer.layer_collection.children[det.name]; lc.exclude=False; lc.hide_viewport=False
for o in det.all_objects:
    o.hide_set(False); o.select_set(True)
bpy.ops.export_scene.gltf(filepath=out, export_format='GLB', use_selection=True, export_materials='EXPORT', export_cameras=False, export_lights=False, export_animations=False, export_normals=True, export_draco_mesh_compression_enable=False)
res['glb_detallado_sin_draco_bytes']=os.path.getsize(out)
print('PESO', json.dumps(res))
