"""Modulos compactos v15. Ejecutar dentro de Fusion mediante MCP.

No modifica v09. Croquis/extrusiones nativos; cotas propuestas identificadas.
Dentado evolvente aproximado por segmentos, no herramienta de fabricacion.
"""
from pathlib import Path
_workspace=Path(globals().get('AFR2_REPO_ROOT',r'D:\ProyectoRobotica2'))
_common=_workspace/'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/build_prototype.py'
exec(compile(_common.read_text(encoding='utf-8'),str(_common),'exec'))
ROOT_DIR=str(_workspace/'AutoFinalRobo2-CAD')
OUT_DIR=os.path.join(ROOT_DIR,'mecanica','brazo-antropomorfo')

MODEL_NAME='AFR2_Compacto_v15_Modulos_mecanicos'
HERE=os.path.join(OUT_DIR,'revision_v15')
PARAMS=[('link1','55 mm','mm'),('link2','50 mm','mm'),('al_t','2 mm','mm'),('lip','8 mm','mm'),
 ('small_al','1.5 mm','mm'),('housing_wall','2 mm','mm'),('servo_clear','0.4 mm','mm'),
 ('b608_id','8 mm','mm'),('b608_od','22 mm','mm'),('b608_w','7 mm','mm'),
 ('b625_id','5 mm','mm'),('b625_od','16 mm','mm'),('b625_w','5 mm','mm'),
 ('b623_id','3 mm','mm'),('b623_od','10 mm','mm'),('b623_w','4 mm','mm'),
 ('big_l','45.23 mm','mm'),('big_w','24.73 mm','mm'),('big_h','35 mm','mm'),('big_offset','12.5 mm','mm'),
 ('mini_l','23 mm','mm'),('mini_w','12 mm','mm'),('mini_h','27.5 mm','mm'),('mini_offset','5.71 mm','mm'),
 ('mini_total','31.9 mm','mm'),('mini_spline_h','3.4 mm','mm'),('mini_spline_d','4.95 mm','mm'),
 ('m2','0.8 mm','mm'),('m3','0.8 mm','mm'),('m5','0.6 mm','mm'),('grip_module','0.8 mm','mm'),
 ('g2_out_t','12 mm','mm'),('g2_in_t','8 mm','mm'),('g3_t','10 mm','mm'),('g5_t','6 mm','mm'),('gg_t','8 mm','mm'),
 ('grip_gap','60 mm','mm'),('grip_pivot','26.5 mm','mm'),('grip_L','36 mm','mm'),('jaw_t','4 mm','mm'),('pad_t','1 mm','mm'),
 ('jaw_pad_offset','6 mm','mm'),
 ('grip_theta','asin(((grip_gap/2+jaw_pad_offset)-grip_pivot)/grip_L)','deg'),
 ('q2','0 deg','deg'),('q3','0 deg','deg'),('q5','0 deg','deg')]

def part(parent,name,kind='PLA',catalog_mass=None):
    c=sub(parent,name);c.attributes.add('AFR2','material',kind)
    if catalog_mass is not None:c.attributes.add('AFR2','catalog_mass_g',str(catalog_mass))
    return c

def yplate(parent,name,x,z,w,h,y,t,kind='Al6061',target=None):
    c=target if target else part(parent,name,kind)
    # XZ sketch plane has +Y normal and sketch ordinate -Z.
    s=c.sketches.add(datum(c,'Y',y,'Plano_'+name));rect(s,x,'-('+str(z)+')-('+str(h)+')',w,h)
    f=extrude(c,s,t,OP.NewBodyFeatureOperation,name)
    for b in f.bodies:b.name=name;b.appearance=AP['steel'] if kind=='Al6061' else AP['red']
    return c

def cut_y(c,name,x,z,d,y,length):ring(c,name,'Y',y,x,z,d,None,length,cut=True)

def simple_bearing(parent,name,y,x,z,kind='625'):
    c=part(parent,name,'Rodamiento',{'608':12,'625':5,'623':1.8}[kind])
    p='b'+kind
    ring(c,'Rodamiento_cerrado_REF','Y',y,x,z,p+'_od',p+'_id',p+'_w','steel')
    return c

def shaft(parent,name,x,z,y,d,length):
    c=part(parent,name,'Acero');ring(c,'Eje_liso_REF','Y',y,x,z,d,None,length,'steel');return c

