"""Blender: derivados de la captura BRep/malla, sin modificar Fusion."""
import bpy,json,math,sys
from pathlib import Path
from mathutils import Vector,Matrix
from mathutils.bvhtree import BVHTree
H=Path(__file__).resolve().parent;OLD=H.parent/'laminas_20261102'
G=json.loads((OLD/'cache/geometria.json').read_text(encoding='utf-8'))
N=json.loads((H/'interfaces_nativas.json').read_text(encoding='utf-8'))
C=json.loads((H/'catalogo_piezas.json').read_text(encoding='utf-8'))
defs={x['id']:x for x in G['definitions']};partcodes={p['name']:p['code'] for p in C['parts']}
bpy.ops.wm.read_factory_settings(use_empty=True);S=bpy.context.scene
S.render.engine='CYCLES';S.cycles.samples=8;S.cycles.use_denoising=True
S.render.resolution_x=1050;S.render.resolution_y=900;S.render.resolution_percentage=100
S.render.image_settings.file_format='PNG';S.render.film_transparent=True
S.world=bpy.data.worlds.new('Estudio documental');S.world.use_nodes=True
S.world.node_tree.nodes['Background'].inputs[0].default_value=(.8,.8,.8,1)
S.world.node_tree.nodes['Background'].inputs[1].default_value=.5
S.view_settings.view_transform='AgX';materials={};meshes={}
for key,d in defs.items():
 name=d['appearance']
 if name not in materials:
  m=bpy.data.materials.new(name);m.use_nodes=True
  color=(.83,.85,.88,1)
  if 'Blanco' in name:color=(.88,.90,.92,1)
  elif 'Negro' in name:color=(.035,.045,.055,1)
  elif 'Laton' in name:color=(.55,.34,.08,1)
  elif 'PVC' in name:color=(.74,.76,.72,1)
  elif 'Goma' in name:color=(.055,.055,.055,1)
  node=m.node_tree.nodes.get('Principled BSDF');node.inputs['Base Color'].default_value=color
  node.inputs['Roughness'].default_value=.4;node.inputs['Metallic'].default_value=.55 if 'Acero' in name else 0
  materials[name]=m
 me=bpy.data.meshes.new(key)
 me.from_pydata([tuple(v*.001 for v in q) for q in zip(*[iter(d['vertices_mm'])]*3)],[],list(zip(*[iter(d['triangles'])]*3)))
 me.materials.append(materials[name]);me.update();meshes[key]=me

def clear():
 for o in list(S.objects):bpy.data.objects.remove(o,do_unlink=True)

def transform(flat):
 a=Matrix([flat[i:i+4] for i in range(0,16,4)]);a.translation*=.01;return a

def obj(inst):
 o=bpy.data.objects.new(inst['component'],meshes[inst['definition']]);S.collection.objects.link(o)
 o.matrix_world=transform(inst['matrix_cm']);return o

def render(objects,filename,labels=None):
 bpy.context.view_layer.update();pts=[o.matrix_world@Vector(v) for o in objects for v in o.bound_box]
 lo=Vector(tuple(min(p[i] for p in pts) for i in range(3)));hi=Vector(tuple(max(p[i] for p in pts) for i in range(3)))
 center=(lo+hi)/2;diag=max((hi-lo).length,.015)
 cd=bpy.data.cameras.new('Camera');cam=bpy.data.objects.new('Camera',cd);S.collection.objects.link(cam);S.camera=cam
 cam.location=center+Vector((1,-1.6,1.1)).normalized()*diag*3
 cam.rotation_euler=(center-cam.location).to_track_quat('-Z','Y').to_euler();cd.type='ORTHO'
 rot=cam.rotation_euler.to_matrix().transposed();proj=[rot@(p-center) for p in pts]
 width=max(p.x for p in proj)-min(p.x for p in proj);height=max(p.y for p in proj)-min(p.y for p in proj)
 cd.ortho_scale=max(width,height*S.render.resolution_x/S.render.resolution_y)*1.14
 for name,delta,power in [('Key',(-1,-2,3),170),('Fill',(2,0,1),100),('Rim',(0,2,2),130)]:
  ld=bpy.data.lights.new(name,'AREA');ld.energy=power*diag*diag;ld.size=2*diag
  light=bpy.data.objects.new(name,ld);S.collection.objects.link(light);light.location=center+Vector(delta)*diag
  light.rotation_euler=(center-light.location).to_track_quat('-Z','Y').to_euler()
 S.render.filepath=str(H/'renders'/filename);bpy.ops.render.render(write_still=True)
 out={};inv=cam.matrix_world.inverted();w=cd.ortho_scale;h=w*S.render.resolution_y/S.render.resolution_x
 for lab,oo in (labels or {}).items():
  pp=[o.matrix_world@Vector(v) for o in oo for v in o.bound_box];pt=inv@(sum(pp,Vector())/len(pp))
  out[lab]=[.5+pt.x/w,.5+pt.y/h]
 return out

