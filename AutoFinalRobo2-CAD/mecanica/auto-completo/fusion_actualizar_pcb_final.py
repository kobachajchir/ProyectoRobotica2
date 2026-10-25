"""Incorpora el STEP vigente sin reconstruir soporte ni cambiar sus materiales."""
import adsk.core, adsk.fusion, json, re
from pathlib import Path
BASE=Path('D:/ProyectoRobotica2/AutoFinalRobo2-CAD')

def run(_context: str):
    app=adsk.core.Application.get()
    doc=next(x for x in app.documents if x.name=='AutoFinalRobo2');doc.activate()
    d=adsk.fusion.Design.cast(app.activeProduct);root=d.rootComponent
    ui=next(o for o in root.occurrences if o.isVisible and o.component.name.startswith('AFR2_UI'))
    old=next(o for o in ui.component.occurrences if o.isVisible and o.component.name.startswith('PCB_UI'))
    old_world=next(o for o in root.allOccurrences if o.fullPathName==ui.name+'+'+old.name).transform2.copy()
    archive=BASE/'mecanica/auto-completo/AutoFinalRobo2-Usuario-20261025.f3d'
    if not archive.exists():
        assert d.exportManager.execute(d.exportManager.createFusionArchiveExportOptions(str(archive)))
    def key(name):return re.sub(r' \(\d+\)', '',name)
    appearances={}
    def collect(c):
        for i,b in enumerate(c.bRepBodies):appearances[(key(c.name),i)]=b.appearance
        for o in c.occurrences:collect(o.component)
    collect(old.component)
    before=ui.component.occurrences.count
    opts=app.importManager.createSTEPImportOptions(str(BASE/'exports/oled-encoder/PCB_UI_Final_20261025.step'))
    assert app.importManager.importToTarget(opts,ui.component)
    assert ui.component.occurrences.count==before+1
    new=ui.component.occurrences.item(before)
    new.component.name='PCB_UI_Final_20261025'
    next(o for o in root.allOccurrences if o.fullPathName==ui.name+'+'+new.name).transform2=old_world
    copied=0
    def transfer(c):
        nonlocal copied
        for i,b in enumerate(c.bRepBodies):
            appearance=appearances.get((key(c.name),i))
            if appearance:b.appearance=appearance;copied+=1
        for o in c.occurrences:transfer(o.component)
    transfer(new.component)
    old.isLightBulbOn=False
    new.component.attributes.add('AFR2','source','KiCad guardado por el usuario; PCB_UI_Final_20261025.step')
    if d.snapshots.hasPendingSnapshot:d.snapshots.add()
    out={'assembly':ui.name,'old_hidden':old.name,'new':new.name,'appearance_bodies_copied':copied,'new_world_matrix':new.createForAssemblyContext(ui).transform2.asArray()}
    def boards(c):
        for o in c.occurrences:
            if '_PCB' in o.component.name:
                out['board_local_bounds_cm']=[[b.boundingBox.minPoint.asArray(),b.boundingBox.maxPoint.asArray()] for b in o.component.bRepBodies]
            boards(o.component)
    boards(new.component)
    print(json.dumps(out))
