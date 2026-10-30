"""Brazo compacto completo. Fases de construccion una sola vez, en Fusion.

Maestros v09/v15 intactos. Productos comerciales representados por geometria
de montaje: las envolventes no son modelos de fabricacion de esos productos.
"""
from pathlib import Path
_repo=Path(globals().get('AFR2_REPO_ROOT',r'D:\ProyectoRobotica2'))
_old=_repo/'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v15/modelar_v15.py'
exec(compile(_old.read_text(encoding='utf-8'),str(_old),'exec'))
NAME16='AFR2_Brazo_Compacto_v16_Correas'
HERE16=_old.parent.parent/'revision_v16'
def extrude(c,s,depth,op,name,profile=None):
    # Evitar volver a resolver todo el ensamble tras cada corte. Ambito
    # explicito como en mount_low_v05; se activa una vez por componente.
    if D.activeComponent.name!=c.name:
        target=next((o for o in R.allOccurrences if o.component==c),None)
        if target:target.activate()
    ip=c.features.extrudeFeatures.createInput(profile if profile else s.profiles.item(0),op)
    ip.setDistanceExtent(False,V(str(depth)))
    if op==OP.CutFeatureOperation:ip.participantBodies=[b for b in c.bRepBodies if b.isSolid]
    f=c.features.extrudeFeatures.add(ip);f.name=name;s.isVisible=False
    with open(HERE16/'progreso_cad.jsonl','a',encoding='utf-8') as log:log.write(json.dumps(dict(component=c.name,feature=name))+'\n')
    if any(b.parentComponent!=c for b in f.bodies):raise RuntimeError('Operacion fuera de ambito')
    return f
def par(n,e,u='mm'):
    p=D.userParameters.itemByName(n)
    if p:p.expression=e
    else:D.userParameters.add(n,V(e),u,'v16: dimension de diseno, procedencia en informe; recalcular al editar.')
def root16(n):return main(n).component
def piece(parent,n,kind='Al6061',mass=None):return part(parent,n,kind,mass)
def newbox(parent,n,x,y,z,w,d,h,kind='Al6061'):
    c=piece(parent,n,kind);box(c,n,x,y,z,w,d,h,'red' if kind=='PLA' else 'steel');return c
def bearing16(parent,n,axis,start,u,v,id=8,od=16,w=5,mass=4):
    c=piece(parent,n,'Rodamiento',mass);ring(c,'Rodamiento_comercial',axis,start,u,v,str(od)+' mm',str(id)+' mm',str(w)+' mm','steel')
    return c
def bush(parent,n,axis,start,u,v,od,id,w,kind='Al6061'):
    c=piece(parent,n,kind);ring(c,n,axis,start,u,v,od,id,w,'steel' if kind!='PLA' else 'red');return c
def shaft16(parent,n,axis,start,u,v,d,length):
    c=piece(parent,n,'Acero');ring(c,'Eje',axis,start,u,v,d,None,length,'steel')
    c.attributes.add('AFR2','spec','Acero certificado Rp>=600MPa requerido; diarios redondos, retencion y zonas planas locales.')
    return c
def bolt16(parent,n,axis,start,u,v,d,length):
    c=shaft16(parent,n,axis,start,u,v,d,length)
    ring(c,'Cabeza_REF',axis,f'({start})+({length})',u,v,'5.5 mm',None,'2.5 mm','darksteel');join_all(c,'Tornillo_REF');return c
def pipe16(c,n,points,diam):
    target=next((o for o in R.allOccurrences if o.component==c),None)
    if target:target.activate()
    sk=c.sketches.add(c.xYConstructionPlane);sk.name='Recorrido_'+n
    pts=adsk.core.ObjectCollection.create()
    for x,y,z in points:pts.add(P(x/10,y/10,z/10))
    curve=sk.sketchCurves.sketchFittedSplines.add(pts)
    path=c.features.createPath(curve,False);ip=c.features.pipeFeatures.createInput(path,OP.NewBodyFeatureOperation);ip.sectionSize=V(diam)
    f=c.features.pipeFeatures.add(ip);f.name=n;sk.isVisible=False
    for b in f.bodies:b.appearance=AP['steel']
    return f
def servo16(parent,n,model,axis,face,u='0 mm',v='0 mm',side=1):
    # Coordenadas u=X,v=Z paraY; u=X,v=Y paraZ. Carcasa completa antes
    # del plano de salida, de modo que el eje sobresale hacia side.
    mini=model=='STS3032';m=21.6 if mini else (75.5 if model=='STS3250' else 56)
    A,B,H,off=('23 mm','12 mm','27.5 mm','5.71 mm') if mini else ('45.22 mm','24.72 mm','35 mm','12.5 mm')
    c=piece(parent,n,'Servo',m)
    if axis=='Y':
        start=f'({face})-({H})' if side>0 else face
        box(c,'Carcasa',f'({u})-({off})',start,f'({v})-({B})/2',A,H,B,'servo')
        sy=face if side>0 else f'({face})-4.4 mm'
        ring(c,'Spline25T_ENVOLVENTE','Y',sy,u,v,'4.95 mm' if mini else '5.9 mm',None,'4.4 mm','brass')
    else:
        box(c,'Carcasa',f'({u})-({off})',f'({v})-({B})/2',f'({face})-({H})',A,B,H,'servo')
        ring(c,'Spline25T_ENVOLVENTE','Z',face,u,v,'4.95 mm' if mini else '5.9 mm',None,'4.4 mm','brass')
    c.attributes.add('AFR2','catalog',model+'; spline comercial25T simplificado. No fabricar desde esta envolvente. Fijacion mediante cuna y abrazadera externa.')
    return c
