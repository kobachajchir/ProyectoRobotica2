"""Fondo único con rebaje de 2 mm, sin tapas ni postes impresos.

Trabaja en el AutoFinalRobo2 abierto. Conserva revisión A oculta y guarda B local.
Los metales son copias de referencia: el STL contiene sólo el fondo plástico.
"""
import adsk.core,adsk.fusion,json,math
ROOT='D:/ProyectoRobotica2/AutoFinalRobo2-CAD'
def run(_context: str):
 app=adsk.core.Application.get();doc=next(x for x in app.documents if x.name=='AutoFinalRobo2');doc.activate()
 d=adsk.fusion.Design.cast(app.activeProduct);root=d.rootComponent
 cfg=json.load(open(ROOT+'/mecanica/pieza-nueva/ui-panel-B-parameters.json',encoding='utf-8'))
 if any(o.component.name==cfg['assembly_name'] for o in root.occurrences):raise RuntimeError('La revision B ya existe; no se duplica.')
 old=next(o for o in root.occurrences if o.component.name=='AFR2_UI_A_OLED_Encoder')
 temp=adsk.fusion.TemporaryBRepManager.get()
 def p(x,y,z):return adsk.core.Point3D.create(x/10,y/10,z/10)
 def box(x1,y1,z1,x2,y2,z2):
  return temp.createBox(adsk.core.OrientedBoundingBox3D.create(p((x1+x2)/2,(y1+y2)/2,(z1+z2)/2),adsk.core.Vector3D.create(1,0,0),adsk.core.Vector3D.create(0,1,0),(x2-x1)/10,(y2-y1)/10,(z2-z1)/10))
 def cyl(x,y,z1,z2,r):return temp.createCylinderOrCone(p(x,y,z1),r/10,p(x,y,z2),r/10)
 def boolean(a,b,kind):
  if not temp.booleanOperation(a,b,kind):raise RuntimeError('No se pudo construir una operacion del fondo')
  return a
 def cut(a,b):return boolean(a,b,adsk.fusion.BooleanTypes.DifferenceBooleanType)
 def join(a,b):return boolean(a,b,adsk.fusion.BooleanTypes.UnionBooleanType)
 def component(parent,name):
  c=parent.occurrences.addNewComponent(adsk.core.Matrix3D.create()).component;c.name=name;return c
 def persist(c,name,body,appearance=None):
  f=c.features.baseFeatures.add();f.name=name;f.startEdit();b=c.bRepBodies.add(body,f);f.finishEdit();b.name=name
  if appearance:b.appearance=appearance
  return b
 occ=root.occurrences.addNewComponent(adsk.core.Matrix3D.create());assembly=occ.component;assembly.name=cfg['assembly_name']
 assembly.attributes.add('AFR2','parameters',json.dumps(cfg,ensure_ascii=False))
 # Bastidor de fondo: hueco central para terminales y resistencias de B.Cu.
 body=box(-3.3,-38.3,0,23.3,3.3,3)
 cut(body,box(1.1,-30,-1,18.9,-1.1,4))
 for x,y in cfg['pcb_mounts_mm']:join(body,cyl(x,y,0,3,4.5))
 # Orejas planas del OLED: las torretas de 15 mm son comerciales, no impresas.
 oled_frame=box(-6.25,-4.5,0,26.25,26.45,3)
 cut(oled_frame,box(-3.25,-1.5,-1,23.25,23.45,4));join(body,oled_frame)
 for x,y in cfg['oled_mounts_mm']:join(body,cyl(x,y,0,3,4.5))
 cut(body,box(1.1,-30,-1,18.9,-1.1,3.05))
 # Bandas anchas y orejas planas, sin columnas.
 a,b=cfg['auto_mounts_mm'];v=adsk.core.Vector3D.create(b[0]-a[0],b[1]-a[1],0);length=v.length;v.normalize()
 normal=adsk.core.Vector3D.create(-v.y,v.x,0)
 rib=temp.createBox(adsk.core.OrientedBoundingBox3D.create(p((a[0]+b[0])/2,(a[1]+b[1])/2,1.5),v,normal,(length+1)/10,.9,.3))
 join(body,rib)
 for x,y in cfg['auto_mounts_mm']:join(body,cyl(x,y,0,3,4.5))
 # Borde de 2 mm de alto para alojar la placa de 2 mm, componentes a la vista.
 rim=box(-3.3,-38.3,3,23.3,3.3,5)
 cut(rim,box(-.3,-35.3,2.9,20.3,.3,5.1));join(body,rim)
 for x,y in cfg['pcb_mounts_mm']+cfg['auto_mounts_mm']+cfg['oled_mounts_mm']:cut(body,cyl(x,y,-1,6,1.5))
 # Alivio local bajo soldadura del terminal DT, junto a la banda de montaje.
 cut(body,cyl(13.5,-28,-1,3.05,1.2))
 cut(body,cyl(18.92,-18.3,-1,3.05,1.2))
 base=component(assembly,'Fondo_UI_B_Rebaje2mm_SinTorres')
 appearance=d.appearances.itemByName('AFR2_UI_PLA_oscuro')
 result=persist(base,'Fondo_unico_bandas_y_borde',body,appearance)
 edges=adsk.core.ObjectCollection.create()
 for e in result.edges:
  if e.geometry.objectType==adsk.core.Line3D.classType() and e.startVertex and e.endVertex:
   a=e.startVertex.geometry;b=e.endVertex.geometry
   inner=any(abs(a.x-v)<1e-6 and abs(b.x-v)<1e-6 for v in [-.03,2.03]) or any(abs(a.y-v)<1e-6 and abs(b.y-v)<1e-6 for v in [-3.53,.03])
   if inner and abs(a.z-.5)<1e-6 and abs(b.z-.5)<1e-6:edges.add(e)
 ci=base.features.chamferFeatures.createInput(edges,True);ci.setToEqualDistance(adsk.core.ValueInput.createByString('0.5 mm'))
 base.features.chamferFeatures.add(ci).name='Entrada_biselada_PCB_05mm'
 # Referencia comercial reutilizada; no forma parte del STL.
 hardware=component(assembly,'Herrajes_comerciales_NO_IMPRIMIR')
 for i,(x,y) in enumerate(cfg['auto_mounts_mm'],1):
  source=next(o.component for o in old.component.occurrences if o.component.name=='Torreta_Laton_M3x15_'+str(i))
  sx=3 if i==1 else 17;sy=-32
  m=adsk.core.Matrix3D.create();m.translation=adsk.core.Vector3D.create((x-sx)/10,(y-sy)/10,-.8)
  for sb in source.bRepBodies:
   bb=temp.copy(sb)
   boolean(bb,box(sx-4,sy-4,3,sx+4,sy+4,8),adsk.fusion.BooleanTypes.IntersectionBooleanType)
   temp.transform(bb,m)
   join(bb,cyl(x,y,-1,0,1.6));join(bb,cyl(x,y,-.2,6,1.5))
   persist(hardware,'Torreta_comercial_AUTO_5mm_'+str(i),bb,sb.appearance)
  nut=next(o.component for o in old.component.occurrences if o.component.name=='Tuerca_Chasis_M3_'+str(i))
  sx,sy=cfg['auto_mounts_mm'][i-1]
  for sb in nut.bRepBodies:
   bb=temp.copy(sb);m=adsk.core.Matrix3D.create();m.translation=adsk.core.Vector3D.create(0,0,.84);temp.transform(bb,m);persist(hardware,'Tuerca_superior_torreta5_'+str(i),bb,sb.appearance)
 for i,(x,y) in enumerate(cfg['oled_mounts_mm'],1):
  source=next(o.component for o in old.component.occurrences if o.component.name=='Torreta_Laton_M3x15_'+str(i))
  sx=3 if i==1 else 17;sy=-32
  for sb in source.bRepBodies:
   bb=temp.copy(sb);m=adsk.core.Matrix3D.create();m.translation=adsk.core.Vector3D.create((x-sx)/10,(y-sy)/10,0);temp.transform(bb,m)
   join(bb,cyl(x,y,17,18,1.6));join(bb,cyl(x,y,17.8,24,1.5))
   persist(hardware,'Torreta_comercial_OLED_15mm_'+str(i),bb,sb.appearance)
  nut=next(o.component for o in old.component.occurrences if o.component.name=='Tuerca_Chasis_M3_'+str(i))
  nsx,nsy=cfg['auto_mounts_mm'][i-1]
  for sb in nut.bRepBodies:
   bb=temp.copy(sb);m=adsk.core.Matrix3D.create();m.translation=adsk.core.Vector3D.create((x-nsx)/10,(y-nsy)/10,2.49);temp.transform(bb,m);persist(hardware,'Tuerca_OLED_'+str(i),bb,sb.appearance)
 # Tornillos M3 y tuercas de la PCB como referencias de montaje.
 for i,(x,y) in enumerate(cfg['pcb_mounts_mm'],1):
  sc=next(o.component for o in old.component.occurrences if o.component.name=='Tornillos_PCB_M3x6_'+str(i))
  sb=sc.bRepBodies.item(0)
  bb=temp.copy(sb);m=adsk.core.Matrix3D.create();m.translation=adsk.core.Vector3D.create(0,0,-1.45);temp.transform(bb,m);persist(hardware,'M3_PCB_Cabeza5_'+str(i),bb,sb.appearance)
  nc=next(o.component for o in old.component.occurrences if o.component.name=='Tuerca_Chasis_M3_'+str(i))
  sx,sy=cfg['auto_mounts_mm'][i-1]
  for sb in nc.bRepBodies:
   bb=temp.copy(sb);m=adsk.core.Matrix3D.create();m.translation=adsk.core.Vector3D.create((x-sx)/10,(y-sy)/10,.3);temp.transform(bb,m);persist(hardware,'Tuerca_bajo_fondo_'+str(i),bb,sb.appearance)
 # Base a 5 mm sobre la placa superior del auto; Y del vehículo es vertical.
 m=adsk.core.Matrix3D.create();x,y,z=cfg['origin_in_auto_mm'];m.setWithArray([1,0,0,x/10,0,0,1,y/10,0,-1,0,z/10,0,0,0,1]);occ.transform2=m
 opts=app.importManager.createSTEPImportOptions(ROOT+'/exports/oled-encoder/PCB_UI_B.step')
 imported=app.importManager.importToTarget(opts,assembly)
 if not imported:raise RuntimeError('No se pudo insertar PCB_UI_B.step')
 pcb=assembly.occurrences.item(assembly.occurrences.count-1);pcb.component.name='PCB_UI_KiCad_B_Usuario'
 proxy=pcb.createForAssemblyContext(occ);m=occ.transform2.copy();m.translation=adsk.core.Vector3D.create(4.8,3.6,8.8);proxy.transform2=m
 for previous in root.occurrences:
  if previous.component.name.startswith('AFR2_UI') and previous.component.name!=cfg['assembly_name']:previous.isLightBulbOn=False
 if d.snapshots.hasPendingSnapshot:d.snapshots.add()
 out=ROOT+'/exports/oled-encoder'
 d.exportManager.execute(d.exportManager.createSTEPExportOptions(out+'/Fondo_UI_B.step',base))
 mesh=d.exportManager.createSTLExportOptions(base,out+'/Fondo_UI_B.stl');mesh.meshRefinement=adsk.fusion.MeshRefinementSettings.MeshRefinementMedium;d.exportManager.execute(mesh)
 d.exportManager.execute(d.exportManager.createSTEPExportOptions(out+'/Panel_UI_Completo_B.step',assembly))
 print('AUTO_NATIVE',d.exportManager.execute(d.exportManager.createFusionArchiveExportOptions(ROOT+'/mecanica/auto-completo/AutoFinalRobo2-UI-B.f3d')))
 # Fuente de impresión separada: únicamente el fondo, sin herrajes ni componentes.
 sd=app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType);design=adsk.fusion.Design.cast(app.activeProduct)
 target=design.rootComponent.occurrences.addNewComponent(adsk.core.Matrix3D.create()).component;target.name=base.name
 bf=target.features.baseFeatures.add();bf.startEdit()
 for b in base.bRepBodies:target.bRepBodies.add(temp.copy(b),bf)
 bf.finishEdit();target.attributes.add('AFR2','parameters',json.dumps(cfg,ensure_ascii=False))
 print('FONDO_NATIVE',design.exportManager.execute(design.exportManager.createFusionArchiveExportOptions(ROOT+'/mecanica/pieza-nueva/Soporte_UI_B_Fondo.f3d')))
 doc.activate();cam=app.activeViewport.camera;cam.eye=p(150,140,210);cam.target=p(60,46,97);cam.upVector=adsk.core.Vector3D.create(0,1,0);cam.isFitView=False;app.activeViewport.camera=cam;app.activeViewport.refresh()
 app.activeViewport.saveAsImageFile(out+'/Panel_UI_Fusion_B.png',1600,1200)
 print('CREATED fondo unico, TH, PCB visible; OLED15mm, auto5mm, sin torres impresas')
