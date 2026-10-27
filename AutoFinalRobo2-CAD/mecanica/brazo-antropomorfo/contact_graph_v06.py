# Contactos entre componentes del brazo en la pose nativa; no equivale a calculo de uniones.
import adsk.core,adsk.fusion,os,json,itertools,math
OUT=r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\revision_v06'

def run(context):
    app=adsk.core.Application.get();d=adsk.fusion.Design.cast(app.activeProduct);r=d.rootComponent;parts=[];edges=[]
    for o in r.allOccurrences:
        if o.fullPathName.startswith(('80_Auto_referencia:','81_Pan_HCSR04:','82_Envolvente_pan_REFERENCIA:','90_Pieza_REFERENCIA_50mm_100g:')):continue
        bodies=[]
        for b in o.component.bRepBodies:
            if not b.isSolid or b.volume<1e-9:continue
            bodies.append(b.createForAssemblyContext(o))
        if not bodies:continue
        boxes=[tuple(b.boundingBox.minPoint.asArray())+tuple(b.boundingBox.maxPoint.asArray()) for b in bodies]
        bb=tuple(min(z[k] for z in boxes) for k in range(3))+tuple(max(z[k+3] for z in boxes) for k in range(3))
        parts.append({'name':o.fullPathName,'bodies':bodies,'bbox':bb,'parent':len(parts)})
    def find(i):
        while parts[i]['parent']!=i:
            parts[i]['parent']=parts[parts[i]['parent']]['parent'];i=parts[i]['parent']
        return i
    def gapbox(a,b):return math.sqrt(sum(max(a[k]-b[k+3],b[k]-a[k+3],0)**2 for k in range(3)))
    for i,a in enumerate(parts):
        for j in range(i+1,len(parts)):
            b=parts[j]
            if gapbox(a['bbox'],b['bbox'])>.003:continue
            dist=min(app.measureManager.measureMinimumDistance(x,y).value*10 for x in a['bodies'] for y in b['bodies'])
            if dist<=.03:
                ai=find(i);bi=find(j);parts[bi]['parent']=ai;edges.append({'a':a['name'],'b':b['name'],'gap_mm':round(dist,6)})
    groups={}
    for i,a in enumerate(parts):groups.setdefault(find(i),[]).append(a['name'])
    sets=sorted(groups.values(),key=lambda a:-len(a))
    result={'scope':'Contacto geometrico nominal de componentes del brazo; cada modulo comercial se trata como un conjunto. Umbral 0.03 mm. No prueba apriete ni resistencia. Carro y cubo excluidos.','components':len(parts),'connected_sets':sets,'edges':edges}
    with open(os.path.join(OUT,'grafo_contactos.json'),'w',encoding='utf-8') as f:json.dump(result,f,ensure_ascii=False,indent=2)
    print('GRAFO',len(parts),'COMPONENTES',len(sets),'GRUPOS',json.dumps(sets[1:],ensure_ascii=False))