def motor_y(parent,name,mini,x,z,face):
    pre='mini' if mini else 'big';c=part(parent,name,'Servo',21.6 if mini else 56)
    # Mini carcasa girada180 alrededor de salida: extremo corto5.71mm.
    off='mini_offset' if mini else 'big_offset'
    box(c,'Carcasa_comercial',f'({x})-{off}',face,f'({z})-{pre}_w/2',pre+'_l',pre+'_h',pre+'_w','servo')
    boss='1 mm' if mini else '0.1 mm'
    ring(c,'Resalte','Y',f'({face})-({boss})',x,z,'8 mm' if mini else '12 mm',None,boss,'servo')
    ring(c,'Spline_envolvente_25T_NO_ESTRIADO','Y',f'({face})-({boss})-3.4 mm',x,z,'mini_spline_d' if mini else '5.9 mm',None,'3.4 mm','brass')
    # Pestañas y agujeros mini de plano p6; altura20.5..22 desde el fondo.
    if mini:
        for px in [f'({x})-8.21 mm',f'({x})+19.79 mm']:
            box(c,'Pestana_'+px,f'({px})-2 mm',f'({face})+mini_h-22 mm',f'({z})-mini_w/2','4 mm','1.5 mm','mini_w','servo')
            for sg in [-1,1]:cut_y(c,'Paso_M2p5_REF'+str(sg)+px,px,f'({z})+({sg})*3.5 mm','2.5 mm',f'({face})+mini_h-22.1 mm','1.7 mm')
        c.attributes.add('AFR2','mount_note','Centros transversales3.5mm inferidos del dibujo; verificar antes de fabricar. Carcasa23, orejas32, total31.9.')
    return c

def cradle_y(parent,name,mini,x,z,face,target=None):
    pre='mini' if mini else 'big';off='mini_offset' if mini else 'big_offset';c=target if target else part(parent,name)
    # Una cuna continua: piso, dos costados y frente unidos por solape.
    left=f'({x})-{off}-servo_clear-housing_wall';width=pre+'_l+2*servo_clear+2*housing_wall'
    box(c,'Piso',left,f'({face})-housing_wall',f'({z})-{pre}_w/2-servo_clear-housing_wall',width,pre+'_h+housing_wall', 'housing_wall')
    for xx in [left,f'({x})-{off}+{pre}_l+servo_clear']:
        box(c,'Costado_'+xx,xx,f'({face})-housing_wall',f'({z})-{pre}_w/2-servo_clear', 'housing_wall',pre+'_h+housing_wall',pre+'_w+servo_clear')
    yplate(parent,name+'_frente',left,f'({z})-{pre}_w/2-servo_clear-housing_wall',width,pre+'_w+2*servo_clear+housing_wall',f'({face})-housing_wall','housing_wall','PLA',c)
    join_all(c,'Cuna_una_pieza')
    cut_y(c,'Paso_salida',x,z,'14 mm',f'({face})-housing_wall-0.1 mm','housing_wall+0.2 mm')
    return c

def involute(parent,name,n,module,width,bore,y,phase=0):
    c=part(parent,name)
    ring(c,'Disco_raiz','Y',y,'0 mm','0 mm',f'{module}*({n}-2.5)',bore,width)
    rp=n/2;rb=rp*math.cos(math.radians(20));ra=rp+1;rr=rp-1.25
    invp=math.tan(math.radians(20))-math.radians(20)
    def a(r):
        t=math.acos(min(1,rb/r));return math.pi/(2*n)+invp-(math.tan(t)-t)
    pts=[(rr-.15,-a(rb))]+[(rb+(ra-rb)*i/6,-a(rb+(ra-rb)*i/6)) for i in range(7)]
    pts += [(rb+(ra-rb)*i/6,a(rb+(ra-rb)*i/6)) for i in reversed(range(7))]+[(rr-.15,a(rb))]
    s=c.sketches.add(datum(c,'Y',y,'Plano_diente'));s.isComputeDeferred=True
    coords=[(r*math.cos(t+phase),r*math.sin(t+phase)) for r,t in pts]
    lines=[]
    for i,(u,v) in enumerate(coords):
        u2,v2=coords[(i+1)%len(coords)]
        lines.append(s.sketchCurves.sketchLines.addByTwoPoints(P(val(f'{module}*({u})'),val(f'{module}*({v})'),0),P(val(f'{module}*({u2})'),val(f'{module}*({v2})'),0)))
    for constraint in list(s.geometricConstraints):
        if constraint.objectType in ['adsk::fusion::HorizontalConstraint','adsk::fusion::VerticalConstraint']:constraint.deleteMe()
    # Coordenadas ligadas al modulo; numero de dientes fijo por componente.
    for i,line in enumerate(lines):
        point_=line.startSketchPoint
        for coord,orientation in [(coords[i][0],ORI.HorizontalDimensionOrientation),(coords[i][1],ORI.VerticalDimensionOrientation)]:
            if point_.isFullyConstrained:break
            try:dim=s.sketchDimensions.addDistanceDimension(s.originPoint,point_,orientation,P(1,1,0))
            except RuntimeError as error:
                # El cierre del poligono puede fijar una coordenada por la
                # coincidencia del extremo. Se omite solo esa cota redundante.
                if 'OVER_CONSTRAINTS' not in str(error):raise
                continue
            dim.parameter.expression=f'{module}*({abs(coord)})'
    s.isComputeDeferred=False
    f=extrude(c,s,width,OP.JoinFeatureOperation,'Diente_evolvente_segmentado')
    es=adsk.core.ObjectCollection.create();es.add(f)
    ip=c.features.circularPatternFeatures.createInput(es,c.yConstructionAxis);ip.quantity=V(str(n));ip.totalAngle=V('360 deg')
    c.features.circularPatternFeatures.add(ip).name='Dentado_'+str(n)
    if n>=32:
        # Cuatro ventanas circulares, conservando aro y hub continuos.
        radius=f'{module}*{n}/4';hole=f'{module}*{n}*0.20'
        for k in range(4):cut_y(c,'Ventana'+str(k),f'({radius})*({math.cos(k*math.pi/2)})',f'({radius})*({math.sin(k*math.pi/2)})',hole,f'({y})-0.1 mm',f'({width})+0.2 mm')
    c.attributes.add('AFR2','gear','Evolvente20deg segmentada. Sin correccion de perfil, holgura/calibracion ni resistencia acreditadas. Dientes fijos; cambio de numero requiere regenerar.')
    return c