def pulley16(parent,n,teeth,axis,start,u,v,bore='8 mm'):
    # Discos de masa conservadora: no se sustraen dientes ni ventanas.
    # El dentado comercial se indica en atributos/croquis, no se fabrica aqui.
    dp=f'{teeth*3/math.pi:.12f} mm';c=piece(parent,n)
    ring(c,'Envolvente_raiz_comercial',axis,start,u,v,f'({dp})-0.762 mm',bore,'pulley_face','steel')
    flange=str(math.ceil((teeth*3/math.pi+2)/2)*2)+' mm'
    for yy in [f'({start})-0.8 mm',f'({start})+pulley_face']:
        ring(c,'Pestana',axis,yy,u,v,flange,bore,'0.8 mm','steel')
    ring(c,'Cubo',axis,f'({start})+pulley_face+0.8 mm',u,v,'16 mm' if teeth>30 else '10 mm',bore,'6 mm','steel');join_all(c,'Polea_comercial_REF')
    c.attributes.add('AFR2','catalog',f'Polea HTD3M {teeth}T aluminio, cara9mm para correa6. Croquis y taladros editables; perfil comercial no fabricable desde cilindro. Masa de calculo por disco sin deducir dientes.')
    return c
def coupler16(parent,n,axis,start,u,v,reverse=False):
    c=piece(parent,n,'Al6061');ring(c,'Acople_comercial_REF',axis,start,u,v,'12 mm',None,'10.4 mm','steel')
    ring(c,'Paso_eje5',axis,f'({start})-0.1 mm',u,v,'5.1 mm',None,'10.6 mm',cut=True)
    ss=start if reverse else f'({start})+6 mm'
    ring(c,'Alojamiento25T_ENVOLVENTE',axis,ss,u,v,'5.9 mm',None,'4.4 mm',cut=True)
    c.attributes.add('AFR2','interface','Acople25T metalico; spline comercial de referencia, no taladro para fabricar. Interfaz exacta debe corresponder al horn seleccionado.')
    return c
def belt16(parent,n,small,big,centre,y,along='Z'):
    # Banda cerrada por tangentes exteriores y arcos, espesor2.3mm.
    # Coordenadas constantes de un producto comercial de longitud elegida.
    c=piece(parent,n,'Elastomero',3.024);r=small*3/(2*math.pi);Rr=big*3/(2*math.pi)
    C=val(centre)*10;a=math.asin((Rr-r)/C)
    # contorno en plano xz: centro pequeño(0,0), grande(0,C).
    def contour(delta):
        pts=[]
        # normal de tangencia: (cos(a),-sin(a)); arco pequeño exterior inferior
        for i in range(33):
            t=-a-(math.pi-2*a)*i/32;pts.append(((r+delta)*math.cos(t),(r+delta)*math.sin(t)))
        for i in range(49):
            t=-math.pi+a-(math.pi+2*a)*i/48;pts.append(((Rr+delta)*math.cos(t),C+(Rr+delta)*math.sin(t)))
        return pts
    s=c.sketches.add(datum(c,'Y',y,'Plano_correa'))
    for delta in [-1.1,1.2]:
        pts=contour(delta)
        if along=='X':pts=[(-z,x) for x,z in pts]
        for i,(x,z) in enumerate(pts):
            xx,zz=pts[(i+1)%len(pts)];s.sketchCurves.sketchLines.addByTwoPoints(P(x/10,-z/10,0),P(xx/10,-zz/10,0))
    profiles=[pr for pr in s.profiles if pr.profileLoops.count==2]
    if len(profiles)!=1:raise RuntimeError('Contorno de correa ambiguo')
    f=extrude(c,s,'6 mm',OP.NewBodyFeatureOperation,'Correa_210_3M_6_REF',profiles[0])
    for b in f.bodies:b.appearance=AP['rubber']
    c.attributes.add('AFR2','spec','OMEGA HP 210 3M 6, referencia envolvente sin dientes. Contacto polea/correa intencional; no sustituir por OMEGA basica sin recalcular.')
    return c
def phase16_init():
    global D,R,AP
    app=adsk.core.Application.get();options=app.importManager.createFusionArchiveImportOptions(str(_old.parent/'AFR2_Compacto_v15_Modulos_mecanicos.f3d'))
    doc=app.importManager.importToNewDocument(options);doc.name=NAME16;setup()
    for j in list(R.joints):
        if not j.name.startswith('S_'):j.deleteMe()
    for o in list(R.occurrences):
        if not o.component.name.startswith('S_'):o.deleteMe()
    for o in R.occurrences:o.isGrounded=False;o.isGroundToParent=False
    for n,e,u in [('link1','50 mm','mm'),('link2','45 mm','mm'),('wrist45','30 mm','mm'),('wrist56','48 mm','mm'),('base_axis_z','64 mm','mm'),('shoulder_local','16 mm','mm'),('pulley_face','9 mm','mm'),('j2_centre','40.394711379 mm','mm'),('j3_centre','54.858210911 mm','mm'),('tool_dx','-10.772 mm','mm'),('tool_dy','-8.711 mm','mm'),('q1','0 deg','deg'),('q2','30 deg','deg'),('q3','-50 deg','deg'),('q4','0 deg','deg'),('q5','20 deg','deg'),('q6','0 deg','deg')]:par(n,e,u)
    par('link2','50 mm');par('wrist56','50 mm')
    for n in ['00_Base','01_Plataforma','02_Brazo','03_Antebrazo','04_Giro','05_Muneca','06_Herramienta','J2_Pinion','J3_Pinion']:root16(n)
    occ('00_Base').isGrounded=True
    print('Copia v16 creada; v09/v15 conservadas. Gripper existente retenido con articulaciones internas.')
