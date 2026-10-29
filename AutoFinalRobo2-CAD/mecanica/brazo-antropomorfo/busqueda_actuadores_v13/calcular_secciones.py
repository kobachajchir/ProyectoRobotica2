"""Envolventes de seccion, no FEA ni resistencia acreditada de materiales.

Usa norma de momento como cota superior SIMULTANEA de flexion y torsion;
no atribuye a un estado real los maximos independientes de fuerza/momento.
"""
from pathlib import Path
import math,json
H=Path(__file__).resolve().parent
source=json.loads((H/'resultados_liviano.json').read_text(encoding='utf-8'))
result=[]
for row in source['rows']:
    if 'PWM' not in row['name']:continue
    F=row['peak_joint_force_N'];M=row['peak_joint_moment_Nm'];shafts=[];tubes=[];plates=[]
    for j,d in enumerate([8,8,8,6,5,5]):
        A=math.pi*d*d/4;Z=math.pi*d**3/32;Zp=2*Z
        sigma=F[j]/A+M[j]*1000/Z;tau=4*F[j]/(3*A)+M[j]*1000/Zp
        vm=math.sqrt(sigma*sigma+3*tau*tau)
        shafts.append(dict(joint=j+1,d_mm=d,envelope_vm_MPa=vm,required_Rp_n2_Kt2_MPa=4*vm,
            diameter_bending_torsion_only_for_Rp600_mm=(4*M[j]*1000*math.sqrt(32**2+3*16**2)/(math.pi*600))**(1/3)))
    for name,j,od,idd,L in [('PVC25',1,25,22,row['parameters_mm']['link1']),('PVC20',2,20,17,row['parameters_mm']['link2'])]:
        A=math.pi*(od*od-idd*idd)/4;I=math.pi*(od**4-idd**4)/64;J=2*I
        sigma=F[j]/A+M[j]*1000*(od/2)/I;tau=2*F[j]/A+M[j]*1000*(od/2)/J
        vm=math.sqrt(sigma*sigma+3*tau*tau)
        tubes.append(dict(name=name,od_mm=od,id_mm=idd,length_between_axes_mm=L,
            envelope_sigma_MPa=sigma,envelope_tau_MPa=tau,envelope_vm_MPa=vm,
            required_effective_strength_n2_Kt2_MPa=4*vm,
            limits='No incluye aplastamiento, taladros, creep, ovalizacion ni contacto con manguitos. PVC sin resistencia efectiva asignada.'))
    for name,j,w,t,h in [('Torre',0,14,4,40),('Hombro',1,17,4,45),('Codo',2,13,4,35),('HorquillaJ5',4,15,2.5,60)]:
        # Par de placas; se toma la mitad de F y M por placa, pero sin acreditar
        # reparto real. Flexion DEBIL de la placa, no fuerte ficticia por espesor.
        A=w*t;Z=w*t*t/6;sigma=F[j]/(2*A)+M[j]*1000/(2*Z)
        plates.append(dict(name=name,plate_count=2,net_width_mm=w,thickness_mm=t,
            nominal_weak_axis_stress_MPa=sigma,required_Rp_n2_Kt2_MPa=4*sigma,
            thickness_weak_axis_bending_only_Rp240_n2_Kt2_mm=math.sqrt(12*M[j]*1000/(w*240))))
    # Pinon24T modulo1, dos racks opuestos: suma fuerzas normales = eta*T/r.
    total_normal=.7*3.5*.0980665/.012
    bearings=[]
    for j,span in enumerate([10,34,40,10,32,9]):
        radial=F[j]/2+M[j]/(span*.001);axial=F[j]
        bearings.append(dict(joint=j+1,center_spacing_assumed_mm=span,radial_upper_N=radial,
            axial_upper_N=axial,required_C0_upper_n2_N=2*(radial+axial),
            limit='Cota conservadora Fr+Fa. Separacion y retencion axial propuestas, no montaje comprobado.'))
    base_stress=F[0]/(36*4)+M[0]*1000/(36*4*4/6)
    result.append(dict(name=row['name'],shafts=shafts,tubes=tubes,plates=plates,bearings=bearings,
        base_plate=dict(net_band_width_assumed_mm=36,thickness_mm=4,required_Rp_n2_Kt2_MPa=4*base_stress,
            limit='Requiere bandas continuas y ventanas fuera de anclajes. Acrilico, apriete y soporte de pies pendientes.'),
        gripper=dict(peak_payload_force_N=row['peak_payload_force_N'],MG92B_stall_budget_kgfcm=3.5,
            eta_assumed=.7,total_jaw_normal_upper_budget_N=total_normal,
            minimum_pad_mu_for_factor2=2*row['peak_payload_force_N']/total_normal,
            caveat='Par de bloqueo no continuo, sin precarga/friccion real, dedos/racks no verificados.')))
(H/'secciones_preliminares.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
for r in result:
    print(r['name'],'Rp ejes',[round(x['required_Rp_n2_Kt2_MPa']) for x in r['shafts']],
        'Rp placas',[round(x['required_Rp_n2_Kt2_MPa']) for x in r['plates']])
