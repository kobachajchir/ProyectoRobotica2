"""Extraccion geometrica sin editar el maestro. Ejecutar en Fusion, v06 activa."""
import adsk.core,adsk.fusion,os,json

def run(context):
    app=adsk.core.Application.get();d=adsk.fusion.Design.cast(app.activeProduct);r=d.rootComponent
    if app.activeDocument.name!='AFR2_Brazo_PVC32_AGV_v06':raise RuntimeError('Activar v06')
    folder=r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\analisis_cargas_200g';os.makedirs(folder,exist_ok=True)
    parts=[]
    for o in r.allOccurrences:
        root=o.fullPathName.split('+')[0].split(':')[0]
        if root in ['80_Auto_referencia','81_Pan_HCSR04','82_Envolvente_pan_REFERENCIA']:continue
        for b in o.component.bRepBodies:
            if not b.isSolid or b.volume<1e-9:continue
            pp=b.getPhysicalProperties(adsk.fusion.CalculationAccuracy.HighCalculationAccuracy)
            parts.append({'root':root,'path':o.fullPathName,'component':o.component.name,'body':b.name,'volume_cm3':pp.volume,'centroid_component_mm':[10*v for v in pp.centerOfMass.asArray()],'appearance':b.appearance.name if b.appearance else '','material_assigned':b.material.name if b.material else '','bbox_component_mm':[10*v for v in list(b.boundingBox.minPoint.asArray())+list(b.boundingBox.maxPoint.asArray())],'component_to_root_cm':list(o.transform2.asArray())})
    data={'document':app.activeDocument.name,'id':app.activeDocument.dataFile.id,'source_native':'AFR2_Brazo_PVC32_AGV_v06.f3d','scope':'Native volumes and uniform centroids; physical mass assigned separately. No edit.','parameters':{p.name:{'expression':p.expression,'value_internal':p.value,'unit':p.unit} for p in d.userParameters},'parts':parts,'root_transforms_cm':{o.component.name:list(o.transform2.asArray()) for o in r.occurrences}}
    with open(os.path.join(folder,'geometria_v06.json'),'w',encoding='utf-8') as f:json.dump(data,f,ensure_ascii=False,indent=2)
