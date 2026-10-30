"""Continuacion autorizada: conservar topologia V7 y correcciones de v09.

No se integra pinza v15 ni ensamble alternativo v16. Cada fase una sola vez.
"""
from pathlib import Path
import adsk.core,adsk.fusion,json,os
REPO=Path(globals().get('AFR2_REPO_ROOT',r'D:\ProyectoRobotica2'))
ARM=REPO/'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo'
NAME='AFR2_Brazo_v17_Base_V7_J3_Remoto'
def run(context):
    app=adsk.core.Application.get();phase=globals().get('FASE17','init')
    if phase=='init':
        src=ARM/'AFR2_Brazo_PVC32_AGV_v09_J1_GX3270BLS.f3d'
        doc=app.importManager.importToNewDocument(app.importManager.createFusionArchiveImportOptions(str(src)));doc.name=NAME
        d=adsk.fusion.Design.cast(app.activeProduct)
        d.rootComponent.attributes.add('AFR2','scope_v17','Continuidad V7 con correcciones v09 y pinza existente. MotorJ3 trasladado junto aJ2 sobre primer eslabon, correa hasta codo. Rediseño en curso.')
        doc.saveAs(NAME,app.data.activeProject.rootFolder,'Copia del ensamble antropomorfo existente, conservar pinza y cadenaV7; correaJ3 con motor junto aJ2 en desarrollo.','')
        if not d.exportManager.execute(d.exportManager.createFusionArchiveExportOptions(str(ARM/'revision_v17'/(NAME+'.f3d')))):raise RuntimeError('No se exporto la copia')
        print(json.dumps(dict(document=doc.name,source=src.name,components=d.allComponents.count,joints=d.rootComponent.joints.count,scope='Copia original, todavia sin cambio de motor/correa.')))
    else:raise RuntimeError('Fase no definida')
