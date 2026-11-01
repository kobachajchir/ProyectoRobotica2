"""Balance de traccion preliminar; no determina capacidad homologada de carga."""
from pathlib import Path
import json,math,csv,itertools
H=Path(__file__).resolve().parent
g=9.80665;r=.04;torque=.2941995
# La caida resistiva usa RDSon tipico de la pagina OSOYOO; la transferencia
# lineal par/tension es SOLO sensibilidad, no ley nominal continua del motor.
rows=[]
for mass,voltage,eta,slope,crr,rot in itertools.product(
    [2.8,3.9004327,7.319],[7.4,8.4,9.0],[.5,.6,.7,1.0],[0,5],[.03,.05,.10],[0,1,2]):
    vm=max(0,voltage-1.2*.49);theta=math.radians(slope);a=.5
    axis=4*torque/r*(vm/9)*eta;diagonal=axis/math.sqrt(2)
    required=mass*(a+g*math.sin(theta)+crr*g*math.cos(theta))+rot*a
    normal=mass*g*math.cos(theta)
    rows.append(dict(mass_kg=mass,battery_V=voltage,motor_V_estimated=vm,efficiency_assumed=eta,
        slope_deg=slope,Crr_assumed=crr,equivalent_rotating_mass_kg=rot,acceleration_m_s2=a,
        available_axis_N=axis,available_diagonal_N=diagonal,required_N=required,
        ratio_axis=axis/required,ratio_diagonal=diagonal/required,
        necessary_mu_axis=required*math.sqrt(2)/normal,necessary_mu_diagonal=required*2/normal))
with (H/'traccion_sensibilidad.csv').open('w',newline='',encoding='utf-8') as f:
    w=csv.DictWriter(f,fieldnames=list(rows[0]));w.writeheader();w.writerows(rows)
selected=[x for x in rows if x['battery_V']==7.4 and x['efficiency_assumed']==.6 and x['Crr_assumed']==.05 and x['equivalent_rotating_mass_kg']==1]
out=dict(scope='Hipotesis de piso y tension, no ensayos. Variante GM25-370-24140-75-14.5D10 pendiente de confirmar instalada.',
    source_motor='Documentacion/GM25-370-motor specification.pdf p1',
    source_driver='https://osoyoo.com/2022/02/25/osoyoo-model-y-4-channel-motor-driver/',
    rated_motor_Nm=torque,rated_motor_V=9,rated_motor_A_max=1.2,stall_motor_A_max=4.5,
    wheel_radius_m=r,ideal_9V_axis_N=4*torque/r,ideal_9V_diagonal_N=4*torque/r/math.sqrt(2),
    ideal_rated_speed_m_s=100*2*math.pi/60*r,selected=selected)
(H/'traccion_resumen.json').write_text(json.dumps(out,indent=2),encoding='utf-8')
for x in selected:print(round(x['mass_kg'],3),x['slope_deg'],'F',round(x['required_N'],2),'margen diagonal',round(x['ratio_diagonal'],2))
