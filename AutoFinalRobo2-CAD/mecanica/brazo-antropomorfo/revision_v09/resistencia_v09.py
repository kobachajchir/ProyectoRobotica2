"""Cargas y secciones reales v09. Predimensionado, no FEA/contactos ni aprobacion FDM."""
from pathlib import Path
import importlib.util,json,csv,math,itertools,os
# Small section matrices do not benefit from a large BLAS thread pool.
os.environ['OPENBLAS_NUM_THREADS']='1'
import numpy as np
H=Path(__file__).resolve().parent
def module(name,path):
    s=importlib.util.spec_from_file_location(name,path);m=importlib.util.module_from_spec(s);s.loader.exec_module(m);return m
old=module('j1_sections',H.parent/'analisis_J1_uniones/calcular_J1.py')

def convex_vertices(points):
 # Affine normal stress reaches its maximum absolute value on the convex hull.
 # Internal-hole vertices are irrelevant to this extremum, but remain in A/I.
 pp=sorted(set(map(tuple,points)))
 def turn(a,b,c):return (b[0]-a[0])*(c[1]-a[1])-(b[1]-a[1])*(c[0]-a[0])
 lo=[];hi=[]
 for a in pp:
  while len(lo)>1 and turn(lo[-2],lo[-1],a)<=0:lo.pop()
  lo.append(a)
 for a in reversed(pp):
  while len(hi)>1 and turn(hi[-2],hi[-1],a)<=0:hi.pop()
  hi.append(a)
 return np.array(lo[:-1]+hi[:-1])

def section_stress(sec,F,M):
 if '_stress_kernel' not in sec:
  xy=convex_vertices(sec['_points'])-np.array([sec['cx_mm'],sec['cy_mm']])
  sec['_stress_kernel']=xy@np.linalg.inv([[sec['Iy_mm4'],sec['Ixy_mm4']],[sec['Ixy_mm4'],sec['Ix_mm4']]])
 vals=F[2]/sec['A_mm2']+sec['_stress_kernel']@[-M[1],M[0]]
 return float(max(abs(vals.min()),abs(vals.max())))
old.stress=section_stress
dyn=module('dyn09',H/'calcular_v09.py')
raw=json.loads((H/'secciones_CAD_v09.json').read_text(encoding='utf-8'))
p={k:v['value_internal']*10 for k,v in raw['parameters'].items()}
parts,ff=dyn.load()
states=[]
# Finite smoothstop, each axis separately. Horizontal endpoint; added J1 sense.
for axis in range(3):
 for direction in [-1,1]:
  for tb in [.2,.1,.05]:
   end=np.radians([0,90,0,0,0,0]);speed=np.zeros(6);speed[axis]=direction*math.radians(30)
   start=end-speed*tb/2
   lim=next(v for k,v in dyn.st.RAW['joint_limits'].items() if k.startswith(f'J{axis+1}_'))
   if min(start[axis],end[axis])<lim['min_rad']-1e-8 or max(start[axis],end[axis])>lim['max_rad']+1e-8:continue
   for t in np.linspace(0,tb,21):
    ss=t/tb;q=end-speed*tb/2+speed*tb*(ss-ss**3+.5*ss**4)
    dq=speed*(1-3*ss*ss+2*ss**3);ddq=speed*(-6*ss+6*ss*ss)/tb
    jj=dyn.state_at(parts,q,dq,ddq)
    j=jj[0];states.append({'case':f'J{axis+1}_{direction:+d}_{tb*1000:.0f}ms','time_s':float(t),'q':np.degrees(q).tolist(),'F':-np.array(j['force_N']),'M':-np.array(j['moment_Nm'])*1000})
j=dyn.state_at(parts,np.radians([0,90,0,0,0,0]),np.zeros(6),np.zeros(6))[0]
states.append({'case':'estatica','time_s':0,'q':[0,90,0,0,0,0],'F':-np.array(j['force_N']),'M':-np.array(j['moment_Nm'])*1000})
coupled_joint_peaks={}
for signs in itertools.product([-1,1],repeat=6):
 for tb in [.2,.1,.05]:
  end=np.radians([0,90,0,0,0,0]);speed=np.radians(signs)*30
  case='S6_'+'_'.join(f'{x:+d}' for x in signs)+f'_{tb*1000:.0f}ms'
  for t in np.linspace(0,tb,21):
   ss=t/tb;q=end-speed*tb/2+speed*tb*(ss-ss**3+.5*ss**4)
   dq=speed*(1-3*ss**2+2*ss**3);ddq=speed*(-6*ss+6*ss**2)/tb
   jj=dyn.state_at(parts,q,dq,ddq);j=jj[0]
   states.append({'case':case,'time_s':float(t),'q':np.degrees(q).tolist(),'F':-np.array(j['force_N']),'M':-np.array(j['moment_Nm'])*1000})
   for j in jj:
    if j['joint'] not in coupled_joint_peaks or j['servo_torque_kgfcm']>coupled_joint_peaks[j['joint']]['servo_torque_kgfcm']:
     coupled_joint_peaks[j['joint']]=dict(j,case=case,time_s=float(t),q_deg=np.degrees(q).tolist())