def link_module(name,L,offset):
    g=main(name).component
    for sign in [-1,1]:
        yy=f'({sign})*15 mm-al_t/2'
        c=yplate(g,'Mejilla_plegada_'+str(sign),'-14 mm','-14 mm','28 mm',f'{L}+28 mm',yy,'al_t')
        # Ventana central reduce masa sin separar los extremos de los ejes.
        cut_y(c,'Eje_proximal','0 mm','0 mm','8 mm',f'({yy})-0.1 mm','al_t+0.2 mm')
        cut_y(c,'Eje_distal','0 mm',L,'22.2 mm',f'({yy})-0.1 mm','al_t+0.2 mm')
        box(c,'Ventana_aligerado','-8 mm',f'({yy})-0.1 mm','14 mm','16 mm','al_t+0.2 mm',f'{L}-28 mm',cut=True)
        for sx in [-1,1]:box(c,'Borde_plegado_'+str(sx),f'({sx})*14 mm-al_t/2',f'({yy})-lip+al_t','0 mm','al_t','lip',L,'steel')
        join_all(c,'Mejilla_unica')
    for zz in ['7 mm',f'{L}-7 mm']:
        c=part(g,'Separador_'+zz,'Al6061');ring(c,'Separador','Y','-14 mm','0 mm',zz,'6 mm','3.2 mm','28 mm','steel')
        shaft(g,'Tornillo_M3_'+zz,'0 mm',zz,'-17 mm','3 mm','34 mm')
    origin(g,'ORIGEN');origin(g,'DISTAL','0 mm','0 mm',L,'Y')
    # Modulo separado de estudio, no colocacion final en el auto.
    o=next(o for o in R.occurrences if o.component==g);t=adsk.core.Matrix3D.create();t.translation=adsk.core.Vector3D.create(offset/10,0,0);o.transform2=t;o.isGrounded=True

def phase_init():
    global D,R,AP
    app=adsk.core.Application.get();app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
    D=adsk.fusion.Design.cast(app.activeProduct);R=D.rootComponent
    D.designIntent=adsk.fusion.DesignIntentTypes.HybridDesignIntentType;D.designType=adsk.fusion.DesignTypes.ParametricDesignType
    D.fusionUnitsManager.distanceDisplayUnits=adsk.fusion.DistanceUnits.MillimeterDistanceUnits
    app.activeDocument.name=MODEL_NAME;setup()
    for n,e,u in PARAMS:D.userParameters.add(n,V(e),u,'v15: propuesto salvo cotas de catalogo identificadas en informe.')
    R.attributes.add('AFR2','scope','Banco de modulos compactos. No ensamble final, no fabricacion aprobada. v09 conservado.')

def phase_links():
    link_module('L1_Al_55mm','link1',-150);link_module('L2_Al_50mm','link2',-100)

