"""Postproceso de cargas: apoyos, diagramas e informe navegable. No FEA."""
from pathlib import Path
import json,math,csv
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import calcular_estatica as a

H=a.HERE;data=json.loads((H/'resultados_estatica.json').read_text(encoding='utf-8'))
C=data['cases']['CAD_macizo_200g'];parts,ff=a.physical_parts([90,90,0,0,0,0,109.8]);J={j['joint']:j for j in C['joints']}

def vec(x):return '('+', '.join(f'{v:.2f}' for v in x)+')'
def table(headers,rows):return '| '+' | '.join(headers)+' |\n|'+'|'.join(' --- ' for _ in headers)+'|\n'+''.join('| '+' | '.join(str(x) for x in row)+' |\n' for row in rows)
def save(name,text):(H/name).write_text(text,encoding='utf-8')

def com_component(cn):
    pp=[p for p in parts if p['component']==cn];m=sum(p['mass_g'] for p in pp)
    return sum((np.array(p['com_mm'])*p['mass_g'] for p in pp),np.zeros(3))/m

pairs={
 'J1':['V03_R01_608_J1_inferior','V03_R01_608_J1_superior'],
 'J2':['R02_608ZZ_Hombro_-1','R02_608ZZ_Hombro_1'],
 'J3':['R03_608ZZ_Codo_-1','R03_608ZZ_Codo_1'],
 'J4':['V03_R04_608_J4_inferior','V02_R04_608_J4'],
 'J5':['V02_R05_625_-1','V02_R05_625_1'],
 'J6':['V03_R06_625_J6_inferior','V02_R06_625_J6']}
bear=[];gears=[]
for name,cn in pairs.items():
    p1,p2=[com_component(n) for n in cn];u=(p2-p1)/np.linalg.norm(p2-p1);L=np.linalg.norm(p2-p1)/1000;center=(p1+p2)/2
    j=J[name];F=np.array(j['gravity_force_N']);M=np.array(j['gravity_moment_Nm'])+np.cross((np.array(j['point_mm'])-center)/1000,F)
    if name in ['J2','J4']:
        axis=np.array(j['axis_global']);rp=.03 if name=='J2' else .018
        if name=='J2':n=np.array([0.,0.,-1.]);contact=np.array(j['point_mm'])+axis*40+n*rp*1000
        else:
            n=ff['03_Antebrazo_PVC'][:3,0];contact=np.array(j['point_mm'])+axis*8+n*rp*1000
        tangent=np.cross(axis,n);Ft=-j['axis_torque_Nm']/rp;Fr=abs(Ft)*math.tan(math.radians(20));Fg=Ft*tangent-Fr*n
        F=F+Fg;M=M+np.cross((contact-center)/1000,Fg)
        gears.append({'joint':name,'pitch_radius_m':rp,'tangential_force_N':abs(Ft),'radial_separation_force_N':Fr,'wheel_force_vector_N':Fg.tolist(),'contact_global_mm':contact.tolist(),'pressure_angle_deg':20,'scope':'engranaje recto ideal, sin impacto ni concentracion de contacto'})
    Fperp=F-u*(F@u);Mperp=M-u*(M@u);R1=-Fperp/2-np.cross(u,Mperp)/L;R2=-Fperp/2+np.cross(u,Mperp)/L
    bear.append({'joint':name,'bearing_names':cn,'centers_global_mm':[p1.tolist(),p2.tolist()],'spacing_mm':L*1000,'radial_reaction_vectors_N':[R1.tolist(),R2.tolist()],'radial_reaction_magnitudes_N':[float(np.linalg.norm(R1)),float(np.linalg.norm(R2))],'total_axial_reaction_to_distribute_N':float(-F@u),'pair_bending_moment_Nm':float(np.linalg.norm(Mperp)),'includes_external_mesh_force':name in ['J2','J4'],'scope':'par ideal de apoyos; reparto axial depende de hombros/precarga. La reaccion completa no se obtiene duplicando el momento en cada aro.'})

roll={}
for name in ['J4','J6']:
    candidates=[(abs(j['axis_torque_Nm']),row['q4'],row['q6']) for row in data['roll_sweep'] for j in row['joints'] if j['joint']==name]
    value,q4,q6=max(candidates)
    roll[name]={'max_abs_gravity_axis_torque_Nm_sampled':value,'q4_deg':q4,'q6_deg':q6,'scope':'169 combinaciones de roll en la postura extendida, paso 15 grados'}

od=32.;idd=28.3;I=math.pi*(od**4-idd**4)/64;W=I/(od/2);AA=math.pi*(od**2-idd**2)/4
# Virtual work for vertical deflection contributed only by exposed PVC. Joints,
# sleeves and printed parts are treated as rigid here, so this is not total sag.
shoulder=np.array(J['J2']['point_mm']);ax=ff['02_Brazo_PVC'][:3,2];payload=next(p for p in parts if p['category']=='Carga util');Lload=(np.array(payload['com_mm'])-shoulder)@ax
def load_moment_at(s):
    total=0.
    for p in parts:
        if p['root'] not in J['J2']['supported_roots']:continue
        pos=(np.array(p['com_mm'])-shoulder)@ax
        if p['category']=='PVC':
            start=25 if p['root']=='02_Brazo_PVC' else 155;end=start+(80 if start==25 else 70)
            if s<end:
                lower=max(s,start);mass=p['mass_g']*(end-lower)/(end-start);cg=(end+lower)/2
                total+=mass*a.G/1000*(cg-s)
        elif pos>s:total+=p['mass_g']*a.G/1000*(pos-s)
    return total