groups={}
for s in states:groups.setdefault(s['case'],[]).append(s)
allstates=[]
for case,gg in groups.items():
 for yaw in range(-90,91,10):
  if min(s['q'][0]+yaw for s in gg)<-90-1e-8 or max(s['q'][0]+yaw for s in gg)>90+1e-8:continue
  a=math.radians(yaw);R=np.array([[math.cos(a),-math.sin(a),0],[math.sin(a),math.cos(a),0],[0,0,1]])
  for s in gg:allstates.append(dict(s,yaw_deg=yaw,F=R@s['F'],M=R@s['M']))
shaft=[old.section(z) for z in raw['sections'] if z['component']=='V03_E00_Husillo_base_completo']
housing=[old.section(z,[-24.3,-35,43.3,7]) for z in raw['sections'] if z['component'] in ['V05_Montura_baja_PETG','V09_Cartucho6000_desmontable_PETG']]
def frame_model():
 E=1500.;G=E/2.76;beams=[];top=60.;base=12.1
 for name,roi in [('izquierda',[-24.21,-28.51,-11.09,.51]),('derecha',[30.59,-28.51,43.21,.51])]:
  ss=[old.section(z,roi) for z in raw['sections'] if z['component']=='V05_Montura_baja_PETG' and 12<z['z_mm']<60]
  tip=ss[-1];zz=np.array([x['z_mm'] for x in ss]);C=np.zeros((6,6))
  for z in np.linspace(base,top,161)[:-1]+(top-base)/320:
   get=lambda k:float(np.interp(z,zz,[x[k] for x in ss]))
   A,Ix,Iy,Ixy=[get(k) for k in ['A_mm2','Ix_mm4','Iy_mm4','Ixy_mm4']]
   L=top-z;dx=tip['cx_mm']-get('cx_mm');dy=tip['cy_mm']-get('cy_mm');dz=(top-base)/160
   B=np.array([[-L,0,dx,0,-1,0],[0,-L,dy,1,0,0]])
   C+=B.T@np.linalg.inv([[Iy,Ixy],[Ixy,Ix]])@B*dz/E;C[2,2]+=dz/(E*A)
   v=np.array([-dy,dx,0,0,0,1.]);C+=np.outer(v,v)*dz/(G*.5*(Ix+Iy))
  r=np.array([tip['cx_mm'],tip['cy_mm']-p['v05_axis_y'],0.])
  B=np.block([[np.eye(3),-old.skew(r)],[np.zeros((3,3)),np.eye(3)]])
  beams.append(dict(name=name,ss=ss,tip=tip,B=B,K=np.linalg.inv(C)))
 K=sum((b['B'].T@b['K']@b['B'] for b in beams),np.zeros((6,6)))
 return beams,K
beams,K=frame_model()
# Fixed J1 hardware and printed wall self-weight, separate from arm wrench.
dead=[b for b in parts if b['root']=='00_Base_control' and (b['component'].startswith('V09_') or b['component'] in ['V09_R01_6000_J1_inferior','V09_R01_6000_J1_superior','V03_Tapa_rodamientos_J1'])]
wallrows=sorted([x for x in housing if x['component']=='V05_Montura_baja_PETG'],key=lambda x:x['z_mm'])
for a,b in zip(wallrows,wallrows[1:]):
 vol=(a['A_mm2']+b['A_mm2'])/2*(b['z_mm']-a['z_mm'])
 dead.append({'component':'peso_paredes_integrado','mass_g':vol/1000*1.24,'com_mm':[(a['cx_mm']+b['cx_mm'])/2,(a['cy_mm']+b['cy_mm'])/2,(a['z_mm']+b['z_mm'])/2]})