def phase_j2():
    g=main('J2_Carcasa_fija_5a1').component
    # Etapas20:40 y20:50; centros24 y28mm. Salida0, intermedio28, motor52.
    back=yplate(g,'Bastidor_continuo','-15 mm','-30 mm','110 mm','57 mm','13 mm','3 mm','PLA')
    for x in ['0 mm','28 mm','52 mm']:
        for y in ['18 mm','51 mm']:
            c=back
            od='27 mm' if x=='0 mm' else '20 mm';id='22.2 mm' if x=='0 mm' else '16.2 mm';ww='7 mm' if x=='0 mm' else '5 mm'
            ring(c,'Asiento','Y',y,x,'0 mm',od,id,ww)
            # Conexion continua con bastidor por puente inferior al dentado.
            box(c,'Pie',f'({x})-({od})/2','13 mm','-30 mm',od,f'({y})+({ww})-13 mm','6 mm')
            box(c,'Cuello',f'({x})-({od})/2',y,'-24 mm',od,ww,f'24 mm-({id})/2')
            simple_bearing(g,'Rodamiento_'+x+y,y,x,'0 mm','608' if x=='0 mm' else '625')
    motor_y(g,'Motor_ST3215_C047',False,'52 mm','0 mm','65 mm')
    cradle_y(g,'Housing_ST3215',False,'52 mm','0 mm','65 mm',back)
    # Union del housing con el bastidor: una pieza estructural, no pared suelta.
    c=back
    box(c,'Puente','75 mm','13 mm','-30 mm','16 mm','54 mm','17.3 mm')
    # Integrar todos los apoyos impresos con la cuna/bastidor.
    join_all(back,'Housing_y_bastidor_una_pieza')
    for x,d in [('0 mm','8.2 mm'),('28 mm','5.2 mm'),('52 mm','5.2 mm')]:cut_y(back,'Paso_eje_'+x,x,'0 mm',d,'12.9 mm','3.2 mm')
    origin(g,'ENTRADA','52 mm','0 mm','0 mm','Y');origin(g,'MEDIO','28 mm','0 mm','0 mm','Y');origin(g,'SALIDA',axis='Y')
    for name,definition in [('J2_Entrada',[(20,'m2','g2_in_t','5 mm','42 mm',0)]),('J2_Intermedio',[(40,'m2','g2_in_t','5 mm','42 mm',math.pi/40),(20,'m2','g2_out_t','5 mm','26 mm',0)]),('J2_Salida',[(50,'m2','g2_out_t','8 mm','26 mm',math.pi/50)])]:
        p=main(name).component
        for i,args in enumerate(definition):involute(p,name+'_'+str(i),*args)
        if name=='J2_Intermedio':
            hub=part(p,'Hub_intermedio','Al6061');ring(hub,'Union_dos_ruedas','Y','37.5 mm','0 mm','0 mm','10 mm','5 mm','5 mm','steel')
        shaft(p,'Eje_solidario_con_ruedas','0 mm','0 mm','10 mm' if name=='J2_Salida' else '17 mm','8 mm' if name=='J2_Salida' else '5 mm','54 mm' if name=='J2_Salida' else '42 mm')
        origin(p,'EJE',axis='Y')
    # Los ejes se muestran en sus asientos. Relaciones nativas de movimiento.
    occ('J2_Carcasa_fija_5a1').isGrounded=True
    js=[]
    for child,jo,angle in [('J2_Entrada','ENTRADA','5*q2'),('J2_Intermedio','MEDIO','-2.5*q2'),('J2_Salida','SALIDA','q2')]:js.append(make_joint(child,child,'EJE','J2_Carcasa_fija_5a1',jo,'Y',angle))
    for j,k,v in [(js[0],js[2],5),(js[1],js[2],-2.5)]:
        ip=R.motionLinks.createInput(j,k);ip.valueOne=V(str(abs(v))+' deg');ip.valueTwo=V('1 deg');ip.isReversed=v<0;R.motionLinks.add(ip)

def phase_save():
    app=adsk.core.Application.get();os.makedirs(HERE,exist_ok=True)
    app.activeViewport.fit();adsk.doEvents()
    doc=app.activeDocument
    if not doc.isSaved:doc.saveAs(MODEL_NAME,app.data.activeProject.rootFolder,'Modulos mecanicos compactos en desarrollo. Sin aprobacion fisica.','')
    elif doc.isModified:doc.save('Montajes compactos en desarrollo.')
    ok=D.exportManager.execute(D.exportManager.createFusionArchiveExportOptions(os.path.join(HERE,MODEL_NAME+'.f3d')))
    if not ok:raise RuntimeError('Fusion no exporto el archivo nativo')
    print(json.dumps({'document':doc.name,'saved':doc.isSaved,'modified':doc.isModified,'components':D.allComponents.count,'joints':R.joints.count,'parameters':D.userParameters.count}))

