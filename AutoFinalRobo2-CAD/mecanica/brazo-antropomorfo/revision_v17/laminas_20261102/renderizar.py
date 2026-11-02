"""Blender background: vistas derivadas, sin tocar Fusion ni el .blend abierto."""
import bpy,json,math,sys
from pathlib import Path
from mathutils import Vector,Matrix
from mathutils.bvhtree import BVHTree
H=Path(__file__).resolve().parent
g=json.loads((H/'cache/geometria.json').read_text(encoding='utf-8'));defs={x['id']:x for x in g['definitions']}
(H/'renders').mkdir(exist_ok=True)
bpy.ops.wm.read_factory_settings(use_empty=True)
s=bpy.context.scene;s.render.engine='CYCLES';s.cycles.samples=16;s.cycles.use_denoising=True
s.render.resolution_x=1350;s.render.resolution_y=1150;s.render.resolution_percentage=100
s.render.image_settings.file_format='PNG';s.render.film_transparent=True
s.world=bpy.data.worlds.new('Estudio blanco');s.world.use_nodes=True;s.world.node_tree.nodes['Background'].inputs[0].default_value=(.8,.8,.8,1);s.world.node_tree.nodes['Background'].inputs[1].default_value=.5
s.view_settings.view_transform='AgX'
def mat(name):
 m=bpy.data.materials.new(name);m.diffuse_color=(.82,.84,.86,1)
 if 'Blanco' in name:m.diffuse_color=(.88,.9,.92,1)
 elif 'Negro' in name:m.diffuse_color=(.035,.045,.055,1)
 elif 'Laton' in name:m.diffuse_color=(.55,.34,.08,1)
 elif 'PVC' in name:m.diffuse_color=(.74,.76,.72,1)
 elif 'Goma' in name:m.diffuse_color=(.06,.06,.06,1)
 m.use_nodes=True;p=m.node_tree.nodes.get('Principled BSDF');p.inputs['Base Color'].default_value=m.diffuse_color;p.inputs['Roughness'].default_value=.3 if 'Acero' in name else .45;p.inputs['Metallic'].default_value=.65 if 'Acero' in name or 'Laton' in name else 0
 return m
mats={x['appearance']:mat(x['appearance']) for x in defs.values()}
meshes={}
for k,d in defs.items():
 me=bpy.data.meshes.new(k);v=list(zip(*[iter(d['vertices_mm'])]*3));faces=list(zip(*[iter(d['triangles'])]*3));me.from_pydata([[a*.001 for a in p] for p in v],[],faces);me.materials.append(mats[d['appearance']]);me.update();meshes[k]=me
def mtx(flat):
 M=Matrix([flat[i:i+4] for i in range(0,16,4)]);M.translation*=.01;return M
def make(inst,relative=False):
 o=bpy.data.objects.new(inst['component'],meshes[inst['definition']]);s.collection.objects.link(o)
 M=mtx(inst['matrix_cm'])
 if relative:M=mtx(g['roots'][inst['root']]).inverted()@M
 o.matrix_world=M;return o
def clear():
 for o in list(s.objects):bpy.data.objects.remove(o,do_unlink=True)
def camera(objects,path,direction=(1,-1,.8),labels=None):
 bpy.context.view_layer.update();points=[o.matrix_world@Vector(v) for o in objects for v in o.bound_box]
 lo=Vector(tuple(min(p[i] for p in points) for i in range(3)));hi=Vector(tuple(max(p[i] for p in points) for i in range(3)));center=(lo+hi)/2;diag=max((hi-lo).length,.02)
 cd=bpy.data.cameras.new('Camara');cam=bpy.data.objects.new('Camara',cd);s.collection.objects.link(cam);s.camera=cam;cam.location=center+Vector(direction).normalized()*diag*3;cam.rotation_euler=(center-cam.location).to_track_quat('-Z','Y').to_euler();cd.type='ORTHO'
 rot=cam.rotation_euler.to_matrix().transposed();proj=[rot@(p-center) for p in points];width=max(p.x for p in proj)-min(p.x for p in proj);height=max(p.y for p in proj)-min(p.y for p in proj);cd.ortho_scale=max(width,height*s.render.resolution_x/s.render.resolution_y)*1.18
 for name,delta,power,size in [('Principal',(-1,-2,3),160,2),('Relleno',(2,0,1),90,2),('Borde',(0,2,2),120,1.5)]:
  ld=bpy.data.lights.new(name,'AREA');ld.energy=power*diag*diag;ld.shape='DISK';ld.size=size*diag;o=bpy.data.objects.new(name,ld);s.collection.objects.link(o);o.location=center+Vector(delta)*diag;o.rotation_euler=(center-o.location).to_track_quat('-Z','Y').to_euler()
 s.render.filepath=str(H/'renders'/path);bpy.ops.render.render(write_still=True)
 inv=cam.matrix_world.inverted();coords={}
 if labels:
  aspect=s.render.resolution_x/s.render.resolution_y;viewwidth=cd.ortho_scale;viewheight=viewwidth/aspect
  for key,ob in labels.items():
   center_ob=sum((ob.matrix_world@Vector(v) for v in ob.bound_box),Vector())/8
   p=inv@center_ob;coords[key]=[.5+p.x/viewwidth,.5+p.y/viewheight]
 return coords

