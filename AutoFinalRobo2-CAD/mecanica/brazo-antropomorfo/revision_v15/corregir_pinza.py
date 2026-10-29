"""Iteracion de interferencias v15: bielas escalonadas y pernos independientes."""
from pathlib import Path
_workspace=Path(globals().get('AFR2_REPO_ROOT',r'D:\ProyectoRobotica2'))
_source=_workspace/'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v15/modelar_v15.py'
exec(compile(_source.read_text(encoding='utf-8'),str(_source),'exec'))

def shift_planes(c,delta):
    for pl in c.constructionPlanes:
        par=pl.definition.offset;par.expression='('+par.expression+')+('+str(delta)+' mm)'

def run(context):
    setup()
    if adsk.core.Application.get().activeDocument.name!=MODEL_NAME:raise RuntimeError('Documento incorrecto')
    base=comp('S_Pinza_base');palm=next(o.component for o in base.occurrences if o.component.name=='S_Palma_y_mordaza_fija_U')
    frame=next(o.component for o in base.occurrences if o.component.name=='S_Housing_servo_y_rodamientos')
    for o in list(base.occurrences):
        if o.component.name.startswith(('S_Anclaje_M3','S_Fijacion_housing','S_Anclaje_paralelogramo')):o.deleteMe()
    # Cerrar perforaciones reemplazadas y abrir pasos que realmente se usan.
    for xx,yy in [('26.5 mm','-20 mm'),('26.5 mm','20 mm'),('27.5 mm','22 mm'),('27.5 mm','28 mm')]:
        ring(palm,'Cerrar_paso_anterior','Z','0 mm',xx,yy,'3.2 mm',None,'small_al','steel')
    join_all(palm,'Palma_sin_fijaciones_superpuestas')
    box(palm,'Ranura_palanca','19 mm','17.5 mm','-0.1 mm','11.5 mm','3 mm','small_al+0.2 mm',cut=True)
    anchor=part(base,'S_Anclaje_paralelogramo')
    box(anchor,'Pie','12 mm','-30 mm','small_al','20.5 mm','60 mm','2 mm')
    for yy in ['-28.5 mm','26.5 mm']:
        yplate(base,'Apoyo_interior','20.5 mm','small_al','12 mm','24 mm-small_al',yy,'2 mm','PLA',anchor)
        cut_y(anchor,'Paso_libre_eje4','grip_pivot','10 mm','6.5 mm',f'({yy})-0.1 mm','2.2 mm')
        cut_y(anchor,'Pivote_superior','grip_pivot','20 mm','3.2 mm',f'({yy})-0.1 mm','2.2 mm')
    for yy in ['-42.5 mm','27.5 mm']:box(anchor,'Puente_apoyo_exterior','21.5 mm',yy,'small_al','10 mm','15 mm','2 mm')
    for yy in ['-41.5 mm','39.5 mm']:
        yplate(base,'Apoyo_exterior','21.5 mm','3.5 mm','10 mm','11.5 mm',yy,'2 mm','PLA',anchor)
        cut_y(anchor,'Pivote_inferior','grip_pivot','10 mm','4.2 mm',f'({yy})-0.1 mm','2.2 mm')
    join_all(anchor,'Apoyos_escalonados_una_pieza')
    box(anchor,'Paso_palanca','19 mm','17.5 mm','small_al-0.1 mm','11.5 mm','3 mm','2.2 mm',cut=True)
    for yy in ['-20 mm','8 mm']:
        ring(anchor,'Paso_M3','Z','small_al-0.1 mm','20.5 mm',yy,'3.2 mm',None,'2.2 mm',cut=True)
        ring(palm,'Paso_M3','Z','-0.1 mm','20.5 mm',yy,'3.2 mm',None,'small_al+0.2 mm',cut=True)
        screw_z(base,'S_Anclaje_nuevo_'+yy,'20.5 mm',yy,'-3 mm','3 mm','8 mm')
    box(frame,'Puente_fijacion_palmar','-8 mm','24 mm','-3 mm','40 mm','8 mm','3 mm')
    join_all(frame,'Housing_con_puente_palmar')
    for xx in ['-4 mm','10 mm']:
        ring(frame,'Paso_nuevo_M3','Z','-3.1 mm',xx,'28 mm','3.2 mm',None,'3.2 mm',cut=True)
        ring(palm,'Paso_nuevo_M3','Z','-0.1 mm',xx,'28 mm','3.2 mm',None,'small_al+0.2 mm',cut=True)
        screw_z(base,'S_Housing_nuevo_'+xx,xx,'28 mm','-6 mm','3 mm','8 mm')
    # Pasos completos y rebaje de cabeza para cuatro fijaciones del mini.
    for xx in ['33.79 mm','61.79 mm']:
        for zz in ['-12.5 mm','-5.5 mm']:
            cut_y(frame,'Paso_completo_M2p5',xx,zz,'2.7 mm','53.9 mm','8.3 mm')
            cut_y(frame,'Rebaje_cabeza_M2p5',xx,zz,'4.8 mm','52.1 mm','2 mm')
    base.jointOrigins.itemByName('G0').offsetY.expression='-10 mm'
    base.jointOrigins.itemByName('G1').offsetY.expression='-20 mm'
    for k in [0,1]:
        p=comp('S_Biela'+str(k));distance=9 if k==0 else 4
        for o in p.occurrences:
            if o.component.name.startswith('S_Biela_Al_'):
                delta=-distance if '-28.5' in o.component.name else distance;shift_planes(o.component,delta)
                if k==0:
                    yy='-37.5 mm' if delta<0 else '35.5 mm';c=o.component
                    ring(c,'Boss_proximal','Y',yy,'0 mm','0 mm','12 mm',None,'2 mm','steel');join_all(c,'Biela_y_boss')
                    cut_y(c,'Paso_D4','0 mm','0 mm','4.1 mm',f'({yy})-0.1 mm','2.2 mm')
                    box(c,'Cara_D4','1.6 mm',yy,'-2.1 mm','1 mm','2 mm','4.2 mm','steel');join_all(c,'Interfaz_D4')
        for o in list(p.occurrences):
            if o.component.name.startswith(('S_Pasador_proximal','S_Pasador_corto_')):o.deleteMe()
        if k==0:
            sh=shaft(p,'S_Eje_inferior4','0 mm','0 mm','-44 mm','4 mm','88 mm')
            for yy in ['-37.5 mm','35.5 mm','18 mm']:box(sh,'Plano_D','1.5 mm',yy,'-3 mm','2 mm','2 mm','6 mm',cut=True)
            c=next(o.component for o in p.occurrences if o.component.name=='S_Palanca_tendon')
            ring(c,'Boss_palanca','Y','18 mm','0 mm','0 mm','12 mm',None,'2 mm','steel');join_all(c,'Palanca_y_boss')
            cut_y(c,'Paso_D4','0 mm','0 mm','4.1 mm','17.9 mm','2.2 mm')
            box(c,'Cara_D','1.6 mm','18 mm','-2.1 mm','1 mm','2 mm','4.2 mm','steel');join_all(c,'Palanca_D4')
        else:
            for yy in ['-34 mm','25 mm']:shaft(p,'S_Pasador_superior_'+yy,'0 mm','0 mm',yy,'3 mm','9 mm')
    jaw=comp('S_Mordaza_movil');face=next(o.component for o in jaw.occurrences if o.component.name=='S_Mordaza_Al_plegada')
    for yy in ['-41.5 mm','39.5 mm']:
        yplate(jaw,'Oreja_exterior','-5 mm','-5 mm','8 mm','10 mm',yy,'2 mm','Al5052',face)
        cut_y(face,'Paso_exterior','0 mm','0 mm','3.1 mm',f'({yy})-0.1 mm','2.2 mm')
    for yy in ['-41.5 mm','27.5 mm']:box(face,'Puente_oreja','-5 mm',yy,'-5 mm','2 mm','14 mm','2 mm','steel')
    join_all(face,'Mordaza_con_apoyos_independientes')
    for o in list(jaw.occurrences):
        if o.component.name.startswith('S_Pasador_'):o.deleteMe()
    for zz,starts in [('0 mm',['-43 mm','34 mm']),('10 mm',['-34 mm','25 mm'])]:
        for yy in starts:shaft(jaw,'S_Perno_escalonado_'+yy+zz,'0 mm',zz,yy,'3 mm','9 mm')
    # Ajustar recorrido exacto del tendon al nuevo eje inferior de10mm.
    D.userParameters.itemByName('tendon_az').expression='10 mm-15 mm*cos(grip_theta)'
    th=math.asin((60-25+6-26.5)/36);ax=26.5-15*math.sin(th);az=10-15*math.cos(th);dist=math.hypot(42-ax,-9-az)
    phi=math.pi+math.atan((az+9)/(ax-42))-math.acos(4/dist)
    D.userParameters.itemByName('simple_spool_angle').expression=f'(sqrt(tendon_dist^2-tendon_radius^2)-({math.sqrt(dist*dist-16)} mm))/tendon_radius*1 rad+tangent_angle-({phi} rad)'
    fixed=next(o.component for o in base.occurrences if o.component.name=='S_Pad_fijo')
    fixed.constructionPlanes.item(0).definition.offset.expression='40.7 mm'
    print('Pinza: ejes10/20, bielas en planos31.5/36.5, pernos separados y fijaciones recolocadas.')