def motion_pair(one,two,ratio,name):
    ip=R.motionLinks.createInput(one,two);ip.valueOne=V(str(abs(ratio))+' deg');ip.valueTwo=V('1 deg');ip.isReversed=ratio<0
    R.motionLinks.add(ip).name=name

def phase_layout():
    # Ubicacion por uniones rigidas nativas: no capturas libres que el solver
    # pueda devolver al origen. No representa el ensamble final del brazo.
    bench=main('00_Banco_REF').component;occ('00_Banco_REF').isGrounded=True
    for i,(name,x) in enumerate([('L1_Al_55mm',-150),('L2_Al_50mm',-95),('J2_Carcasa_fija_5a1',0),('S_Pinza_base',180)]):
        par='banco_x'+str(i)
        if not D.userParameters.itemByName(par):D.userParameters.add(par,V(str(x)+' mm'),'mm','Solo presentacion de modulos separados, no cota funcional.')
        o=occ(name);o.isGrounded=False;o.isGroundToParent=False
        origin(o.component,'BANCO_ANCLA');origin(bench,'DESTINO'+str(i),par)
        make_joint('BANCO_'+str(i),name,'BANCO_ANCLA','00_Banco_REF','DESTINO'+str(i),kind='rigid')

def phase_keying():
    for group,diam in [('J2_Entrada',5),('J2_Intermedio',5),('J2_Salida',8)]:
        p=comp(group);flat=diam/2-.5
        for o in p.occurrences:
            c=o.component;n=c.name
            if n.startswith('Eje_solidario_con_ruedas'):
                box(c,'Plano_D',str(flat)+' mm','9 mm',str(-diam/2-1)+' mm','2 mm','56 mm',str(diam+2)+' mm',cut=True)
            elif n.startswith(group+'_') or n=='Hub_intermedio':
                # Las caras y anchos estan dados por la etapa, sin inferir caras.
                if group=='J2_Entrada':y,width='42 mm','g2_in_t'
                elif group=='J2_Salida':y,width='26 mm','g2_out_t'
                elif n=='Hub_intermedio':y,width='37.5 mm','5 mm'
                elif n.endswith('_0'):y,width='42 mm','g2_in_t'
                else:y,width='26 mm','g2_out_t'
                box(c,'Llave_plana_D',str(flat+.1)+' mm',y,str(-diam/2-.1)+' mm','1 mm',width,str(diam+.2)+' mm')
                join_all(c,'Rueda_con_interfaz_D')
    p=comp('J2_Entrada');c=part(p,'Acople25T_REF','Al6061')
    ring(c,'Hub','Y','56.1 mm','0 mm','0 mm','12 mm',None,'8.8 mm','steel')
    cut_y(c,'Bore_D5','0 mm','0 mm','5 mm','56 mm','3.1 mm')
    box(c,'Llave_D5','2.1 mm','56.1 mm','-2.6 mm','1 mm','3 mm','5.2 mm')
    join_all(c,'Acople_D')
    cut_y(c,'Spline25T_NO_FABRICACION','0 mm','0 mm','5.9 mm','61.5 mm','3.5 mm')
    c.attributes.add('AFR2','interface_status','25T es envolvente comercial. Seleccionar horn/acople real; este agujero liso no transmite torque por si solo.')

