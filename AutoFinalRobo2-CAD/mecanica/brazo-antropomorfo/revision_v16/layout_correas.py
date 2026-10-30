"""Comparacion fisica pedida: discos macizos, motorJ3 distal o en hombro."""
from pathlib import Path
import runpy,math,json
H=Path(__file__).resolve().parent
RHO=.0027
def disk(d,t):return math.pi*d*d*t/4*RHO
def pulley(n,b=9):
    dp=n*3/math.pi;flange=math.ceil((dp+2)/2)*2
    # Se conserva TODO el disco a diametro primitivo: sin restar dientes/agujero.
    body=disk(dp,b);lip=2*(disk(flange,.8)-disk(dp,.8));hub=disk(16 if n>=40 else 10,6)
    return dict(teeth=n,pitch_diameter_mm=dp,body_width_mm=b,solid_disk_g=body,flanges_g=lip,hub_g=hub,total_g=body+lip+hub)
pulleys={str(n):pulley(n) for n in [16,20,48,60]}
def belt_C(L,n,N):
    r=n*3/(2*math.pi);R=N*3/(2*math.pi);low=R+r+.01;high=L
    def length(c):
        a=math.asin((R-r)/c)
        return 2*math.sqrt(c*c-(R-r)**2)+math.pi*(R+r)+2*a*(R-r)
    for _ in range(80):
        mid=(low+high)/2
        if length(mid)>L:high=mid
        else:low=mid
    c=(low+high)/2;wrap=math.pi-2*math.asin((R-r)/c)
    return dict(length_mm=L,centre_mm=c,small_teeth_in_mesh=n*wrap/(2*math.pi),small_wrap_deg=math.degrees(wrap))
belts={'J2':belt_C(210,20,60),'J3':belt_C(210,16,48)}
results=[]
for mode in ['motor_codo','motor_hombro']:
 def adjust(pp,p):
    p['wrist56']=32
    # Eje de herramienta pasa por CG combinado pinza+carga200g, no por centro
    # geometrico de la palma. TCP lleva la misma excentricidad y se calcula.
    tool=[-126.2555*27.8366/326.2555,-126.2555*22.5086/326.2555,0]
    pp[:]=[x for x in pp if not (x['name'].startswith('TransmisionJ') or x['name']=='Montura+apoyosJ1+retencion')]
    def add(root,name,m,cg,box,kind='Presupuesto'):
        pp.append(dict(root=root,name=name,mass_g=m,cg_local_mm=cg,box_mm=box,material=kind))
    add('00','Montura+apoyosJ1 con688',80,[0,0,14],[90,80,28])
    for x in pp:
        if x['name']=='STS3250 J3 remoto junto al hombro':x['cg_local_mm']=[0,-35,0 if mode=='motor_hombro' else 55]
        if x['name'] in ['ST3009 J4','ST3009 J6']:
            x['name']=x['name'].replace('ST3009','STS3032');x['mass_g']=21.6;x['box_mm']=[23,12,27.5]
        if x['name'].startswith('Pinza CAD') or x['root']=='90':
            x['cg_local_mm']=[a+b for a,b in zip(x['cg_local_mm'],tool)]
    for axis,root,N,n,location in [('J2','01',60,20,40),('J3','02',48,16,55)]:
        remote=mode=='motor_hombro' or axis=='J2';input_z=(0 if axis=='J3' and remote else location)
        add(root,axis+' polea salida disco macizo',pulleys[str(N)]['total_g'],[0,24 if axis=='J2' else -24,location],[pulleys[str(N)]['pitch_diameter_mm'],17,pulleys[str(N)]['pitch_diameter_mm']],'Al6061')
        add(root,axis+' polea entrada disco macizo',pulleys[str(n)]['total_g'],[-belts[axis]['centre_mm'] if axis=='J2' else 0,24 if axis=='J2' else -24,input_z],[20,17,20],'Al6061')
        add(root,axis+' correa OMEGA_HP_210_3M_6',.0024*6*210,[0,24 if axis=='J2' else -24,(location+input_z)/2],[60,6,60],'Elastomero')
        # 688 en lugar de608: masa presupuestada4g por rodamiento, dato exacto
        # y capacidades SKF se contrastan antes de cerrar los asientos CAD.
        add(root,axis+' dos688 y eje8x45',8+math.pi*8**2/4*45*.00785,[0,0,location],[16,45,16])
        add(root,axis+' rodamiento625 entrada eje5x18 acople',5+math.pi*5**2/4*18*.00785+3,[0,32 if axis=='J2' else -32,input_z],[16,23,16])
        add(root,axis+' soporte motor abierto Al2 y PLA',20,[0,35 if axis=='J2' else -35,input_z],[45,40,35])
        add(root,axis+' fijaciones salida y separadores',12,[0,0,location],[32,32,20])
    # Orientacion de sensor/auto pendiente: esta comparacion mantiene centro
    # de base igual en ambos layouts, sin atribuir el beneficio a trasladarla.
 g=runpy.run_path(str(H/'seleccion_previa.py'),init_globals={'AJUSTAR':adjust,'PARES_NOM':[10,16,16,1.5,10,1.5],'ARCHIVO':mode+'.json','SILENCIOSO':True})
 result=g['out'];result['layout']=mode;results.append(result)
 for a in result['axes']:
    if a['axis'] in ['J4','J6']:a['motor']='STS3032'
for result in results:
    for axis,small,power20 in [('J2',20,6.4),('J3',16,4.8)]:
        demand=result['axes'][int(axis[1])-1]['peak_Nm'];C=belts[axis]['centre_mm']
        # HP a20rpm, tabla p105. No mezclar con OMEGA basica. Debajo de20rpm
        # se usa mismo par, no misma potencia; extrapolacion a confirmar.
        cap=power20/(20*2*math.pi/60)*.61*.9*3*.8
        result.setdefault('belt_filter',[]).append(dict(axis=axis,belt='OMEGA HP 210 3M 6',output_torque_reference_Nm=cap,margin=cap/demand,table_rpm=20,operating_input_rpm=15,scope='Extension de par desde20 a15rpm no certificada. Perfil/pretension/reversion/vida deben cerrarse.'))
out=dict(scope='Comparacion antes de CAD; mismos discos macizos y BOM, cambia ubicacion del motorJ3 y su soporte. No brazo terminado ni aptitud fisica.',pulleys=pulleys,belts=belts,rows=results)
(H/'layout_correas_resultados.json').write_text(json.dumps(out,indent=2),encoding='utf-8')
print(json.dumps({'pulleys':pulleys,'belts':belts,'layouts':[{k:r[k] for k in ['layout','mass_without_load_g','axes','support_halfspan_mm','belt_filter']} for r in results]},indent=2))
