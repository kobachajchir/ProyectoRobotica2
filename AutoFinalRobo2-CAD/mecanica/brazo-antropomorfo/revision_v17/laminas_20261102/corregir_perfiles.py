"""Vistas en primer diedro con siluetas de superficies curvas."""
from pathlib import Path
H=Path(__file__).resolve().parent;source=H/'renderizar.py'
exec(compile(source.read_text(encoding='utf-8').split('\nparts=[')[0],str(source),'exec'))
data=json.loads((H/'vistas.json').read_text(encoding='utf-8'))
def project_views(definitions,matrices=None):
 verts=[];tri=[];edges=[]
 for i,d in enumerate(definitions):
  M=matrices[i] if matrices else Matrix.Identity(4);offset=len(verts)
  verts.extend([M@Vector(x) for x in zip(*[iter(d['vertices_mm'])]*3)])
  tri.extend([[a+offset,b+offset,c+offset] for a,b,c in zip(*[iter(d['triangles'])]*3)])
  edges.extend([[M@Vector(x) for x in e] for e in d['edges_mm']])
 bv=BVHTree.FromPolygons(verts,tri,all_triangles=True)
 normals=[(verts[b]-verts[a]).cross(verts[c]-verts[a]) for a,b,c in tri]
 adjacent={}
 for face,indices in enumerate(tri):
  for a,b in zip(indices,indices[1:]+indices[:1]):adjacent.setdefault(tuple(sorted((a,b))),[]).append(face)
 views=[]
 for title,u,v,eye in [('ALZADO',(1,0,0),(0,0,1),(0,-1,0)),('PERFIL IZQUIERDO',(0,-1,0),(0,0,1),(-1,0,0)),('PLANTA',(1,0,0),(0,1,0),(0,0,1))]:
  u=Vector(u);v=Vector(v);eye=Vector(eye);lines=[]
  def add(a,b):
   p=(a+b)/2;hit=bv.ray_cast(p+eye*500,-eye,501)
   lines.append({'a':[a.dot(u),a.dot(v)],'b':[b.dot(u),b.dot(v)],'hidden':bool(hit[0] is not None and hit[3]<499.74)})
  for e in edges:
   for a,b in zip(e,e[1:]):add(a,b)
  # Una superficie cilindrica tiene silueta aunque no tenga una aristaBRep
  # longitudinal. Extraer las generatrices aparentes de la malla nativa.
  for (a,b),faces in adjacent.items():
   signs=[normals[f].dot(eye) for f in faces]
   if len(signs)>=2 and min(signs)<-1e-10 and max(signs)>1e-10:add(verts[a],verts[b])
  views.append({'title':title,'lines':lines,'bounds':[min(x.dot(u) for x in verts),min(x.dot(v) for x in verts),max(x.dot(u) for x in verts),max(x.dot(v) for x in verts)]})
 return views
for p in data['parts']:p['views']=project_views([d for d in defs.values() if d['component']==p['component']])
instances=[i for i in g['instances'] if i['root']=='05_Inclinacion_muneca'];matrices=[]
for i in instances:
 M=mtx(g['roots'][i['root']]).inverted()@mtx(i['matrix_cm']);M.translation*=1000;matrices.append(M)
data['assembly_views']=project_views([defs[i['definition']] for i in instances],matrices)
(H/'vistas.json').write_text(json.dumps(data),encoding='utf-8')
print('PERFILES_PRIMER_DIEDRO_LISTOS',flush=True)
