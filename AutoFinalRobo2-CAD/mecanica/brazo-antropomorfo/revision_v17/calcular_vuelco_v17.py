"""Vuelco con el CAD actual y AGV detenido; no sustituye apoyo real medido.

Centros inferidos del CAD del auto: X+-107,Y=-32+-62, pisoZ=-80mm.
Masa de auto1.5kg aportada por el usuario; CG horizontal centrado supuesto.
Se conservan por separado peso fijo y carga/movimiento del brazo.
"""
from pathlib import Path
import importlib.util,json,math,itertools
import numpy as np
H=Path(__file__).resolve().parent
sp=importlib.util.spec_from_file_location('c17',H/'calcular_cad_v17.py');c=importlib.util.module_from_spec(sp);sp.loader.exec_module(c)

def wrench_split(groups,q,dq,ddq):
    h=1e-4;f=c.frames(q);fp=c.frames(q+dq*h+.5*ddq*h*h);fm=c.frames(q-dq*h+.5*ddq*h*h)
    pivot=f['01_Giro_base'][:3,3];floor=np.array([0,-.032,-.08]);fixedF=np.zeros(3);fixedM=np.zeros(3);F=np.zeros(3);M=np.zeros(3)
    for g in groups:
      root=g['root'];A=f[root];R=A[:3,:3];p=np.r_[g['cg'],1];x=(A@p)[:3]
      acc=((fp[root]@p)[:3]-2*x+(fm[root]@p)[:3])/(h*h)
      Rd=(fp[root][:3,:3]-fm[root][:3,:3])/(2*h);Rdd=(fp[root][:3,:3]-2*R+fm[root][:3,:3])/(h*h)
      w=c.axial(Rd@R.T);alpha=c.axial(Rdd@R.T+Rd@Rd.T);I=R@g['I']@R.T
      force=g['mass_g']/1000*(acc-np.array([0,0,-c.s.G]));tor=I@alpha+np.cross(w,I@w)
      if root=='00_Base_control':fixedF+=force;fixedM+=np.cross(x-floor,force)+tor
      else:F+=force;M+=np.cross(x-pivot,force)+tor
    fixedF[2]+=1.5*c.s.G
    return F,M,fixedF,fixedM,pivot-floor

