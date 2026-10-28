"""Comprobacion analitica v08 J1/uniones. No FEA ni aprobacion de material FDM.

Unidades internas de resistencia: N, mm, MPa. Cargas provienen de v08.
"""
from pathlib import Path
import json,csv,math,importlib.util
import numpy as np

HERE=Path(__file__).resolve().parent
RAW=json.loads((HERE/'secciones_CAD_v08.json').read_text(encoding='utf-8'))
P={n:v['value_internal']*10 for n,v in RAW['parameters'].items()}

def loops_from_edges(edges):
    remaining=[np.array(e)[:,:2] for e in edges];out=[]
    while remaining:
        chain=remaining.pop(0).tolist()
        while np.linalg.norm(np.array(chain[-1])-chain[0])>1e-4:
            if not remaining:raise ValueError('Contorno abierto')
            choices=[(float(np.linalg.norm(e[k]-chain[-1])),i,k) for i,e in enumerate(remaining) for k in [0,-1]]
            gap,i,k=min(choices)
            if gap>1e-4:raise ValueError('Aristas sin continuidad '+str(gap))
            e=remaining.pop(i)
            if k==-1:e=e[::-1]
            chain.extend(e[1:].tolist())
        out.append(np.array(chain[:-1]))
    return out

def inside(pt,poly):
    x,y=pt;ans=False
    for a,b in zip(poly,np.roll(poly,-1,axis=0)):
        if (a[1]>y)!=(b[1]>y) and x<(b[0]-a[0])*(y-a[1])/(b[1]-a[1])+a[0]:ans=not ans
    return ans

def clip(poly,rect):
    pp=poly.tolist()
    for axis,bound,sgn in [(0,rect[0],1),(0,rect[2],-1),(1,rect[1],1),(1,rect[3],-1)]:
        if not pp:break
        new=[]
        for a,b in zip(pp,pp[1:]+pp[:1]):
            ia=sgn*(a[axis]-bound)>=-1e-9;ib=sgn*(b[axis]-bound)>=-1e-9
            if ia:new.append(a)
            if ia!=ib:
                t=(bound-a[axis])/(b[axis]-a[axis]);new.append([a[k]+t*(b[k]-a[k]) for k in range(2)])
        pp=new
    return np.array(pp)

def integrals(poly):
    if len(poly)<3:return np.zeros(6)
    x,y=poly.T;u,v=np.roll(poly,-1,axis=0).T;c=x*v-u*y
    a=c.sum()/2
    if abs(a)<1e-10:return np.zeros(6)
    z=np.array([a,((x+u)*c).sum()/6,((y+v)*c).sum()/6,
        ((y*y+y*v+v*v)*c).sum()/12,((x*x+x*u+u*u)*c).sum()/12,
        ((2*x*y+x*v+u*y+2*u*v)*c).sum()/24])
    return z*(1 if a>0 else -1)

def section(row,roi=None):
    original=loops_from_edges(row['edges']);polys=[];vv=np.zeros(6)
    for i,poly in enumerate(original):
        sign=(-1)**sum(inside(poly[0],other) for j,other in enumerate(original) if i!=j)
        pp=clip(poly,roi) if roi else poly
        vv+=sign*integrals(pp)
        if len(pp)>2:polys.append(pp)
    A,sx,sy,ix,iy,ixy=vv
    if A<=0:raise ValueError('Area no positiva '+row['component']+' '+str(row['z_mm']))
    cx,cy=sx/A,sy/A
    return {'component':row['component'],'z_mm':row['z_mm'],'A_mm2':A,'cx_mm':cx,'cy_mm':cy,
      'Ix_mm4':ix-A*cy*cy,'Iy_mm4':iy-A*cx*cx,'Ixy_mm4':ixy-A*cx*cy,
      '_points':np.vstack(polys),'roi_mm':roi}

def stress(sec,F,M):
    xy=sec['_points']-np.array([sec['cx_mm'],sec['cy_mm']])
    coeff=np.linalg.solve(np.array([[sec['Iy_mm4'],sec['Ixy_mm4']],
                                   [sec['Ixy_mm4'],sec['Ix_mm4']]]),[-M[1],M[0]])
    vals=F[2]/sec['A_mm2']+xy@coeff
    return float(max(abs(vals.min()),abs(vals.max())))

def skew(v):
    x,y,z=v;return np.array([[0,-z,y],[z,0,-x],[-y,x,0]])