def phase16_base():
    g=comp('00_Base')
    top=newbox(g,'Montura_superior_Al2','-76 mm','-26 mm','6 mm','152 mm','84 mm','2 mm')
    # Paso libre del servo y acceso central; ala inferior unica, no dos piezas.
    box(top,'Ventana_centro','-27 mm','-20 mm','5.9 mm','54 mm','40 mm','2.2 mm',cut=True)
    low=newbox(g,'Contraplaca_inferior_unica_Al2','-76 mm','10 mm','-5 mm','152 mm','48 mm','2 mm')
    for label,x,y in [('A',31.051,51.739),('B',67.997,51.739),('C',31.051,10.964),('D',68.418,10.964),('E',-68.2986,19.843),('F',-31.1423,19.8009)]:
        for c,z in [(top,'5.9 mm'),(low,'-5.1 mm')]:ring(c,'Paso_'+label,'Z',z,f'{x} mm',f'{y} mm','3.4 mm',None,'2.2 mm',cut=True)
        bush(g,'Separador_'+label,'Z','0 mm',f'{x} mm',f'{y} mm','6 mm','3.4 mm','6 mm')
        bolt16(g,'M3_anclaje_'+label,'Z','-7 mm',f'{x} mm',f'{y} mm','3 mm','15 mm')
    servo16(g,'M1_ST3215','ST3215','Z','41 mm')
    c=newbox(g,'Cuna_J1_PLA','-15 mm','-15 mm','6 mm','51 mm','30 mm','2 mm','PLA')
    for x in ['-15 mm','34 mm']:box(c,'Costado',x,'-15 mm','8 mm','2 mm','30 mm','33 mm')
    for y in ['-15 mm','13 mm']:box(c,'Retencion_carcasa','-15 mm',y,'8 mm','51 mm','2 mm','4 mm')
    join_all(c,'Cuna_una_pieza')
    # Apoyos688 sin cargar el tren interno del servo con la flexion de todo el brazo.
    c=piece(g,'Cartucho_J1_PLA','PLA')
    for z in ['50 mm','58 mm']:ring(c,'Asiento688','Z',z,'0 mm','0 mm','22 mm','16.1 mm','5 mm')
    for x in ['-11 mm','9 mm']:box(c,'Pared_portante',x,'-8 mm','8 mm','2 mm','16 mm','55 mm')
    join_all(c,'Cartucho_continuo')
    for z in ['50 mm','58 mm']:bearing16(g,'688_J1_'+z,'Z',z,'0 mm','0 mm')
    # Electronica al costado, posicion editable; no se declara firmware integrado.
    pcb=newbox(g,'Control_ESP32_DEVKIT_REF','-65 mm','-22 mm','10 mm','28 mm','52 mm','1.6 mm','Electronica')
    pcb.attributes.add('AFR2','catalog_mass_g','12');finish(pcb,'pcb')
    dc=newbox(g,'Reguladores_y_TTL_REF','39 mm','-22 mm','12 mm','30 mm','50 mm','22 mm','Electronica')
    dc.attributes.add('AFR2','catalog_mass_g','138');finish(dc,'pcb')
    batt=newbox(g,'Bateria_brazo_REF','-34 mm','-63 mm','4 mm','68 mm','33 mm','26 mm','Potencia');batt.attributes.add('AFR2','catalog_mass_g','130');finish(batt,'servo')
    origin(g,'J1','0 mm','0 mm','base_axis_z')