delta={}
for E in [2700,3000]:
    components=[]
    for label,start,end in [('PVC1',42,88),('PVC2',172,208)]:
        ss=np.linspace(start,end,1001);vals=np.array([load_moment_at(s)*(Lload-s)/(E*I) for s in ss]);d=float(np.trapezoid(vals,ss));components.append({'section':label,'start_s_mm':start,'end_s_mm':end,'deflection_contribution_mm':d})
    delta[str(E)]={'E_MPa':E,'total_exposed_PVC_deflection_mm':sum(x['deflection_contribution_mm'] for x in components),'parts':components}

grip=[]
for mu in [.2,.3,.5]:
    N=.2*a.G/(2*mu);torque=2*N*.012/.8
    grip.append({'mu_assumed':mu,'minimum_normal_force_each_N':N,'servo_axis_torque_Nm_eta08':torque,'servo_static_kgfcm':torque/.0980665,'servo_2x_kgfcm':2*torque/.0980665})

extra={'bearing_pairs':bear,'gear_contact_forces':gears,'roll_sensitivity':roll,'PVC_area_mm2':AA,'PVC_I_mm4':I,'PVC_W_mm3':W,'exposed_PVC_deflection':delta,'gripper_friction_sensitivity':grip}
Fmount,Mmount=a.wrench(parts,np.array([.333,-8.501,0.]));mt=sum(p['mass_g'] for p in parts);cgmount=sum((np.array(p['com_mm'])*p['mass_g'] for p in parts),np.zeros(3))/mt
extra['complete_added_assembly']={'mass_g':mt,'com_global_mm':cgmount.tolist(),'application_point_mm':[.333,-8.501,0],'gravity_force_N':Fmount.tolist(),'gravity_moment_Nm':Mmount.tolist(),'scope':'brazo, soporte y electronica anadida; excluye auto original. Resultante para transmitir al chasis, no reparto por tornillo.'}
contrib=[]
for name in ['J2','J3','J5']:
    j=J[name]
    for cat in sorted({p['category'] for p in parts}):
        pp=[p for p in parts if p['category']==cat and p['root'] in j['supported_roots']]
        F,M=a.wrench(pp,np.array(j['point_mm']));contrib.append({'joint':name,'category':cat,'supported_mass_g':sum(p['mass_g'] for p in pp),'axis_gravity_torque_Nm':float(M@np.array(j['axis_global']))})
extra['moment_contributions']=contrib
maxbase=max((j['non_axis_bending_Nm'],rr['q4'],rr['q6']) for rr in data['roll_sweep'] for j in rr['joints'] if j['joint']=='J1')
extra['roll_sensitivity']['J1']={'max_bending_Nm_sampled':maxbase[0],'q4_deg':maxbase[1],'q6_deg':maxbase[2]}
save('apoyos_y_contactos.json',json.dumps(extra,ensure_ascii=False,indent=2))
a.write_csv(H/'contribuciones_momento.csv',contrib,['joint','category','supported_mass_g','axis_gravity_torque_Nm'])
with (H/'reacciones_para_FEA.csv').open('w',encoding='utf-8-sig',newline='') as f:
    w=csv.writer(f);w.writerow(['apoyo','x_mm','y_mm','z_mm','radial_Rx_N','radial_Ry_N','radial_Rz_N','axial_total_par_N_no_individual','tipo'])
    for b in bear:
        for i in range(2):w.writerow([b['bearing_names'][i],*b['centers_global_mm'][i],*b['radial_reaction_vectors_N'][i],b['total_axial_reaction_to_distribute_N'] if i==0 else '', 'radial: apoyo al eje; signo opuesto en alojamiento. Axial: total del par, NO fuerza individual; reparto pendiente'])

plt.rcParams.update({'font.size':10,'axes.spines.top':False,'axes.spines.right':False,'savefig.facecolor':'white'})
fig,axs=plt.subplots(3,1,figsize=(12,10),gridspec_kw={'height_ratios':[1.4,1,1]})
av=axs[0];av.plot([0,458],[0,0],lw=5,color='#666666');dist=[0,130,250,305,370,458];labels=['J2','J3','J4','J5','J6','Carga']
for s,n in zip(dist,labels):av.plot(s,0,'o',color='#222222');av.text(s,-.095,n,ha='center')
groups=[g for g in C['groups'] if g['name'] in J['J2']['supported_roots'] and g['mass_g']>20]
for g in groups:
    s=(np.array(g['com_mm'])-shoulder)@ax;force=g['mass_g']*a.G/1000;av.annotate('',xy=(s,0),xytext=(s,.14+force*.055),arrowprops={'arrowstyle':'->','color':'#bd2530','lw':1.8});av.text(s,.18+force*.055,f"{g['mass_g']:.0f} g",ha='center',fontsize=8)
av.set(xlim=(-35,500),ylim=(-.16,.52),xlabel='Distancia desde el hombro (mm)',title='Brazo horizontal: centros de masa y carga útil, caso macizo conservador');av.set_yticks([])
av.text(5,.43,'Cada flecha representa el peso del conjunto en su centro de masa.\nNo es un mapa de tensiones ni una simulación FEA.',fontsize=9)
names=['J2 Hombro','J3 Codo','J5 Inclinación'];js=[J[n] for n in ['J2','J3','J5']];xs=np.arange(3)
pay=np.array([abs(j['payload_axis_torque_Nm']) for j in js]);own=np.array([abs(j['self_weight_axis_torque_Nm']) for j in js]);axs[1].bar(xs,own,label='Peso propio',color='#607d8b');axs[1].bar(xs,pay,bottom=own,label='Carga 200 g',color='#bd2530');axs[1].set_xticks(xs,names);axs[1].set_ylabel('Par de gravedad (N·m)');axs[1].legend(frameon=False)
ss=np.linspace(0,458,500);mm=[load_moment_at(s)/1000 for s in ss];axs[2].plot(ss,mm,color='#bd2530',lw=2);axs[2].fill_between(ss,0,mm,alpha=.12,color='#bd2530')
for l,r in [(42,88),(172,208)]:axs[2].axvspan(l,r,color='#409a77',alpha=.2)
axs[2].set(xlabel='Distancia desde el hombro (mm)',ylabel='Momento de flexión (N·m)',title='Diagrama longitudinal aproximado por masas concentradas; verde = PVC expuesto')
fig.tight_layout();fig.savefig(H/'cargas_y_momentos.png',dpi=180);fig.savefig(H/'cargas_y_momentos.svg');plt.close(fig)

