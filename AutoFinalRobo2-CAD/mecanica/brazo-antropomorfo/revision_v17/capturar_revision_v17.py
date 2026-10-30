"""Vista del prototipo nativo, sin sustituir geometria por ilustraciones."""
import adsk.core,json
from pathlib import Path

def run(context):
    app=adsk.core.Application.get()
    if app.activeDocument.name!='AFR2_Brazo_v17_Base_V7_J3_Remoto':raise RuntimeError('Activar v17')
    viewport=app.activeViewport;camera=viewport.camera
    # El modelo mecanico y la gravedad usanZ vertical; la preferencia de
    # vistas isometricas de la instalacion puede estar configurada conYup.
    camera.target=adsk.core.Point3D.create(8,0,12)
    camera.eye=adsk.core.Point3D.create(65,-80,65)
    camera.upVector=adsk.core.Vector3D.create(0,0,1)
    camera.isFitView=True;viewport.camera=camera;viewport.refresh()
    folder=Path(r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\revision_v17\renders')
    folder.mkdir(exist_ok=True)
    target=folder/'vista_prototipo_v17.png'
    ok=viewport.saveAsImageFile(str(target),1600,1000)
    if not ok:raise RuntimeError('No se exporto la captura nativa')
    print(json.dumps(dict(image=str(target),scope='Vista del CAD en desarrollo; restantes actuadores aun pendientes de integrar.')))