def phase16_hombro():
    g=comp('01_Plataforma');c=bush(g,'Plato_giro_Al','Z','0 mm','0 mm','0 mm','60 mm','8 mm','2 mm')
    sh=shaft16(g,'Eje_J1','Z','-17 mm','0 mm','0 mm','8 mm','19 mm')
    ring(sh,'Brida_integral','Z','-17 mm','0 mm','0 mm','28 mm',None,'2 mm','steel');join_all(sh,'Eje_brida_una_pieza')
    bush(g,'Horn25T_J1_REF','Z','-22 mm','0 mm','0 mm','28 mm','5.9 mm','5 mm')
    # Collar y flange unir eje/plato; las interfaces de horn son comerciales.
    bush(g,'Cubo_J1','Z','-3 mm','0 mm','0 mm','16 mm','8 mm','5 mm')
    for y in ['-26 mm','21 mm']:
        c=yplate(g,'Horquilla_J2_'+y,'-12 mm','2 mm','24 mm','shoulder_local+12 mm',y,'2 mm','Al6061')
        cut_y(c,'Asiento688','0 mm','shoulder_local','16.1 mm',f'({y})-0.1 mm','2.2 mm')
        bearing16(g,'688_J2_'+y,'Y',y,'0 mm','shoulder_local')
    # Placa lateral de motor y polea, con ventanas y union al plato.
    plate=yplate(g,'Placa_portante_J2_Al','-58 mm','-16 mm','90 mm','48 mm','28 mm','2 mm','Al6061')
    for x,z in [('0 mm','shoulder_local'),('-j2_centre','shoulder_local')]:cut_y(plate,'Paso_eje',x,z,'9 mm','27.9 mm','2.2 mm')
    for xx in ['-56 mm','28 mm']:box(plate,'Pestana_union',xx,'0 mm','0 mm','2 mm','30 mm','2 mm','steel')
    join_all(plate,'Placa_con_alas')
    c=piece(g,'Cuna_J2_PLA','PLA')
    box(c,'Piso','-j2_centre-13 mm','61 mm','shoulder_local-14.4 mm','47 mm','39 mm','2 mm')
    for xx in ['-j2_centre-13 mm','-j2_centre+32 mm']:box(c,'Pared',xx,'61 mm','shoulder_local-12.4 mm','2 mm','39 mm','26 mm')
    join_all(c,'Housing_motor_una_pieza')
    servo16(g,'M2_STS3250','STS3250','Y','65.4 mm','-j2_centre','shoulder_local',side=-1)
    # side=-1 deja el motor detras del plano de salida +Y, spline hacia polea.
    bearing16(g,'625_entrada_J2','Y','48 mm','-j2_centre','shoulder_local',5,16,5,5)
    c=yplate(g,'Soporte625_J2','-j2_centre-10 mm','shoulder_local-10 mm','20 mm','20 mm','53 mm','2 mm','Al6061');cut_y(c,'Asiento','-j2_centre','shoulder_local','16.1 mm','52.9 mm','2.2 mm')
    box(c,'Pie','-j2_centre-10 mm','28 mm','shoulder_local-12 mm','20 mm','27 mm','2 mm','steel');join_all(c,'Soporte_con_pie')
    origin(g,'J1');origin(g,'J2','0 mm','0 mm','shoulder_local','Y');origin(g,'M2','-j2_centre','0 mm','shoulder_local','Y')
    p=comp('J2_Pinion');shaft16(p,'Eje_entrada_J2','Y','33 mm','0 mm','0 mm','5 mm','28 mm');pulley16(p,'Polea20_J2',20,'Y','34 mm','0 mm','0 mm','5 mm');coupler16(p,'Acople25T_J2_REF','Y','55 mm','0 mm','0 mm');origin(p,'EJE',axis='Y')
    print('Base giratoria, hombro y transmision3:1 modelados.')
def phase16_correas():
    g=comp('01_Plataforma')
    belt=belt16(g,'Correa_J2_HP_210',20,60,'j2_centre','35.5 mm',along='X')
    # Belt helper tiene polea pequena en0 y grande en-C. Trasladar al centro
    # motor y orientar hacia salida con giro180Z sobre su componente.
    oo=next(o for o in g.occurrences if o.component==belt);t=adsk.core.Matrix3D.create();t.setToRotation(math.pi,adsk.core.Vector3D.create(0,1,0),P(0,0,0));t.translation=adsk.core.Vector3D.create(-val('j2_centre'),0,val('shoulder_local'));oo.transform2=t
    g=comp('02_Brazo');z='link1-j3_centre'
    belt=belt16(g,'Correa_J3_HP_210',16,48,'j3_centre','-41.5 mm')
    oo=next(o for o in g.occurrences if o.component==belt);tm=adsk.core.Matrix3D.create();tm.translation=adsk.core.Vector3D.create(0,0,val(z));oo.transform2=tm
def phase16_links():
    par('link2','50 mm');par('wrist56','50 mm')
    for name,L,half in [('02_Brazo','link1','15 mm'),('03_Antebrazo','link2','18 mm')]:
        g=comp(name)
        for sign in [-1,1]:
            yy=f'({sign})*({half})-1 mm';c=yplate(g,'Mejilla_'+str(sign),'-14 mm','-14 mm','28 mm',L+'+28 mm',yy,'2 mm','Al6061')
            cut_y(c,'Proximal','0 mm','0 mm','8.1 mm',f'({yy})-0.1 mm','2.2 mm');cut_y(c,'Distal','0 mm',L,'8.1 mm',f'({yy})-0.1 mm','2.2 mm')
            box(c,'Ventana','-8 mm',f'({yy})-0.1 mm','14 mm','16 mm','2.2 mm',L+'-28 mm',cut=True)
            for xx in ['-14 mm','12 mm']:box(c,'Borde',xx,yy,'0 mm','2 mm','6 mm',L,'steel')
            join_all(c,'Mejilla_plegada_una_pieza')
        for z in ['7 mm',L+'-7 mm']:
            sx='-20 mm' if name=='03_Antebrazo' else '0 mm'
            if name=='03_Antebrazo':
                for o in g.occurrences:
                    if o.component.name.startswith('Mejilla_'):
                        yy=f'({-1 if o.component.name.startswith("Mejilla_-1") else 1})*({half})-1 mm'
                        box(o.component,'Oreja_espaciador','-24 mm',yy,f'({z})-4 mm','12 mm','2 mm','8 mm','steel');join_all(o.component,'Mejilla_con_orejas')
            for o in g.occurrences:
                if o.component.name.startswith('Mejilla_'):
                    yy=f'({-1 if o.component.name.startswith("Mejilla_-1") else 1})*({half})-1 mm'
                    cut_y(o.component,'Paso_M3_espaciador',sx,z,'3.2 mm',f'({yy})-0.1 mm','2.2 mm')
            bush(g,'Separador_'+z,'Y',f'-({half})+1 mm',sx,z,'6 mm','3.2 mm',f'2*({half})-2 mm')
            bolt16(g,'M3_union_'+z,'Y',f'-({half})-3 mm',sx,z,'3 mm',f'2*({half})+4 mm')
        origin(g,'PROXIMAL',axis='Y');origin(g,'DISTAL','0 mm','0 mm',L,'Y')
    # SalidaJ2 se mueve con primer eslabon; salidaJ3 con el segundo.
    for gname,label in [('02_Brazo','J2'),('03_Antebrazo','J3')]:
        g=comp(gname);shaft16(g,'Eje_salida_'+label,'Y','-29 mm' if label=='J2' else '-51 mm','0 mm','0 mm','8 mm','82 mm' if label=='J2' else '80 mm')
        pulley16(g,'Polea_salida_'+label,60 if label=='J2' else 48,'Y','34 mm' if label=='J2' else '-43 mm','0 mm','0 mm')
    print('Eslabones50/45mm y ejes/poleas de salida modelados.')