mass_total=sum(g['mass_g'] for g in C['groups']);printed=next(g for g in C['categories'] if g['name']=='PETG impreso');plastic_volume=printed['volume_cm3'];weight_total=mass_total*a.G/1000
group_rows=[]
for g in C['groups']:
    pt=np.array(g['com_mm']);s=(pt-shoulder)@ax
    pets=[p for p in parts if p['root']==g['name'] and p['category']=='PETG impreso'];pm=sum(p['mass_g'] for p in pets)
    group_rows.append([g['label'],f"{g['mass_g']:.1f}",f'{pm:.1f}',vec(pt),f'{s:.1f}'])
printed_parts=a.aggregate([p for p in parts if p['category']=='PETG impreso'],'component')
print_rows=[[p['name'],f"{p['volume_cm3']:.2f}",f"{p['mass_g']:.1f}"] for p in printed_parts]
motor_rows=[]
for cn,m in a.MOTOR_MASS.items():
    pt=com_component(cn);motor_rows.append([cn,str(m),vec(pt),f'{(pt-shoulder)@ax:.1f}'])
mass_report=f'''# 1. Materiales, masas y centros de masa

Maestro geométrico v06, 264 cuerpos extraídos con `HighCalculationAccuracy`. Los colores del CAD no eran materiales físicos asignados: muchas piezas figuraban como Steel. Por eso no se usa la masa automática original de Fusion. Se extraen volumen y centroide y se asignan densidades o masas de catálogo fuera del CAD. [Geometría](geometria_v06.json), [cálculo](calcular_estatica.py).

El modelo analítico se cotejó con las transformaciones nativas de la copia horizontal: error máximo menor a 10⁻⁹ mm. Se usan centros geométricos uniformes dentro de cada sólido; en servos/módulos se reparte su masa total entre las envolventes según volumen. Su distribución interna real y la de cables no están identificadas.

## Propiedades y supuestos

- PETG PrintaLot: densidad 1,27 g/cm³, [fabricante](https://printalot.com.ar/productos/filamentos/pet-g/petg-1-75-mm-1-kg/). Se utiliza densidad del polímero sólido; no se inventa un módulo o resistencia de la pieza impresa.
- PVC-U de referencia: densidad 1,42–1,48 g/cm³; se adopta 1,45. Módulo de flexión de corta duración 2,7–3,0 GPa y resistencia última típica 52 MPa, [Vinidex](https://www.vinidex.com.au/resources/technical-resources/material-properties/pvc-properties/). El tubo argentino real, su formulación y su estado deben confirmarse. No se traslada la resistencia a presión a una unión impresa.
- Acero 7,85 g/cm³, latón 8,5, nylon de horns 1,14 y goma 1,10: valores de cálculo supuestos. Sin marca/grado de metal ni polímero de los horns identificados. Tornillos y ejes son sólidos CAD simplificados, sin corrección por hélices de rosca.
- Rodamientos completos: 608-2Z = 12,5 g, [ficha SKF](https://docs.rs-online.com/8f0c/0900766b8151f2a7.pdf); 625-2Z = 5 g, [SKF](https://www.emarketplace.in.skf.com/deep-groove-ball-bearing/625-2z). Se reemplaza la masa de los aros simplificados por la masa de catálogo: 8×608 + 4×625 = 120 g. Las tapas que llevan 608/625 en el nombre siguen siendo PETG, no rodamientos.
- Servos: MG946R 55 g ([TowerPro](https://towerpro.com.tw/product/mg946r/)), MG90S 13,4 g ([TowerPro](https://towerpro.com.tw/product/mg90s-3/)), DS3218 60 g ([DSSERVO](https://www.dsservo.com/d_file/DS3218%20datasheet.pdf)). GX3240: **100 g supuesto conservador**, intervalo de sensibilidad 80–100 g; no se obtuvo una masa neta de fabricante verificable. La publicación local informa 0,1 kg junto a dimensiones de bulto y no se toma como una medición del servo. Esta masa está en la torre J1, no en el eslabón que gira sobre J2.
- Electrónica fija, a pesar: XL4016 50 g cada uno, Nano 7 g, PCA9685 12 g y bornera 8 g. Total 177 g. Se usa el centro geométrico del módulo simplificado. Los disipadores no se computan como acero además de sumar la masa del módulo.
- Carga útil 200 g, fuerza **1,96133 N**. El nombre histórico del componente dice 100g, pero la copia de análisis usa 200 g y la masa en el cálculo se impone directamente. Se conservan las dimensiones 50×50×50 mm.

## Cuánto plástico hay

Las 25 piezas rojas suman **{plastic_volume:.2f} cm³**. Macizas equivalen a **{printed['mass_g']:.1f} g de PETG**. Es una cota de peso propio conservadora de este CAD, no la predicción del laminador.

Un 40 % de relleno no produce una pieza del 40 % de masa: perímetros, tapas, nervios y regiones de poco espesor pueden quedar casi macizos. Se comparan fracciones **efectivas** de masa 100 %, 70 % y 50 %, conservando el centroide por hipótesis. La masa y el centro reales se deberán reemplazar por los del G-code o por pesaje. Los escenarios de fracción uniforme no acreditan resistencia equivalente ni permiten dividir el límite elástico por ese porcentaje.

{table(['Pieza PETG','Volumen (cm³)','Masa maciza (g)'],print_rows)}
## Presupuesto de masas

{table(['Categoría','Masa (g)'],[[g['name'],f"{g['mass_g']:.1f}"] for g in C['categories']])}
Total brazo, montaje, electrónica añadida y carga: **{mass_total/1000:.3f} kg**, peso **{weight_total:.2f} N**. Sin los 200 g: **{(mass_total-200)/1000:.3f} kg**. No incluye el auto base ni su batería. Masa giratoria sostenida por J1, incluida carga: **{J['J1']['supported_mass_g']/1000:.3f} kg**. Masa a partir del hombro J2, incluida carga: **{J['J2']['supported_mass_g']/1000:.3f} kg**.

## Centros de masa por conjunto

Coordenadas globales: X transversal, Y hacia el HC-SR04 frontal, Z vertical; origen del CAD del brazo, cara superior del chasis en Z=0. Hombro en (0, −14, 209) mm. Se define `s = Y + 14` como distancia longitudinal desde el hombro, solo para esta postura de avance frontal.

{table(['Conjunto','Masa total (g)','PETG macizo (g)','CG global X,Y,Z (mm)','s (mm)'],group_rows)}

Los conjuntos incluyen motores que están fijados a ellos: el DS3218 viaja con el eslabón 1, el MG90S J4 con el antebrazo y el MG946R J5 con la horquilla giratoria J4. No se coloca toda la masa de motor en la punta ni se la suma dos veces. CSV por cada cuerpo, con volumen, masa, CG y fuente: [masas_por_cuerpo.csv](masas_por_cuerpo.csv).

## Ubicación de los motores

{table(['Componente','Masa adoptada (g)','CG global (mm)','s (mm)'],motor_rows)}

Los CG internos reales de los motores no están publicados; esta primera aproximación usa la carcasa/salida modeladas. Una incertidumbre de ±5 mm en la posición longitudinal del CG de todos los servos transportados produce una variación adicional pequeña frente a la del porcentaje efectivo de plástico, pero debe actualizarse con componentes reales.
'''
save('01_materiales_y_masas.md',mass_report)