def phase_mounts():
    # Corregir pasos de las orejas mini y darles apoyo atornillado real.
    base=comp('S_Pinza_base');frame=next(o.component for o in base.occurrences if o.component.name=='S_Housing_servo_y_rodamientos')
    motor=next(o.component for o in base.occurrences if o.component.name=='S_STS3032_directo')
    for x in ['36.19 mm','59.19 mm']:box(motor,'Ligamento_oreja_carcasa',x,'56.1 mm','-15 mm','0.7 mm','1.5 mm','12 mm','servo')
    for center,left in [('33.79 mm','31.29 mm'),('61.79 mm','59.29 mm')]:
        box(frame,'Relieve_oreja',left,'55.7 mm','-15.4 mm','5.5 mm','2.3 mm','12.8 mm',cut=True)
        for zz in ['-12.5 mm','-5.5 mm']:
            box(frame,'Apoyo_M2p5',left,'58 mm',f'({zz})-2.5 mm','5 mm','4 mm','5 mm')
            cut_y(frame,'Paso_M2p5',center,zz,'2.7 mm','57.9 mm','4.2 mm')
            screw=part(base,'S_Tornillo_M2p5_'+center+zz,'Acero')
            ring(screw,'Vastago','Y','54 mm',center,zz,'2.5 mm',None,'10 mm','steel')
            ring(screw,'Cabeza','Y','52.2 mm',center,zz,'4.5 mm',None,'1.8 mm','steel')
    join_all(frame,'Housing_con_apoyos_orejas')
    # Plano D unicamente fuera de los asientos: conservar diarios redondos.
    rotor=comp('S_Tambor_directo')
    shaft_=next(o.component for o in rotor.occurrences if o.component.name=='S_Eje_y_tambor')
    box(shaft_,'Plano_D_solo_acople','2 mm','40.2 mm','-3 mm','2 mm','3.4 mm','6 mm',cut=True)
    adapter=next(o.component for o in rotor.occurrences if o.component.name=='S_Acople25T_REFERENCIA')
    box(adapter,'Llave_D5','2.1 mm','40.2 mm','-2.6 mm','1 mm','3.3 mm','5.2 mm','steel');join_all(adapter,'Acople_D')
    # La carcasa grande se retiene con tapa L desmontable, sin reutilizar
    # como montaje los tornillos autorroscantes de cierre de FEETECH.
    big=comp('J2_Carcasa_fija_5a1');c=next(o.component for o in big.occurrences if o.component.name=='Bastidor_continuo')
    box(c,'Extension_pie_tapa','37.1 mm','99 mm','-16.765 mm','50 mm','11.4 mm','2 mm')
    box(c,'Nervio_union_pie','37.1 mm','99 mm','-16.765 mm','50 mm','1 mm','4 mm')
    join_all(c,'Housing_con_retencion')
    cap=part(big,'J2_Tapa_reten_motor_L','Al5052')
    yplate(big,'Respaldo','37.1 mm','-14.765 mm','50 mm','28 mm','100.4 mm','small_al','Al5052',cap)
    box(cap,'Pie_L','37.1 mm','100.4 mm','-14.765 mm','50 mm','10 mm','small_al','steel')
    join_all(cap,'Tapa_L_una_pieza')
    # Ventana de acceso a conector/cableado, conservando marco.
    box(cap,'Ventana_cable','44 mm','100.3 mm','-8 mm','35 mm','small_al+0.2 mm','15 mm',cut=True)
    for xx in ['43.1 mm','81.1 mm']:
        ring(cap,'Paso_M3','Z','-14.865 mm',xx,'106.4 mm','3.2 mm',None,'small_al+0.2 mm',cut=True)
        ring(c,'Paso_M3','Z','-16.865 mm',xx,'106.4 mm','3.2 mm',None,'2.2 mm',cut=True)
        screw_z(big,'J2_Fijacion_tapa_'+xx,xx,'106.4 mm','-20 mm','3 mm','8 mm')
    comp('J2_Salida').description='Maqueta de empaquetado modulo0.8. No aprobar dientes PLA: redimensionar antes de uso.'