def vector_views(ds,edges,matrices=None):
 vv=[];tri=[]
 for n,d in enumerate(ds):
  M=matrices[n] if matrices else Matrix.Identity(4);off=len(vv)
  vv.extend(M@Vector(p) for p in zip(*[iter(d['vertices_mm'])]*3))
  tri.extend([a+off,b+off,c+off] for a,b,c in zip(*[iter(d['triangles'])]*3))
 bv=BVHTree.FromPolygons(vv,tri,all_triangles=True);adj={};normals=[]
 for fi,(a,b,c) in enumerate(tri):
  normals.append((vv[b]-vv[a]).cross(vv[c]-vv[a]))
  for e in [(a,b),(b,c),(c,a)]:adj.setdefault(tuple(sorted(e)),[]).append(fi)
 views=[]
 for title,u,v,eye in [('ALZADO',(1,0,0),(0,0,1),(0,-1,0)),('PERFIL IZQUIERDO',(0,-1,0),(0,0,1),(-1,0,0)),('PLANTA',(1,0,0),(0,1,0),(0,0,1))]:
  u=Vector(u);v=Vector(v);eye=Vector(eye);lines=[];far=max((max(p.z for p in vv)-min(p.z for p in vv))*4,1000)
  def add(a,b):
   p=(a+b)/2;hit=bv.ray_cast(p+eye*far,-eye,far+1)
   lines.append({'a':[a.dot(u),a.dot(v)],'b':[b.dot(u),b.dot(v)],'hidden':hit[0] is not None and hit[3]<far-.20})
  for edge in edges:
   for a,b in zip(edge,edge[1:]):add(Vector(a),Vector(b))
  for (a,b),faces in adj.items():
   sign=[normals[i].dot(eye) for i in faces]
   if len(sign)>1 and min(sign)<-1e-9 and max(sign)>1e-9:add(vv[a],vv[b])
  views.append({'title':title,'lines':lines,'bounds':[min(p.dot(u) for p in vv),min(p.dot(v) for p in vv),max(p.dot(u) for p in vv),max(p.dot(v) for p in vv)]})
 return views,vv,tri

def section(vv,tri,bodies):
 # Corte longitudinal que contiene el eje de mayor diametro interno.
 cylinders=[c for b in bodies for c in b['cylinders'] if c['internal']]
 cy=max(cylinders,key=lambda c:c['diameter_mm']) if cylinders else None
 axis=Vector(cy['axis']) if cy else Vector((0,0,1))
 normal_axis=min(range(3),key=lambda i:abs(axis[i]))
 pos=cy['origin_mm'][normal_axis] if cy else sum(p[normal_axis] for p in vv)/len(vv)
 plane=Vector(tuple(1 if i==normal_axis else 0 for i in range(3)))
 axes=[i for i in range(3) if i!=normal_axis];u,v=axes
 lines=[]
 for ids in tri:
  points=[vv[i] for i in ids];hits=[]
  for a,b in zip(points,points[1:]+points[:1]):
   da=a[normal_axis]-pos;db=b[normal_axis]-pos
   if da*db<-1e-12:hits.append(a+(b-a)*(da/(da-db)))
   elif abs(da)<1e-7:hits.append(a)
  unique=[]
  for p in hits:
   if not any((p-q).length<1e-5 for q in unique):unique.append(p)
  if len(unique)==2 and (unique[0]-unique[1]).length>1e-5:
   lines.append({'a':[unique[0][u],unique[0][v]],'b':[unique[1][u],unique[1][v]],'hidden':False})
 points=[l[k] for l in lines for k in ['a','b']]
 return {'title':'CORTE A-A | '+['X','Y','Z'][normal_axis]+' = %.2f mm'%pos,'plane_axis':normal_axis,'plane_mm':pos,'axes':axes,'lines':lines,'bounds':[min(p[0] for p in points),min(p[1] for p in points),max(p[0] for p in points),max(p[1] for p in points)] if points else [0,0,1,1],'method':'interseccion de malla nativa, tolerancia de superficie 0.12 mm'}

