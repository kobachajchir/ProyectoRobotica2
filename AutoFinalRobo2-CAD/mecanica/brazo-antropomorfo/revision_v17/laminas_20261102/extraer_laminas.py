"""Captura documental del brazo abierto. No mueve piezas del maestro."""
import adsk.core,adsk.fusion,json,datetime,math
from pathlib import Path
H=Path('D:/ProyectoRobotica2/AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/laminas_20261102')
def run(context):
 app=adsk.core.Application.get();d=adsk.fusion.Design.cast(app.activeProduct);r=d.rootComponent
 expected='urn:adsk.wipprod:dm.lineage:EcPmilmLR5e65I7_wwDoSg'
 if not app.activeDocument.dataFile or app.activeDocument.dataFile.id!=expected:raise RuntimeError('No es el maestro de brazo esperado')
 H.mkdir(parents=True,exist_ok=True);(H/'cache').mkdir(exist_ok=True)
 defs={};instances=[];components={};rootnames={o.component.name:list(o.transform2.asArray()) for o in r.occurrences}
 for o in r.allOccurrences:
  root=o.fullPathName.split('+')[0].split(':')[0]
  if root.startswith(('80_','81_','82_','90_')):continue
  solids=[b for b in o.component.bRepBodies if b.isSolid]
  if not solids:continue
  cn=o.component.name;tag=o.component.attributes.itemByName('AFR2','material_v17');mat=tag.value if tag else ''
  if cn not in components:
   components[cn]={'name':cn,'roots':[],'occurrences':0,'material':mat,'volume_cm3':sum(b.volume for b in solids),'bodies':len(solids),'printed':any(b.appearance and b.appearance.name=='AFR2_PLA_PrintALot_Blanco' for b in solids)}
  components[cn]['occurrences']+=1
  if root not in components[cn]['roots']:components[cn]['roots'].append(root)
  for b in solids:
   key=b.entityToken
   if key not in defs:
    calc=b.meshManager.createMeshCalculator();calc.surfaceTolerance=.012;calc.maxNormalDeviation=.22;mesh=calc.calculate()
    edges=[]
    if components[cn]['printed'] or root=='05_Inclinacion_muneca':
     for edge in b.edges:
      ev=edge.evaluator;ok,s,e=ev.getParameterExtents()
      if not ok:continue
      ok,points=ev.getStrokes(s,e,.003)
      if ok and len(points)>1:edges.append([[10*p.x,10*p.y,10*p.z] for p in points])
    cylinders=[]
    for face in b.faces:
     g=face.geometry
     if g.surfaceType==adsk.core.SurfaceTypes.CylinderSurfaceType:
      cy=adsk.core.Cylinder.cast(g)
      cylinders.append({'diameter_mm':20*cy.radius,'axis':list(cy.axis.asArray()),'origin_mm':[10*v for v in cy.origin.asArray()]})
    defs[key]={'id':'D%04d'%len(defs),'component':cn,'body':b.name,'appearance':b.appearance.name if b.appearance else '',
     'vertices_mm':[10*v for v in mesh.nodeCoordinatesAsDouble],'triangles':list(mesh.nodeIndices),'edges_mm':edges,'cylinders':cylinders,'volume_cm3':b.volume}
   instances.append({'definition':defs[key]['id'],'component':cn,'root':root,'path':o.fullPathName,'matrix_cm':list(o.transform2.asArray())})
 data={'document':app.activeDocument.name,'id':app.activeDocument.dataFile.id,'captured_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'units':'mm','mesh_tolerance_mm':.12,'edge_tolerance_mm':.03,'roots':rootnames,'definitions':list(defs.values()),'instances':instances,'components':list(components.values()),'excluded_roots':['80_','81_','82_','90_']}
 (H/'cache/geometria.json').write_text(json.dumps(data),encoding='utf-8')
 inventory={k:v for k,v in data.items() if k not in ['definitions','instances']};inventory['solid_instances']=len(instances);inventory['unique_solids']=len(defs)
 (H/'inventario.json').write_text(json.dumps(inventory,indent=2),encoding='utf-8')
 print(json.dumps({'document':data['document'],'solids':len(instances),'unique':len(defs),'components':len(components),'out':str(H)}))