print('PARES',json.dumps(bear,ensure_ascii=False))
print('GEARS',json.dumps(gears))
print('DELTA',json.dumps(delta))
print('ROLL',json.dumps(roll))
print('TOTAL',mass_total)

joint_rows=[]
for j in C['joints']:
    joint_rows.append([j['joint'],f"{j['supported_mass_g']/1000:.3f}",f"{-j['gravity_force_N'][2]:.2f}",f"{abs(j['axis_torque_Nm']):.3f}",f"{j['non_axis_bending_Nm']:.3f}",f"{j['servo_static_kgfcm_eta']:.2f}"])
scenario_rows=[]
for name,cc in data['cases'].items():
    jj=cc['joints'];scenario_rows.append([name,f"{sum(x['mass_g'] for x in cc['groups'])/1000:.3f}",f"{jj[1]['axis_torque_Nm']:.3f}",f"{jj[2]['axis_torque_Nm']:.3f}",f"{jj[4]['axis_torque_Nm']:.3f}"])
pairrows=[[b['joint'],f"{b['spacing_mm']:.1f}",f"{b['radial_reaction_magnitudes_N'][0]:.1f}",f"{b['radial_reaction_magnitudes_N'][1]:.1f}",f"{abs(b['total_axial_reaction_to_distribute_N']):.2f}"] for b in bear]
contribrows=[]
for cat in ['PETG impreso','PVC','Servo','Rodamiento','Acero','Carga util']:
    values=[next(x['axis_gravity_torque_Nm'] for x in contrib if x['joint']==name and x['category']==cat) for name in ['J2','J3','J5']]
    contribrows.append([cat,*[f'{v:.3f}' for v in values]])
