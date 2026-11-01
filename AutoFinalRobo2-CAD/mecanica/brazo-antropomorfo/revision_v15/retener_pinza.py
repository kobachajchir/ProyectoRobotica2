"""Retenciones DIN6799 por catalogo Beneri; envolventes no fabricables de Eclips."""
from pathlib import Path
_workspace=Path(globals().get('AFR2_REPO_ROOT',r'D:\ProyectoRobotica2'))
_source=_workspace/'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v15/modelar_v15.py'
exec(compile(_source.read_text(encoding='utf-8'),str(_source),'exec'))
def clip(parent,name,y,z,d,root,od,mass):
    c=part(parent,name,'Acero',mass)
    ring(c,'Envolvente_instalada_NO_PERFIL_E','Y',y,'0 mm',z,str(od)+' mm',str(root+.1)+' mm','0.6 mm','steel')
    c.attributes.add('AFR2','catalog','Beneri DIN6799 nominal'+str(root)+'. Envolvente circular, sustituir por CAD comercial. No fabricar este anillo.')
    return c
def run(context):
    setup()
    if adsk.core.Application.get().activeDocument.name!=MODEL_NAME:raise RuntimeError('Documento incorrecto')
    # Separar tambien la punta interior del perno3mm del cubo:0.5mm nominal.
    D.userParameters.itemByName('grip_pivot').expression='27 mm'
    base=comp('S_Pinza_base');frame=next(o.component for o in base.occurrences if o.component.name=='S_Housing_servo_y_rodamientos')
    for o in base.occurrences:
        if o.component.name.startswith('S_Tornillo_M2p5'):
            # Longitud comercial12mm; la rosca ocupa toda la tuerca de2mm.
            n=o.component.name;xx='33.79 mm' if '33.79' in n else '61.79 mm';zz='-12.5 mm' if '-12.5' in n else '-5.5 mm'
            ring(o.component,'Extremo12mm','Y','63.9 mm',xx,zz,'2.5 mm',None,'2.1 mm','steel');join_all(o.component,'Tornillo_una_pieza')
            cut_y(frame,'Paso_roscado_hasta66',xx,zz,'2.7 mm','66 mm','0.2 mm')
    # Reemplazar las retenciones genericas por dimensiones del catalogo.
    p=comp('S_Biela0');sh=next(o.component for o in p.occurrences if o.component.name=='S_Eje_inferior4')
    for o in list(p.occurrences):
        if o.component.name.startswith('Retencion_eje4_'):o.deleteMe()
    for y,c in [(-42.35,-42.3),(41.65,41.7)]:
        ring(sh,'Ranura_DIN6799_3p2','Y',str(y)+' mm','0 mm','0 mm','4.2 mm','3.2 mm','0.7 mm',cut=True)
        clip(p,'Eclip3p2_'+str(c),str(c)+' mm','0 mm',4,3.2,7.3,.074)
    sh.attributes.add('AFR2','material_spec','Eje4mm con diarios redondos, planos localesD, acero Rp>=400MPa propuesto; no material comprado certificado.')
    # Pernos3mm: DIN6799 nominal2.3, ranura2.3/ancho0.7, anillo0.6.
    upper=comp('S_Biela1')
    for o in list(upper.occurrences):
        if o.component.name.startswith('S_Pasador_superior_'):o.deleteMe()
    jaw=comp('S_Mordaza_movil')
    for o in list(jaw.occurrences):
        if o.component.name.startswith('S_Perno_escalonado_'):o.deleteMe()
    sets=[(upper,'0 mm',False),(jaw,'10 mm',False),(jaw,'0 mm',True)]
    for parent,z,outer in sets:
        lo,hi=(33,44) if outer else (24.5,35)
        grooves=[34.2,42.0] if outer else [25.5,33.2]
        for sign in [-1,1]:
            start=lo if sign>0 else -hi
            c=shaft(parent,'Perno3_retenido_'+str(sign)+z,'0 mm',z,str(start)+' mm','3 mm',str(hi-lo)+' mm')
            for gy in grooves:
                begin=gy if sign>0 else -gy-.7
                ring(c,'Ranura_DIN6799_2p3','Y',str(begin)+' mm','0 mm',z,'3.2 mm','2.3 mm','0.7 mm',cut=True)
                cy=gy+.05 if sign>0 else -gy-.65
                clip(parent,'Eclip2p3_'+str(cy)+z,str(cy)+' mm',z,3,2.3,6.3,.060)
    # Espaciador del reductor entre caras; no invadir las dos ruedas.
    mid=comp('J2_Intermedio')
    for o in list(mid.occurrences):
        if o.component.name=='Hub_intermedio':o.deleteMe()
    spacer=part(mid,'Separador_intermedio_sin_solape','Al6061');ring(spacer,'Separador','Y','38 mm','0 mm','0 mm','10 mm','5.1 mm','4 mm','steel')
    # Los diarios de los tres ejes de J2 deben ser redondos. Llaves locales.
    for name,diam,start,length,keyzones in [('J2_Entrada',5,17,42,[(42,8),(56.1,2.9)]),('J2_Intermedio',5,17,42,[(26,24)]),('J2_Salida',8,10,54,[(26,12),(61,3)])]:
        pp=comp(name)
        for o in list(pp.occurrences):
            if o.component.name.startswith('Eje_solidario_con_ruedas'):o.deleteMe()
        c=shaft(pp,'Eje_diarios_redondos','0 mm','0 mm',str(start)+' mm',str(diam)+' mm',str(length)+' mm')
        for yy,ll in keyzones:box(c,'Plano_D_local',str(diam/2-.5)+' mm',str(yy)+' mm',str(-diam/2-1)+' mm','2 mm',str(ll)+' mm',str(diam+2)+' mm',cut=True)
    th=math.asin((60-25+6-27)/36);ax=27-15*math.sin(th);az=10-15*math.cos(th);dist=math.hypot(42-ax,-9-az)
    phi=math.pi+math.atan((az+9)/(ax-42))-math.acos(4/dist)
    D.userParameters.itemByName('simple_spool_angle').expression=f'(sqrt(tendon_dist^2-tendon_radius^2)-({math.sqrt(dist*dist-16)} mm))/tendon_radius*1 rad+tangent_angle-({phi} rad)'
    print('Retenciones DIN6799 y diarios redondos incorporados. Acoples25T y elementos flexibles siguen pendientes.')
