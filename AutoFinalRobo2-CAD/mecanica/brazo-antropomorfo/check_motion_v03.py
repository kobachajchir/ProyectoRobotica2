# Comprobacion CAD pedida por el usuario. No es un ensayo de hardware/carga.
import adsk.core,adsk.fusion,math,json,os,itertools,time,builtins
HERE=r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\revision_v03'

def ident():return [1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]
def mul(a,b):return [sum(a[4*r+k]*b[4*k+c] for k in range(4)) for r in range(4) for c in range(4)]
def trans(x=0,y=0,z=0):
    a=ident();a[3]=x;a[7]=y;a[11]=z;return a
def rot(axis,deg):
    c=math.cos(math.radians(deg));s=math.sin(math.radians(deg))
    return [c,0,s,0,0,1,0,0,-s,0,c,0,0,0,0,1] if axis=='Y' else [c,-s,0,0,s,c,0,0,0,0,1,0,0,0,0,1]
def inverse(a):
    b=[a[4*c+r] if r<3 and c<3 else (1 if r==c else 0) for r in range(4) for c in range(4)]
    for r in range(3):b[4*r+3]=-sum(b[4*r+k]*a[4*k+3] for k in range(3))
    return b
def matrix(a):
    m=adsk.core.Matrix3D.create();m.setWithArray(a);return m
def box(b):return tuple(b.boundingBox.minPoint.asArray())+tuple(b.boundingBox.maxPoint.asArray())
def moved_box(b,m):
    pts=[[b[i] if not mask&(1<<i) else b[i+3] for i in range(3)] for mask in range(8)]
    pp=[[sum(m[4*r+k]*p[k] for k in range(3))+m[4*r+3] for r in range(3)] for p in pts]
    return tuple(min(p[i] for p in pp) for i in range(3))+tuple(max(p[i] for p in pp) for i in range(3))
def overlap(a,b):return all(min(a[i+3],b[i+3])-max(a[i],b[i])>1e-7 for i in range(3))
def union_box(bb):return tuple(min(b[i] for b in bb) for i in range(3))+tuple(max(b[i+3] for b in bb) for i in range(3))

def fk(q,p):
    q1,q2,q3,q4,q5,q6,g=q
    aa={'00_Base_control':ident(),'80_Auto_referencia':ident()}
    a=mul(trans(z=p['base_top']),rot('Z',q1));aa['01_Giro_base']=a
    a=mul(mul(a,trans(z=p['tower_h'])),rot('Y',q2));aa['02_Brazo_PVC']=a
    a=mul(mul(a,trans(z=p['link1'])),rot('Y',q3));aa['03_Antebrazo_PVC']=a
    a=mul(mul(a,trans(z=p['link2'])),rot('Z',q4));aa['04_Giro_muneca']=a
    a=mul(mul(a,trans(z=p['wrist45'])),rot('Y',q5));aa['05_Inclinacion_muneca']=a
    a=mul(mul(a,trans(z=p['wrist56'])),rot('Z',q6));aa['06_Herramienta']=a
    gap=p['grip_min']+2*p['grip_r']*math.radians(g)
    aa['07_Mandibula_izquierda']=mul(a,trans(x=-gap/2-p['finger_t']/2-p['pad_t']))
    aa['08_Mandibula_derecha']=mul(a,trans(x=gap/2+p['finger_t']/2+p['pad_t']))
    aa['10_Pinion_pinza']=mul(mul(a,trans(z=p['grip_deck']+0.8)),rot('Z',g))
    aa['90_Pieza_REFERENCIA_50mm_100g']=mul(a,trans(z=p['grip_deck']+5.2))
    aa['11_Pinion_J4']=mul(mul(aa['03_Antebrazo_PVC'],trans(3.6,0,p['link2']+0.6)),rot('Z',7.5-q4))
    aa['09_Pinion_hombro']=mul(mul(aa['01_Giro_base'],trans(z=p['tower_h']-p['gear_centers'])),rot('Y',p.get('shoulder_phase',9)-2*q2))
    return aa

def initialize():
    d=adsk.fusion.Design.cast(adsk.core.Application.get().activeProduct);t=adsk.fusion.TemporaryBRepManager.get()
    names=['base_top','tower_h','link1','link2','wrist45','wrist56','grip_min','grip_r','grip_deck','finger_t','pad_t','gear_centers']
    p={n:d.userParameters.itemByName(n).value for n in names}
    z=d.userParameters.itemByName('shoulder_phase');p['shoulder_phase']=math.degrees(z.value) if z else 11.25
    groups={}
    for o in d.rootComponent.occurrences:
        if not o.isLightBulbOn:continue
        bb=[]
        def walk(c,xf):
            for b in c.bRepBodies:
                if not b.isSolid or b.volume<1e-10:continue
                tmp=t.copy(b);t.transform(tmp,matrix(xf))
                bb.append({'name':c.name+'/'+b.name,'body':tmp,'bbox':box(tmp),'appearance':b.appearance.name if b.appearance else ''})
            for oo in c.occurrences:walk(oo.component,mul(xf,list(oo.transform2.asArray())))
        walk(o.component,ident())
        groups[o.component.name]={'parts':bb,'bbox':union_box([b['bbox'] for b in bb])}
    builtins._afr2_check={'groups':groups,'p':p,'cache':{},'tested':0,'colliding':0,'findings':[],'scenarios':[],'start':time.time()}
    os.makedirs(HERE,exist_ok=True)
    print('CACHE',len(groups),sum(len(g['parts']) for g in groups.values()))