def frame_model(sections):
    # Variable-section fixed-foot beams; top connected by rigid bridge at Z66.
    # Ignore the outermost 3 mm of ribs to avoid counting servo posts/racks.
    E=1500.;nu=.38;G=E/(2*(1+nu));top=66.;base=12.1;beam=[]
    for sx,sy,roi in [(-1,-1,[-30,-32,-18,-20]),(-1,1,[-30,-4,-18,4]),
                      (1,-1,[18,-32,30,-20]),(1,1,[18,-4,30,4])]:
        ss=[section(z,roi) for z in sections if z['component']=='V05_Montura_baja_PETG' and 12<z['z_mm']<66]
        tip=ss[-1];cz=np.array([z['z_mm'] for z in ss]);C=np.zeros((6,6))
        def interp(key,z):return float(np.interp(z,cz,[q[key] for q in ss]))
        # Midpoint numerical integration of Castigliano flexibility, 160 segments.
        for z in np.linspace(base,top,161)[:-1]+(top-base)/320:
            dz=(top-base)/160;L=top-z;A=interp('A_mm2',z);Ix=interp('Ix_mm4',z);Iy=interp('Iy_mm4',z);Ixy=interp('Ixy_mm4',z)
            dx=tip['cx_mm']-interp('cx_mm',z);dy=tip['cy_mm']-interp('cy_mm',z)
            H=np.array([[-L,0,dx,0,-1,0],[0,-L,dy,1,0,0]])
            C+=H.T@np.linalg.inv([[Iy,Ixy],[Ixy,Ix]])@H*dz/E
            C[2,2]+=dz/(E*A)
            tors=np.array([-dy,dx,0,0,0,1.])
            # Torsion stiffness is only a sensitivity approximation for these
            # noncircular sections. It does not yield exact Saint-Venant stress.
            J=.5*(Ix+Iy);C+=np.outer(tors,tors)*dz/(G*J)
        r=np.array([tip['cx_mm'],tip['cy_mm']-P['v05_axis_y'],0.])
        B=np.block([[np.eye(3),-skew(r)],[np.zeros((3,3)),np.eye(3)]])
        beam.append({'name':f'columna_{sx}_{sy}','ss':ss,'tip':tip,'B':B,'K':np.linalg.inv(C)})
    K=sum((b['B'].T@b['K']@b['B'] for b in beam),np.zeros((6,6)))
    return beam,K

def anchor_loads(F,M,points,weights):
    # Elastic rigid-plate group; negative axial increments represent compression
    # to be carried by contact. No preload or unilateral-contact solution here.
    n=len(points);A=[];B=[]
    for x,y in points:
        A.append([1,y,-x]);B.append([[1,0,-y],[0,1,x]])
    A=np.array(A);B=np.array(B);w=np.array(weights)
    dn=np.linalg.solve(A.T@(w[:,None]*A),[F[2],M[0],M[1]])
    axial=w*(A@dn)
    K=sum((ww*bb.T@bb for ww,bb in zip(w,B)),np.zeros((3,3)))
    du=np.linalg.solve(K,[F[0],F[1],M[2]])
    shear=np.array([ww*(bb@du) for ww,bb in zip(w,B)])
    return axial,shear

def serial(v):
    if isinstance(v,np.ndarray):return v.tolist()
    if isinstance(v,np.generic):return v.item()
    if isinstance(v,dict):return {k:serial(x) for k,x in v.items() if not k.startswith('_')}
    if isinstance(v,list):return [serial(x) for x in v]
    return v

def write_csv(name,rows):
    if not rows:return
    fields=list(rows[0])
    with open(HERE/name,'w',encoding='utf-8-sig',newline='') as f:
        w=csv.DictWriter(f,fieldnames=fields);w.writeheader()
        for row in rows:w.writerow({k:json.dumps(serial(v),ensure_ascii=False) if isinstance(v,(dict,list,np.ndarray)) else v for k,v in row.items()})

