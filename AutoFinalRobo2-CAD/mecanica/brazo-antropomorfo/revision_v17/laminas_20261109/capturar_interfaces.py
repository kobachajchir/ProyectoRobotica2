"""Lectura BRep de la copia documental 20261102. No modifica el diseno."""
import adsk.core, adsk.fusion, json, datetime, math
from pathlib import Path
H=Path(__file__).resolve().parent

def run(context):
 app=adsk.core.Application.get(); design=adsk.fusion.Design.cast(app.activeProduct)
 if not design: raise RuntimeError('No hay diseno Fusion activo')
 root=design.rootComponent
 catalog=json.loads((H/'catalogo_piezas.json').read_text(encoding='utf-8'))
 targets={p['name'] for p in catalog['parts']}
 comps={o.component.name:o.component for o in root.allOccurrences if o.component.name in targets}
 if 'V02_A18_Nucleo_J5_y_columna_J6' not in comps or 'V05_Montura_baja_PETG' not in comps:
  raise RuntimeError('Documento no corresponde a la copia documental V17')
 out={'document_open':app.activeDocument.name,'source_archive':'../laminas_20261102/AFR2_Fuente_Laminas_20261102.f3d','source_id':catalog['id'],'captured_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'units':'mm','edge_tolerance_mm':0.03,'parts':{},'parameters':[],'scope':'lectura BRep, sin nuevos calculos mecanicos ni cambios CAD'}
 for name,component in comps.items():
  bodies=[]
  for b in component.bRepBodies:
   if not b.isSolid:continue
   bb=b.boundingBox;edges=[];cylinders=[];planes=[]
   for e in b.edges:
    try:
     ev=e.evaluator;ok,s,t=ev.getParameterExtents()
     if not ok:continue
     ok,pts=ev.getStrokes(s,t,0.003)
     if ok and len(pts)>1:edges.append([[10*x.x,10*x.y,10*x.z] for x in pts])
    except:pass
   for f in b.faces:
    g=f.geometry
    if g.surfaceType==adsk.core.SurfaceTypes.CylinderSurfaceType:
     cy=adsk.core.Cylinder.cast(g);axis=list(cy.axis.asArray());origin=[10*x for x in cy.origin.asArray()]
     p=f.pointOnFace;ev=f.evaluator;ok,n=ev.getNormalAtPoint(p)
     vec=[10*p.x-origin[0],10*p.y-origin[1],10*p.z-origin[2]];along=sum(vec[i]*axis[i] for i in range(3));rad=[vec[i]-along*axis[i] for i in range(3)]
     sign=sum(rad[i]*list(n.asArray())[i] for i in range(3)) if ok else 0
     edgepts=[]
     for e in f.edges:
      try:
       ee=e.evaluator;ok,a,z=ee.getParameterExtents()
       if not ok:continue
       ok,pts=ee.getStrokes(a,z,.005)
       if ok:edgepts.extend([[10*x.x,10*x.y,10*x.z] for x in pts])
      except:pass
     values=[sum((p[i]-origin[i])*axis[i] for i in range(3)) for p in edgepts]
     cylinders.append({'diameter_mm':20*cy.radius,'axis':axis,'origin_mm':origin,'internal':sign<0,'normal_radial_dot':sign,'axial_interval_mm':[min(values),max(values)] if values else None})
    elif g.surfaceType==adsk.core.SurfaceTypes.PlaneSurfaceType:
     pl=adsk.core.Plane.cast(g)
     planes.append({'origin_mm':[10*x for x in pl.origin.asArray()],'normal':list(pl.normal.asArray()),'area_mm2':f.area*100})
   bodies.append({'body':b.name,'bounds_mm':[10*bb.minPoint.x,10*bb.minPoint.y,10*bb.minPoint.z,10*bb.maxPoint.x,10*bb.maxPoint.y,10*bb.maxPoint.z],'volume_cm3':b.volume,'edges_mm':edges,'cylinders':cylinders,'planes':planes})
  out['parts'][name]=bodies
 for p in design.userParameters:
  out['parameters'].append({'name':p.name,'expression':p.expression,'value_internal':p.value,'unit':p.unit,'comment':p.comment})
 (H/'interfaces_nativas.json').write_text(json.dumps(out,ensure_ascii=False),encoding='utf-8')
 print(json.dumps({'parts':len(out['parts']),'missing_components':sorted(targets-set(comps)),'parameters':len(out['parameters']),'output':str(H/'interfaces_nativas.json')}))