def pair(ga,gb,ma,mb,state):
    rel=mul(inverse(ma),mb);key=(ga,gb,tuple(round(v,9) for v in rel))
    if key in state['cache']:return state['cache'][key]
    a=state['groups'][ga];b=state['groups'][gb]
    if not overlap(a['bbox'],moved_box(b['bbox'],rel)):
        state['cache'][key]=[];return []
    t=adsk.fusion.TemporaryBRepManager.get();out=[];mm=matrix(rel)
    for bp in b['parts']:
        bb=moved_box(bp['bbox'],rel)
        cand=[ap for ap in a['parts'] if overlap(ap['bbox'],bb)]
        if not cand:continue
        tb=t.copy(bp['body']);t.transform(tb,mm)
        for ap in cand:
            ab=t.copy(ap['body'])
            try:ok=t.booleanOperation(ab,tb,adsk.fusion.BooleanTypes.IntersectionBooleanType)
            except Exception as ex:raise RuntimeError('Kernel de interseccion: '+ap['name']+' vs '+bp['name']+': '+str(ex))
            if ok:
                v=ab.volume*1000
                if v>0.001:out.append({'a':ap['name'],'b':bp['name'],'mm3':round(v,5)})
    state['cache'][key]=out;return out

def check(q,loaded=True,scope=None):
    s=builtins._afr2_check;ff=fk(q,s['p']);out=[]
    names=[n for n in s['groups'] if n in ff and (loaded or not n.startswith('90_'))]
    for ga,gb in itertools.combinations(names,2):
        if scope and not (ga in scope or gb in scope):continue
        rr=pair(ga,gb,ff[ga],ff[gb],s)
        if rr:out.extend([dict(v,ga=ga,gb=gb) for v in rr])
    # Ground below the car. Deck is checked using its actual reference solid.
    tm=adsk.fusion.TemporaryBRepManager.get();table=None
    for gn in names:
        if gn in ['00_Base_control','80_Auto_referencia']:continue
        if moved_box(s['groups'][gn]['bbox'],ff[gn])[2]<-9.464-1e-7:
            if table is None:
                ob=adsk.core.OrientedBoundingBox3D.create(adsk.core.Point3D.create(0,0,-59.464),adsk.core.Vector3D.create(1,0,0),adsk.core.Vector3D.create(0,1,0),200,200,100)
                table=tm.createBox(ob)
            for b in s['groups'][gn]['parts']:
                if moved_box(b['bbox'],ff[gn])[2]>=-9.464:continue
                cp=tm.copy(b['body']);tm.transform(cp,matrix(ff[gn]))
                ok=tm.booleanOperation(cp,table,adsk.fusion.BooleanTypes.IntersectionBooleanType)
                if ok and cp.volume*1000>0.001:out.append({'ga':gn,'gb':'SUELO_AUTO','a':b['name'],'mm3':round(cp.volume*1000,5)})
    s['tested']+=1
    if out:
        s['colliding']+=1
        if len(s['findings'])<500:s['findings'].append({'q_deg':q,'loaded':loaded,'contacts':out})
    return out

def native_parity():
    s=builtins._afr2_check;d=adsk.fusion.Design.cast(adsk.core.Application.get().activeProduct)
    js=['J1_Giro_base','J2_Hombro','J3_Codo','J4_Giro_muneca','J5_Inclinacion_muneca','J6_Giro_herramienta','J7_Servo_pinza']
    q=[math.degrees(d.rootComponent.joints.itemByName(n).jointMotion.rotationValue) for n in js]
    q[6]+=90
    fs=fk(q,s['p']);deltas=[]
    for o in d.rootComponent.occurrences:
        if o.component.name not in fs:continue
        a=list(o.transform2.asArray()); b=fs[o.component.name]
        err=max(abs(x-y) for x,y in zip(a,b));deltas.append({'component':o.component.name,'maximum_matrix_error':err})
    print('PARIDAD_CINEMATICA',json.dumps({'q_deg':q,'matrix_differences':deltas},ensure_ascii=False))
    with open(os.path.join(HERE,'paridad_cinematica.json'),'w',encoding='utf-8') as f:json.dump({'q_deg':q,'matrix_differences':deltas},f,ensure_ascii=False,indent=2)

