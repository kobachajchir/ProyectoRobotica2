# Despiece derivado del ensamble terminado. No mueve ni suprime piezas del maestro.
import adsk.core,adsk.fusion,os,json
OUT=r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\renders\2026-10-27'
EXPORT=r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\exports\brazo-v03'

def run(context):
    app=adsk.core.Application.get();master=app.activeDocument;d=adsk.fusion.Design.cast(app.activeProduct);r=d.rootComponent;tm=adsk.fusion.TemporaryBRepManager.get()
    mode=globals().get('VIEW','J4');items=[];mapping=[]
    roots={o.component.name:o for o in r.occurrences};frame=roots['04_Giro_muneca'].transform2 if mode=='J4' else adsk.core.Matrix3D.create();frame.invert()
    for o in r.allOccurrences:
        path=o.fullPathName;cn=o.component.name;root=path.split('+')[0].split(':')[0]
        shift=None
        if mode=='J4':
            if cn=='A12_Encastre_distal_antebrazo_PETG':shift=(0,0,0)
            elif cn=='V03_R04_608_J4_inferior':shift=(0,0,2.4)
            elif cn=='V02_R04_608_J4':shift=(0,0,3.3)
            elif cn=='V03_Tapa_608_J4':shift=(0,0,4.5)
            elif cn=='V03_Separador_axial_J4':shift=(0,0,6)
            elif cn=='V02_E03_Eje_J4' or cn=='V03_E03_Retencion_superior_J4' or cn=='V03_Tornillo_axial_J4':shift=(0,0,7.2)
            elif root=='04_Giro_muneca' and cn in ['V02_A15_Horquilla_J5_PETG','V02_Rueda24_J4']:shift=(0,0,10)
            elif root=='11_Pinion_J4':shift=(3,0,3)
            elif cn=='V02_M04_MG90S_J4':shift=(3,0,-3)
            elif cn.startswith('V03_Tornillo_rosca_plastico_J4'):shift=(3,0,5)
        else:
            if root!='00_Base_control' and cn!='REF_AUTO_CHASIS':continue
            if cn=='REF_AUTO_CHASIS':shift=(0,0,0)
            elif cn=='V03_Plataforma_sandwich_superior':shift=(0,0,4)
            elif cn=='V03_Contraplaca_aluminio_3mm':shift=(0,0,-3)
            elif cn.startswith('V03_Separador_M3') or cn=='V03_Casquillo_central_compresion':shift=(0,0,0)
            elif cn.startswith('V03_Anclaje_M3'):shift=(0,0,9)
            elif cn=='V03_Anclaje_M10x45':shift=(0,0,13)
            elif cn.startswith('V03_Tuerca_arandela'):shift=(0,0,-5)
            elif cn.startswith('V03_XL4016_vertical_'):shift=(-5,0,4)
            elif cn in ['V03_PCA9685_vertical','V03_Nano_vertical']:shift=(4,0,4)
            elif cn=='V03_Bornera_y_alimentacion_REF':shift=(0,-4,3)
            elif cn=='V03_M01_MG946_Base':shift=(0,6,4)
            elif cn.startswith('V03_R01_608_J1_'):shift=(0,0,7 if 'inferior' in cn else 8)
            elif cn=='V03_Tapa_rodamientos_J1':shift=(0,0,10)
        if shift is None:continue
        count=0
        for b in o.component.bRepBodies:
            if cn=='REF_AUTO_CHASIS' and b.name!='CapaSuperior':continue
            if not b.isSolid or b.volume<1e-10:continue
            cp=tm.copy(b);tm.transform(cp,o.transform2);tm.transform(cp,frame)
            m=adsk.core.Matrix3D.create();m.translation=adsk.core.Vector3D.create(*shift);tm.transform(cp,m)
            items.append((cn,b.name,cp,b.appearance));count+=1
        if count:mapping.append({'source_component':cn,'exploded_translation_cm':shift,'bodies':count})
    doc=app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType);dd=adsk.fusion.Design.cast(app.activeProduct)
    dd.designIntent=adsk.fusion.DesignIntentTypes.HybridDesignIntentType;dd.designType=adsk.fusion.DesignTypes.DirectDesignType
    doc.name='AFR2_Despiece_'+mode+'_v03_REFERENCIA';rr=dd.rootComponent;groups={};aps={}
    for cn,name,b,ap in items:
        if cn not in groups:
            oc=rr.occurrences.addNewComponent(adsk.core.Matrix3D.create());oc.component.name=cn;groups[cn]=oc.component
        body=groups[cn].bRepBodies.add(b);body.name=name
        if ap:
            if ap.name not in aps:aps[ap.name]=dd.appearances.addByCopy(ap,'REF_'+ap.name)
            body.appearance=aps[ap.name]
    cam=app.activeViewport.camera;cam.upVector=adsk.core.Vector3D.create(0,0,1)
    cam.eye=adsk.core.Point3D.create(38,44,29);cam.target=adsk.core.Point3D.create(0,0,7);cam.isFitView=True;app.activeViewport.camera=cam
    os.makedirs(OUT,exist_ok=True);os.makedirs(EXPORT,exist_ok=True)
    app.activeViewport.saveAsImageFile(os.path.join(OUT,'despiece_'+mode.lower()+'_v03.png'),1600,1500)
    dd.exportManager.execute(dd.exportManager.createFusionArchiveExportOptions(os.path.join(EXPORT,'Despiece_'+mode+'_v03.f3d')))
    with open(os.path.join(EXPORT,'despiece_'+mode+'.json'),'w',encoding='utf-8') as f:json.dump(mapping,f,ensure_ascii=False,indent=2)
    master.activate()
    print('DESPIECE',mode,len(items),'cuerpos; copia derivada, maestro conservado')
