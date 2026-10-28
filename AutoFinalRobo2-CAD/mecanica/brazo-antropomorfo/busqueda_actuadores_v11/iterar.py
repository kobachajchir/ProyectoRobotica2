"""Busqueda siguiente: heredar motor de cargas v10 sin editar el maestro.

Las sustituciones literales estan acotadas y se comprueban para impedir
ejecutar otra version inadvertidamente. Los escenarios se registran aparte.
"""
from pathlib import Path
import json
H=Path(__file__).resolve().parent;PRE=H.parent/'preseleccion_actuadores_v10'
cfg=json.loads((PRE/'candidatos.json').read_text(encoding='utf-8'))
cfg['motors']['DS150_10']={'name':'ANNIMOS DS51150-12V 270; 10V','mass_g':175,'box_xyz_mm':[65,48,30],'offset_from_left_mm':17,'travel_deg':270,'torque_kgfcm':{'10.0':150},'no_load_deg_s':{'10.0':250},'stall_A':{'10.0':7.4},'evaluation_voltage':'10.0','availability':'Compra importada para Argentina; confirmar SKU270 y ficha ANNIMOS enlazada, no mezclar con variantes175g/125kg'}
cfg['motors']['DS150_12']=dict(cfg['motors']['DS150_10'],name='ANNIMOS DS51150-12V 270; 12V',evaluation_voltage='12.0',torque_kgfcm={'12.0':165},no_load_deg_s={'12.0':60/.21},stall_A={'12.0':8.0})
cfg['motors']['DS150_bajo10']=dict(cfg['motors']['DS150_10'],name='DS51150 10V sensibilidad125kg',evaluation_voltage='10.0',torque_kgfcm={'10.0':125})
cfg['motors']['DS150_206g']=dict(cfg['motors']['DS150_bajo10'],name='DS51150 sensibilidad206g/125kg',mass_g=206)
cfg['motors']['DS35']=dict(name='ZOSKAY DS3235 posicional270',mass_g=60,box_xyz_mm=[40,38.5,20],offset_from_left_mm=10,travel_deg=270,torque_kgfcm={'7.4':35},no_load_deg_s={'7.4':60/.11},stall_A={'7.4':2.3},evaluation_voltage='7.4',availability='Oferta de importacion Argentina B07S9XZYN2; descripcion DS3235, plano de marca pendiente de recuperar')
cfg['sources'].update(DS150_pdf='https://m.media-amazon.com/images/I/81atmTkZyeL.pdf',
 DS150_Argentina='https://tiendamia.com.ar/p/amz/b07ktscn4j/annimos-132-3-lbs-digital-servo-8-4v-engranaje-de-acero-inoxidable',
 DS35_Argentina='https://tiendamia.com.ar/p/amz/b07s9xzyn2/zoskay-servo-de-motor-sin-nucleo-de-35-kg-de')