def phase16_codo():
    g=comp('02_Brazo');z='link1-j3_centre'
    for yy in ['-26 mm','21 mm']:
        c=yplate(g,'Oreja_codo_'+yy,'-12 mm','link1-12 mm','24 mm','24 mm',yy,'2 mm','Al6061');cut_y(c,'Asiento','0 mm','link1','16.1 mm',f'({yy})-0.1 mm','2.2 mm');bearing16(g,'688_J3_'+yy,'Y',yy,'0 mm','link1')
    c=piece(g,'Housing_J3_remoto_PLA','PLA')
    box(c,'Piso','-13 mm','-101 mm',f'({z})-14.4 mm','47 mm','39 mm','2 mm')
    for xx in ['-13 mm','32 mm']:box(c,'Pared',xx,'-101 mm',f'({z})-12.4 mm','2 mm','39 mm','26 mm')
    join_all(c,'Cuna_J3_una_pieza')
    servo16(g,'M3_STS3250_remoto','STS3250','Y','-65.4 mm','0 mm',z,side=1)
    plate=yplate(g,'Soporte_remoto_Al','-15 mm',f'({z})-16 mm','50 mm','32 mm','-56 mm','2 mm','Al6061');cut_y(plate,'Asiento625','0 mm',z,'16.1 mm','-56.1 mm','2.2 mm')
    for xx in ['-15 mm','33 mm']:box(plate,'Pestana_union',xx,'-56 mm',f'({z})-16 mm','2 mm','42 mm','2 mm','steel')
    join_all(plate,'Soporte_remoto_con_alas');bearing16(g,'625_entrada_J3','Y','-53 mm','0 mm',z,5,16,5,5)
    origin(g,'M3','0 mm','0 mm',z,'Y')
    p=comp('J3_Pinion');shaft16(p,'Eje_entrada_J3','Y','-61 mm','0 mm','0 mm','5 mm','28 mm');pulley16(p,'Polea16_J3',16,'Y','-43 mm','0 mm','0 mm','5 mm');coupler16(p,'Acople25T_J3_REF','Y','-65.4 mm','0 mm','0 mm',True);origin(p,'EJE',axis='Y')
    print('Motor del codo trasladado al hombro; correa en plano lateral opuesto aJ2.')
def rotary_z(parent,rotor,label,face,zdatum):
    servo16(parent,'M_'+label+'_STS3032','STS3032','Z',face)
    c=piece(parent,'Housing_'+label+'_PLA','PLA');box(c,'Piso','-8 mm','-8 mm',f'({face})-29.5 mm','27 mm','16 mm','2 mm')
    for yy in ['-8 mm','6 mm']:box(c,'Costado','-8 mm',yy,f'({face})-27.5 mm','27 mm','2 mm','27.5 mm')
    for zz in [f'({face})+6 mm',f'({face})+11 mm']:
        ring(c,'Porta625','Z',zz,'0 mm','0 mm','21 mm','16.1 mm','5 mm')
        for xx in ['-10.5 mm','8.5 mm']:box(c,'Columna',xx,'-4 mm',face,'2 mm','8 mm',f'({zz})-({face})')
        bearing16(parent,'625_'+label+zz,'Z',zz,'0 mm','0 mm',5,16,5,5)
    join_all(c,'Housing_soportes_continuos')
    shaft16(rotor,'Eje_'+label,'Z',f'({face})+5 mm-({zdatum})','0 mm','0 mm','5 mm',f'({zdatum})-({face})-5 mm')
    bush(rotor,'Acople25T_'+label+'_REF','Z',f'({face})-({zdatum})','0 mm','0 mm','12 mm','4.95 mm','5 mm')
    bush(rotor,'Brida_'+label,'Z','0 mm','0 mm','0 mm','26 mm','5 mm','2 mm')