def main():
    sections=[section(z) for z in RAW['sections']]
    write_csv('propiedades_secciones.csv',[serial(s) for s in sections])
    dyn=json.loads((HERE.parent/'revision_v08'/'frenadas_hipotesis_v08.json').read_text(encoding='utf-8'))
    static=json.loads((HERE.parent/'revision_v08'/'estatica_v08.json').read_text(encoding='utf-8'))
    jj=next(j for j in static['joints'] if j['joint']=='J1')
    states=[{'case':'estatica_200g','time_s':0,'q_deg':[90,90,0,0,0,0],
      'F':np.array(jj['gravity_force_N']),'M':np.array(jj['gravity_moment_Nm'])*1000}]
    for c in dyn['cases']:
        for s in c['snapshots']:
            j=next(j for j in s['joints'] if j['joint']=='J1')
            states.append({'case':c['name'],'time_s':s['time_s'],'q_deg':s['q_deg'],
              'F':-np.array(j['force_N']),'M':-np.array(j['moment_Nm'])*1000})
    # F,M are the moving assembly's load on stationary support, at J1 Z96.
    source_states=[s for s in states if not s['case'].endswith('_600deg_s')]
    # Original v08 approached the +90deg endpoint in positive J1 direction.
    # Add its opposite direction with the SAME rigid dynamics, ending at 0deg.
    spec=importlib.util.spec_from_file_location('j1_v08_dynamics',HERE.parent/'revision_v08'/'calcular_v08.py')
    module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
    parts,_=module.load()
    for tb in [.2,.1,.05]:
        q_end=np.radians([0,90,0,0,0,0]);speed=np.zeros(6);speed[0]=-math.radians(30)
        for t in np.linspace(0,tb,21):
            s=t/tb;q=q_end-speed*tb/2+speed*tb*(s-s**3+.5*s**4)
            dq=speed*(1-3*s*s+2*s**3);ddq=speed*(-6*s+6*s*s)/tb
            j=next(j for j in module.state_at(parts,q,dq,ddq) if j['joint']=='J1')
            source_states.append({'case':f'J1_-1_{tb*1000:.0f}ms_30deg_s','time_s':float(t),
              'q_deg':np.degrees(q).tolist(),'F':-np.array(j['force_N']),'M':-np.array(j['moment_Nm'])*1000})
    # Rigid rotation about the vertical J1 axis preserves gravity/dynamics of
    # each source case. Stationary motor/electronics and anchor geometry do not
    # rotate. Discard a WHOLE rotated case if its J1 path exceeds native limits.
    groups={}
    for st in source_states:groups.setdefault(st['case'],[]).append(st)
    primary=[];rejected=[]
    for name,gg in groups.items():
        end_yaw=gg[-1]['q_deg'][0]
        for target in range(-90,91,10):
            delta=target-end_yaw
            if min(s['q_deg'][0]+delta for s in gg)<-90-1e-7 or max(s['q_deg'][0]+delta for s in gg)>90+1e-7:
                rejected.append({'case':name,'yaw_end_deg':target});continue
            a=math.radians(delta);rot=np.array([[math.cos(a),-math.sin(a),0],[math.sin(a),math.cos(a),0],[0,0,1]])
            for st in gg:
                q=list(st['q_deg']);q[0]+=delta
                primary.append(dict(st,q_deg=q,F=rot@st['F'],M=rot@st['M'],yaw_end_deg=target))
    write_csv('estados_carga.csv',[serial(st) for st in primary])
    beam,K=frame_model(RAW['sections']);housing=[];bearings=[];shaft=[];frame=[];posts=[];anchors=[];coupling=[]
    pts=np.array([[P['agv_'+n+'_x'],P['agv_'+n+'_y']] for n in 'ABCDEF']+[[P['agv_center_x'],P['agv_center_y']]])
    fixed=next(g for g in static['groups'] if g['name']=='00_Base_control')
    fixedF=np.array([0,0,-fixed['mass_g']/1000*9.80665]);fixedM=np.cross(fixed['com_mm'],fixedF)
    boltAt=np.array([5.03]*6+[58.0]);boltOD=np.array([7.]*6+[30.]);boltID=np.array([3.4]*6+[10.5])
    post_geometry=[]
    xx=[P['mg_l']/2-P['std_shaft_x']+sgn*(P['mg_l']/2+P['std_ear']/2) for sgn in [-1,1]]
    top=P['v05_motor_face']-P['std_ear_z']
    for x in xx:
        roi=[x-P['std_ear']/2,P['v05_axis_y']-P['mg_w']/2,x+P['std_ear']/2,P['v05_axis_y']+P['mg_w']/2]
        post_geometry.append([section(row,roi) for row in RAW['sections'] if row['component']=='V05_Montura_baja_PETG' and 12<row['z_mm']<top])
    residuals=[];frame_residuals=[]
    for st in primary:
        F,M=st['F'],st['M'];tag={'case':st['case'],'time_s':st['time_s'],'yaw_end_deg':st['yaw_end_deg']}
        # Radial bearing forces ON SHAFT = opposite of load on housing.
        FF=F;MM=M;za,zb=77.5-96,85.5-96
        rx=np.linalg.solve([[1,1],[za,zb]],[-FF[0],-MM[1]])
        ry=np.linalg.solve([[1,1],[za,zb]],[-FF[1],MM[0]])
        R=[np.array([rx[i],ry[i],0]) for i in [0,1]]
        residuals.append({'case':st['case'],'time_s':st['time_s'],
          'radial_force_N':float(np.linalg.norm(FF[:2]+R[0][:2]+R[1][:2])),
          'radial_moment_Nmm':float(np.linalg.norm((MM+np.cross([0,0,za],R[0])+np.cross([0,0,zb],R[1]))[:2]))})
        for i,r in enumerate(R):
            fr=float(np.linalg.norm(r));fa=abs(float(F[2])) # full axial allocated conservatively to either bearing
            p0=max(fr,.6*fr+.5*fa)
            bearings.append(dict(tag,bearing=['inferior','superior'][i],Fr_N=fr,Fa_bound_N=fa,
              P0_N=p0,C0_reference_N=1370,s0_reference=1370/p0,
              projected_pressure_MPa=fr/(22*7),reaction_on_shaft_N=r))
        # Shaft normal stress at native sections; torsion circular surrogate
        # is reported separately because D/hole geometry requires 3D resolution.
        q1=math.radians(st['q_deg'][0]);rot=np.array([[math.cos(q1),-math.sin(q1),0],[math.sin(q1),math.cos(q1),0],[0,0,1]])
        for sec in [q for q in sections if q['component']=='V03_E00_Husillo_base_completo']:
            z=sec['z_mm'];fc=FF.copy();mc=MM+np.cross([0,0,-z],FF)
            for zz,rr in zip([za,zb],R):
                if zz>z:fc+=rr;mc+=np.cross([0,0,zz-z],rr)
            fl=rot.T@fc;ml=rot.T@mc
            sigma=stress(sec,fl,ml);tau=abs(M[2])*16/(math.pi*8**3)
            vm=math.sqrt(sigma*sigma+3*tau*tau)
            shaft.append(dict(tag,z_mm=z,normal_stress_nominal_MPa=sigma,
              torsion_circle8_surrogate_MPa=tau,vm_surrogate_MPa=vm,
              yield_required_n2_Kt2_MPa=4*vm))
        # Cartucho bears radial/axial/bending; servo posts carry drive Mz.
        # Do not route full motor torque through freely turning 608 outer rings.
        capM=M+np.cross([0,0,30.],F);capM[2]=0
        u=np.linalg.solve(K,np.r_[F,capM])
        frame_residuals.append(float(np.linalg.norm(K@u-np.r_[F,capM])))
        for b in beam:
            f=b['K']@(b['B']@u);peak=0.;peakz=0
            for ss in b['ss']:
                dv=np.array([b['tip']['cx_mm']-ss['cx_mm'],b['tip']['cy_mm']-ss['cy_mm'],66-ss['z_mm']])
                mom=f[3:]+np.cross(dv,f[:3]);sig=stress(ss,f[:3],mom)
                if sig>peak:peak=sig;peakz=ss['z_mm']
            frame.append(dict(tag,column=b['name'],sigma_nominal_MPa=peak,z_mm=peakz,
              required_strength_n2_Kt2_MPa=4*peak,cap_translation_mm=u[:3],cap_rotation_deg=np.degrees(u[3:]),tip_force_moment=f))
        # Servo posts: conservative free-end cantilever, pure drive torque couple.
        Ft=abs(M[2])/(xx[1]-xx[0])
        for i,x in enumerate(xx):
            maxsig=0.;peakz=0.
            for sec in post_geometry[i]:
                fp=np.array([0,(-1 if i==0 else 1)*M[2]/(xx[1]-xx[0]),-.055*9.80665/2])
                mp=np.cross([x-sec['cx_mm'],P['v05_axis_y']-sec['cy_mm'],top-sec['z_mm']],fp)
                sig=stress(sec,fp,mp)
                if sig>maxsig:maxsig=sig;peakz=sec['z_mm']
            posts.append(dict(tag,post=['izquierdo','derecho'][i],shear_per_screw_N=Ft,
              sigma_post_nominal_MPa=maxsig,z_mm=peakz,required_strength_n2_Kt2_MPa=4*maxsig,
              screw_shear_thread_MPa=Ft/5.03,insert_transverse_projected_MPa=Ft/(4.5*4)))
        # Foundation combines arm load and fixed hardware weight, referred to Z0.
        Fg=F+fixedF;Mg=M+np.cross([0,P['v05_axis_y'],P['base_top']],F)+fixedM
        for model,weights in [('rigidez_igual',[1]*7),('rigidez_At_sobre_L',boltAt/np.array([19]*6+[14.]))]:
            axial,sh=anchor_loads(Fg,Mg,pts,weights)
            for i,n in enumerate(list('ABCDEF')+['M10']):
                tension=max(0,float(axial[i]));V=float(np.linalg.norm(sh[i]));washerA=math.pi*(boltOD[i]**2-boltID[i]**2)/4
                anchors.append(dict(tag,model=model,bolt=n,normal_increment_N=float(axial[i]),
                  service_tension_N=tension,shear_N=V,screw_vm_service_MPa=math.sqrt((tension/boltAt[i])**2+3*(V/boltAt[i])**2),
                  washer_pressure_service_MPa=tension/washerA,
                  washer_pressure_with100N_preload_MPa=(tension+100)/washerA))
        a=math.sqrt(4**2-3.3**2);h=8.8;T=abs(M[2])
        coupling.append(dict(tag,torque_Nmm=T,half_chord_mm=a,engagement_mm=h,
          D_flat_pressure_minimum_average_MPa=T/(h*a*a),
          D_flat_pressure_uniform_half_MPa=2*T/(h*a*a),
          D_flat_pressure_triangular_peak_MPa=3*T/(h*a*a),
          proposed_flange4M3_R25_shear_per_bolt_N=T/(4*25),
          proposed_flange_bearing_PETG_t5_5_MPa=T/(4*25*3*5.5),
          bearing_cap_axial_per_screw_bound_N=abs(F[2])/2,
          cap_insert_average_pullout_shear_required_MPa=(abs(F[2])/2)/(math.pi*4.5*4)))
    # Preserve extrema by case/yaw/physical item instead of duplicating every
    # output at all21 times. Full input wrenches remain in estados_carga.csv.
    def retain_extrema(rows,keys,metrics):
        groups={}
        for row in rows:
            key=tuple(row.get(k) for k in ['case','yaw_end_deg']+keys)
            groups.setdefault(key,[]).append(row)
        out=[]
        for gg in groups.values():
            keep={}
            for metric in metrics:
                row=max(gg,key=lambda x:x[metric]);keep.setdefault(id(row),[row,[]])[1].append(metric)
            for row,why in keep.values():out.append(dict(row,retained_peak_metrics=why))
        return out
    for name,rows,keys,metrics in [
      ('rodamientos',bearings,['bearing'],['P0_N']),
      ('eje',shaft,['z_mm'],['normal_stress_nominal_MPa','vm_surrogate_MPa']),
      ('columnas',frame,['column'],['sigma_nominal_MPa']),
      ('postes_servo',posts,['post'],['sigma_post_nominal_MPa']),
      ('anclajes',anchors,['model','bolt'],['service_tension_N','screw_vm_service_MPa']),
      ('acople_D',coupling,[],['D_flat_pressure_uniform_half_MPa','bearing_cap_axial_per_screw_bound_N'])]:
        write_csv(name+'.csv',retain_extrema(rows,keys,metrics))
    def peak(rows,key):return max(rows,key=lambda x:x[key])
    summary={'scope':'J1 v08, estatica/frenadas individuales30gr/s, J1 ambos sentidos y19 orientaciones finales cada10gr. Secciones, barras y fijaciones; no FEA de solidos/contactos ni aprobacion FDM.',
      'sources_geometry':'secciones_CAD_v08.json','states':len(primary),'source_states':len(source_states),
      'source_cases':len(groups),'yaw_end_deg':list(range(-90,91,10)),
      'cases_excluded_by_native_J1_limits':rejected,
      'assumptions':{'PETG_E_MPa':1500,'PETG_poisson':.38,'FDM_strength_assigned':False,
        'material_strength_multiplier_for_n2_Kt2':4,'bearing_C0_reference_N':1370,
        'frame_torsion_J_approx':'0.5*(Ix+Iy), solo sensibilidad de rigidez; no tension exacta',
        'rigid_bridge_z_mm':66,'fixed_feet_z_mm':12.1,
        'shaft_torsion_warning':'circulo8 de referencia; D y agujero no resueltos en torsion',
        'anchor_warning':'placa rigida y fuerzas incrementales; sin precarga/contacto unilateral/prying',
        'D_contact_warning':'sin friccion, media cara activa y reparto ideal; no contacto FEA ni rotura demostrada'},
      'max_bearing_load':peak(bearings,'P0_N'),'max_shaft_nominal':peak(shaft,'normal_stress_nominal_MPa'),
      'max_frame_nominal':peak(frame,'sigma_nominal_MPa'),'max_servo_post_nominal':peak(posts,'sigma_post_nominal_MPa'),
      'max_anchor_tension':peak(anchors,'service_tension_N'),'max_anchor_vm_service':peak(anchors,'screw_vm_service_MPa'),
      'max_D_contact':peak(coupling,'D_flat_pressure_uniform_half_MPa')}
    # Separation sensitivity at same load, not a proposed assembled geometry.
    bmax=summary['max_bearing_load'];worst=next(s for s in primary if s['case']==bmax['case'] and s['time_s']==bmax['time_s'] and s['yaw_end_deg']==bmax['yaw_end_deg'])
    summary['bearing_spacing_sensitivity']=[{'centres_mm':d,'couple_radial_N':float(np.linalg.norm(worst['M'][:2]))/d,
      'nominal_projected_pressure_MPa':float(np.linalg.norm(worst['M'][:2]))/(d*22*7)} for d in [8,16,24,32]]
    summary['numerical_equilibrium_residuals']={'radial_force_max_N':max(r['radial_force_N'] for r in residuals),
      'radial_moment_max_Nmm':max(r['radial_moment_Nmm'] for r in residuals),
      'frame_generalized_max_mixed_N_Nmm':max(frame_residuals)}
    ends=[s for s in primary if s['case']!='estatica_200g' and s['time_s']==float(s['case'].split('_')[2].replace('ms',''))/1000]
    statics={s['yaw_end_deg']:s for s in primary if s['case']=='estatica_200g'}
    summary['numerical_equilibrium_residuals']['zero_speed_endpoint_vs_static_max_N']=max(float(np.linalg.norm(s['F']-statics[s['yaw_end_deg']]['F'])) for s in ends)
    summary['numerical_equilibrium_residuals']['zero_speed_endpoint_vs_static_max_Nmm']=max(float(np.linalg.norm(s['M']-statics[s['yaw_end_deg']]['M'])) for s in ends)
    summary['anchor_model_maxima']={m:serial(peak([a for a in anchors if a['model']==m],'service_tension_N')) for m in ['rigidez_igual','rigidez_At_sobre_L']}
    summary['max_cap_axial_bound']=serial(peak(coupling,'bearing_cap_axial_per_screw_bound_N'))
    T=summary['max_D_contact']['torque_Nmm']
    # Proposed flange group, not native CAD: 4M3 on PCD50 at +/-45 degrees.
    flangepts=np.array([[25*math.cos(math.radians(a)),25*math.sin(math.radians(a))] for a in [45,135,225,315]])
    flangerows=[]
    for st in primary:
        angle=math.radians(st['q_deg'][0]);rz=np.array([[math.cos(angle),-math.sin(angle)],[math.sin(angle),math.cos(angle)]])
        ax,sh=anchor_loads(st['F'],st['M'],flangepts@rz.T,[1]*4)
        flangerows.append({'case':st['case'],'time_s':st['time_s'],'yaw_end_deg':st['yaw_end_deg'],
          'max_tension_N':float(max(0,max(ax))),
          'max_shear_N':float(max(np.linalg.norm(sh,axis=1))),
          'max_hole_bearing_MPa':float(max(np.linalg.norm(sh,axis=1)))/(3*5.5)})
    max_flange_tension=peak(flangerows,'max_tension_N')['max_tension_N']
    flange_bending=[]
    for hub_radius in [5,10]:
        sector_width=2*math.pi*hub_radius/4;lever=25-hub_radius
        sigma=max_flange_tension*lever/(sector_width*3**2/6)
        flange_bending.append({'hub_radius_mm':hub_radius,'sector_width_at_root_mm':sector_width,
          'cantilever_span_mm':lever,'sector_bending_MPa':sigma,'yield_required_n2_Kt2_MPa':4*sigma})
    summary['proposed_flange_no_CAD']={'PCD_mm':50,'plate_OD_mm':60,'metal_thickness_mm':3,'hub_OD_proposed_mm':20,
      'PETG_effective_thickness_mm':5.5,'bolts':'4 M3 pasantes, arandelas/tuercas y limitadores de compresion',
      'loadcase_max_tension':peak(flangerows,'max_tension_N'),
      'loadcase_max_shear':peak(flangerows,'max_shear_N'),
      'announced_63kgfcm_stall_shear_per_bolt_N':63*.0980665*1000/(4*25),
      'flange_four_sector_bending_sensitivity':flange_bending,
      'warning':'Propuesta dimensional; union metal/eje, placa a flexion, encastres, holguras y montaje no comprobados.'}
    summary['motor_sizing']={'J1_MG946R_announced_kgfcm_at6V':13,
      'case_peak_required_kgfcm':T/1000/.0980665,
      'preliminary_n2_peak_requirement_kgfcm':2*T/1000/.0980665,
      'same30deg_s_singleaxis_ideal_stop_ms_at_announced_limit':50*(T/1000/.0980665)/13,
      'same30deg_s_singleaxis_ideal_stop_ms_n2':100*(T/1000/.0980665)/13,
      'GX3270BLS_candidate_announced_kgfcm_at7_4V':63,
      'GX3270BLS_catalogue_to_calculated_peak_ratio':63/(T/1000/.0980665),
      'warning':'No curva de par continuo, torque-velocidad, temperatura o frenada real; no parametros aprobados.'}
    plate=[]
    load=summary['max_anchor_tension']['service_tension_N']
    for preload in [0,100]:
        for span in [5,10,20]:
            force=load+preload
            plate.append({'service_tension_N':load,'preload_assumed_N':preload,'total_N':force,
              'unsupported_strip_span_mm':span,'strip_width_assumed_mm':20,'underplate_t_mm':3,
              'cantilever_strip_bending_MPa':6*force*span/(20*3**2),
              'punching_average_MPa':force/(math.pi*7*3),
              'scope':'Sensibilidad de falta de apoyo, no modelo de contacto real de la contraplaca/acrilico.'})
    write_csv('sensibilidad_contraplaca.csv',plate)
    summary['underplate_contact_sensitivity']=plate
    ratio=summary['motor_sizing']['GX3270BLS_catalogue_to_calculated_peak_ratio']
    summary['hypothetical_new_motor_stall_no_mass_change']={
      'post_stress_scaled_MPa':summary['max_servo_post_nominal']['sigma_post_nominal_MPa']*ratio,
      'post_required_strength_n2_Kt2_MPa':summary['max_servo_post_nominal']['sigma_post_nominal_MPa']*ratio*4,
      'D_uniform_half_pressure_scaled_MPa':summary['max_D_contact']['D_flat_pressure_uniform_half_MPa']*ratio,
      'warning':'Escalado lineal de torque a63kgfcm, sin CAD, fijaciones ni masas del nuevo motor; solo muestra que no basta cambiar servo.'}
    shaft_sizing=[]
    for diam in [8,10,12]:
        vals=[]
        for st in primary:
            bend=float(np.linalg.norm(st['M'][:2]));torque=abs(st['M'][2])
            sigma=32*bend/(math.pi*diam**3)+4*abs(st['F'][2])/(math.pi*diam**2)
            tau=16*torque/(math.pi*diam**3);vm=math.sqrt(sigma*sigma+3*tau*tau)
            vals.append({'case':st['case'],'time_s':st['time_s'],'yaw_end_deg':st['yaw_end_deg'],'diameter_mm':diam,
              'solid_circular_vm_MPa':vm,'yield_required_n2_Kt2_MPa':4*vm})
        shaft_sizing.append(peak(vals,'solid_circular_vm_MPa'))
    summary['solid_circular_shaft_sizing_no_CAD']=shaft_sizing
    (HERE/'resultados_J1.json').write_text(json.dumps(serial(summary),ensure_ascii=False,indent=2),encoding='utf-8')
    print(json.dumps(serial(summary),ensure_ascii=False))

if __name__=='__main__':main()
