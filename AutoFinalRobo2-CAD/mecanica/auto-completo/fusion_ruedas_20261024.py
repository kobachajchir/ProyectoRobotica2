"""Ubica la rueda aportada; conserva fuente y alinea prisionero con plano D.
La mano opuesta es un reflejo del CAD, no una rueda rediseñada o escalada.
Sin comprobaciones de interferencia, fabricación ni dinámica.
"""
import os, sys, math
import adsk.core, adsk.fusion
sys.path.insert(0, os.path.dirname(__file__))
import fusion_ensamble as h

def run(_context: str):
    a,d,r=h.context()
    source=h.find('Wheel80mm')
    coupling=next(o for o in source.component.occurrences if 'Coupling' in o.name)
    cm=coupling.transform2
    origin=adsk.core.Point3D.create(0,1.8,0)
    origin.transformBy(cm)
    cx,cy,cz=[v*10 for v in origin.asArray()]
    screw=adsk.core.Vector3D.create(0.5,0,math.sqrt(3)/2)
    screw.transformBy(cm)
    screw.normalize()
    mgr=adsk.fusion.TemporaryBRepManager.get()
    hands={}
    for hand in (1,-1):
        name='Rueda80_ManoA' if hand==1 else 'Rueda80_ManoB'
        existing=next((o for o in r.occurrences if o.component.name==name),None)
        if existing is None:
            pieces=[]
            reflection=adsk.core.Matrix3D.create()
            reflection.setWithArray([-1,0,0,2*cx/10,0,1,0,0,0,0,1,0,0,0,0,1])
            for o in source.component.occurrences:
                for b in o.component.bRepBodies:
                    shape=mgr.copy(b)
                    if not mgr.transform(shape,o.transform2): raise RuntimeError('Transformacion de pieza no realizada')
                    if hand==-1 and not mgr.transform(shape,reflection): raise RuntimeError('Reflejo no realizado')
                    pieces.append((o.name,shape,b.appearance))
            existing=h.part(name,pieces,
                'Derivado de Wheel80mm aportado por usuario. Geometria sin escalar; mano B reflejada para configuracion X. Acople fuente con bore 6 mm; motor de 4 mm, ajuste fisico pendiente.')
            existing.component.attributes.add('AFR2','change_id','AFR2-20261024-01')
        hands[hand]=existing
    def rotation(hand,side):
        sx=hand*screw.x; sy=screw.y
        n=math.hypot(sx,sy); sx/=n; sy/=n
        # Source axial +Z points inward, radial screw direction points downward.
        # Orthonormal basis s,t,z -> -Y, side*Z, -side*X.
        return [0,0,-side,-sx,-sy,0,-side*sy,side*sx,0]
    # Determine handed diagonal from the actual roller axes of the imported model.
    R=rotation(1,1)
    rollers=[]
    for o in source.component.occurrences:
        if o.component.name!='Part4': continue
        p=adsk.core.Point3D.create(2.1,0,0); p.transformBy(o.transform2)
        v=adsk.core.Vector3D.create(1,0,0); v.transformBy(o.transform2)
        height=R[3]*(p.x*10-cx)+R[4]*(p.y*10-cy)
        dx=R[0]*v.x+R[1]*v.y+R[2]*v.z
        dz=R[6]*v.x+R[7]*v.y+R[8]*v.z
        rollers.append((height,dx*dz))
    source_sign=1 if max(rollers)[1]>0 else -1
    used={1:False,-1:False}
    for role,side,z,wanted in [('FR',1,-62,-1),('FL',-1,-62,1),('RR',1,62,1),('RL',-1,62,-1)]:
        hand=1 if wanted==source_sign else -1
        seed=hands[hand]
        if not used[hand]:
            o=seed; used[hand]=True
        else:
            h.capture()
            o=r.occurrences.addExistingComponent(seed.component,h.matrix())
        rot=rotation(hand,side)
        dest=(side*81,-14,z)
        src=(cx,cy,cz)
        xyz=[dest[i]-sum(rot[3*i+k]*src[k] for k in range(3)) for i in range(3)]
        h.place(o,xyz,rot,'Rueda_'+role)
        o.attributes.add('AFR2','grub_alignment','Eje del agujero radial dirigido hacia -Y; cara plana del eje motor en Y=-15.4 mm; centro axial del apriete X=+/-84 mm.')
    source.isLightBulbOn=False
    silver=h.color('AFR2 Metal',(175,180,187))
    pieces=[]
    for side in (-1,1):
        for z in (-62,62):
            screw_body=h.cylinder((side*84,-20,z),(side*84,-15.4,z),1.2)
            h.cut(screw_body,h.box((side*84,-19.8,z),(1,1,0.7)))
            pieces.append(('Prisionero_alineado_cara_D',screw_body,silver))
    if not any(o.component.name=='Prisioneros_Ruedas' for o in r.occurrences):
        p=h.part('Prisioneros_Ruedas',pieces,
            'Referencia de apriete radial alineada con cara plana D del motor. Bore del acople fuente 6 mm frente a eje 4 mm: no implica ajuste fisico correcto sin reductor.')
        p.component.attributes.add('AFR2','change_id','AFR2-20261024-01')
    h.capture(); h.view()
    print('Ruedas colocadas en cuatro esquinas; diagonales con manos opuestas. Prisioneros en cara D. Fuente preservada y oculta.')