mtorque=J['J2']['axis_torque_Nm'];pload=J['J2']['payload_axis_torque_Nm'];codo=J['J3']['axis_torque_Nm'];selfcodo=J['J3']['self_weight_axis_torque_Nm'];maxpay=(21.5*.0980665/2-selfcodo)/(a.G*.328)*1000
save('02_estatica_y_reacciones.md',f'''# 2. Caso de carga, estática y reacciones

## Postura horizontal

Se guarda una copia nativa [AFR2_Brazo_PVC32_Estatica_200g_v06.f3d](AFR2_Brazo_PVC32_Estatica_200g_v06.f3d), manteniendo intacto el maestro v06. Ángulos: J1=90°, J2=90°, J3=0°, J4=0°, J5=0°, J6=0°, servo de pinza=109,8°. J1 apunta hacia el frente del auto; todos los eslabones de elevación están alineados horizontalmente. Es el caso pesimista de alcance para gravedad pedido; no demuestra ser el peor estado de aceleración, frenada o choque.

Hombro a Z=209 mm sobre el chasis. Posiciones longitudinales desde el hombro: J3 130 mm, J4 250 mm, J5 305 mm y J6 370 mm. La pinza sostiene el centro del cubo en **s=458 mm**, a Z=209 mm. El TCP nominal está en s=448,5 mm: no se debe usarlo en lugar del CG del cubo, que queda 9,5 mm más lejos. El frente del cubo alcanza s=483 mm, pero la resultante de su peso actúa en su centro.

![Postura nativa](pose_horizontal_cad.png)

## Ecuaciones usadas

Para cada cuerpo i: `m_i = densidad_i × V_i`. Para un conjunto, `r_G = suma(m_i × r_i) / suma(m_i)`. Volúmenes en cm³ y densidades en g/cm³ dan masas en gramos. En la estática se convierten a kg y m.

Se aplica `g = (0,0,−9,80665) m/s²`. En el centro de cada cuerpo se aplica `F_i = m_i g`. Sobre la sección/junta j, con todos los componentes aguas abajo:

```text
F_j = Σ F_i
M_j = Σ [(r_i − r_j) × F_i]
tau_j = eje_unitario_j · M_j
M_flex_j = M_j − tau_j × eje_unitario_j
```

Las reacciones que la junta ejerce sobre el conjunto distal son `−F_j` y `−M_j`. En la pieza proximal se aplica la acción opuesta. `tau_j` es el par de gravedad sobre el grado de libertad; la magnitud de `M_flex_j` es el momento perpendicular que deben transmitir eje, rodamientos, horquilla y alojamiento. En J1/J4/J6 pueden ser muy diferentes.

## Resultados estáticos, PETG macizo

{table(['Junta','Masa distal con carga (kg)','Peso distal (N)','Par sobre eje (N·m)','Flexión perpendicular (N·m)','Par servo (kgf·cm)'],joint_rows)}

El par del servo J2 incluye reducción 2:1 y eficiencia **supuesta 0,85**: `tau_motor = tau_J2/(2×0,85)`. J4 incluye relación 1:1 y eficiencia supuesta 0,90. Los demás accionamientos de eje son directos en el modelo; rozamiento y aceleración no están incluidos. `1 kgf·cm = 0,0980665 N·m`. Los momentos con signo y las coordenadas están en [resultados_estatica.json](resultados_estatica.json).

En el hombro: el cubo solo aporta `0,200×9,80665×0,458 = {pload:.3f} N·m`. El peso propio aporta **{mtorque-pload:.3f} N·m**, por lo que el total es **{mtorque:.3f} N·m**, aproximadamente {mtorque/.0980665:.1f} kgf·cm en el eje de salida del hombro. **{100*(mtorque-pload)/mtorque:.1f} %** procede del propio brazo. El hombro sostiene {J['J2']['supported_mass_g']/1000:.3f} kg, no solamente 0,2 kg.

{table(['Origen del peso','Aporte J2 (N·m)','Aporte J3 (N·m)','Aporte J5 (N·m)'],contribrows)}

Las categorías menores también están sumadas en el resultado; tabla completa: [contribuciones_momento.csv](contribuciones_momento.csv). No se carga la masa de un servo sobre su propio eje de salida si su carcasa está fijada al eslabón proximal. Por ejemplo, el DS3218 aumenta el momento del hombro, pero su carcasa no forma parte de la carga distal que él mismo sostiene en el codo.

## Sensibilidad del peso propio

{table(['Caso','Masa total añadida con carga (kg)','J2 (N·m)','J3 (N·m)','J5 (N·m)'],scenario_rows)}

La fracción efectiva 70/50 % afecta exclusivamente al PETG impreso y conserva el CG por hipótesis. El caso con cables agrega 30 g en el TCP, no incluidos en el CAD. No se modifica el alcance entre casos. El escenario TCP es una comparación geométrica; el centro del cubo real es más distal y gobierna el caso principal.

## Motores: capacidad publicada frente a demanda

Para una primera selección se propone un margen **2 respecto de la estática**. Es un criterio de selección, no un factor dinámico medido ni una certificación del par continuo. Par de bloqueo no equivale a par que pueda mantenerse indefinidamente sin calentamiento.

| Eje | Demanda estática en servo | Objetivo preliminar ×2 | Dato publicado / interpretación |
| --- | ---: | ---: | --- |
| Hombro J2 | {J['J2']['servo_static_kgfcm_eta']:.2f} kgf·cm | {J['J2']['preliminary_2x_servo_kgfcm']:.2f} kgf·cm | GX anunciado 40 kgf·cm: margen geométrico aparente, ficha neta/curva continua por confirmar |
| Codo J3 | {J['J3']['servo_static_kgfcm_eta']:.2f} kgf·cm | {J['J3']['preliminary_2x_servo_kgfcm']:.2f} kgf·cm | DS3218: bloqueo 18 a 5 V, 21,5 a 6,8 V; no alcanza este objetivo |
| Inclinación J5 | {J['J5']['servo_static_kgfcm_eta']:.2f} kgf·cm | {J['J5']['preliminary_2x_servo_kgfcm']:.2f} kgf·cm | MG946R: bloqueo 10,5 a 4,8 V / 13 a 6 V; desempeño térmico pendiente |

Fuentes: [DSSERVO](https://www.dsservo.com/d_file/DS3218%20datasheet.pdf), [TowerPro MG946R](https://towerpro.com.tw/product/mg946r/), selección comercial GX documentada en [fuentes](FUENTES.md). No se interpola una curva de par para tensiones distintas de las publicadas.

El codo requiere **{codo/.0980665:.2f} kgf·cm** con 200 g: incluso usando su máximo anunciado de 21,5 kgf·cm el margen de bloqueo sería solo {21.5/(codo/.0980665):.2f}. Con el criterio preliminar ×2 y PETG macizo, la carga compatible con ese criterio sería alrededor de **{maxpay:.0f} g**, no una carga útil física certificada. Esto identifica un cuello de botella de selección del actuador, no prueba una rotura del eslabón.

Para J1 el par de gravedad alrededor de su eje vertical es cero, pero hay **{J['J1']['non_axis_bending_Nm']:.3f} N·m de vuelco** y carga axial: no se puede concluir que J1 carece de carga. J4 tiene aproximadamente 0,061 N·m de par de roll y 0,723 N·m de flexión; J6 tiene casi nulo par de roll en la pose simétrica y 0,209 N·m de flexión. La exploración de 169 combinaciones de roll a 15° conserva el alcance horizontal y da máximos gravitatorios aproximados de {roll['J4']['max_abs_gravity_axis_torque_Nm_sampled']:.4f} N·m en J4 y {roll['J6']['max_abs_gravity_axis_torque_Nm_sampled']:.4f} N·m en J6. Una carga excéntrica cambia estos valores: 200 g a 25 mm del eje agregan 0,049 N·m.

## Dientes, ejes y alojamientos: cargas de interfaz

Engranajes rectos de 20°: `F_t = tau_rueda/r_primitivo`, `F_r = F_t tan(20°)`. En el hombro resultan **{gears[0]['tangential_force_N']:.1f} N tangenciales y {gears[0]['radial_separation_force_N']:.1f} N radiales**; en J4, {gears[1]['tangential_force_N']:.2f}/{gears[1]['radial_separation_force_N']:.2f} N para esta pose. Son cargas sobre el diente/contacto, no los 1,96 N del cubo. La fuerza tangencial ideal entre engranajes es la misma en rueda y piñón; las pérdidas se añadieron a la demanda del servo.

Los apoyos se modelan como dos reacciones a distancia L, con eje u. Tras agregar las fuerzas de engrane y trasladar el momento al centro del par:

```text
R_1 = −F_perp/2 − (u × M_perp)/L
R_2 = −F_perp/2 + (u × M_perp)/L
```

{table(['Par de apoyos','Separación centros (mm)','Carga radial apoyo 1 (N)','Carga radial apoyo 2 (N)','Axial total a repartir (N)'],pairrows)}

Se incluyó el engrane desplazado lateralmente en J2 y J4. En J3/J5/J6 se idealiza el accionamiento coaxial como un par puro: el modelo no resuelve concentraciones en horn, estriado o tornillos. La distribución axial depende de los hombros, arandelas, contactos y precarga. Los valores son reacciones del apoyo sobre el eje; el alojamiento recibe la fuerza opuesta. El momento calculado respecto del centro de un par de apoyos no es automáticamente el máximo momento interno de cada tramo del eje.

En J1 la separación actual es solo 8 mm: `2,933/0,008 ≈ 367 N` por efecto del par. A esto se añade la carga axial total 16,53 N. La capacidad nominal del rodamiento metálico no demuestra la resistencia del asiento PETG, de la tapa o de sus insertos. [Vectores y coordenadas para la fase FEA](reacciones_para_FEA.csv), [contactos completos](apoyos_y_contactos.json).

## Carga que recibe el chasis

Brazo, soporte y electrónica añadida suman **{mass_total/1000:.3f} kg con la carga**; CG global **{vec(cgmount)} mm**. En el centro del apriete M10 `(0,333; −8,501; 0) mm`, la gravedad equivale a fuerza **{vec(Fmount)} N** y momento **{vec(Mmount)} N·m**. Es la resultante externa, no un reparto igual entre seis M3 y un M10. Ese reparto requiere rigidez de placas, contacto con el acrílico y precarga.

El CG añadido queda hacia delante. La masa y el CG del auto base no se han medido; por tanto no se acredita estabilidad. Para una línea de vuelco frontal en `y_f`, el criterio estático es `m_auto (y_f − y_auto) ≥ suma[m_i (y_i − y_f)]`, con las coordenadas de apoyo reales. Este análisis no sustituye esa comprobación ni la de aceleración del AGV.
''')

