"""Dientes y masa de ruedas antes de ampliar el CAD. No acepta engranajes.

Lewis preliminar para20deg; K=1.5 y n=2 impuestos, fuerza de frenada de v14.
PLA FDM sin resistencia asignada. Areas de evolvente ideal integradas en radio;
no sustituyen volumen CAD ni masa del resto del reductor.
"""
from pathlib import Path
import math,json
import numpy as np
H=Path(__file__).resolve().parent
def wheel(n,m,width,bore):
    rp=m*n/2;rb=rp*math.cos(math.radians(20));ra=rp+m;rr=rp-1.25*m
    rs=np.linspace(rr,ra,4001);al=np.arccos(np.minimum(1,rb/rs));inv=np.tan(al)-al
    half=math.pi/(2*n)+math.tan(math.radians(20))-math.radians(20)-inv
    area=math.pi*rr*rr+n*np.trapezoid(2*half*rs,rs)-math.pi*bore*bore/4
    if n>=32:area-=4*math.pi*(m*n*.1)**2
    return area*width*.00124
def wheel_thin_web(n,m,width,bore):
    mass=wheel(n,m,width,bore)
    if n<32:return mass
    inner=m*(n/2-1.25-3);hub=8 if bore>=8 else 6
    area=math.pi*(inner*inner-hub*hub)-4*math.pi*(m*n*.1)**2
    return mass-max(0,area)*(width-2)*.00124
rows=[]
for name,module,n,b,width,torque in [('J2_v14_estrecho',.8,50,8,12,14.075537*.0980665),('J2_propuesto',1.25,50,8,12,14.075537*.0980665),('J3_propuesto',1.25,48,8,10,9.051567*.0980665),('J5_propuesto',.8,72,5,6,3.277426*.0980665)]:
    F=2*torque/(module*n*.001);Y=.484-2.87/n;stress=F/(width*module*Y)
    rows.append(dict(name=name,module_mm=module,output_teeth=n,face_mm=width,reference_output_Nm=torque,tangential_N=F,
        lewis_factor=Y,nominal_bending_MPa=stress,required_FDM_strength_MPa_factor2_K15=3*stress,output_wheel_mass_g_ideal=wheel(n,module,width,b),
        outside_diameter_mm=module*(n+2),mass_thin_web2mm_ideal_g=wheel_thin_web(n,module,width,b)))
train2=sum(wheel(n,1.25,w,b) for n,w,b in [(20,8,5),(40,8,5),(20,12,5),(50,12,8)])
train2web=sum(wheel_thin_web(n,1.25,w,b) for n,w,b in [(20,8,5),(40,8,5),(20,12,5),(50,12,8)])
data=dict(scope='Predimensionamiento de dientes sobre esfuerzos v14; falta recalcular con masas nuevas. Sin fatiga/contacto/root fillet/FDM aprobados.',
    rows=rows,J2_total_gears_g_ideal=train2,J2_total_gears_thin_web2mm_ideal_g=train2web,J2_motor_offset_mm=(20+40)*1.25/2+(20+50)*1.25/2,
    J2_intermediate_offset_mm=(20+50)*1.25/2,
    J2_note='Ruedas por si solas consumen/superan la reserva25g de transmision v14. Agregar eje, rodamientos, housing, tornillos y acople; no conservar1.17kg como masa final.')
(H/'transmisiones_resultados.json').write_text(json.dumps(data,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(data,ensure_ascii=False,indent=2))