deadF=sum((np.array([0,0,-b['mass_g']*9.80665/1000]) for b in dead),np.zeros(3))
deadM=sum((np.cross(np.array(b['com_mm'])-np.array([0,p['v05_axis_y'],60]),[0,0,-b['mass_g']*9.80665/1000]) for b in dead),np.zeros(3))
peaks={};static_peaks={};bearingrows=[];inputs=[];equilibrium_force=[];equilibrium_moment=[]
def peak(name,value,data):
 if name not in peaks or value>peaks[name]['value']:peaks[name]=dict(data,value=float(value))
 if data.get('case')=='estatica' and (name not in static_peaks or value>static_peaks[name]['value']):static_peaks[name]=dict(data,value=float(value))
pts=np.array([[p['agv_'+n+'_x'],p['agv_'+n+'_y']] for n in 'ABCDEF']+[[p['agv_center_x'],p['agv_center_y']]])
fixed=dyn.st.aggregate(parts,'root');fixed=next(x for x in fixed if x['name']=='00_Base_control')
Fg0=np.array([0,0,-fixed['mass_g']*9.80665/1000]);Mg0=np.cross(fixed['com_mm'],Fg0)
for s in allstates:
 F,M=s['F'],s['M'];tag={k:s[k] for k in ['case','time_s','yaw_deg']}
 q_actual=list(s['q']);q_actual[0]+=s['yaw_deg']
 inputs.append(dict(tag,F_N=F.tolist(),M_Nmm=M.tolist(),q_deg=q_actual))
 za,zb=p['v09_lower']-p['base_top'],p['v09_upper']-p['base_top']
 # Shaft loads F/M equal the arm-on-support wrench in external load convention.
 rx=np.linalg.solve([[1,1],[za,zb]],[-F[0],-M[1]])
 ry=np.linalg.solve([[1,1],[za,zb]],[-F[1],M[0]])
 reactions=[np.array([rx[i],ry[i],0]) for i in range(2)]
 reactions[1][2]=-F[2]  # axial reaction at upper locating bearing, not motor
 equilibrium_force.append(float(np.linalg.norm(F+sum(reactions,np.zeros(3)))))
 residual=M+np.cross([0,0,za],reactions[0])+np.cross([0,0,zb],reactions[1]);residual[2]=0
 equilibrium_moment.append(float(np.linalg.norm(residual)))
 for i,r in enumerate(reactions):
  Fr=float(np.linalg.norm(r[:2]));P0=max(Fr,.6*Fr+.5*abs(F[2]))
  peak('bearing_P0_N',P0,dict(tag,bearing=i,C0_N=1960,s0=1960/P0,pressure_projected_MPa=Fr/(26*8)))
 for sec in shaft:
  z=sec['z_mm'];fc=F.copy();mc=M+np.cross([0,0,-z],F)
  for zz,rr in zip([za,zb],reactions):
   if zz>z:fc+=rr;mc+=np.cross([0,0,zz-z],rr)
  a=math.radians(q_actual[0]);R=np.array([[math.cos(a),-math.sin(a),0],[math.sin(a),math.cos(a),0],[0,0,1]])
  sigma=old.stress(sec,R.T@fc,R.T@mc)
  # Annular torsion only where the section is the circular drilled shaft.
  # Lower double-D/counterbore: smaller inscribed annulus as an additional
  # torsion sensitivity. Neither surrogate resolves noncircular peak stress.
  od,idd=(8.,5.8) if z<66.8-p['base_top'] else (9.6,3.4)
  tau=abs(M[2])*16/(math.pi*od**3*(1-(idd/od)**4))
  vm=math.sqrt(sigma*sigma+3*tau*tau)
  peak('shaft_vm_reference_MPa',vm,dict(tag,z_mm=z,sigma_nominal_MPa=sigma,torsion_surrogate_MPa=tau,torsion_reference_diameters_mm=[od,idd],required_yield_n2_Kt2_MPa=4*vm))
 for sec in housing:
  z=sec['z_mm'];mc=M+np.cross([0,p['v05_axis_y'],p['base_top']]-np.array([sec['cx_mm'],sec['cy_mm'],z]),F)
  sf=F
  if sec['component']=='V09_Cartucho6000_desmontable_PETG':
   sf=np.zeros(3);mc=np.zeros(3)
   for zz,rr in zip([p['v09_lower'],p['v09_upper']],reactions):
    if zz>z:
     sf-=rr;mc+=np.cross(np.array([0,p['v05_axis_y'],zz])-np.array([sec['cx_mm'],sec['cy_mm'],z]),-rr)
  # Above the motor banks, drive torque reacts through motor, not bearing seats.
  if z>43.95:mc[2]=0
  sigma=old.stress(sec,sf,mc)
  peak('housing_normal_nominal_MPa',sigma,dict(tag,z_mm=z,required_normal_n2_Kt2_MPa=4*sigma))
 # Conservative drive torque applied at Z60 rather than actual tabs Z43.95.
 capM=M+np.cross([0,0,p['base_top']-60],F)+deadM
 u=np.linalg.solve(K,np.r_[F+deadF,capM])
 peak('housing_cap_translation_mm',np.linalg.norm(u[:3]),tag)
 peak('housing_cap_rotation_deg',np.linalg.norm(np.degrees(u[3:])),tag)
 for b in beams:
  load=b['K']@(b['B']@u)
  for sec in b['ss']:
   off=[b['tip']['cx_mm']-sec['cx_mm'],b['tip']['cy_mm']-sec['cy_mm'],60-sec['z_mm']]
   moment=load[3:]+np.cross(off,load[:3]);sig=old.stress(sec,load[:3],moment)
   peak('housing_frame_normal_nominal_MPa',sig,dict(tag,wall=b['name'],z_mm=sec['z_mm'],required_normal_n2_Kt2_MPa=4*sig,cap_translation_mm=u[:3].tolist(),cap_rotation_deg=np.degrees(u[3:]).tolist()))
 # Four M4 flange bolts, rigid flange, separate peak metrics.
 a=np.radians(np.array([45,135,225,315])+q_actual[0]);xy=np.c_[25*np.cos(a),25*np.sin(a)]
 # External demand: positive normal increment is uplift/tension demand;
 # negative normal increment belongs to compressive faying-surface contact.
 ax,sh=old.anchor_loads(F,M,xy,np.ones(4))
 peak('flange_M4_tension_N',max(0,ax.max()),tag)
 peak('flange_M4_shear_N',np.linalg.norm(sh,axis=1).max(),tag)
 peak('flange_M4_vm_service_MPa',max(math.sqrt((max(0,ax[i])/8.78)**2+3*(np.linalg.norm(sh[i])/8.78)**2) for i in range(4)),tag)
 peak('flange_M4_PLA_washer_pressure_MPa',max(0,ax.max())/(math.pi*(9**2-6.1**2)/4),tag)
 peak('flange_sleeve_PLA_hole_bearing_MPa',np.linalg.norm(sh,axis=1).max()/(6*8),tag)
 cart_xy=np.array([[-21.5,-23],[-21.5,-5],[40.5,-23],[40.5,-5]])
 cart_M=M+np.cross([0,p['v05_axis_y'],p['base_top']-60],F)
 ca,cs=old.anchor_loads(F,cart_M,cart_xy,np.ones(4))
 peak('cart_M3_tension_N',max(0,ca.max()),tag)
 peak('cart_M3_shear_N',np.linalg.norm(cs,axis=1).max(),tag)
 peak('cart_M3_washer_pressure_MPa',max(0,ca.max())/(math.pi*(7**2-3.2**2)/4),tag)
 peak('clip_axial_demand_bound_N',abs(F[2]),tag)
 # Torque at the four servo screws: 48x10mm pattern, fixed housing.
 T=abs(M[2]);peak('motor_demand_kgfcm',T/98.0665,tag)
 peak('servo_M3_shear_N',T/(4*math.hypot(24,5)),tag)
 peak('horn_twoM3_shear_N',T/15,tag)
 Fg=F+Fg0;Mg=M+np.cross([0,p['v05_axis_y'],p['base_top']],F)+Mg0
 for model,w in [('igual',np.ones(7)),('At_L',np.array([5.03/19]*6+[58/14]))]:
  ax,sh=old.anchor_loads(Fg,Mg,pts,w)
  i=int(np.argmax(ax));peak('anchor_tension_N',max(0,ax[i]),dict(tag,bolt=(list('ABCDEF')+['M10'])[i],model=model))