pipe_rows=[[s['section'],f"{s['z_from_proximal_joint_mm']:.0f}",f"{s['bending_Nm']:.3f}",f"{s['nominal_bending_stress_MPa']:.3f}",f"{s['sigma_at_2x_load_MPa']:.3f}"] for s in C['pipe_sections']]
griprows=[[f"{g['mu_assumed']:.2f}",f"{g['minimum_normal_force_each_N']:.2f}",f"{g['servo_static_kgfcm']:.2f}",f"{g['servo_2x_kgfcm']:.2f}"] for g in grip]
save('03_tensiones_preliminares_y_refuerzos.md',f'''# 3. Tensiones nominales y criterios para los refuerzos

Este capítulo hace una primera comprobación analítica de los tramos de tubo y prepara las cargas locales. **No es FEA ni un dictamen de resistencia de las articulaciones impresas.** No se agregan refuerzos al CAD antes de resolver el peso y las cargas que ellos mismos sumarían.

## Tubos PVC Ø32/28,3 mm

La pared de estos diámetros es 1,85 mm; no 3,2 mm. El área e inercias de la sección anular son:

```text
A = pi (D^2 − d^2)/4 = {AA:.2f} mm²
I = pi (D^4 − d^4)/64 = {I:.2f} mm⁴
J_polar = 2I = {2*I:.2f} mm⁴
W = I/(D/2) = {W:.2f} mm³
sigma_flex = M_flex/W
tau_torsion = T (D/2)/J_polar
```

Las fuerzas/momentos de corte incluyen carga útil y peso propio distal. Para el PVC que atraviesa una sección se integra la masa del tramo restante; los otros cuerpos se concentran en sus CG. Las zonas impresas se consideran rígidas en esta primera aproximación. Las secciones próximas a los manguitos necesitan un modelo de contacto posterior.

{table(['Tubo','Distancia desde su eje proximal (mm)','Momento (N·m)','Tensión nominal (MPa)','Con carga multiplicada ×2 (MPa)'],pipe_rows)}

La parte expuesta del primer tubo va de 42 a 88 mm desde el hombro y la del segundo de 42 a 78 mm desde el codo. La mayor tensión nominal en la salida proximal libre es aproximadamente **1,97 MPa** en el primero y **1,00 MPa** en el segundo; con cargas duplicadas serían 3,94 y 2,00 MPa. Es una comparación muy favorable respecto de los 52 MPa típicos de rotura a tracción de un PVC-U de referencia, pero no autoriza a usar ese cociente como factor de seguridad del conjunto: faltan la formulación real, muescas, fatiga, aplastamiento de abrazadera, temperatura y creep.

La extensión horizontal no es un caso de compresión axial que deba resolverse como pandeo de Euler. Predominan flexión, torsión y deformación de uniones. El peso propio de los dos tubos es solamente **38,1 g**; no parece conveniente comenzar agregando caño o metal a toda su longitud.

Con trabajo virtual `delta = integral[M(s) × (458−s)/(E I) ds]`, sumando solo los dos tramos de PVC expuestos y haciendo rígidas las demás piezas, la contribución estimada a la caída de la carga es **{delta['3000']['total_exposed_PVC_deflection_mm']:.2f}…{delta['2700']['total_exposed_PVC_deflection_mm']:.2f} mm** para E=3,0…2,7 GPa de corta duración. La caída total real será mayor si hay flexibilidad de PETG, juego, deslizamiento o giro de engranajes. No es una predicción de deformación a largo plazo.

## Dónde reforzar primero

| Zona | Carga que gobierna en este análisis | Dirección de diseño propuesta, todavía sin modelar |
| --- | --- | --- |
| Cartucho J1 y apoyo a montura | Par de vuelco 2,93 N·m; alrededor de 367 N radiales por apoyo con 8 mm entre centros | Buscar más separación entre apoyos, asiento continuo y nervios a la placa. Mantener la altura baja; estudiar accionamiento lateral si ampliar el cartucho lo exige |
| Hombro y dientes 20/40T | Par de salida 2,91 N·m; 97 N tangenciales en el engrane | Nervios desde el portarodamiento a las raíces de la torre; revisar raíz de diente, ancho de cara y unión del cubo/D, no solo el tubo |
| Manguito proximal eslabón 1 | Momento de salida de tubo cercano a 2,46 N·m | Estudiar solape útil 25–30 mm frente a los 17 mm actuales, transiciones redondeadas y nervios en el plano principal de flexión, conservando giro y accesos |
| Codo/horquilla distal del eslabón 1 | Par de actuador 1,55 N·m y carga distal 8,39 N | Nervios simétricos hasta el manguito; revisar D, horn y fijaciones. Resolver además el margen del DS3218: más plástico no aumenta su par |
| Antebrazo y cartucho J4 | Momento de tubo cercano a 1,25 N·m; apoyos J4 ~92/97 N | Refuerzo local de la copa y raíces, mayor separación de apoyos si cabe. Evitar hacer macizo todo el antebrazo |
| Horquilla J5 y cuello J6 | Par de J5 0,427 N·m; J6 en flexión 0,209 N·m | Nervios cortos de la horquilla al alojamiento; en J6 estudiar apoyos más separados, hoy solo 6 mm entre centros |
| Palma y dedos | Peso distal 3,06 N sobre J6, más fuerza de cierre dependiente de fricción | Reforzar pie del dedo, unión carro–cremallera y guías; conservar recorrido y holguras |
| Contraplaca y montura | Resultante añadida 21,86 N y momento externo de unos 2,8 N·m sobre el apriete | Verificar contacto con acrílico, arandelas, flexión de la placa de 3 mm y precarga; no tratar todos los agujeros como empotramientos ideales |

La separación de apoyos merece prioridad: para el mismo par de J1, pasar idealmente de 8 a 20 mm bajaría la fuerza de par de 367 a unos 147 N. Esto es un objetivo geométrico, **no un cambio directo aprobado**: hay que acomodar motor y eje manteniendo la montura baja y volver a comprobar el barrido. Un refuerzo distal adicional de 50 g a 450 mm agrega `0,05×9,80665×0,45 ≈ 0,221 N·m` al hombro; a 65 mm agrega solo 0,032 N·m. El refuerzo debe justificar la masa que incorpora.

No se determina un espesor final de nervio a partir de la densidad. Para una sección simple, el requisito nominal es `W_requerido ≥ n M / sigma_admisible`, pero `sigma_admisible` debe proceder de la pieza impresa, orientación, temperatura y duración de carga. Aún no se conoce para el PETG real. Los asientos y las raíces de horquilla no se reducen a esa fórmula sin revisar concentración y contacto.

## Pinza: la carga de cierre no es el peso del cubo

En una orientación de agarre lateral, para sostener el cubo por fricción: `2 mu N ≥ m g`. Con piñón de radio 12 mm y eficiencia supuesta 0,8, `tau_motor ≈ 2 N r / 0,8`. Las fuerzas sobre los dos racks se suman en el par del piñón.

{table(['Fricción supuesta mu','Normal mínima por dedo (N)','Servo estático (kgf·cm)','Objetivo ×2 (kgf·cm)'],griprows)}

Los coeficientes son sensibilidad asumida, no propiedades medidas de las almohadillas. Para mu=0,30 el objetivo es 2,0 kgf·cm, mayor que los 1,8 kgf·cm de bloqueo publicados del MG90S a 4,8 V. Con mu=0,50 resulta 1,2 kgf·cm. Deben confirmarse goma/superficie y fuerza real; el análisis de gravedad con el cubo perfectamente centrado no resuelve por sí solo el agarre.

## Criterios antes del análisis de tensiones detallado

1. Confirmar filamento y caño, orientación de impresión, perímetros, capas superiores/inferiores y masa del laminador. Medir masa real de servos, rodamientos, horns y tornillos que se comprarán. Sustituir los supuestos del presupuesto de masas.
2. Resolver primero el margen del codo y revisar agarre. Una reducción mecánica reduce el recorrido angular disponible: duplicar relación exigiría revisar el rango de 180° del codo y el recorrido del servo. Alternativas: actuador de mayor par, menor masa distal o compensación elástica que se dimensionará para todo el recorrido.
3. Preparar material de PETG impreso, incluyendo dirección entre capas y comportamiento dependiente del tiempo. No usar un material isotrópico macizo para declarar segura una impresión con relleno parcial.
4. Aplicar gravedad por cuerpo y carga útil en su CG. Para un submodelo usar la resultante de interfaz equivalente. No aplicar simultáneamente gravedad de las mismas piezas distales y la fuerza resultante que ya las representa.
5. Representar contactos de manguitos/PVC, aros/shaft y precarga de tornillos; imponer la libertad de cada articulación y el par del accionamiento. El [CSV de reacciones](reacciones_para_FEA.csv) da fuerzas del apoyo sobre el eje; sobre el alojamiento se invierten. No duplicar ese par en ambos aros.
6. Estudiar primero el caso estático, luego una envolvente de carga ×1,5/×2 claramente identificada. Esos multiplicadores no sustituyen un modelo de aceleración ni una curva térmica del servo. Evaluar desplazamientos, raíz de nervios/dientes, asientos, D, aplastamiento y separación de contactos, además de una tensión equivalente.
7. Con resultados de tensión y desplazamiento, iterar masa, CG y par después de cada refuerzo. La revisión geométrica de interferencias deberá repetirse cuando cambien las piezas.

**Estado:** masas, CG y esfuerzos estáticos estimados calculados; primera flexión de tubos calculada; refuerzos definidos como criterios. No se ejecutó FEA, no se modificaron las piezas con nervios nuevos y no se acredita todavía «resiste/no resiste» para las articulaciones completas.
''')