def main():
    if globals().get('GEOMETRY_PATH17'):c.GEOMETRY_PATH17=globals()['GEOMETRY_PATH17']
    raw,parts,groups=c.load()
    if globals().get('AJUSTAR_VUELCO17'):parts,groups=globals()['AJUSTAR_VUELCO17'](c,raw,parts,groups)
    yaw_values=list(range(-90,91,5));rows={a:dict(yaw_deg=a,peak_utilization=0,static_utilization=0,peak_cop_mm=[0,0]) for a in yaw_values}
    signs=list(itertools.product([-1,1],repeat=6))+[tuple(s if i==j else 0 for i in range(6)) for j in range(6) for s in [-1,1]]
    worst=None;count=0;states=0
    for q4,q5 in globals().get('WRIST_POSES17',list(itertools.product([-90,0,90],repeat=2))):
      end=np.radians([0,90,0,q4,q5,0])
      brake_ms=round(float(globals().get('BRAKE_SECONDS17',.05))*1000)
      cases=[(np.zeros(6),0,'estatico')]+[(np.radians(np.array(sign)*30),u,f'frenada{brake_ms}ms') for sign,u in itertools.product(signs,[0,.25,.5,.75,1])]
      for speed,u,kind in cases:
        tb=float(globals().get('BRAKE_SECONDS17',.05));q=end-speed*tb/2+speed*tb*(u-u**3+.5*u**4);dq=speed*(1-3*u*u+2*u**3);ddq=speed*(-6*u+6*u*u)/tb
        valid=True
        for j in range(1,6):
          lim=next(v for k,v in raw['joint_limits'].items() if k.startswith('J'+str(j+1)+'_'));start=end[j]-speed[j]*tb/2
          if (lim['min_enabled'] and min(start,end[j])<lim['min_rad']-1e-8) or (lim['max_enabled'] and max(start,end[j])>lim['max_rad']+1e-8):valid=False
        if not valid:continue
        F,M,F0,M0,r=wrench_split(groups,q,dq,ddq);states+=1
        for yaw in yaw_values:
          start=yaw-math.degrees(speed[0])*tb/2
          if min(start,yaw)<-90-1e-8 or max(start,yaw)>90+1e-8:continue
          angle=math.radians(yaw);cc=math.cos(angle);ss=math.sin(angle);R=np.array([[cc,-ss,0],[ss,cc,0],[0,0,1]])
          totalF=R@F+F0;totalM=R@M+np.cross(r,R@F)+M0
          if totalF[2]<=0:raise ValueError('Perdida de apoyo vertical en el caso muestreado')
          cop=np.array([-totalM[1],totalM[0]])/totalF[2]*1000;util=max(abs(cop[0])/107,abs(cop[1])/62)
          row=rows[yaw];count+=1
          if kind=='estatico':row['static_utilization']=max(row['static_utilization'],float(util))
          if util>row['peak_utilization']:row.update(peak_utilization=float(util),peak_cop_mm=cop.tolist())
          if worst is None or util>worst['utilization']:
            worst=dict(utilization=float(util),yaw_deg=yaw,wrist_deg=[q4,q5],type=kind,time_fraction=u,speed_before_deg_s=np.degrees(speed).tolist(),cop_relative_chassis_mm=cop.tolist(),vertical_force_N=float(totalF[2]),moment_at_ground_Nm=totalM.tolist(),min_axle_pair_N=float(min((totalF[2]-abs(totalM[0])/.062)/2,(totalF[2]-abs(totalM[1])/.107)/2)))
    out=dict(scope='CAD en curso,200g,AGV1.5kg centrado y detenido; apoyo rectangular inferido, no medido. Frenada cubica50ms desde30deg/s articulares; no velocidad nominal maxima del fabricante.',document=raw['document'],parameters=raw['parameters'],mass_arm_without_payload_g=sum(b['mass_g'] for b in parts)-200,contact_rectangle_mm=dict(center=[0,-32],half_width=107,half_length=62,floor_z=-80),worst=worst,by_yaw=list(rows.values()),safe_yaws_sampled_fs1_2=[a for a,row in rows.items() if row['peak_utilization']<=1/1.2],wrench_states=states,configurations=count,limitations=['Rectangulo de centros del CAD, no poligono real de rodillos','Masa auto aproximada; CG centrado supuesto','No aceleracion/frenada del chasis, pendiente, flexibilidad, deslizamiento ni reparto por rigidez de las cuatro ruedas','Muestreo de brazo extendido y9 munecas; no envolvente global','No autoriza operacion fisica ni acredita un interbloqueo de control implementado'])
    out['wrist_poses_deg']=globals().get('WRIST_POSES17','9 posturas generales')
    out['geometry_sha256']=raw['source_sha256']
    out['brake_seconds']=float(globals().get('BRAKE_SECONDS17',.05))
    out['calculation_parameters_internal']={n:c.s.P[n] for n in ['base_top','tower_h','link1','link2','wrist45','wrist56','v05_axis_y','j3_motor_z','j3_ratio','gear_ratio','gear_centers']}
    if globals().get('LAYOUT_NOTE17'):out['scope']='CONTRAFACTUAL: '+globals()['LAYOUT_NOTE17']
    (H/globals().get('OUT_FILE17','vuelco_cad_actual.json')).write_text(json.dumps(out,indent=2),encoding='utf-8')
    print(json.dumps({k:out[k] for k in ['mass_arm_without_payload_g','worst','safe_yaws_sampled_fs1_2','wrench_states','configurations']}))

if __name__=='__main__':main()
