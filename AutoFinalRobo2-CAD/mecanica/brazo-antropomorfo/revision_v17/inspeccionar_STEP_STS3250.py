"""Medir CAD comercial en documento temporal; no editar el maestro del brazo."""
import adsk.core,adsk.fusion,json
from pathlib import Path

def run(context):
    app=adsk.core.Application.get();original=app.activeDocument
    if original.name!='AFR2_Brazo_v17_Base_V7_J3_Remoto':raise RuntimeError('Activar v17 antes de inspeccionar referencia')
    root=Path(r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD')
    h=root/'mecanica/brazo-antropomorfo/revision_v17'
    source=root/'.downloads/actuadores/STS3250_comercial_20211119.stp'
    doc=None
    try:
      doc=app.importManager.importToNewDocument(app.importManager.createSTEPImportOptions(str(source)))
      if doc is None:raise RuntimeError('No se importo el STEP comercial')
      d=adsk.fusion.Design.cast(doc.products.itemByProductType('DesignProductType'));r=d.rootComponent
      records=[]
      def measure(b,path):
        if not b.isSolid:return
        bb=b.boundingBox
        row=dict(path=path,body=b.name,bbox_mm=[10*v for v in list(bb.minPoint.asArray())+list(bb.maxPoint.asArray())],volume_cm3=b.volume,cylinders=[],large_planes=[])
        for face in b.faces:
          cy=adsk.core.Cylinder.cast(face.geometry)
          if cy and cy.radius>=.2:
            fb=face.boundingBox
            row['cylinders'].append(dict(radius_mm=cy.radius*10,axis=list(cy.axis.asArray()),origin_mm=[v*10 for v in cy.origin.asArray()],bbox_mm=[10*v for v in list(fb.minPoint.asArray())+list(fb.maxPoint.asArray())]))
          pl=adsk.core.Plane.cast(face.geometry)
          if pl and face.area>1:
            row['large_planes'].append(dict(area_cm2=face.area,origin_mm=[v*10 for v in pl.origin.asArray()],normal=list(pl.normal.asArray())))
        records.append(row)
      for b in r.bRepBodies:measure(b,r.name)
      for o in r.allOccurrences:
        for b in o.bRepBodies:measure(b,o.fullPathName)
      out=dict(source=source.name,scope='Referencia comercial2021, comparar con ficha2024 antes de adoptar interfaces. No motor fisico medido.',bodies=records)
      (h/'medidas_STEP_STS3250.json').write_text(json.dumps(out,indent=2),encoding='utf-8')
      print(json.dumps(dict(bodies=len(records),bounding_boxes=[dict(path=v['path'],body=v['body'],bbox_mm=v['bbox_mm']) for v in records])))
    finally:
      if doc:doc.close(False)
      original.activate()