# Stall torque sensitivity is independent of the assumed movement trajectory.
stall=63*98.0665
result={'scope':'Analisis nominal de secciones nativas y grupos rigidos. Sin FEA, contacto ni aprobacion FDM. Frenadas individuales y64 signos simultaneos de6 ejes a30gr/s, 200g, 200/100/50ms.',
 'printed_material':{'name':'PLA PrintaLot','density_g_cm3':1.24,'E_reference_MPa':1500,'nu_assumed':0.38,'strength_assigned':False,'note':'Current official-linked TDS v1 12FEB17 reports ambiguous elastic modulus3.6MPa. It is not silently changed to GPa; E1500 is only a stiffness hypothesis. Published53MPa D882 is not FDM Z-direction strength.'},
 'source_states':len(states),'source_cases':len(groups),'coupled_sign_combinations':64,'evaluated_states':len(allstates),'peaks':peaks,'static_peaks':static_peaks,
 'coupled_joint_peaks':coupled_joint_peaks,
 'numerical_equilibrium_residuals':{'force_N':max(equilibrium_force),'bending_moment_Nmm':max(equilibrium_moment),'drive_torque_route':'Mz reacted separately at motor mounting pattern'},
 'bearing_centres_mm':[p['v09_lower'],p['v09_upper']],'bearing_spacing_mm':p['v09_upper']-p['v09_lower'],
 'fixed_J1_dead_load_frame':{'force_N':deadF.tolist(),'moment_at_cap_Nmm':deadM.tolist(),'mass_g':sum(b['mass_g'] for b in dead),'method':'Actual hardware CGs plus trapezoidal integration of printed wall sections. Referred to rigid cap; self-weight not distributed along beams, conservative axial magnitude sensitivity.'},
 'stall_sensitivity_63kgfcm':{'servo_M3_each_N':stall/(4*math.hypot(24,5)),'horn_M3_each_N':stall/15,'flange_M4_each_N':stall/100},
 'doubleD_metal_contact_reference':{'engagement_mm':3.35,'half_chord_mm':3,'uniform_half_both_faces_at_peak_MPa':peaks['motor_demand_kgfcm']['value']*98.0665/(3.35*9),'uniform_half_both_faces_at_63kgfcm_MPa':stall/(3.35*9),'warning':'Metal-metal simplified contact, not FEA; pressure is not equivalent tensile stress; doubleD torsion not resolved locally'},
 'flange_sector_sensitivity':{'root_radius_mm':10,'thickness_mm':3,'sigma_bending_MPa':peaks['flange_M4_tension_N']['value']*15*6/(math.pi*10/2*3**2),'required_yield_n2_Kt2_MPa':4*peaks['flange_M4_tension_N']['value']*15*6/(math.pi*10/2*3**2),'warning':'Four radial cantilever sectors, no plate/contact solution'},
 'limitations':['housing whole-section and two-variable-section-beam models; rigid cap/fixed feet, isotropic E1500 and torsion stiffness J=0.5(Ix+Iy) approximate; no exact torsional stress, local contacts or FDM orthotropy','drive torque at Z60 conservative for lever versus tabs Z43.95; local bank bending still unresolved','shaft annular torsion reference is not local solution at root/bores/flange; stress concentration factors assumed','fastener groups omit preload, unilateral contact and prying','PLA PrintaLot selected by user; FDM profile/orthotropic strength undefined; E1500MPa assumed, not TDS','no AGV braking, impact, fatigue or creep envelope; coupled cases do not cover every trajectory, speed or joint configuration','reference bearing mass20g and catalogue C0 do not identify a purchased unit']}