dataset={'source':G['document'],'source_id':G['id'],'source_capture':G['captured_utc'],'parts':[],'assemblies':[],'mesh_tolerance_mm':.12,'edge_tolerance_mm':.03}
for c in C['parts']:
 clear();ds=[d for d in defs.values() if d['component']==c['name']];objects=[]
 for d in ds:
  o=bpy.data.objects.new(c['name'],meshes[d['id']]);S.collection.objects.link(o);objects.append(o)
 render(objects,c['code']+'.png')
 bodies=N['parts'][c['name']];edges=[e for b in bodies for e in b['edges_mm']]
 views,vv,tri=vector_views(ds,edges)
 bounds=[min(b['bounds_mm'][i] for b in bodies) for i in range(3)]+[max(b['bounds_mm'][i] for b in bodies) for i in range(3,6)]
 dataset['parts'].append({**c,'bounds_mm':bounds,'views':views,'section':section(vv,tri,bodies),'render':str(H/'renders'/(c['code']+'.png'))})
 print('PIEZA',c['code'],flush=True)
 (H/'vistas_completas.json').write_text(json.dumps(dataset,ensure_ascii=False),encoding='utf-8')

groups=[('A100','Montura y control fijo',['00_Base_control']),('A200','Base J1 y transmision de hombro',['01_Giro_base','09_Pinion_hombro']),('A300','Brazo y accionamiento remoto J3',['02_Brazo_PVC','12_Polea_entrada_J3']),('A400','Antebrazo y muneca J4/J5',['03_Antebrazo_PVC','04_Giro_muneca','11_Pinion_J4']),('A500','Inclinacion J5 y soporte J6',['05_Inclinacion_muneca']),('A600','Herramienta y pinza V7',['06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','10_Pinion_pinza'])]
for code,title,roots in groups:
 ins=[i for i in G['instances'] if i['root'] in roots];clear();oo=[obj(i) for i in ins]
 render(oo,'AFR2-'+code+'.png')
 bypath={}
 for inst in ins:
  path=inst['path'];segments=path.split('+');group=path
  # Una placa comercial con hijos sigue siendo una unidad de compra.
  for n,seg in enumerate(segments[1:],1):
   if any(t in seg.lower() for t in ['nano','pca9685','xl4016']):group='+'.join(segments[:n+1]);break
  bypath.setdefault(group,[]).append(inst)
 items=[]
 for n,(path,instances) in enumerate(sorted(bypath.items()),1):
  cn=instances[0]['component'];display=path.split('+')[-1].rsplit(':',1)[0]
  items.append({'position':n,'path':path,'component':cn,'label':display,'part_code':partcodes.get(cn,'COMERCIAL/REFERENCIA'),'instances':instances,'qty':1})
 exploded=[]
 for start in range(0,len(items),8):
  clear();allobs=[];labels={};offsets=[];selection=items[start:start+8]
  # Orientaciones y coordenadas ensambladas conservadas; solo separacion documental.
  for j,it in enumerate(selection):
   objects=[obj(i) for i in it['instances']]
   pts=[o.matrix_world@Vector(v) for o in objects for v in o.bound_box];center=sum(pts,Vector())/len(pts)
   base=Vector((sum(p.x for p in pts)/len(pts),sum(p.y for p in pts)/len(pts),sum(p.z for p in pts)/len(pts)))
   radial=Vector((base.x,base.y,0));radial=radial.normalized() if radial.length>1e-5 else Vector((1,0,0))
   delta=radial*(.008+.004*(j%3))+Vector((.014*((j%3)-1),.012*((j%2)*2-1),.014*(j//3)))
   for o in objects:o.location+=delta
   allobs.extend(objects);labels[str(it['position'])]=objects
   offsets.append({'position':it['position'],'translation_mm':[v*1000 for v in delta]})
  file='AFR2-'+code+'-E%02d.png'%(len(exploded)+1)
  coords=render(allobs,file,labels)
  exploded.append({'render':str(H/'renders'/file),'items':[{k:v for k,v in x.items() if k!='instances'} for x in selection],'balloons':coords,'offsets':offsets})
 assembly={'code':'AFR2-'+code,'title':title,'roots':roots,'render':str(H/'renders'/('AFR2-'+code+'.png')),'items':[{k:v for k,v in x.items() if k!='instances'} for x in items],'exploded':exploded}
 dataset['assemblies'].append(assembly)
 (H/'vistas_completas.json').write_text(json.dumps(dataset,ensure_ascii=False),encoding='utf-8')
 print('CONJUNTO',code,len(items),flush=True)
clear();objects=[obj(i) for i in G['instances']];render(objects,'AFR2-BRAZO-COMPLETO.png')
print('DOCUMENTACION_GEOMETRICA_LISTA',flush=True)
