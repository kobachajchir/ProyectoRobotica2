"""Predimensionamiento de pinza con una mordaza fija y otra paralela movil.

No es ensayo ni aprobacion. Fuerzas por ambas tangencias del tendon a tambor,
para no escoger solo el angulo de traccion mas favorable.
"""
from pathlib import Path
import math,json
import numpy as np
H=Path(__file__).resolve().parent
rows=[]
for gap in np.linspace(49.5,60,106):
    theta=math.asin((gap-25+6-27)/36)
    pivot=np.array([27.,10.])
    anchor=pivot+np.array([-15*math.sin(theta),-15*math.cos(theta)])
    drum=np.array([42.,-9.]);delta=drum-anchor;distance=np.linalg.norm(delta)
    e=delta/distance;ep=np.array([-e[1],e[0]])
    tension=(1.5*.0980665)/.004*.8
    normals=[];lever=[]
    for sign in [-1,1]:
        u=math.sqrt(1-(4/distance)**2)*e+sign*(4/distance)*ep
        r=anchor-pivot
        arm=abs(r[0]*u[1]-r[1]*u[0])
        # Reserva de resorte de apertura equivalente a1N en la mordaza.
        normal=tension*arm/(36*math.cos(theta))-1
        normals.append(normal);lever.append(arm)
    rows.append(dict(gap_mm=float(gap),theta_deg=math.degrees(theta),normal_each_face_N=min(normals),normal_max_N=max(normals),lever_min_mm=min(lever),tendon_N=tension))
worst=min(x['normal_each_face_N'] for x in rows)
# Carga de la arquitectura v14 mas corta, recalcular al cerrar CAD de todo el brazo.
payload_force=3.4425575619572024
# Mordaza fija U: cara16x2 y dos alas8x2, altura efectiva45mm.
A=[32,16,16];xs=[-27,-30,-30];xc=sum(a*x for a,x in zip(A,xs))/sum(A)
I=16*2**3/12+32*(-27-xc)**2+2*(2*8**3/12+16*(-30-xc)**2)
W=I/max(abs(-34-xc),abs(-26-xc))
F=max(x['normal_max_N'] for x in rows)+payload_force # reaccion de la cara fija por fuerza normal de la pieza
length=55.0 # extremo superior del pad fijo alineado con centro45mm
sigma=F*length/W
data=dict(scope='Predimensionamiento de pinza nueva; no integra todavia masas/CAD al brazo ni aprueba accionamiento o agarre.',
    nominal_motor_kgfcm=1.5,drum_radius_mm=4,efficiency_assumed=.8,opening_spring_equivalent_N_each_face=1,
    minimum_normal_each_face_N=worst,mu_required_factor2=payload_force/math.sqrt(worst*worst-payload_force*payload_force),
    holding_margin_at_mu04=2*worst*.4/(payload_force*math.sqrt(1+.4**2)),tangential_only_margin_at_mu04=2*worst*.4/payload_force,payload_force_N_reference=payload_force,
    contact_scope='Fuerzas normales y tangenciales con orientacion arbitraria. No cierra momentos, presion distribuida o friccion local sobre los pads.',
    fixed_U=dict(I_mm4=I,W_mm3=W,normal_load_N=F,bending_stress_MPa=sigma,required_proof_MPa_FSn2_Kt2=4*sigma,required_proof_MPa_stall3x_FSn2_Kt2=12*sigma,
                 effective_length_mm=length,tip_deflection_mm=F*length**3/(3*70000*I),E_MPa_assumed=70000),
    rows=rows,remaining=['Curva real del servo bajo carga y frenado','Par de apertura/despegue y reserva del muelle','Presion/compresion y mu real de pads','Terminaciones y fatiga de tendon','Retencion, acople25T, rodamientos y soportes','Masa y colisiones CAD nuevas'])
# Cota conservadora combinada del eje: todos los esfuerzos radiales acotados
# al centro de la luz; suma de normales y cable. No reemplaza FEM de contactos.
alpha_max=6*math.radians(900)+15*math.radians(30)**2
moment_cube=(.2*.05**2/6)*alpha_max
theta_min=math.asin((49.5-25+6-27)/36)
grasp_z=10+math.sqrt(36**2-4**2)
my_bound=F*abs(grasp_z-45)*.001+payload_force*.006+moment_cube
upper_link_bound=my_bound/(.01*math.sin(theta_min))
radial_bound=rows[0]['tendon_N']+F+upper_link_bound
moment_bound=radial_bound*.081/4
torque_bound=rows[0]['tendon_N']*.015
shaft=[]
for dia in [4,6]:
    eff=(dia-.2-.5)*.001 # tolerancia propuesta de vastago y planoD0.5mm
    area=math.pi*eff*eff/4
    normal=32*moment_bound/(math.pi*eff**3)+radial_bound/area
    shear=16*torque_bound/(math.pi*eff**3)+4*radial_bound/(3*area)
    vm=math.sqrt(normal*normal+3*shear*shear)/1e6
    shaft.append(dict(diameter_mm=dia,mass_g=math.pi*dia*dia/4*88*.00785,
                      equivalent_MPa=vm,required_Rp_MPa_FSn2_Kt2=4*vm))
data['shaft_combined']=dict(radial_bound_N=radial_bound,bending_bound_Nm=moment_bound,torque_bound_Nm=torque_bound,
    angular_acceleration_bound_rad_s2=alpha_max,payload_moment_bound_Nm=moment_cube,rows=shaft,
    selected_diameter_mm=6,minimum_material_Rp_MPa=600,
    scope='Predimensionamiento conservador: flexion+axial+torsion+corte; luz81mm, carga radial concentrada al centro como cota. Material/ajustes/contacto/FDM pendientes.')
(H/'pinza_simple_resultados.json').write_text(json.dumps(data,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps({k:v for k,v in data.items() if k!='rows'},ensure_ascii=False,indent=2))
