"""Exporta solo la pieza90 del Fusion abierto, en su marco local centrado."""
import adsk.core,adsk.fusion,json
from pathlib import Path
HERE=Path('D:/ProyectoRobotica2/AutoFinalRobo2-CAD/mecanica/modelos-auxiliares/cubo-50mm')

def run(context):
 app=adsk.core.Application.get();design=adsk.fusion.Design.cast(app.activeProduct)
 if app.activeDocument.name!='AFR2_Brazo_v17_Base_V7_J3_Remoto':raise RuntimeError('Documento inesperado')
 found=[(o,b) for o in design.rootComponent.allOccurrences if o.fullPathName.startswith('90_Pieza_REFERENCIA') for b in o.component.bRepBodies if b.isSolid]
 if len(found)!=1:raise RuntimeError('La referencia debe contener exactamente un solido')
 occurrence,body=found[0];calc=body.meshManager.createMeshCalculator();calc.surfaceTolerance=.005;mesh=calc.calculate()
 if not mesh:raise RuntimeError('No se pudo teselar el cubo')
 ap=body.appearance;color=ap.appearanceProperties.itemById('opaque_albedo').value
 low=body.boundingBox.minPoint.asArray();high=body.boundingBox.maxPoint.asArray();center=[(a+b)/2 for a,b in zip(low,high)]
 xyz=mesh.nodeCoordinatesAsDouble
 data=dict(source_document=app.activeDocument.name,source_path=occurrence.fullPathName,source_body=body.name,units='m',size_m=[(b-a)*.01 for a,b in zip(low,high)],origin='Centro geometrico, sin pose de la herramienta del brazo',
  vertices=[[.01*(xyz[k+i]-center[i]) for i in range(3)] for k in range(0,len(xyz),3)],normals=mesh.normalVectorsAsDouble,indices=mesh.nodeIndices,
  material=dict(name=ap.name,srgb=[color.red/255,color.green/255,color.blue/255],metallic=0,roughness=.55),scope='Modelo auxiliar de objeto manipulado; nombre historico100g no fija masa de la simulacion.')
 (HERE/'cubo_fusion.json').write_text(json.dumps(data,indent=2),encoding='utf-8')
 print(json.dumps(dict(file=str(HERE/'cubo_fusion.json'),size_m=data['size_m'],triangles=mesh.triangleCount,appearance=ap.name)))