(H/'resistencia_v09.json').write_text(json.dumps(old.serial(result),ensure_ascii=False,indent=2),encoding='utf-8')
labels=sorted({x['case'] for x in inputs});label_index={n:i for i,n in enumerate(labels)}
np.savez_compressed(H/'estados_J1_v09.npz',case_labels=np.array(labels),case_index=np.array([label_index[x['case']] for x in inputs],dtype=np.int16),time_s=np.array([x['time_s'] for x in inputs]),yaw_end_deg=np.array([x['yaw_deg'] for x in inputs]),F_N=np.array([x['F_N'] for x in inputs]),M_Nmm=np.array([x['M_Nmm'] for x in inputs]),q_deg=np.array([x['q_deg'] for x in inputs]))
subset={}
for row in inputs:
 if row['yaw_deg'] in [-90,0,90]:subset.setdefault((row['case'],row['yaw_deg']),[]).append(row)
poses=[]
for rows in subset.values():
 for i in sorted({0,len(rows)//2,len(rows)-1}):
  row=rows[i];poses.append({'case':row['case'],'time_s':row['time_s'],'q_deg':row['q_deg']+[109.8,0]})
(H/'poses_carga_muestreo.json').write_text(json.dumps({'scope':'Inicio, mitad y final; J1 finales-90/0/90, excluyendo trayectorias fuera de limites. Muestreo geometrico propuesto, no ejecutado por este calculo.','poses':poses},ensure_ascii=False),encoding='utf-8')
(H/'propiedades_J1_v09.json').write_text(json.dumps(old.serial(shaft+housing),ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(old.serial(result),ensure_ascii=False))
