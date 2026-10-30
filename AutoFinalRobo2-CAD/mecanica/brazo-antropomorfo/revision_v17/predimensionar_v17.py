"""Preseleccion con pinzaV7 intacta y trasladoJ3 sobre primer eslabon.

Geometria fuente v09 conserva pinzaV7. Resto: presupuesto constructivo antes
de fabricar/modelar, nunca masas medidas ni prueba de frenado del fabricante.
"""
from pathlib import Path
import sys,importlib.util,runpy,json,math
import numpy as np
H=Path(__file__).resolve().parent
MIN_MARGIN=1.2  # Criterio de preseleccion aceptado por el usuario, 29/10/2026.
spec=importlib.util.spec_from_file_location('c09',H.parent/'revision_v09/calcular_v09.py');c=importlib.util.module_from_spec(spec);spec.loader.exec_module(c)
source,_=c.load();frames=c.root_frames(np.zeros(6));F6=frames['06_Herramienta'];pinza=[]
for b in source:
    if b['root'][:2] not in ['06','07','08','10']:continue
    T=np.linalg.inv(F6)@frames[b['root']];cg=(T@np.r_[b['_local_g'],1])[:3]*1000
    pinza.append(dict(root='06',name=b['component']+'/'+b['body'],mass_g=b['mass_g'],cg_local_mm=cg.tolist(),box_mm=[0,0,0],tensor_kg_m2=(T[:3,:3]@b['_I_group']@T[:3,:3].T).tolist(),material=b['category']))
rows=[]
for position in ['codo','junto_a_J2']:
 def ajustar(pp,p):
    pp.clear();p.update(base_top=64,tower_h=50,axis_y_mm=0,link1=70,link2=60,wrist45=55,wrist56=65,payload_z=88)
    def add(root,name,m,cg,box,kind='Presupuesto'):
        pp.append(dict(root=root,name=name,mass_g=m,cg_local_mm=cg,box_mm=box,material=kind))
    add('00','MotorJ1_STS3250',75.5,[5,0,30],[45.22,24.72,35],'Servo')
    add('00','Control regulacion y bateria',320,[-25,-20,20],[80,60,35])
    add('00','MonturaAl y contraplaca unica',65,[0,0,4],[172,63,6],'Al6061')
    add('00','Housing y cartuchoJ1 abierto',25,[0,0,36],[60,38,55],'PLA')
    add('00','Dos688 y tornilleria base',26,[0,0,48],[60,40,50])
    add('01','Eje y bridaJ1',33,[0,0,-10],[60,60,35])
    add('01','Plataforma horquillaAl J2 y pods',55,[0,0,25],[60,50,65],'Al6061')
    add('01','MotorJ2_STS3250',75.5,[0,62,8],[45.22,36.5,24.72],'Servo')
    add('01','Soporte y entrada de reduccionJ2',30,[0,38,8],[50,55,30])
    add('02','Polea o rueda salidaJ2 Al maciza',67,[0,35,0],[60,9,60],'Al6061')
    add('02','EjeJ2 8x76',30,[0,0,0],[8,76,8],'Acero')
    add('02','Nucleo hombro Al y manguito PLA',32,[0,0,15],[34,38,28])
    add('02','PVC25x22 corte42',math.pi*(25**2-22**2)/4*42*.00145,[0,0,35],[25,25,42],'PVC')
    z=70 if position=='codo' else 11
    add('02','MotorJ3_STS3250',75.5,[5,-61,z],[45.22,36.5,24.72],'Servo')
    add('02','Soporte entradaJ3 Al PLA',24,[0,-36,z],[40,45,30])
    add('02','Polea entrada20 Al eje625',19,[0,-36,z],[20,25,20])
    add('02','CorreaJ3 y guardas',7,[0,-38,(70+z)/2],[42,12,70])
    add('02','Horquilla codo Al y manguito',28,[0,0,57],[40,50,40])
    add('02','Dos688 codo',8,[0,0,70],[16,50,16],'Rodamiento')
    add('03','Eje y brida codo',24,[0,0,0],[8,60,8],'Acero')
    add('03','Polea salida40 Al maciza',32,[0,-36,0],[40,9,40],'Al6061')
    add('03','Nucleo codo Al y manguito',26,[0,0,15],[34,38,28])
    add('03','PVC20x17 corte32',math.pi*(20**2-17**2)/4*32*.00145,[0,0,30],[20,20,32],'PVC')
    add('03','HousingJ4 Al y PLA',20,[0,0,48],[42,35,30])
    add('03','MotorJ4_STS3032',21.6,[36,0,45],[23,12,27.5],'Servo')
    add('03','Apoyos transmision y fijacionesJ4',22,[15,0,60],[60,30,25])
    add('04','HorquillaJ5 Al y cubiertas',28,[0,0,28],[40,50,55])
    add('04','MotorJ5_STS3250',75.5,[5,55,55],[45.22,36.5,24.72],'Servo')
    add('04','EjeJ4 y dos625',24,[0,0,15],[38,48,30])
    add('05','Nucleo y columnaJ6 Al PLA',16,[0,0,24],[32,30,60])
    add('05','MotorJ6_STS3032',21.6,[5,0,40],[23,12,27.5],'Servo')
    add('05','EjeJ5 y apoyosJ6 fijaciones',25,[0,0,15],[28,50,45])
    pp.extend(pinza)
    add('90','Carga50mm200g',200,[0,0,0],[50,50,50],'Carga')
 result=runpy.run_path(str(H.parent/'revision_v16/seleccion_previa.py'),init_globals={'AJUSTAR':ajustar,'PARES_NOM':[16,16,16,1.5,16,1.5],'RELACIONES':[1,3.5,2,1,1,1],'ARCHIVO':'../revision_v17/pre_'+position+'.json','SILENCIOSO':True})['out']
 for a,motor in zip(result['axes'],['STS3250','STS3250','STS3250','STS3032','STS3250','STS3032']):
    a['motor']=motor
    a['passes_preselection']=a['margin']>=MIN_MARGIN
 result['layout']=position;result['pinza_original_g']=sum(x['mass_g'] for x in pinza);rows.append(result)
out=dict(scope='PinzaV7 intacta, resto presupuesto antes de CAD. Se compara solo traslado de motorJ3/soporte, no capacidad fisica. Mismos componentes y masas en ambos layouts.',minimum_actuator_margin=MIN_MARGIN,criterion_origin='Usuario acepta margen1.2 para esta preseleccion; no acredita resistencia del PLA, correas ni frenado real.',rows=rows)
(H/'predimensionado_resultados.json').write_text(json.dumps(out,indent=2),encoding='utf-8')
print(json.dumps([{k:r[k] for k in ['layout','mass_without_load_g','pinza_original_g','axes','support_halfspan_mm','cg_extended_with_payload_mm']} for r in rows],indent=2))