def phase16_muneca():
    arm=comp('03_Antebrazo');turn=comp('04_Giro');wrist=comp('05_Muneca');tool=comp('06_Herramienta')
    rotary_z(arm,turn,'J4','link2-17 mm','link2')
    origin(arm,'J4','0 mm','0 mm','link2');origin(turn,'J4')
    # InclinarJ5: horquilla y motor lateral sobre placa giratoriaJ4.
    for yy in ['-16 mm','11 mm']:
        c=yplate(turn,'Horquilla_J5_'+yy,'-11 mm','0 mm','22 mm','wrist45+10 mm',yy,'2 mm','Al6061');cut_y(c,'Asiento625','0 mm','wrist45','16.1 mm',f'({yy})-0.1 mm','2.2 mm');bearing16(turn,'625_J5_'+yy,'Y',yy,'0 mm','wrist45',5,16,5,5)
    servo16(turn,'M5_ST3215','ST3215','Y','32 mm','0 mm','wrist45',side=-1)
    c=piece(turn,'Cuna_J5_PLA','PLA');box(c,'Piso','-15 mm','31 mm','wrist45-14.4 mm','51 mm','37 mm','2 mm')
    for xx in ['-15 mm','34 mm']:box(c,'Lateral',xx,'31 mm','wrist45-12.4 mm','2 mm','37 mm','26 mm')
    join_all(c,'Cuna_J5_continua')
    shaft16(wrist,'Eje_J5','Y','-18 mm','0 mm','0 mm','5 mm','44 mm')
    horn=bush(wrist,'Horn_J5_REF','Y','20 mm','0 mm','0 mm','18 mm','5.1 mm','12 mm');cut_y(horn,'Spline25T_REF','0 mm','0 mm','5.9 mm','27.6 mm','4.5 mm')
    for yy in ['-12 mm','10 mm']:newbox(wrist,'Ala_muneca_'+yy,'-12 mm',yy,'0 mm','24 mm','2 mm','wrist56-20 mm')
    bridge=newbox(wrist,'Puente_muneca','-12 mm','-12 mm','wrist56-22 mm','24 mm','24 mm','2 mm')
    box(bridge,'Paso_carcasa_mini','-6.2 mm','-6.5 mm','wrist56-22.1 mm','24 mm','13 mm','2.2 mm',cut=True)
    # Cara del mini a27.5mm: deja el cuerpo arriba deJ5 y dos apoyos externos.
    rotary_z(wrist,tool,'J6','wrist56-18 mm','wrist56')
    origin(turn,'J5','0 mm','0 mm','wrist45','Y');origin(wrist,'J5',axis='Y');origin(wrist,'J6','0 mm','0 mm','wrist56');origin(tool,'J6')
    origin(tool,'PINZA','tool_dx','tool_dy','2 mm')
    origin(comp('S_Pinza_base'),'HERRAMIENTA')
    print('Muneca3ejes y acople de herramienta modelados; comprobar recorridos y masas reales.')
def phase16_assembly():
    for child,co,parent,po,axis,angle in [('01_Plataforma','J1','00_Base','J1','Z','q1'),('02_Brazo','PROXIMAL','01_Plataforma','J2','Y','q2'),('03_Antebrazo','PROXIMAL','02_Brazo','DISTAL','Y','q3'),('04_Giro','J4','03_Antebrazo','J4','Z','q4'),('05_Muneca','J5','04_Giro','J5','Y','q5'),('06_Herramienta','J6','05_Muneca','J6','Z','q6')]:make_joint('V16_'+angle,child,co,parent,po,axis,angle)
    make_joint('V16_Pinza','S_Pinza_base','HERRAMIENTA','06_Herramienta','PINZA',kind='rigid')
    for name,parent,org,q in [('J2_Pinion','01_Plataforma','M2','q2'),('J3_Pinion','02_Brazo','M3','q3')]:
        j=make_joint('V16_'+name,name,'EJE',parent,org,'Y','3*'+q);motion_pair(j,R.joints.itemByName('V16_'+q),3,'Correa_3a1_'+q)
    for c in D.allComponents:
        c.isSketchFolderLightBulbOn=False;c.isConstructionFolderLightBulbOn=False
    adsk.core.Application.get().activeViewport.fit();print('Conjunto6ejes montado; conexiones comerciales y rango se revisan sobre esta geometria.')
