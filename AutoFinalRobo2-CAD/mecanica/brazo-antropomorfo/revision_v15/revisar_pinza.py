"""Revision CAD solicitada, acotada a pinza. BRep temporales, sin mover modelo.

No verifica brazo, reductor J2, tolerancias de fabricacion ni funcionamiento
fisico. Se conserva como hallazgo cualquier booleano fallido.
"""
import adsk.core,adsk.fusion,json,math,os
H=os.path.join(globals().get('AFR2_REPO_ROOT',r'D:\ProyectoRobotica2'),'AutoFinalRobo2-CAD','mecanica','brazo-antropomorfo','revision_v15')
P=adsk.core.Point3D.create
def mat(x=0,z=0,a=0):
    t=adsk.core.Matrix3D.create();t.setToRotation(a,adsk.core.Vector3D.create(0,1,0),P(0,0,0));t.translation=adsk.core.Vector3D.create(x/10,0,z/10);return t
def product(a,b):
    # Composicion explicita evita invertir el orden de transformBy.
    aa=a.asArray();bb=b.asArray();out=adsk.core.Matrix3D.create();out.setWithArray([sum(aa[4*i+k]*bb[4*k+j] for k in range(4)) for i in range(4) for j in range(4)]);return out
def tangent(theta):
    ax=27-15*math.sin(theta);az=10-15*math.cos(theta);dist=math.hypot(42-ax,-9-az)
    phi=math.pi+math.atan((az+9)/(ax-42))-math.acos(4/dist)
    return math.sqrt(dist*dist-16),phi
def overlap(a,b):
    aa=a.boundingBox;bb=b.boundingBox
    return all(min(aa.maxPoint.asArray()[i],bb.maxPoint.asArray()[i])-max(aa.minPoint.asArray()[i],bb.minPoint.asArray()[i])>1e-7 for i in range(3))
def run(context):
    app=adsk.core.Application.get();d=adsk.fusion.Design.cast(app.activeProduct);r=d.rootComponent
    if app.activeDocument.name!='AFR2_Compacto_v15_Modulos_mecanicos':raise RuntimeError('Documento incorrecto')
    template=[];base=next(o for o in r.occurrences if o.component.name=='S_Pinza_base').transform2
    for o in r.occurrences:
        if not o.component.name.startswith('S_'):continue
        def walk(c,T,path):
            for b in c.bRepBodies:
                if b.isSolid:template.append((o.component.name,path,b,T))
            for sub in c.occurrences:walk(sub.component,product(T,sub.transform2),path+'+'+sub.component.name)
        walk(o.component,adsk.core.Matrix3D.create(),o.component.name)
    thopen=math.asin((60-25+6-27)/36);l0,p0=tangent(thopen);tm=adsk.fusion.TemporaryBRepManager.get();rows=[]
    for gap in [50,52.5,55,57.5,60]:
        th=math.asin((gap-25+6-27)/36);ll,phi=tangent(th);alpha=(ll-l0)/4+phi-p0
        frames={'S_Pinza_base':base,'S_Biela0':product(base,mat(27,10,th)),'S_Biela1':product(base,mat(27,20,th)),
                'S_Mordaza_movil':product(base,mat(27+36*math.sin(th),10+36*math.cos(th))), 'S_Tambor_directo':product(base,mat(42,-9,alpha))}
        bs=[]
        for root,name,b,local in template:
            cp=tm.copy(b);tm.transform(cp,product(frames[root],local));bs.append((root,name,cp))
        hits=[];errors=[];checked=0
        for i,(ar,an,a) in enumerate(bs):
            for br,bn,b in bs[i+1:]:
                if an==bn or not overlap(a,b):continue
                checked+=1
                try:
                    cp=tm.copy(a);ok=tm.booleanOperation(cp,b,adsk.fusion.BooleanTypes.IntersectionBooleanType)
                    if not ok:errors.append([an,bn]);continue
                    v=cp.volume*1000
                    if v>.001:hits.append(dict(a=an,b=bn,volume_mm3=v))
                except RuntimeError as e:errors.append(dict(a=an,b=bn,error=str(e)))
        rows.append(dict(gap_mm=gap,theta_deg=math.degrees(th),motor_deg=math.degrees(alpha),boolean_pairs=checked,collisions=hits,errors=errors))
    out=dict(scope='Solo pinza simple; cinco aperturas, BRep temporales y elementos modelados. Sin prueba fisica, tolerancias, cable completo, brazo o J2.',rows=rows)
    with open(os.path.join(H,'revision_pinza.json'),'w',encoding='utf-8') as f:json.dump(out,f,ensure_ascii=False,indent=2)
    print(json.dumps([dict(gap=x['gap_mm'],hits=len(x['collisions']),errors=len(x['errors'])) for x in rows]))