def phase_grip():
    """Pinza vigente: pared fija en U, un paralelogramo, accionamiento directo."""
    extra=[('jaw_pad_offset','6 mm','mm'),('tendon_radius','4 mm','mm'),('tendon_ax','grip_pivot-15 mm*sin(grip_theta)','mm'),('tendon_az','6 mm-15 mm*cos(grip_theta)','mm'),
      ('tendon_dist','sqrt((42 mm-tendon_ax)^2+(-9 mm-tendon_az)^2)','mm'),
      ('tangent_angle','180 deg+atan((tendon_az+9 mm)/(tendon_ax-42 mm))-acos(tendon_radius/tendon_dist)','deg')]
    for n,e,u in extra:
        if not D.userParameters.itemByName(n):D.userParameters.add(n,V(e),u,'v15 pinza asimetrica propuesta; tendon inextensible.')
    D.userParameters.itemByName('grip_theta').expression='asin((grip_gap-25 mm+jaw_pad_offset-grip_pivot)/grip_L)'
    # Referencia abierta calculada analiticamente, para una rama de tangencia.
    th=math.asin((60-25+6-26.5)/36);ax=26.5-15*math.sin(th);az=6-15*math.cos(th);dist=math.hypot(42-ax,-9-az)
    phi=math.pi+math.atan((az+9)/(ax-42))-math.acos(4/dist)
    expr=f'(sqrt(tendon_dist^2-tendon_radius^2)-({math.sqrt(dist*dist-16)} mm))/tendon_radius*1 rad+tangent_angle-({phi} rad)'
    if not D.userParameters.itemByName('simple_spool_angle'):D.userParameters.add('simple_spool_angle',V(expr),'deg','Longitud de tangente mas arco, respecto de apertura60mm; sin elasticidad.')
    g=main('S_Pinza_base').component
    palm=part(g,'S_Palma_y_mordaza_fija_U','Al5052')
    box(palm,'Palma','-36 mm','-31 mm','0 mm','67 mm','62 mm','small_al','steel')
    box(palm,'Ventana_palma','-18 mm','-20 mm','-0.1 mm','36 mm','40 mm','small_al+0.2 mm',cut=True)
    box(palm,'Cara_fija','-28 mm','-8 mm','0 mm','2 mm','16 mm','62 mm','steel')
    for yy in ['-10 mm','8 mm']:box(palm,'Ala_fija_'+yy,'-34 mm',yy,'0 mm','8 mm','2 mm','62 mm','steel')
    join_all(palm,'Palma_y_fija_una_pieza_plegada_REF')
    pad=part(g,'S_Pad_fijo','Elastomero');box(pad,'Pad','-26 mm','-8 mm','36.7 mm','1 mm','16 mm','20 mm','rubber')
    anchor=part(g,'S_Anclaje_paralelogramo')
    box(anchor,'Pie','grip_pivot-4 mm','-30 mm','small_al','8 mm','60 mm','2 mm')
    for yy in ['-28.5 mm','26.5 mm']:
        yplate(g,'Oreja','grip_pivot-4 mm','small_al','8 mm','18 mm-small_al',yy,'2 mm','PLA',anchor)
        for zz in ['6 mm','16 mm']:cut_y(anchor,'Paso_pivote_'+yy+zz,'grip_pivot',zz,'3.2 mm',f'({yy})-0.1 mm','2.2 mm')
    join_all(anchor,'Anclaje_integrado')
    for yy in ['-20 mm','20 mm']:
        ring(anchor,'Paso_M3','Z','small_al-0.1 mm','grip_pivot',yy,'3.2 mm',None,'2.2 mm',cut=True)
        ring(palm,'Paso_M3','Z','-0.1 mm','grip_pivot',yy,'3.2 mm',None,'small_al+0.2 mm',cut=True)
        screw_z(g,'S_Anclaje_M3_'+yy,'grip_pivot',yy,'-3 mm','3 mm','8 mm')
    for k,zz in [(0,'6 mm'),(1,'16 mm')]:
        p=main('S_Biela'+str(k)).component
        for yy in ['-28.5 mm','26.5 mm']:
            cc=yplate(p,'S_Biela_Al_'+yy,'-2.5 mm','-2.5 mm','5 mm','grip_L+5 mm',yy,'2 mm','Al5052')
            for z in ['0 mm','grip_L']:cut_y(cc,'Paso_'+z,'0 mm',z,'3.1 mm',f'({yy})-0.1 mm','2.2 mm')
        if k==0:
            shaft(p,'S_Pasador_proximal','0 mm','0 mm','-31 mm','3 mm','62 mm')
            lever=yplate(p,'S_Palanca_tendon','-2.5 mm','-15 mm','5 mm','18 mm','18 mm','2 mm','Al5052')
            cut_y(lever,'Anclaje_cable','0 mm','-15 mm','1 mm','17.9 mm','2.2 mm')
        else:
            for yy in ['-31 mm','25 mm']:shaft(p,'S_Pasador_corto_'+yy,'0 mm','0 mm',yy,'3 mm','6 mm')
        origin(p,'EJE',axis='Y');origin(p,'DISTAL','0 mm','0 mm','grip_L','Y');origin(g,'G'+str(k),'grip_pivot','0 mm',zz,'Y')
    jaw=main('S_Mordaza_movil').component;face=part(jaw,'S_Mordaza_Al_plegada','Al5052')
    box(face,'Cara','-5 mm','-28.5 mm','-5 mm','2 mm','57 mm','25 mm','steel')
    for yy in ['-28.5 mm','26.5 mm']:
        yplate(jaw,'Oreja','-5 mm','-5 mm','8 mm','20 mm',yy,'2 mm','Al5052',face)
        for z in ['0 mm','10 mm']:cut_y(face,'Paso_'+yy+z,'0 mm',z,'3.1 mm',f'({yy})-0.1 mm','2.2 mm')
    join_all(face,'Mordaza_integrada')
    for yy in ['-31 mm','25 mm']:
        for zz in ['0 mm','10 mm']:shaft(jaw,'S_Pasador_'+yy+zz,'0 mm',zz,yy,'3 mm','6 mm')
    pad=part(jaw,'S_Pad_movil','Elastomero');box(pad,'Pad','-6 mm','-8 mm','-3 mm','1 mm','16 mm','20 mm','rubber');origin(jaw,'EJE',axis='Y')
    frame=part(g,'S_Housing_servo_y_rodamientos')
    box(frame,'Piso','24 mm','20 mm','-20 mm','38 mm','59 mm','2 mm')
    for yy in ['27 mm','35 mm']:
        ring(frame,'Asiento625','Y',yy,'42 mm','-9 mm','20 mm','16.2 mm','5 mm')
        simple_bearing(g,'S_625_'+yy,yy,'42 mm','-9 mm')
    cradle_y(g,'S_Cuna',True,'42 mm','-9 mm','50.6 mm',frame)
    box(frame,'Union_cuna','59 mm','48.6 mm','-20 mm','3 mm','30 mm','5 mm')
    box(frame,'Pilar_fijacion','24 mm','20 mm','-19 mm','8 mm','11 mm','19 mm')
    join_all(frame,'Housing_y_apoyos_una_pieza')
    for yy in ['22 mm','28 mm']:
        ring(frame,'Paso_M3','Z','-20.1 mm','27.5 mm',yy,'3.2 mm',None,'20.2 mm',cut=True)
        ring(palm,'Paso_soporte','Z','-0.1 mm','27.5 mm',yy,'3.2 mm',None,'small_al+0.2 mm',cut=True)
        screw_z(g,'S_Fijacion_housing_'+yy,'27.5 mm',yy,'-22 mm','3 mm','25 mm')
    motor_y(g,'S_STS3032_directo',True,'42 mm','-9 mm','50.6 mm')
    rotor=main('S_Tambor_directo').component
    c=part(rotor,'S_Eje_y_tambor','Al6061')
    ring(c,'Eje','Y','19 mm','0 mm','0 mm','5 mm',None,'24.5 mm','steel')
    ring(c,'Tambor_R4','Y','20 mm','0 mm','0 mm','8 mm',None,'4 mm','steel')
    for yy in ['19 mm','24 mm']:ring(c,'Brida','Y',yy,'0 mm','0 mm','11 mm',None,'1 mm','steel')
    join_all(c,'Eje_tambor_integral')
    adapter=part(rotor,'S_Acople25T_REFERENCIA','Al6061')
    ring(adapter,'Hub','Y','40.2 mm','0 mm','0 mm','12 mm',None,'9.4 mm','steel')
    cut_y(adapter,'Alojamiento_D5_REF','0 mm','0 mm','5 mm','40.1 mm','3.5 mm')
    cut_y(adapter,'Interfaz25T_NO_ESTRIADA','0 mm','0 mm','4.95 mm','46.1 mm','3.6 mm')
    adapter.attributes.add('AFR2','interface_status','Interfaz25T comercial por seleccionar. No fabricar spline a partir de esta envolvente lisa.')
    origin(rotor,'EJE',axis='Y');origin(g,'DRUM','42 mm','0 mm','-9 mm','Y')
    ref=part(g,'S_Cubo_50mm_REFERENCIA','Carga',200);box(ref,'Cubo','-25 mm','-25 mm','20 mm','50 mm','50 mm','50 mm','part')
    # Tendon exacto en tangencia para referencia: no solido de cable fabricado.
    sk=g.sketches.add(datum(g,'Y','22 mm','Plano_tendon'))
    ax='tendon_ax';az='tendon_az';tx='42 mm+tendon_radius*cos(tangent_angle)';tz='-9 mm+tendon_radius*sin(tangent_angle)'
    line=sk.sketchCurves.sketchLines.addByTwoPoints(P(val(ax),-val(az),0),P(val(tx),-val(tz),0));locate(sk,line.startSketchPoint,ax,f'-({az})');locate(sk,line.endSketchPoint,tx,f'-({tz})');sk.name='Tendon_tramo_tangente_REF'
    go=occ('S_Pinza_base');tm=adsk.core.Matrix3D.create();tm.translation=adsk.core.Vector3D.create(15,0,0);go.transform2=tm;go.isGrounded=True
    lower=make_joint('S_G0','S_Biela0','EJE','S_Pinza_base','G0','Y','grip_theta')
    upper=make_joint('S_G1','S_Biela1','EJE','S_Pinza_base','G1','Y','grip_theta')
    jj=make_joint('S_G2','S_Mordaza_movil','EJE','S_Biela0','DISTAL','Y','-grip_theta')
    motion_pair(upper,lower,1,'S_Paralelogramo');motion_pair(jj,lower,-1,'S_Mordaza_paralela')
    make_joint('S_Motor','S_Tambor_directo','EJE','S_Pinza_base','DRUM','Y','simple_spool_angle')

def run(context):
    if globals().get('PHASE')=='init':phase_init()
    else:
        setup()
        if adsk.core.Application.get().activeDocument.name!=MODEL_NAME:raise RuntimeError('Activar solo el documento v15; no se modifica v09.')
        globals()['phase_'+PHASE]()
    adsk.doEvents();print('FASE v15 '+PHASE)

