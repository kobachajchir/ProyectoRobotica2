"""Planos del conjunto y despiece lineal en tramos legibles."""
from pathlib import Path
H=Path(__file__).resolve().parent
source=H/'renderizar.py'
# Cargar utilidades de escena sin volver a ejecutar los seis renders de pieza.
exec(compile(source.read_text(encoding='utf-8').split('\nparts=[')[0],str(source),'exec'))
data=json.loads((H/'cache/vistas.json').read_text(encoding='utf-8'))
ins=[i for i in g['instances'] if i['root']=='05_Inclinacion_muneca']
verts=[];tri=[];edges=[]
for inst in ins:
 d=defs[inst['definition']];M=mtx(g['roots'][inst['root']]).inverted()@mtx(inst['matrix_cm']);M.translation*=1000
 vs=[M@Vector(x) for x in zip(*[iter(d['vertices_mm'])]*3)];offset=len(verts);verts.extend(vs)
 tri.extend([[a+offset,b+offset,c+offset] for a,b,c in zip(*[iter(d['triangles'])]*3)])
 edges.extend([[M@Vector(p) for p in e] for e in d['edges_mm']])
bv=BVHTree.FromPolygons(verts,tri,all_triangles=True);far=500;views=[]
for title,u,v,eye in [('ALZADO',(1,0,0),(0,0,1),(0,-1,0)),('PERFIL IZQUIERDO',(0,-1,0),(0,0,1),(-1,0,0)),('PLANTA',(1,0,0),(0,1,0),(0,0,1))]:
 u=Vector(u);v=Vector(v);eye=Vector(eye);lines=[]
 for edge in edges:
  for a,b in zip(edge,edge[1:]):
   p=(a+b)/2;hit=bv.ray_cast(p+eye*far,-eye,far+1);hidden=bool(hit[0] is not None and hit[3]<far-.26)
   lines.append({'a':[a.dot(u),a.dot(v)],'b':[b.dot(u),b.dot(v)],'hidden':hidden})
 points=[[p.dot(u),p.dot(v)] for p in verts]
 views.append({'title':title,'lines':lines,'bounds':[min(p[0] for p in points),min(p[1] for p in points),max(p[0] for p in points),max(p[1] for p in points)]})
data['assembly_views']=views
tramos=[]
for start in range(0,len(data['assembly']),8):
 clear();objects=[];labels={};x=0;items=data['assembly'][start:start+8]
 for item in items:
  oo=[make(i,True) for i in ins if i['path']==item['path']]
  bpy.context.view_layer.update();pts=[o.matrix_world@Vector(v) for o in oo for v in o.bound_box];ctr=sum(pts,Vector())/len(pts)
  # Mantener orientacion de cada pieza; solamente traslacion sobre una lineaX.
  width=max(p.x for p in pts)-min(p.x for p in pts)
  for o in oo:o.location+=Vector((x-ctr.x,-ctr.y,-ctr.z))
  objects.extend(oo);labels[str(item['balloon'])]=oo[0];x+=max(width,.012)+.025
 name='AFR2-A500-E'+str(len(tramos)+1)+'.png';coords=camera(objects,name,direction=(0,-2,1),labels=labels)
 tramos.append({'render':name,'items':items,'labels':coords})
data['exploded_sections']=tramos
(H/'vistas.json').write_text(json.dumps(data),encoding='utf-8')
print('TRAMOS_LISTOS',len(tramos),flush=True)
