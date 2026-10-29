"""Ultimos pasos de tornillos y barrido de bielas; usar tras corregir_pinza."""
from pathlib import Path
_workspace=Path(globals().get('AFR2_REPO_ROOT',r'D:\ProyectoRobotica2'))
_source=_workspace/'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v15/modelar_v15.py'
exec(compile(_source.read_text(encoding='utf-8'),str(_source),'exec'))
def run(context):
    setup()
    if adsk.core.Application.get().activeDocument.name!=MODEL_NAME:raise RuntimeError('Documento incorrecto')
    base=comp('S_Pinza_base');frame=next(o.component for o in base.occurrences if o.component.name=='S_Housing_servo_y_rodamientos')
    palm=next(o.component for o in base.occurrences if o.component.name=='S_Palma_y_mordaza_fija_U')
    anchor=next(o.component for o in base.occurrences if o.component.name=='S_Anclaje_paralelogramo')
    # El extremo del tornillo tambien debe atravesar la pared de la cuna.
    for xx in ['33.79 mm','61.79 mm']:
        for zz in ['-12.5 mm','-5.5 mm']:
            cut_y(frame,'Paso_hasta_extremo',xx,zz,'2.7 mm','62 mm','2.2 mm')
            cut_y(frame,'Acceso_tuerca',xx,zz,'6.1 mm','64 mm','2.1 mm')
            nut=part(base,'Tuerca_M2p5_'+xx+zz,'Acero')
            s=nut.sketches.add(datum(nut,'Y','64 mm','Plano_tuerca'))
            vertices=[P(val(xx)+.5/math.sqrt(3)*math.cos(math.pi/6+k*math.pi/3),-val(zz)+.5/math.sqrt(3)*math.sin(math.pi/6+k*math.pi/3),0) for k in range(6)]
            for k in range(6):s.sketchCurves.sketchLines.addByTwoPoints(vertices[k],vertices[(k+1)%6])
            f=extrude(nut,s,'2 mm',OP.NewBodyFeatureOperation,'Tuerca_REF')
            for b in f.bodies:b.appearance=AP['steel']
            cut_y(nut,'Rosca_M2p5_ENVOLVENTE',xx,zz,'2.7 mm','63.9 mm','2.2 mm')
    # Separar cabeza de fijacion del pie del anclaje.
    for o in list(base.occurrences):
        if o.component.name=='S_Housing_nuevo_10 mm':o.deleteMe()
    for c,z,h in [(palm,'0 mm','small_al'),(frame,'-3 mm','3 mm')]:
        ring(c,'Cerrar_paso10','Z',z,'10 mm','28 mm','3.2 mm',None,h,'steel' if c==palm else 'red');join_all(c,'Paso_anterior_cerrado')
        ring(c,'Paso6','Z',f'({z})-0.1 mm','6 mm','28 mm','3.2 mm',None,f'({h})+0.2 mm',cut=True)
    screw_z(base,'S_Housing_nuevo_6 mm','6 mm','28 mm','-6 mm','3 mm','8 mm')
    # Llevar el puente de la mordaza al lado opuesto del barrido de las bielas.
    jaw=comp('S_Mordaza_movil');face=next(o.component for o in jaw.occurrences if o.component.name=='S_Mordaza_Al_plegada')
    for yy,h in [('-28.5 mm','20 mm'),('26.5 mm','20 mm'),('-41.5 mm','10 mm'),('39.5 mm','10 mm')]:
        box(face,'Refuerzo_extremo','2.5 mm',yy,'-5 mm','2.5 mm','2 mm',h,'steel')
    for yy in ['-41.5 mm','27.5 mm']:box(face,'Puente_fuera_del_barrido','2.5 mm',yy,'-5 mm','2 mm','14 mm','2 mm','steel')
    join_all(face,'Puente_y_orejas_integrados')
    for yy in ['-39 mm','29 mm']:box(face,'Retirar_puente_interferente','-5.1 mm',yy,'-5.1 mm','2.2 mm','10 mm','2.2 mm',cut=True)
    # Palanca4mm: mayor superficie de apoyo de la llaveD, sin aumentar motor.
    p=comp('S_Biela0');lever=next(o.component for o in p.occurrences if o.component.name=='S_Palanca_tendon')
    box(lever,'Espesor_adicional','-2.5 mm','16 mm','-15 mm','5 mm','2 mm','18 mm','steel')
    ring(lever,'Boss4mm','Y','16 mm','0 mm','0 mm','12 mm',None,'4 mm','steel')
    ring(lever,'Terminal_redondeado','Y','16 mm','0 mm','-15 mm','5 mm',None,'4 mm','steel');join_all(lever,'Palanca4mm')
    cut_y(lever,'Paso_D4_total','0 mm','0 mm','4.1 mm','15.9 mm','4.2 mm')
    box(lever,'Plano_D4_total','1.6 mm','16 mm','-2.1 mm','1 mm','4 mm','4.2 mm','steel');join_all(lever,'Palanca_D')
    cut_y(lever,'Paso_terminal','0 mm','-15 mm','1.2 mm','15.9 mm','4.2 mm')
    sh=next(o.component for o in p.occurrences if o.component.name=='S_Eje_inferior4')
    for yy,width in [('15 mm','6 mm'),('-38.5 mm','4 mm'),('34.5 mm','4 mm')]:box(sh,'Prolongar_plano_D','1.5 mm',yy,'-3 mm','2 mm',width,'6 mm',cut=True)
    for c,z,h in [(palm,'-0.1 mm','small_al+0.2 mm'),(anchor,'small_al-0.1 mm','2.2 mm')]:box(c,'Paso_palanca4mm','19 mm','15.5 mm',z,'11.5 mm','5 mm',h,cut=True)
    # Retenciones axiales de referencia, con ranuras fuera de los asientos.
    for yy,clip in [('-42.2 mm','-42.1 mm'),('41.7 mm','41.8 mm')]:
        ring(sh,'Ranura_retencion','Y',yy,'0 mm','0 mm','4.2 mm','3.6 mm','0.5 mm',cut=True)
        c=part(p,'Retencion_eje4_'+yy,'Acero');ring(c,'Circlip_ENVOLVENTE','Y',clip,'0 mm','0 mm','6 mm','3.7 mm','0.3 mm','steel')
    base.attributes.add('AFR2','state','Pinza asimetrica en revision. Cable, muelle, terminales y acople25T comercial pendientes. Retenciones simplificadas.')
    print('Palanca4, tornillos pasantes, tuercas y barrido de mordaza corregidos.')
