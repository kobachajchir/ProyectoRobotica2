"""Volumen real de impresos activos, sin modificar geometria ni materiales."""
import adsk.core,adsk.fusion,json,datetime,hashlib
from pathlib import Path
HERE=Path('D:/ProyectoRobotica2/AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/web_blanco')
RHO=1.24

def measure(design):
 parts=[]
 for o in design.rootComponent.allOccurrences:
  root=o.fullPathName.split('+')[0].split(':')[0]
  if root.startswith(('80_','81_','82_','90_')):continue
  attr=o.component.attributes.itemByName('AFR2','material_v17');tag=attr.value if attr else ''
  for b in o.component.bRepBodies:
   if not b.isSolid:continue
   ap=b.appearance.name if b.appearance else ''
   # Apariencia no convierte metal, PVC, motores o electronica a PLA.
   if not (tag.startswith('PLA') or ap in ['AFR2_PETG_Rojo','AFR2_PLA_PrintALot_Rojo','AFR2_PLA_PrintALot_Blanco']):continue
   volume=b.volume
   parts.append(dict(root=root,path=o.fullPathName,component=o.component.name,body=b.name,volume_cm3=volume,mass_solid_g=volume*RHO,material_tag=tag,appearance=ap))
 return dict(parts=parts,total_volume_cm3=sum(p['volume_cm3'] for p in parts),total_solid_g=sum(p['mass_solid_g'] for p in parts),instances=len({p['path'] for p in parts}),bodies=len(parts))

def run(context):
 app=adsk.core.Application.get();doc=app.activeDocument;design=adsk.fusion.Design.cast(app.activeProduct)
 if doc.name!='AFR2_Brazo_v17_Base_V7_J3_Remoto':raise RuntimeError('Documento inesperado')
 current=measure(design)
 result=dict(document=doc.name,utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),density_g_cm3=RHO,method='Suma de volumenes BRep activos por ocurrencia x1.24g/cm3; incluye huecos modelados, no infill/soportes de laminador.',current=current)
 file=HERE/'pesos_impresion_20261031.json';file.write_text(json.dumps(result,indent=2),encoding='utf-8')
 print(json.dumps(dict(file=str(file),mass_solid_g=current['total_solid_g'],volume_cm3=current['total_volume_cm3'],instances=current['instances'],bodies=current['bodies'])))