def internal_mounts():
    s=builtins._afr2_check;t=adsk.fusion.TemporaryBRepManager.get();out=[]
    for gn,g in s['groups'].items():
        if gn=='80_Auto_referencia':continue  # Maestro del auto conservado, fuera de esta revision mecanica.
        for a,b in itertools.combinations(g['parts'],2):
            if a['name'].split('/')[0]==b['name'].split('/')[0]:continue
            if not overlap(a['bbox'],b['bbox']):continue
            cp=t.copy(a['body'])
            if t.booleanOperation(cp,b['body'],adsk.fusion.BooleanTypes.IntersectionBooleanType) and cp.volume*1000>0.001:
                out.append({'group':gn,'a':a['name'],'b':b['name'],'mm3':round(cp.volume*1000,5)})
    out.sort(key=lambda v:-v['mm3'])
    with open(os.path.join(HERE,'montajes_internos.json'),'w',encoding='utf-8') as f:json.dump(out,f,ensure_ascii=False,indent=2)
    print('MONTAJES',len(out),json.dumps(out[:25],ensure_ascii=False))

def native_pose():
    d=adsk.fusion.Design.cast(adsk.core.Application.get().activeProduct);r=d.rootComponent
    groups={o.component.name:o.fullPathName.split('+')[0].split(':')[0] for o in r.allOccurrences}
    es=adsk.core.ObjectCollection.create()
    for o in r.occurrences:es.add(o)
    ip=d.createInterferenceInput(es);ip.areCoincidentFacesIncluded=False
    results=d.analyzeInterference(ip);out=[]
    for i in range(results.count):
        rr=results.item(i);a=rr.entityOne;b=rr.entityTwo
        ga=groups.get(a.parentComponent.name);gb=groups.get(b.parentComponent.name)
        if ga==gb:continue
        vol=rr.interferenceBody.volume*1000
        if vol>0.001:out.append({'ga':ga,'gb':gb,'a':a.parentComponent.name+'/'+a.name,'b':b.parentComponent.name+'/'+b.name,'mm3':round(vol,5)})
    with open(os.path.join(HERE,'pose_nativa_interferencias.json'),'w',encoding='utf-8') as f:json.dump(out,f,ensure_ascii=False,indent=2)
    print('INTERFERENCIAS_NATIVAS_ENTRE_CONJUNTOS',len(out),json.dumps(out[:20],ensure_ascii=False))

def write_report(tag):
    s=builtins._afr2_check
    result={k:s[k] for k in ['tested','colliding','findings','scenarios']}
    result.update({'scope':'muestreo de solidos CAD; no ensayo fisico ni prueba continua de todo el espacio articular','volume_threshold_mm3':0.001,'seconds':round(time.time()-s['start'],2),'cached_relative_configurations':len(s['cache'])})
    with open(os.path.join(HERE,tag+'.json'),'w',encoding='utf-8') as f:json.dump(result,f,ensure_ascii=False,indent=2)
    print('INFORME',tag,'poses',s['tested'],'con_contactos',s['colliding'],'cache',len(s['cache']))

def run(context):
    mode=globals().get('MODE','init')
    if mode=='init':initialize();return
    s=builtins._afr2_check
    if mode=='parity':native_parity();return
    if mode=='mounts':internal_mounts();return
    if mode=='native':native_pose();return
    if mode=='pose':
        q=globals().get('Q',[20,50,0,0,0,0,109.816910613]);rr=check(q)
        print(json.dumps(rr[:35],ensure_ascii=False));write_report('auditoria_parcial');return
    if mode=='grip':
        scope={'06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','10_Pinion_pinza'}
        for g in range(181):check([0,45,0,0,0,0,g],False,scope)
        s['scenarios'].append({'type':'pinza_vacia','angle_deg':[0,180],'step_deg':1,'poses':181})
        write_report('apertura_pinza');return
    if mode=='wrist':
        q4=globals().get('Q4',0); count=0
        scope={'03_Antebrazo_PVC','04_Giro_muneca','05_Inclinacion_muneca','06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','10_Pinion_pinza','11_Pinion_J4'}
        for q5,q6 in itertools.product(range(-90,91,15),repeat=2):
            check([0,35,-15,q4,q5,q6,math.degrees((5-s['p']['grip_min'])/(2*s['p']['grip_r']))],True,scope);count+=1
        s['scenarios'].append({'type':'rejilla_muneca','q4':q4,'q5_deg':[-90,90],'q6_deg':[-90,90],'step_deg':15,'poses':count})
        write_report('barrido_muneca');return
    if mode=='axis':
        axis=globals()['AXIS'];lo,hi=globals()['LIMITS'];rest=globals().get('REST',[0,35,-15,0,0,0,109.816910613])
        for angle in sorted(set(list(range(lo,hi+1,2))+[hi])):
            q=list(rest);q[axis]=angle;check(q)
        s['scenarios'].append({'type':'eje','axis':axis+1,'limits':[lo,hi],'step_deg':2,'rest':rest})
        write_report('barrido_ejes');return
    if mode=='report':write_report('informe_movimiento')