parts=[
 ('AFR2-P501','V02_A18_Nucleo_J5_y_columna_J6','Núcleo J5 y soporte J6'),
 ('AFR2-P502','V03_Tapa_625_J6','Tapa de retención 625 de J6'),
 ('AFR2-P503','V17_Tapa_STS3032_J6','Tapa del servo STS3032 J6'),
 ('AFR2-P504','V02_E04_Eje_y_brida_J5','Eje y brida de J5'),
 ('AFR2-P505','V03_Separador_axial_J5','Separador axial de J5'),
 ('AFR2-P506','V03_Separador_trasero_J5','Separador trasero de J5')]
dataset={'parts':[],'assembly':[],'source_document':g['document']}
# Geometria vectorial de pieza desde aristas BRep; visibilidad aproximada
# consultada contra la malla nativa de0,12mm, no contorno inventado.
views=[('ALZADO',(1,0,0),(0,0,1),(0,-1,0)),('PERFIL IZQUIERDO',(0,-1,0),(0,0,1),(-1,0,0)),('PLANTA',(1,0,0),(0,1,0),(0,0,1))]
for code,cn,title in parts:
 clear();ds=[d for d in defs.values() if d['component']==cn];objects=[];edges=[];verts=[];tri=[];cyl=[]
 for d in ds:
  o=bpy.data.objects.new(cn,meshes[d['id']]);s.collection.objects.link(o);objects.append(o);off=len(verts);verts.extend(list(zip(*[iter(d['vertices_mm'])]*3)));tri.extend([[a+off,b+off,c+off] for a,b,c in zip(*[iter(d['triangles'])]*3)]);edges.extend(d['edges_mm']);cyl.extend(d['cylinders'])
 camera(objects,code+'.png')
 bv=BVHTree.FromPolygons(verts,tri,all_triangles=True);vv=[Vector(x) for x in verts];lo=Vector(tuple(min(p[i] for p in vv) for i in range(3)));hi=Vector(tuple(max(p[i] for p in vv) for i in range(3)));far=(hi-lo).length*4
 lineviews=[]
 for titleview,u,v,eye in views:
  u=Vector(u);v=Vector(v);eye=Vector(eye);lines=[]
  for edge in edges:
   for a,b in zip(edge,edge[1:]):
    a=Vector(a);b=Vector(b);p=(a+b)/2;hit=bv.ray_cast(p+eye*far,-eye,far+1)
    hidden=bool(hit[0] is not None and hit[3]<far-.26)
    lines.append({'a':[a.dot(u),a.dot(v)],'b':[b.dot(u),b.dot(v)],'hidden':hidden})
  points=[[p.dot(u),p.dot(v)] for p in vv]
  lineviews.append({'title':titleview,'lines':lines,'bounds':[min(p[0] for p in points),min(p[1] for p in points),max(p[0] for p in points),max(p[1] for p in points)]})
 dataset['parts'].append({'code':code,'component':cn,'title':title,'bounds_mm':list(lo)+list(hi),'volume_cm3':sum(d['volume_cm3'] for d in ds),'views':lineviews,'cylinders':cyl,'render':code+'.png'})
 print('PARTE_LISTA',code,flush=True)

# Ensamble J5/J6, componentes comerciales conservados como unidad.
clear();ins=[i for i in g['instances'] if i['root']=='05_Inclinacion_muneca'];obs=[make(i,True) for i in ins];camera(obs,'AFR2-A500.png')
bycomp={}
for inst,ob in zip(ins,obs):bycomp.setdefault(inst['path'],[]).append(ob)
items=[]
for path,oo in bycomp.items():
 xyz=[o.matrix_world@Vector(v) for o in oo for v in o.bound_box];ctr=sum(xyz,Vector())/len(xyz);length=max(p.z for p in xyz)-min(p.z for p in xyz)
 items.append((ctr.z,path,oo,length,ctr))
items.sort(key=lambda x:(x[0],x[1]));z=0;labelobs={}
# Catalogo lineal: orden espacial Z; los elementos laterales permanecen
# identificados. No se afirma que sea una unica trayectoria de desmontaje.
for n,(_,path,oo,length,ctr) in enumerate(items,1):
 for o in oo:o.location+=Vector((-ctr.x,-ctr.y,z-ctr.z))
 item={'balloon':n,'path':path,'component':oo[0].name.split('.')[0],'catalog_z_m':z};dataset['assembly'].append(item);labelobs[str(n)]=oo[0]
 z+=length+.012
dataset['exploded_labels']=camera(obs,'AFR2-A500-E.png',direction=(1,-2,.5),labels=labelobs)
dataset['exploded_note']='Despiece lineal de inventario, orden espacial Z. No secuencia unica de desmontaje.'
# General del brazo, sin auto ni cubo.
clear();obs=[make(i) for i in g['instances']];camera(obs,'AFR2-A000.png',direction=(1,-2,1))
(H/'cache/vistas.json').write_text(json.dumps(dataset),encoding='utf-8')
bpy.ops.wm.save_as_mainfile(filepath=str(H/'cache/AFR2_Laminas_Derivado.blend'))
print('LAMINAS_RENDERS_TERMINADOS',flush=True)
