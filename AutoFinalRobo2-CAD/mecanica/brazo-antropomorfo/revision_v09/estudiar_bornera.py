"""Envolventes temporales de bornera y cajetin, sin modificar Fusion."""
import adsk.core,adsk.fusion,json,os
OUT=r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\revision_v09'
def run(_context: str):
 d=adsk.fusion.Design.cast(adsk.core.Application.get().activeProduct);tm=adsk.fusion.TemporaryBRepManager.get();targets=[]
 for o in d.rootComponent.allOccurrences:
  if o.component.name in ['V03_Bornera_y_alimentacion_REF','V05_Montura_baja_PETG']:continue
  for b in o.component.bRepBodies:
   if b.isSolid:targets.append((o.component.name,tm.copy(b.createForAssemblyContext(o))))
 rows=[]
 for x,y in [(57,-38),(57,7),(62,-38),(57,26),(-51,23),(-80,23)]:
  a=tm.createBox(adsk.core.OrientedBoundingBox3D.create(adsk.core.Point3D.create(x/10,y/10,1.65),adsk.core.Vector3D.create(1,0,0),adsk.core.Vector3D.create(0,1,0),2.4,1.4,.9));hits=[]
  for n,b in targets:
   aa=a.boundingBox;bb=b.boundingBox
   if any(min(aa.maxPoint.asArray()[i],bb.maxPoint.asArray()[i])-max(aa.minPoint.asArray()[i],bb.minPoint.asArray()[i])<=1e-7 for i in range(3)):continue
   cp=tm.copy(a)
   if tm.booleanOperation(cp,b,adsk.fusion.BooleanTypes.IntersectionBooleanType) and cp.volume*1000>.001:hits.append(n)
  rows.append({'centre_mm':[x,y,16.5],'hits':sorted(set(hits))})
 data={'scope':'Cajetin envolvente24x14x9mm contra cuerpos restantes. Montura excluida, se estudia por separado; no prueba de todo el giro del pan.','candidates':rows,'adopted_centre_mm':[-51,23,16.5]}
 json.dump(data,open(os.path.join(OUT,'reubicacion_bornera.json'),'w',encoding='utf-8'),ensure_ascii=False,indent=2)
 print(json.dumps(data))