(H/'candidatos.json').write_text(json.dumps(cfg,ensure_ascii=False,indent=2),encoding='utf-8')
src=(PRE/'comparar.py').read_text(encoding='utf-8')
old="v='6.0' if m in ['DS9401','MG92B'] else ('6.8' if m=='DS3225' else ('8.4' if hv and m in ['GX70','DS8930'] else '7.4'))"
assert src.count(old)==1
src=src.replace(old,"v=cfg['motors'][m].get('evaluation_voltage', '6.0' if m in ['DS9401','MG92B'] else ('6.8' if m=='DS3225' else ('8.4' if hv and m in ['GX70','DS8930'] else '7.4')))")
# Added masses are deliberately NOT credited against the printed parts they
# replace. Solid steel disks bound the proposed m1.5/20-40 gear blanks.
extra="""
reinforced=[]
for j in old:
    f=features[reserves[j]]
    reinforced.append(feature('reserva_refuerzo:'+j,f['root'],.035,f['cg'],f['I']))
for cn,mass,diam in [('V02_Rueda40_Hombro',.250,.063),('V02_Pinon20_Hombro',.080,.033)]:
    bb=[b for b in parts if b['component']==cn]; g=bb[0]
    tensor=np.diag([diam**2/16+.010**2/12,diam**2/8,diam**2/16+.010**2/12])
    reinforced.append(feature('reserva_acero:'+cn,g['root'],mass,g['_local_g'],tensor))
power=feature('reserva_potencia_base','00_Base_control',.200,np.array([0,0,.020]),np.eye(3)*(.060**2/6))
"""
src=src.replace("case('v09_actual')",extra+"\ncase('v09_actual')",1)
a=src.index("case('v09_actual')");b=src.index('# Features may have been added',a)
scenarios="""
case('v09_actual')
case('A_150_10V_GX',{'J2':'DS150_10','J3':'DS150_10','J4':'GX70','J5':'GX70','J7':'MG92B'},mounts=True)
case('B_150_bajo_GX',{'J2':'DS150_bajo10','J3':'DS150_bajo10','J4':'GX70','J5':'GX70','J7':'MG92B'},mounts=True)
case('C_206g_bajo_GX',{'J2':'DS150_206g','J3':'DS150_206g','J4':'GX70','J5':'GX70','J7':'MG92B'},mounts=True)
case('D_150_12V_GX',{'J2':'DS150_12','J3':'DS150_12','J4':'GX70','J5':'GX70','J7':'MG92B'},mounts=True)
case('E_150_bajo_DS35',{'J2':'DS150_bajo10','J3':'DS150_bajo10','J4':'DS35','J5':'DS35','J7':'MG92B'},mounts=True)
case('F_206g_bajo_DS35',{'J2':'DS150_206g','J3':'DS150_206g','J4':'DS35','J5':'DS35','J7':'MG92B'},mounts=True)
case('G_reforzado_206g_GX',{'J2':'DS150_206g','J3':'DS150_206g','J4':'GX70','J5':'GX70','J7':'MG92B'},mounts=True,note='50g por montaje; 330g acero hombro adicionales sin descontar PLA; 200g potencia fija en base')
cases[-1]['chosen']+=reinforced+[power]
"""
src=src[:a]+scenarios+'\n'+src[b:]
src=src.replace("'analisis-actuadores-v10'","'analisis-actuadores-v11'")
(H/'escenarios_ejecutados.py.txt').write_text(scenarios,encoding='utf-8')
import sys
if '--prepare-only' in sys.argv:
    src=src.replace("['J4','J7']", "['J1','J4','J6','J7']")
    exec(compile(src[:src.index('traj=[]')],str(H/'motor_heredado_v10.py'),'exec'))
elif '--rated-probe' in sys.argv:
    # Rebuild the same mass basis, then use actual no-load catalog speeds.
    # This is a diagnostic counterexample, not a full workspace envelope.
    exec(compile(src[:src.index('traj=[]')],str(H/'motor_heredado_v10.py'),'exec'))
    ci=next(i for i,c in enumerate(cases) if c['name']=='G_reforzado_206g_GX')
    speeds=np.radians([600,125,250,600,600,600]); tb=.05
    peak=np.zeros(6); sign_at=[None]*6
    for signs in itertools.product([-1,1],repeat=6):
        speed=speeds*np.array(signs);end=np.radians([0,90,0,0,0,0])
        for t in np.linspace(0,tb,21):
            s=t/tb;q=end-speed*tb/2+speed*tb*(s-s**3+.5*s**4)
            dq=speed*(1-3*s*s+2*s**3);ddq=speed*(-6*s+6*s*s)/tb
            wr,tau,loc,fr=evaluate(q,dq,ddq)
            demand=np.abs(tau[:6,ci])/(np.array([1,1.7,1,.9,1,1])*.0980665)
            for j in range(6):
                if demand[j]>peak[j]:peak[j]=demand[j];sign_at[j]=dict(signs=signs,time_s=float(t),q_deg=np.degrees(q).tolist())
    out=dict(case=cases[ci]['name'],scope='Contraejemplo local: 64 sentidos,21 tiempos,extremo horizontal. No barrido global.',
        stop_ms=50,joint_initial_speed_deg_s=np.degrees(speeds).tolist(),
        peak_motor_kgfcm=peak.tolist(),catalog_stall_kgfcm=cases[ci]['caps'].tolist(),
        stall_ratio=(cases[ci]['caps']/peak).tolist(),witness=sign_at)
    (H/'diagnostico_velocidades_catalogo.json').write_text(json.dumps(out,ensure_ascii=False,indent=2),encoding='utf-8')
    print(json.dumps(out,ensure_ascii=False))
else:
    exec(compile(src,str(H/'motor_heredado_v10.py'),'exec'))