save('README.md',f'''# Brazo v06: masas y cargas con 200 g

Análisis solicitado el 27/10/2026, primera etapa antes de diseñar los refuerzos y realizar tensiones detalladas. Se parte del maestro `AFR2_Brazo_PVC32_AGV_v06`; la copia nativa de estudio queda extendida horizontalmente y con `part_mass=200 g`. Las fuerzas se aplican en el modelo analítico como gravedad por cuerpo y carga puntual en el CG del cubo. **No hay un estudio FEA ejecutado en Fusion.**

## Resultados principales

- Carga útil: **200 g = 1,961 N**, a **458 mm del hombro** en el agarre CAD. Su contribución al hombro es **{pload:.3f} N·m**.
- PETG geométrico: **{plastic_volume:.1f} cm³**, equivalente a **{printed['mass_g']:.0f} g macizos**. Debe reemplazarse por masa de laminador/pesaje para estimar la impresión real.
- Eslabón 1 con su motor de codo y herrajes: **390 g**; antebrazo con motor J4 y herrajes: **254 g**, en el caso macizo. La herramienta y muñecas añaden masa distal significativa.
- Par de gravedad: **hombro {mtorque:.3f} N·m**, **codo {codo:.3f} N·m**, **inclinación de muñeca {J['J5']['axis_torque_Nm']:.3f} N·m**. El peso propio aporta aproximadamente {100*(mtorque-pload)/mtorque:.0f} % del par del hombro.
- Con reducción 2:1 y eficiencia supuesta 0,85, el servo de hombro necesita **{J['J2']['servo_static_kgfcm_eta']:.1f} kgf·cm estáticos**. El codo directo necesita **{J['J3']['servo_static_kgfcm_eta']:.1f} kgf·cm**; objetivo preliminar con margen 2: **{J['J3']['preliminary_2x_servo_kgfcm']:.1f} kgf·cm**, superior al máximo de bloqueo publicado del DS3218.
- Tensiones nominales en las salidas libres de PVC: **1,97 y 1,00 MPa**. La prioridad de refuerzo parece estar en articulaciones, asientos, dientes y agarre, no en añadir material a todo el caño.
- J1 transmite alrededor de **367 N radiales por apoyo** por su par de vuelco con solo 8 mm de separación. Es una carga para revisar en el alojamiento PETG, aunque el servo no tenga par gravitatorio alrededor del eje vertical.

## Lectura por capítulos

1. [Materiales, masas y CG de cada conjunto y motor](01_materiales_y_masas.md).
2. [Postura, ecuaciones, esfuerzos, transmisión y reacciones](02_estatica_y_reacciones.md).
3. [Tensiones nominales de PVC y criterios de refuerzo/FEA](03_tensiones_preliminares_y_refuerzos.md).
4. [Fuentes y supuestos](FUENTES.md).

![Cargas y momentos](cargas_y_momentos.png)

## Archivos de cálculo

- [Masas y CG por cuerpo](masas_por_cuerpo.csv), [PETG por pieza](petg_por_pieza.csv), [aportes de par](contribuciones_momento.csv), [reacciones para submodelos](reacciones_para_FEA.csv).
- [Geometría extraída](geometria_v06.json), [resultados y sensibilidad](resultados_estatica.json), [apoyos y contactos](apoyos_y_contactos.json).
- [Cálculo reproducible](calcular_estatica.py) y [postproceso](completar_informe.py). Requieren Python con NumPy/Matplotlib; se ejecutaron como cálculo de ingeniería autorizado, no como ensayo físico.
- [Copia F3D horizontal de estudio](AFR2_Brazo_PVC32_Estatica_200g_v06.f3d), [pose nativa](pose_horizontal_nativa.json), [diagrama vectorial](cargas_y_momentos.svg).

El caso principal adopta PETG macizo para una cota conservadora de peso propio. Se comparan masas efectivas 70/50 %, sin confundirlas con porcentaje de relleno. No se usa una masa automática del CAD con material Steel asignado a todas las piezas. Componentes comerciales emplean masas de ficha cuando están disponibles; GX y electrónica conservan supuestos explícitos. No se incluyen cables reales, aceleraciones, golpes, fuerzas del AGV ni precarga medida.

**Conclusión de esta etapa:** el cálculo de pesos/cargas está preparado para elegir refuerzos y revisar actuadores. La resistencia de las articulaciones y la estabilidad del auto no quedan certificadas por este análisis estático. No se modificó el maestro v06, no se hicieron despieces y no se modelaron aún nervios nuevos.
''')
