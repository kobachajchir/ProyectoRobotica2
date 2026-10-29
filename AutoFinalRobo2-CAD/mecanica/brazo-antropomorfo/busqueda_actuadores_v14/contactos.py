"""ANTECEDENTE DESCARTADO: usuario exige trabajo solo sobre ruedas.

Distribucion CONSTRUCTIVA de fuerzas en cuatro pies de Coulomb.

Construye una solucion equilibrada; su mu es suficiente para esta solucion,
no el minimo global. No atribuye friccion isotropa a ruedas Mecanum.
"""
from pathlib import Path
import math,json
import numpy as np
H=Path(__file__).resolve().parent;V13=H.parent/'busqueda_actuadores_v13'
data=json.loads((V13/'resultados_liviano.json').read_text(encoding='utf-8'))
z=np.load(H.parents[2]/'exports'/'analisis-actuadores-v13'/'liviano.npz')
signs=np.array([[-1,-1],[-1,1],[1,-1],[1,1]])

def distribution(F,M,b):
    N=F[:,2];cop=np.c_[-M[:,1]/N,M[:,0]/N]
    normals=N[:,None]/4*np.prod(1+signs[None,:,:]*cop[:,None,:]/b,axis=2)
    rp=signs[None,:,:]*b-cop[:,None,:]
    D=np.sum(normals*np.sum(rp*rp,axis=2),axis=1)
    residual=M[:,2]-(cop[:,0]*F[:,1]-cop[:,1]*F[:,0])
    k=residual/D
    tang_per_n=F[:,None,:2]/N[:,None,None]+k[:,None,None]*np.stack([-rp[:,:,1],rp[:,:,0]],axis=2)
    forces=np.concatenate([normals[:,:,None]*tang_per_n,normals[:,:,None]],axis=2)
    mu=np.linalg.norm(tang_per_n,axis=2)
    forces_sum=forces.sum(axis=1)
    coords=np.c_[signs*b,np.zeros(4)]
    moments_sum=np.cross(coords[None,:,:],forces).sum(axis=1)
    # Residuales de equilibrio son evidencia de ESTE balance analitico,
    # no un test del software del repositorio ni una prueba de suelo real.
    return cop,normals,forces,mu,float(np.max(np.abs(forces_sum-F))),float(np.max(np.abs(moments_sum-M)))

rows=[]
for case in ['centrado_PWM_L90_75','centrado_PWM_L90_75_reserva25']:
    i=list(z['case_names']).index(case);row=next(x for x in data['rows'] if x['name']==case)
    qs=z['q_deg'][i];F0=z['F_moving'][i];M0=z['M_origin_moving'][i]
    group=next(x for x in row['groups'] if x['root']=='00');Ffix=np.array([0,0,group['mass_g']*.001*9.80665]);Mfix=np.cross(np.array(group['cg_local_mm'])*.001,Ffix)
    point=np.array([0,-.032,.060]);ground=np.array([0,-.032,-.080])
    for span in [110,140,160]:
        maximum_mu=0.;minN=1e9;maxF=np.zeros(3);maxnorm=0.;maxshear=0.;states=0;bad=0;errF=0.;errM=0.;witness=None
        for yaw in range(-90,91,10):
            a=math.radians(yaw);R=np.array([[math.cos(a),-math.sin(a),0],[math.sin(a),math.cos(a),0],[0,0,1]])
            valid=(qs[:,0]+yaw>=-90)&(qs[:,0]+yaw<=90)
            Fa=F0[valid]@R.T;Ma=(M0[valid]-np.cross(point,F0[valid]))@R.T+np.cross(point,Fa)
            F=Fa+Ffix;M=Ma+Mfix-np.cross(ground,F);F=F+np.array([0,0,1.5*9.80665])
            cop,n,ff,mu,eF,eM=distribution(F,M,span*.001)
            feasible=(F[:,2]>0)&(n.min(axis=1)>=0);bad+=int((~feasible).sum());states+=len(F)
            if not np.any(feasible):continue
            selected_mu=mu[feasible];arg=int(np.argmax(selected_mu));mval=float(selected_mu.flat[arg])
            if mval>maximum_mu:
                maximum_mu=mval;sid,fid=np.unravel_index(arg,selected_mu.shape);witness=dict(q_deg=(qs[valid][feasible][sid]+np.array([yaw,0,0,0,0,0])).tolist(),foot_index=int(fid),forces_N=ff[feasible][sid].tolist(),cop_mm=(cop[feasible][sid]*1000).tolist())
            minN=min(minN,float(n[feasible].min()));maxF=np.maximum(maxF,np.max(np.abs(ff[feasible]),axis=(0,1)))
            maxnorm=max(maxnorm,float(np.linalg.norm(ff[feasible],axis=2).max()));maxshear=max(maxshear,float(np.linalg.norm(ff[feasible,:,:2],axis=2).max()))
            errF=max(errF,eF);errM=max(errM,eM)
        rows.append(dict(case=case,span_between_feet_mm=span*2,states=states,negative_normal_states=bad,
            sufficient_mu_for_constructed_distribution=maximum_mu,minimum_foot_normal_N=minN,maximum_abs_components_per_foot_N=maxF.tolist(),
            maximum_foot_force_N=maxnorm,maximum_foot_shear_N=maxshear,equilibrium_residual_N=errF,equilibrium_residual_Nm=errM,witness=witness))
        print(case,span*2,'mu',round(maximum_mu,3),'Fmax',round(maxnorm,2),'Nmin',round(minN,2),'bad',bad,flush=True)
(H/'contactos_resultados.json').write_text(json.dumps(dict(scope='Cuatro pies isotropos idealizados; auto quieto, masa1.5kg/CG supuesto. Hereda masas v13 con reserva250g; no aprueba pies aun no diseñados ni suelo.',rows=rows),ensure_ascii=False,indent=2),encoding='utf-8')
