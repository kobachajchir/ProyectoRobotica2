"""Volumen/CG del banco v15. Lectura nativa; masas comerciales sustituyen volumen."""
import adsk.core,adsk.fusion,json,os
H=os.path.join(globals().get('AFR2_REPO_ROOT',r'D:\ProyectoRobotica2'),'AutoFinalRobo2-CAD','mecanica','brazo-antropomorfo','revision_v15')
OUTNAME='masas_modulos.json' # reiniciar: el interprete de Fusion conserva globals entre llamadas
def run(context):
    app=adsk.core.Application.get();d=adsk.fusion.Design.cast(app.activeProduct);r=d.rootComponent
    if app.activeDocument.name!='AFR2_Compacto_v15_Modulos_mecanicos':raise RuntimeError('Documento incorrecto')
    rows=[]
    density={'PLA':1.24,'Al6061':2.7,'Al5052':2.68,'Acero':7.85,'Elastomero':1.1}
    def walk(component,transform,path='',root=''):
        for occurrence in component.occurrences:
            total=transform.copy();total.transformBy(occurrence.transform2)
            current=path+'+'+occurrence.name if path else occurrence.name
            yield occurrence,total,current,root or occurrence.component.name
            yield from walk(occurrence.component,total,current,root or occurrence.component.name)
    for o,transform,path,root in walk(r,adsk.core.Matrix3D.create()):
        c=o.component;bs=[b for b in c.bRepBodies if b.isSolid]
        if not bs:continue
        kind=c.attributes.itemByName('AFR2','material')
        if kind:kind=kind.value
        elif all(b.appearance and 'Acero' in b.appearance.name for b in bs):kind='Acero'
        else:raise RuntimeError('Falta material explicito en '+c.name)
        cat=c.attributes.itemByName('AFR2','catalog_mass_g');volume=sum(b.volume for b in bs)
        mass=float(cat.value) if cat else volume*density[kind]
        cg=[0.,0.,0.]
        for b in bs:
            p=b.physicalProperties.centerOfMass;p.transformBy(transform)
            for j,v in enumerate(p.asArray()):cg[j]+=v*10*b.volume/volume
        rows.append(dict(path=path,root=root,name=c.name,material=kind,solid_bodies=len(bs),volume_cm3=volume,mass_g=mass,catalog_override=bool(cat),cg_world_mm=cg))
    groups={}
    for x in rows:
        name='J2' if x['root'].startswith('J2_') else ('Pinza_simetrica_descartada' if x['root'].startswith('Pinza_') else ('Pinza_simple' if x['root'].startswith('S_') else x['root']))
        if x['material']=='Carga':continue
        a=groups.setdefault(name,dict(mass_g=0,cg_world_mm=[0.,0.,0.]))
        a['mass_g']+=x['mass_g']
        for j in range(3):a['cg_world_mm'][j]+=x['mass_g']*x['cg_world_mm'][j]
    for g in groups.values():g['cg_world_mm']=[v/g['mass_g'] for v in g['cg_world_mm']]
    data=dict(document=app.activeDocument.name,scope='Masas de volumen/densidad supuestos y fichas, no pesadas. Banco de modulos, no brazo completo.',groups=groups,parts=rows)
    with open(os.path.join(H,globals().get('OUTNAME','masas_modulos.json')),'w',encoding='utf-8') as f:json.dump(data,f,ensure_ascii=False,indent=2)
    print(json.dumps(groups,ensure_ascii=False))