def phase16_pinza():
    # El esquema de accionamiento de v15 se concreta: ojo terminal, cable
    # continuo abrazado al tambor y resorte de retorno, sin guias flotantes.
    g=comp('S_Pinza_base');low=comp('S_Biela0');lever=next(o.component for o in low.occurrences if o.component.name=='S_Palanca_tendon')
    ring(lever,'Ojo_terminal','Y','19.8 mm','0 mm','-15 mm','4 mm','2.2 mm','4.2 mm','steel');join_all(lever,'Palanca_y_ojo_terminal')
    cut_y(lever,'Paso_terminal_M2','0 mm','-15 mm','2.2 mm','15.9 mm','8.2 mm')
    bolt16(low,'Pin_terminal_M2','Y','15 mm','0 mm','-15 mm','2 mm','9 mm')
    rotor=comp('S_Tambor_directo');drum=next(o.component for o in rotor.occurrences if o.component.name=='S_Eje_y_tambor')
    drum.sketches.itemByName('Croquis_Tambor_R4').sketchDimensions.item(0).parameter.expression='7.2 mm'
    # Referencia fisica del cable en postura abierta. El mismo calculo de
    # tangencia de v15 gobierna su longitud; al cambiar apertura regenerar su
    # envolvente flexible, no transformarlo como eslabon rigido.
    th=val('grip_theta','rad');ax=val('tendon_ax')*10;az=val('tendon_az')*10;phi=val('tangent_angle','rad')
    tx=42+4*math.cos(phi);tz=-9+4*math.sin(phi)
    c=piece(g,'Cable_pinza_0p8_REF','Acero');points=[(ax,22,az),(tx,22,tz)]
    for i in range(1,66):
        a=phi+2.5*math.pi*i/65;points.append((42+4*math.cos(a),22+1.25*i/65,-9+4*math.sin(a)))
    pipe16(c,'Cable_con_abrazo_tambor',points,'0.8 mm')
    c.attributes.add('AFR2','spec','Cable acero0.8mm y terminal prensado: geometria actual abierta, verificar anclaje real y rotura/fatiga. No atribuir eficiencia al contacto sin terminal.')
    # Resorte extension especificado para fabricar/comprar por caracteristicas:
    # acero resorte d0.4/Dmedio4/N12, Lentreojos10.5, precarga0.4N.
    start=(10,18,-3);end=(ax,18,az);vec=[end[i]-start[i] for i in range(3)];length=math.sqrt(sum(v*v for v in vec));u=[v/length for v in vec];v=[0,1,0];w=[-u[2],0,u[0]]
    pts=[];body=max(4.8,length-5.7)
    # Patas se unen a los ojales; geometria helicoidal continua como referencia.
    pts.append(start)
    for i in range(193):
        s=i/192;a=24*math.pi*s;along=2.85+body*s
        pts.append(tuple(start[j]+along*u[j]+2*math.cos(a)*v[j]+2*math.sin(a)*w[j] for j in range(3)))
    pts.append(end);spring=piece(g,'Resorte_retorno_pinza_REF','Acero');pipe16(spring,'Resorte_helicoidal',pts,'0.4 mm')
    spring.attributes.add('AFR2','spec','Resorte especificado: alambre0.4mm, Dmedio4mm,12espiras activas, Lentreojos10.5mm, tension inicial0.4N, k0.329N/mm teorico. Material ASTM A228 requerido. Fatiga/tolerancias no ensayadas.')
    anchor=newbox(g,'Anclaje_resorte_Al','8 mm','15 mm','-5 mm','4 mm','6 mm','5 mm')
    cut_y(anchor,'Paso_gancho','10 mm','-3 mm','2.2 mm','14.9 mm','6.2 mm')
    bolt16(g,'Pin_resorte_M2','Y','14.5 mm','10 mm','-3 mm','2 mm','8 mm')
    # Terminales y tornillos de presa sobre tambor/ojal, separables para montaje.
    rotor=comp('S_Tambor_directo');endangle=phi+2.5*math.pi
    # Centro del terminal se expresa local al tambor en su posicion actual.
    terminal=piece(rotor,'Presa_cable_tambor','Acero');ring(terminal,'Presa_M2_ENVOLVENTE','Y','22.25 mm',f'{4*math.cos(endangle):.9f} mm',f'{4*math.sin(endangle):.9f} mm','2 mm',None,'2 mm','steel')
    print('Cable, ojo terminal y resorte de retorno incorporados; recorrido flexible y resistencia se comprueban sobre el ensamble.')
def phase16_corregir_mejilla():
    # En el documento importado Fusion agrega '(1)' a nombres repetidos.
    # Reconstruir solo la mejilla negativa cuyos tabs usaban el signo errado.
    g=comp('03_Antebrazo')
    old=next((o for o in g.occurrences if o.component.name.startswith('Mejilla_-1')),None)
    if old:old.deleteMe()
    yy='-19 mm';L='link2';c=yplate(g,'Antebrazo_mejilla_negativa','-14 mm','-14 mm','28 mm',L+'+28 mm',yy,'2 mm','Al6061')
    for zz in ['0 mm',L]:cut_y(c,'Eje','0 mm',zz,'8.1 mm','-19.1 mm','2.2 mm')
    box(c,'Ventana','-8 mm','-19.1 mm','14 mm','16 mm','2.2 mm','link2-28 mm',cut=True)
    for xx in ['-14 mm','12 mm']:box(c,'Borde',xx,yy,'0 mm','2 mm','6 mm',L,'steel')
    for zz in ['7 mm','link2-7 mm']:box(c,'Oreja','-24 mm',yy,f'({zz})-4 mm','12 mm','2 mm','8 mm','steel')
    join_all(c,'Mejilla_y_orejas_unidas')
    for zz in ['7 mm','link2-7 mm']:cut_y(c,'Paso_M3','-20 mm',zz,'3.2 mm','-19.1 mm','2.2 mm')
