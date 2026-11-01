"""Escena de render propia: angulo frontal actual de Fusion, sin mutar el CAD.
Ejecutar en Blender con las piezas completas v17 cargadas. Salida regenerable.
"""
import bpy,math,json
from pathlib import Path
from mathutils import Matrix,Vector

ROOT=Path('D:/ProyectoRobotica2');OUT=ROOT/'AutoFinalRobo2-CAD/renders/frontal-20261031'
OUT.mkdir(parents=True,exist_ok=True)
scene=bpy.data.scenes.new('AFR2_Frontal_Fusion_20261031')
bpy.context.window.scene=scene
models=bpy.data.collections.new('AFR2_Render_Modelos');scene.collection.children.link(models)
mesh_copies={}
def stored_world(ob):
    # Appended objects are not linked to a view layer: matrix_world may still
    # be identity until evaluation. Read their saved transforms explicitly.
    return stored_world(ob.parent)@ob.matrix_parent_inverse@ob.matrix_basis if ob.parent else ob.matrix_basis.copy()
def copy_part(source,matrix):
    ob=source.copy();ob.name='RENDER_'+source.name
    if source.type=='MESH':
        if source.data not in mesh_copies:
            mesh=source.data.copy()
            for face in mesh.polygons:face.use_smooth=True
            mesh.set_sharp_from_angle(angle=math.radians(40));mesh_copies[source.data]=mesh
        ob.data=mesh_copies[source.data]
    ob.hide_render=False;ob.hide_viewport=False
    models.objects.link(ob);ob.matrix_world=matrix
    return ob

# Full solids, not the reduced Web meshes; original collections remain intact.
arm=bpy.data.collections.get('AFR2_Brazo_v17_Piezas_Completas')
if arm is None:
    armfile=ROOT/'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/web_blanco_completo/AFR2_Brazo_v17_Completo.blend'
    with bpy.data.libraries.load(str(armfile),link=False) as (src,dst):
        dst.collections=['AFR2_Brazo_v17_Piezas_Completas']
    arm=dst.collections[0]
for source in arm.objects:
    if source.type=='MESH':copy_part(source,stored_world(source))
carfile=ROOT/'AutoFinalRobo2-CAD/mecanica/auto-completo/AutoFinalRobo2-Final-20261025.blend'
with bpy.data.libraries.load(str(carfile),link=False) as (src,dst):
    dst.collections=['AFR2_Final_Detallado_20261025']
car=dst.collections[0]
# Blender car is floor-normalized. Fusion arm datum is the upper deck surface.
car_to_arm=Matrix.Translation((0,-.032,-.07905134505))
for source in car.objects:
    if source.type=='MESH':copy_part(source,car_to_arm@stored_world(source))

before=set(bpy.data.objects)
bpy.ops.import_scene.gltf(filepath=str(ROOT/'AutoFinalRobo2-Web/public/models/AFR2_Cubo_50mm.glb'))
cube_pose=Matrix(((-.062428726749,-.158297754638,.985415889335,.274861346061),
                  (.8029571651,-.594365014546,-.044609645798,.111366425578),
                  (.592758336164,.788461825557,.164211767388,.266428208779),
                  (0,0,0,1)))
for ob in set(bpy.data.objects)-before:
    if ob.type=='MESH':
        for col in list(ob.users_collection):col.objects.unlink(ob)
        models.objects.link(ob);ob.matrix_world=cube_pose;ob.name='RENDER_Cubo_50mm'

