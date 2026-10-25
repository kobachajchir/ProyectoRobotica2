"""Exporta la revisión manual vigente con materiales, poses e instancias."""
import adsk.core, adsk.fusion, json
from pathlib import Path
BASE=Path('D:/ProyectoRobotica2/AutoFinalRobo2-CAD')
OUT=BASE/'exports/2026-10-25-final'

def run(_context: str):
    app=adsk.core.Application.get();doc=next(x for x in app.documents if x.name=='AutoFinalRobo2');doc.activate()
    d=adsk.fusion.Design.cast(app.activeProduct);root=d.rootComponent;em=d.exportManager
    folder=OUT/'parts';folder.mkdir(parents=True,exist_ok=True)
    definitions=[];instances=[];indices={}
    for o in root.occurrences:
        if not o.isVisible:continue
        token=o.component.entityToken
        if token not in indices:
            indices[token]=len(definitions)
            definitions.append((o.component,'part_%02d.usdz'%len(definitions)))
        instances.append({'name':o.name,'component':o.component.name,'file':definitions[indices[token]][1],'matrix_cm':o.transform2.asArray()})
    for b in root.bRepBodies:
        if not b.isVisible:continue
        filename='body_%02d.obj'%len(definitions);definitions.append((b,filename))
        instances.append({'name':b.name,'component':b.name,'file':filename,'matrix_cm':[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]})
    manifest={'source':'AutoFinalRobo2; revision manual final 2026-10-25','obj_units':'cm','instances':instances}
    (folder/'manifest.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8')
    batch=globals().get('BATCH')
    if batch is None:
        ui=next(o for o in root.occurrences if o.isVisible and o.component.name.startswith('AFR2_UI'))
        support=next(o.component for o in ui.component.occurrences if o.isVisible and o.component.name.startswith('Fondo_UI'))
        assert em.execute(em.createFusionArchiveExportOptions(str(BASE/'mecanica/auto-completo/AutoFinalRobo2-Final-20261025.f3d')))
        assert em.execute(em.createSTEPExportOptions(str(OUT/'Soporte_UI_Final.step'),support))
        stl=em.createSTLExportOptions(support,str(OUT/'Soporte_UI_Final.stl'))
        stl.meshRefinement=adsk.fusion.MeshRefinementSettings.MeshRefinementHigh
        assert em.execute(stl)
        # Fusion no admite archivar este subcomponente directamente: copiar sólo
        # sus sólidos al documento local de impresión, conservando apariencias.
        sd=app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
        single=adsk.fusion.Design.cast(app.activeProduct)
        bf=single.rootComponent.features.baseFeatures.add();bf.startEdit()
        tm=adsk.fusion.TemporaryBRepManager.get()
        for b in support.bRepBodies:
            copy=single.rootComponent.bRepBodies.add(tm.copy(b),bf)
            copy.name=b.name;copy.appearance=b.appearance;copy.material=b.material
        bf.finishEdit()
        assert single.exportManager.execute(single.exportManager.createFusionArchiveExportOptions(str(BASE/'mecanica/pieza-nueva/Soporte_UI_Final_20261025.f3d')))
        doc.activate()
        print(json.dumps({'native_saved':True,'instances':len(instances),'definitions':len(definitions)}))
    else:
        for index in range(batch[0],min(batch[1],len(definitions))):
            geometry,filename=definitions[index];path=folder/filename
            if not path.exists():
                if filename.endswith('usdz'):options=em.createUSDExportOptions(str(path),geometry)
                else:
                    options=em.createOBJExportOptions(geometry,str(path));options.isOneFilePerBody=False
                assert em.execute(options),filename
            print(index,filename,path.stat().st_size)
        if batch[1]>=len(definitions):
            # Exportar el panel desde la raíz conserva las sobreescrituras de
            # posición de instancias que no existen en la definición local.
            ui=next(o for o in root.occurrences if o.isVisible and o.component.name.startswith('AFR2_UI'))
            saved=[(o,o.isLightBulbOn) for o in root.occurrences]+[(b,b.isLightBulbOn) for b in root.bRepBodies]
            try:
                for entity,on in saved:
                    if entity!=ui:entity.isLightBulbOn=False
                panel=folder/'panel_root.usdz'
                assert em.execute(em.createUSDExportOptions(str(panel)))
            finally:
                for entity,on in saved:entity.isLightBulbOn=on
            for item in manifest['instances']:
                if item['name']==ui.name:
                    item['file']='panel_root.usdz'
                    item['matrix_cm']=[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]
                    item['export_context']='root: conserva posiciones de instancias'
            (folder/'manifest.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8')