def phase16_conexiones():
    # Conectar los soportes de motores al padre con piezas continuas de Al,
    # y dejar apoyos en contacto de caras sin superponer material.
    g=comp('01_Plataforma');plate=next(o.component for o in g.occurrences if o.component.name=='Placa_portante_J2_Al')
    for pl in plate.constructionPlanes:
        if pl.name.startswith('Plano_Pestana_union'):pl.definition.offset.expression='2 mm'
    box(plate,'Pie_atornillado_plato','-20 mm','0 mm','2 mm','50 mm','30 mm','2 mm','steel')
    box(plate,'Brazo_bajo_motor','-j2_centre-13 mm','28 mm','shoulder_local-16.8 mm','20 mm','34 mm','2 mm','steel')
    box(plate,'Cama_motor','-j2_centre-17 mm','59 mm','shoulder_local-16.8 mm','56 mm','44 mm','2 mm','steel');join_all(plate,'Portante_J2_con_cama')
    rotorplate=next(o.component for o in g.occurrences if o.component.name=='Plato_giro_Al')
    for xx in ['-12 mm','12 mm']:
        for yy in ['8 mm','20 mm']:
            for c,zz in [(plate,'1.9 mm'),(rotorplate,'-0.1 mm')]:ring(c,'Paso_M3_union','Z',zz,xx,yy,'3.2 mm',None,'4.2 mm',cut=True)
            bolt16(g,'M3_plato_'+xx+yy,'Z','-2 mm',xx,yy,'3 mm','6 mm')
    # SoporteJ3: se conecta a cara exterior de mejilla negativa del humero.
    arm=comp('02_Brazo');sp=next(o.component for o in arm.occurrences if o.component.name=='Soporte_remoto_Al');z='link1-j3_centre'
    box(sp,'Cama_motor_remoto','-17 mm','-104 mm',f'({z})-16.8 mm','56 mm','50 mm','2 mm','steel')
    box(sp,'Union_a_mejilla','-14 mm','-18 mm',f'({z})-16 mm','28 mm','2 mm',f'28 mm-({z})','steel');join_all(sp,'Soporte_remoto_una_pieza')
    cut_y(sp,'Paso_eje_hombro','0 mm','0 mm','8.6 mm','-18.1 mm','2.2 mm')
    cheek=next(o.component for o in arm.occurrences if o.component.name.startswith('Mejilla_-1'))
    for xx in ['-8 mm','8 mm']:
        cut_y(sp,'Paso_M3',xx,'8 mm','3.2 mm','-18.1 mm','2.2 mm');cut_y(cheek,'Paso_union_remota',xx,'8 mm','3.2 mm','-16.1 mm','2.2 mm')
        bolt16(arm,'M3_soporte_remoto_'+xx,'Y','-20 mm',xx,'8 mm','3 mm','6 mm')
    # Piso de PLA separado0.44mm del motor; cama Al toca su cara inferior.
    for parent,name in [(g,'Cuna_J2_PLA'),(arm,'Housing_J3_remoto_PLA')]:
        cc=next(o.component for o in parent.occurrences if o.component.name==name)
        for pl in cc.constructionPlanes:
            if pl.name.startswith('Plano_Piso'):pl.definition.offset.expression='('+pl.definition.offset.expression+')-0.4 mm'
    # Cuna lateral de J5 con escuadra real fijada a la horquilla.
    turn=comp('04_Giro');br=piece(turn,'Escuadra_motor_J5_Al','Al6061')
    box(br,'Flange','-11 mm','13 mm','5 mm','22 mm','2 mm','15 mm','steel')
    box(br,'Cama_motor_J5','-17 mm','13 mm','wrist45-16.4 mm','56 mm','57 mm','2 mm','steel');join_all(br,'Escuadra_J5_una_pieza')
    fork=next(o.component for o in turn.occurrences if o.component.name=='Horquilla_J5_11 mm')
    for xx in ['-7 mm','7 mm']:
        for c,yy in [(fork,'10.9 mm'),(br,'12.9 mm')]:cut_y(c,'Paso_union',xx,'10 mm','3.2 mm',yy,'2.2 mm')
        bolt16(turn,'M3_escuadra_J5_'+xx,'Y','9 mm',xx,'10 mm','3 mm','6 mm')
    print('Camas de motor y escuadras conectadas al padre mediante caras y pasos de fijacion.')
def phase16_save():
    app=adsk.core.Application.get();doc=app.activeDocument
    if not doc.isSaved:doc.saveAs(NAME16,app.data.activeProject.rootFolder,'Brazo compacto6ejes con codo remoto. Revision mecanica en curso.','')
    elif doc.isModified:doc.save('Brazo compacto, revision de geometria y cargas en curso.')
    if not D.exportManager.execute(D.exportManager.createFusionArchiveExportOptions(str(HERE16/(NAME16+'.f3d')))):raise RuntimeError('Exportacion F3D fallida')
    print(json.dumps(dict(document=doc.name,saved=doc.isSaved,modified=doc.isModified,components=D.allComponents.count,joints=R.joints.count)))
def run(context):
    phase=globals()['PHASE16']
    with open(HERE16/'progreso_cad.jsonl','a',encoding='utf-8') as log:log.write(json.dumps(dict(phase=phase,state='started'))+'\n')
    if phase=='init':phase16_init()
    else:
        setup()
        if adsk.core.Application.get().activeDocument.name!=NAME16:raise RuntimeError('Activar solo v16')
        globals()['phase16_'+phase]()
    D.activateRootComponent();adsk.doEvents()
    with open(HERE16/'progreso_cad.jsonl','a',encoding='utf-8') as log:log.write(json.dumps(dict(phase=phase,state='finished'))+'\n')
    print('v16 fase '+phase)
