"""Exporta las definiciones visibles y sus instancias, sin cambiar las poses."""
import adsk.core, adsk.fusion, json, os

def run(_context: str):
    d=adsk.fusion.Design.cast(adsk.core.Application.get().activeProduct)
    r=d.rootComponent
    folder='D:/ProyectoRobotica2/AutoFinalRobo2-CAD/exports/2026-10-24/parts'
    os.makedirs(folder,exist_ok=True)
    items=[];done=set()
    for o in r.occurrences:
        if not o.isLightBulbOn:continue
        items.append({'name':o.name,'component':o.component.name,
                      'file':o.component.name+'.usdz','matrix_cm':o.transform2.asArray()})
    for b in r.bRepBodies:
        if not b.isVisible:continue
        items.append({'name':b.name,'component':b.name,'file':b.name+'.obj',
                      'matrix_cm':[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]})
        path=folder+'/'+b.name+'.obj'
        if not os.path.exists(path):
            opt=d.exportManager.createOBJExportOptions(b,path)
            opt.isOneFilePerBody=False
            d.exportManager.execute(opt)
    with open(folder+'/manifest.json','w',encoding='utf-8') as f:
        json.dump({'source':'AutoFinalRobo2 v16','obj_units':'cm','instances':items},f,indent=2)
    for item in items:
        if not item['file'].endswith('usdz') or item['component'] in done:continue
        done.add(item['component']);path=folder+'/'+item['file']
        if os.path.exists(path):continue
        c=next(o.component for o in r.occurrences if o.component.name==item['component'])
        print(item['component'],d.exportManager.execute(d.exportManager.createUSDExportOptions(path,c)))
