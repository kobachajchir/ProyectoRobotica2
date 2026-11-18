import bpy, json, sys
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.gltf(filepath=sys.argv[sys.argv.index('--')+1])
objs=[o for o in bpy.data.objects if o.type=='MESH']; uniq={o.data for o in objs}
print('WEB', json.dumps({'objetos':len(objs),'mallas_unicas':len(uniq),'triangulos_unicos':sum(sum(len(p.vertices)-2 for p in m.polygons) for m in uniq),'materiales':len(bpy.data.materials)}))
