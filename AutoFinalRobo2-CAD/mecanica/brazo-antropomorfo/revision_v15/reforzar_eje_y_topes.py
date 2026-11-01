"""Combinacion flexion/torsion: eje6 en lugar de4, pads alineados y tope ajustable."""
from pathlib import Path
_workspace=Path(globals().get('AFR2_REPO_ROOT',r'D:\ProyectoRobotica2'))
_source=_workspace/'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v15/modelar_v15.py'
exec(compile(_source.read_text(encoding='utf-8'),str(_source),'exec'))
def run(context):
    setup()
    if adsk.core.Application.get().activeDocument.name!=MODEL_NAME:raise RuntimeError('Documento incorrecto')
    base=comp('S_Pinza_base');anchor=next(o.component for o in base.occurrences if o.component.name=='S_Anclaje_paralelogramo')
    for yy in ['-41.5 mm','39.5 mm']:
        ring(anchor,'Boss_eje6','Y',yy,'grip_pivot','10 mm','14 mm',None,'2 mm');join_all(anchor,'Apoyo_eje6')
        cut_y(anchor,'Paso_eje6','grip_pivot','10 mm','6.2 mm',f'({yy})-0.1 mm','2.2 mm')
    p=comp('S_Biela0')
    for o in list(p.occurrences):
        if o.component.name=='S_Eje_inferior4' or o.component.name.startswith('Eclip3p2_'):o.deleteMe()
    sh=shaft(p,'S_Eje_inferior6','0 mm','0 mm','-44 mm','6 mm','88 mm')
    sh.attributes.add('AFR2','material_spec','Acero Rp>=600MPa requerido. Referencia: vastago liso de bulon8.8 certificado; medir diametro y longitud lisa antes de mecanizar. No es acero generico aprobado.')
    for yy,ll in [('15 mm','6 mm'),('-38.5 mm','4 mm'),('34.5 mm','4 mm')]:box(sh,'Plano_D_local','2.5 mm',yy,'-4 mm','2 mm',ll,'8 mm',cut=True)
    for yy,cy in [('-42.45 mm','-42.4 mm'),('41.65 mm','41.7 mm')]:
        ring(sh,'Ranura_DIN6799_4','Y',yy,'0 mm','0 mm','6.2 mm','4 mm','0.8 mm',cut=True)
        c=part(p,'Eclip4_'+cy,'Acero',.144);ring(c,'Envolvente_E_DIN6799_4','Y',cy,'0 mm','0 mm','9.3 mm','4.1 mm','0.7 mm','steel')
        c.attributes.add('AFR2','catalog','Beneri DIN6799 nominal4: eje5..7, groove4/m0.74, espesor0.7/OD9.3. Ranura0.8 propuesta; confirmar tolerancia del producto.')
    for o in p.occurrences:
        c=o.component
        if c.name.startswith('S_Biela_Al_'):
            yy='-37.5 mm' if '-28.5' in c.name else '35.5 mm';ww='2 mm'
        elif c.name=='S_Palanca_tendon':yy,ww='16 mm','4 mm'
        else:continue
        cut_y(c,'Interfaz_D6','0 mm','0 mm','6.1 mm',f'({yy})-0.1 mm',f'({ww})+0.2 mm')
        box(c,'Cara_D6','2.6 mm',yy,'-3.1 mm','1 mm',ww,'6.2 mm','steel');join_all(c,'Interfaz_eje6')
    # Centros de pads alineados con el CG del cubo en la postura de agarre50.
    if not D.userParameters.itemByName('grasp_joint_z'):D.userParameters.add('grasp_joint_z',V('10 mm+sqrt(grip_L^2-(25 mm+jaw_pad_offset-grip_pivot)^2)'),'mm','Altura de pivote distal con pieza50, independiente de apertura actual.')
    fixed=next(o.component for o in base.occurrences if o.component.name=='S_Pad_fijo');fixed.constructionPlanes.item(0).definition.offset.expression='35 mm'
    jaw=comp('S_Mordaza_movil');pad=next(o.component for o in jaw.occurrences if o.component.name=='S_Pad_movil')
    pad.constructionPlanes.item(0).definition.offset.expression='35 mm-grasp_joint_z'
    face=next(o.component for o in jaw.occurrences if o.component.name=='S_Mordaza_Al_plegada')
    box(face,'Respaldo_pad_bajo','-5 mm','-8 mm','34 mm-grasp_joint_z','2 mm','16 mm','grasp_joint_z-39 mm','steel');join_all(face,'Mordaza_con_respaldo_pad')
    # TopeM3 sin cabeza: limita cierre nominal cerca49.5mm, admite compresion
    # de pads antes de caer en la singularidad del paralelogramo a46mm.
    frame=next(o.component for o in base.occurrences if o.component.name=='S_Housing_servo_y_rodamientos')
    box(frame,'Boss_tope','30 mm','14 mm','-8 mm','6 mm','10 mm','6 mm');join_all(frame,'Housing_con_tope')
    # PlanoYZ tiene normal+X. Paso de tornillo y ventana de tuerca rectangular.
    def xhole(c,name,x,y,z,diam,depth,cut):
        ip=c.constructionPlanes.createInput();ip.setByOffset(c.yZConstructionPlane,V(x));pl=c.constructionPlanes.add(ip);pl.isLightBulbOn=False
        s=c.sketches.add(pl);circle(s,'-('+z+')',y,diam);f=extrude(c,s,depth,OP.CutFeatureOperation if cut else OP.NewBodyFeatureOperation,name)
        if not cut:
            for b in f.bodies:b.appearance=AP['steel']
    xhole(frame,'Paso_tope','29.9 mm','18 mm','-4.93 mm','3.2 mm','6.2 mm',True)
    box(frame,'Cajera_tuerca_tope','31.8 mm','14.75 mm','-7.78 mm','2.8 mm','6.5 mm','5.7 mm',cut=True)
    box(frame,'Acceso_tuerca_tope','31.8 mm','14.75 mm','-2.3 mm','2.8 mm','6.5 mm','0.4 mm',cut=True)
    c=part(base,'S_Tope_M3x8_sin_cabeza','Acero');xhole(c,'Tornillo_tope_ENVOLVENTE','28.04 mm','18 mm','-4.93 mm','3 mm','8 mm',False)
    c.attributes.add('AFR2','function','Tope ajustable. Ajustar cierre real>=49.5mm y limitar corriente. Cilindro representa rosca, no fabricacion.')
    c=part(base,'S_Tuerca_M3_tope','Acero');ip=c.constructionPlanes.createInput();ip.setByOffset(c.yZConstructionPlane,V('32 mm'));pl=c.constructionPlanes.add(ip);pl.isLightBulbOn=False
    s=c.sketches.add(pl);rr=.55/math.sqrt(3);pts=[P(.493-rr*math.sin(k*math.pi/3),1.8+rr*math.cos(k*math.pi/3),0) for k in range(6)]
    for k in range(6):s.sketchCurves.sketchLines.addByTwoPoints(pts[k],pts[(k+1)%6])
    f=extrude(c,s,'2.4 mm',OP.NewBodyFeatureOperation,'Tuerca_M3_tope');
    for b in f.bodies:b.appearance=AP['steel']
    xhole(c,'Rosca_ENVOLVENTE','31.9 mm','18 mm','-4.93 mm','3.2 mm','2.6 mm',True)
    print('Eje6, presion de pads centrada y tope ajustable incorporados; verificar alcance geometrico y recalcular masas.')