camera=bpy.data.objects.new('AFR2_Camara_Frontal_Fusion',bpy.data.cameras.new('AFR2_Camara_Frontal_Fusion'))
scene.collection.objects.link(camera);scene.camera=camera
eye=Vector((-.016720312595,1.255434756223,.433869059427))
target=Vector((.088497456802,.011009528591,.119956392312))
up=Vector((-.010352701007,-.245402871222,.969365902216))
forward=(target-eye).normalized();right=forward.cross(up).normalized();up=right.cross(forward).normalized()
camera.matrix_world=Matrix((right,up,-forward)).transposed().to_4x4();camera.location=eye
camera.data.type='ORTHO';camera.data.clip_start=.001;camera.data.clip_end=100
# Frame the complete model with the same orientation and optical centre.
corners=[camera.matrix_world.inverted()@(ob.matrix_world@Vector(p)) for ob in models.objects if ob.type=='MESH' for p in ob.bound_box]
aspect=4/3
camera.data.ortho_scale=max(max(abs(p.x) for p in corners)*2,max(abs(p.y) for p in corners)*2*aspect)*1.16

floor_z=min((ob.matrix_world@Vector(p)).z for ob in models.objects if ob.type=='MESH' for p in ob.bound_box)-.0004
bpy.ops.mesh.primitive_plane_add(size=200,location=(0,0,floor_z));floor=bpy.context.object;floor.name='AFR2_Suelo_Estudio'
mat=bpy.data.materials.new('AFR2_GrisAzulado_Estudio');mat.use_nodes=True
bs=mat.node_tree.nodes.get('Principled BSDF');bs.inputs['Base Color'].default_value=(.16,.20,.26,1);bs.inputs['Roughness'].default_value=.82
floor.data.materials.append(mat)
world=bpy.data.worlds.new('AFR2_Ambiente_Estudio');world.use_nodes=True
world.node_tree.nodes['Background'].inputs['Color'].default_value=(.27,.33,.42,1)
world.node_tree.nodes['Background'].inputs['Strength'].default_value=.2;scene.world=world
for name,position,power,size,color in [
    ('Principal',(-.45,.7,1.05),18,.85,(1,.96,.9)),
    ('Relleno',(.7,.35,.55),7,.7,(.83,.91,1)),
    ('Recorte',(.2,-.65,.85),24,.65,(1,1,1))]:
    data=bpy.data.lights.new('AFR2_'+name,'AREA');data.energy=power;data.shape='DISK';data.size=size;data.color=color
    light=bpy.data.objects.new(data.name,data);scene.collection.objects.link(light);light.location=position
    light.rotation_euler=(Vector((.08,0,.12))-light.location).to_track_quat('-Z','Y').to_euler()
scene.render.engine='CYCLES';scene.cycles.device='CPU';scene.cycles.samples=128
scene.cycles.use_adaptive_sampling=True;scene.cycles.adaptive_threshold=.02;scene.cycles.adaptive_min_samples=16
scene.cycles.use_denoising=True;scene.cycles.denoiser='OPENIMAGEDENOISE'
scene.cycles.max_bounces=8;scene.cycles.diffuse_bounces=3;scene.cycles.glossy_bounces=4
scene.cycles.caustics_reflective=False;scene.cycles.caustics_refractive=False
scene.render.resolution_x=4096;scene.render.resolution_y=3072;scene.render.resolution_percentage=100
scene.render.image_settings.file_format='PNG';scene.render.image_settings.color_mode='RGB';scene.render.image_settings.color_depth='8'
scene.render.film_transparent=False;scene.render.filepath=str(OUT/'AFR2_Frontal_4K_20261031.png')
scene.view_settings.view_transform='AgX';scene.view_settings.look='AgX - Medium High Contrast';scene.view_settings.exposure=-.5
scene.render.image_settings.compression=35
file=OUT/'AFR2_Frontal_4K_20261031.blend'
bpy.data.libraries.write(str(file),{scene},compress=True,fake_user=True)
report=dict(scene=scene.name,blend=str(file),image=scene.render.filepath,blender=bpy.app.binary_path,
    resolution=[4096,3072],samples=128,angle='Fusion actual, ortografica, frontal',model_meshes=len(models.objects),
    native_cad_modified=False,source_materials_preserved=True)
(OUT/'render.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
print(json.dumps(report))
