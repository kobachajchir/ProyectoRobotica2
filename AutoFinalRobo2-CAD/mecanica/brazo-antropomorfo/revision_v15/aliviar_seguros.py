"""Alivios locales en cara de mordaza para envolvente DIN6799; no cambia ejes."""
from pathlib import Path
_workspace=Path(globals().get('AFR2_REPO_ROOT',r'D:\ProyectoRobotica2'))
_source=_workspace/'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v15/modelar_v15.py'
exec(compile(_source.read_text(encoding='utf-8'),str(_source),'exec'))
def run(context):
    setup()
    if adsk.core.Application.get().activeDocument.name!=MODEL_NAME:raise RuntimeError('Documento incorrecto')
    jaw=comp('S_Mordaza_movil');c=next(o.component for o in jaw.occurrences if o.component.name=='S_Mordaza_Al_plegada')
    for yy in ['25.3 mm','-26.4 mm']:cut_y(c,'Alivio_seguro_E2p3','0 mm','10 mm','6.8 mm',yy,'1.1 mm')
    print('Alivios diametro6.8 en ambos seguros del perno superior, sin invadir las orejas.')
