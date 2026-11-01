"""Prediseno dimensional de piezas nuevas; no escala masas del CAD anterior.

Volumenes de primitivas menos ventanas, materiales explicitos y motores
comerciales. No es el CAD terminado ni acredita rigidez/contacto/FDM.
"""
from pathlib import Path
import math,json,itertools
import numpy as np
H=Path(__file__).resolve().parent
rho={'PLA':.00124,'Al6061':.00270,'Acero':.00785,'PVC':.00145}
parts=[]
def add(root,name,mass,cg,box,material,detail):
    parts.append(dict(root=root,name=name,mass_g=mass,cg_local_mm=cg,box_mm=box,material=material,detail=detail))
def plate(root,name,w,h,t,cg,material,windows=(),holes=()):
    vol=w*h*t-sum(a*b*t for a,b in windows)-sum(math.pi*d*d/4*t*n for d,n in holes)
    if vol<=0:raise ValueError(name)
    side=any(k in name for k in ['Costilla torre','Puente proximal','Placa codo','Placa J4','Horquilla J5'])
    add(root,name,vol*rho[material],cg,[h,t,w] if side else [w,h,t],material,dict(outer_mm=[w,h,t],plane='XZ' if side else 'XY',symmetric_windows_mm=windows,through_holes_d_n=holes,net_volume_mm3=vol))
def ring(root,name,od,idd,length,cg,material):
    vol=math.pi*(od*od-idd*idd)/4*length
    axis='Y' if name in ['Eje hombro acero8mm','Eje codo acero8mm','Eje J5 acero5'] or name.startswith('Boss metalico codo') else 'Z'
    add(root,name,vol*rho[material],cg,[od,length,od] if axis=='Y' else [od,od,length],material,dict(axis=axis,od_mm=od,id_mm=idd,length_mm=length,net_volume_mm3=vol))
def stock(root,name,m,cg,box,material,detail='Presupuesto a concretar, no peso medido'):
    add(root,name,m,cg,box,material,detail)
def housing(root,name,L,W,D,t,cg,window_fraction=.25):
    # Open cup: wallt on sides and floort. Windows only remove a fraction of
    # side walls, never bearing seats or floor; local bosses/ribs are separate.
    outer=(L+2*t)*(W+2*t)*(D+t);inner=L*W*D
    side=((L+2*t)*(W+2*t)-L*W)*D
    vol=outer-inner-window_fraction*side
    add(root,name,vol*rho['PLA'],cg,[L+2*t,W+2*t,D+t],'PLA',dict(inner_mm=[L,W,D],wall_mm=t,side_window_fraction=window_fraction,net_volume_mm3=vol))
R0='00';R1='01';R2='02';R3='03';R4='04';R5='05';R6='06'
# Same low mounting holes as existing AGV plate. Windows are a mass budget;
# exact positions must be checked against holes/cables in the later CAD.
plate(R0,'Montura superior simple',150,60,4,[0,-5,5],'Al6061',[(20,15),(20,24)],[(3.4,6),(10.5,1)])
for s in [-1,1]:plate(R0,'Contrabarra inferior'+str(s),50,12,3,[s*49,-10,-4.5],'Al6061',holes=[(3.4,2)])
housing(R0,'Carcasa base con ventanas',40,20,40,2,[0,-14,28],.30)
stock(R0,'Motor J1 GX3270BLS',90,[0,-14,28],[40,20,47],'Servo','Starware90g; 63kgfcm7.4V,270grados')
stock(R0,'Control PWM+Nano+regulacion6V',77,[40,-20,20],[60,30,25],'Electronica','Nano7,PCA968512,XL401650,borneras8g; dimension de paquete propuesta')
stock(R0,'Bateria adicional2S y sujecion',90,[-35,-30,15],[60,35,22],'Potencia','Reserva de compra; masa y corriente del producto pendientes')
stock(R0,'Cableado y protecciones',20,[0,-20,10],[80,50,10],'Cables')
stock(R0,'Fijaciones base',18,[0,-10,5],[130,45,10],'Acero','Tornillos/tuercas presupuestados; no sustituir por aluminio')
stock(R0,'Par608 base',25,[0,-14,59],[22,22,17],'Rodamiento','2x608 de12.5g; eje8mm')
ring(R1,'Eje yaw acero8mm',8,0,40,[0,0,0],'Acero')
ring(R1,'Brida yaw aluminio',50,8,3,[0,0,2],'Al6061')
ring(R1,'Hub integral brida yaw',20,8,12,[0,0,-5.5],'Al6061')
ring(R1,'Plataforma PLA abierta',62,40,3,[0,0,4],'PLA')
for s in [-1,1]:plate(R1,'Costilla torre'+str(s),40,22,4,[s*20,0,24],'Al6061',[(15,8)],[(8,1),(3.4,2)])
stock(R1,'Nervios y bosses torre',5,[0,0,15],[45,30,30],'PLA')
housing(R1,'Housing hombro aligerado',41.5,22,42.7,2,[0,45,30],.30)
stock(R1,'Motor J2 GX3270BLS',90,[0,45,32],[40,47,20],'Servo','63kgfcm7.4V; directo, sin rueda20/40 de v09')
stock(R1,'Par608 hombro',25,[0,0,40],[22,38,22],'Rodamiento')
stock(R1,'Tornilleria y horn hombro',9,[0,0,35],[40,45,20],'Acero')
L1,L2=140.,120.
ring(R2,'Eje hombro acero8mm',8,0,46,[0,0,0],'Acero')
for z in [15,L1-15]:ring(R2,'Manguito PVC25 '+str(z),31,25.3,25,[0,0,z],'PLA')
for s in [-1,1]:plate(R2,'Puente proximal hombro'+str(s),30,45,4,[0,s*17,12],'Al6061',[(10,12)],[(8,1),(3.4,2)])
ring(R2,'Tubo25x1.5 corte',25,22,L1-30,[0,0,L1/2],'PVC')
for s in [-1,1]:
    plate(R2,'Placa codo'+str(s),35,35,4,[0,s*20,L1-3],'Al6061',holes=[(22,1),(3.4,2)])
    ring(R2,'Boss metalico codo'+str(s),28,22,7,[0,s*20,L1],'Al6061')
housing(R2,'Housing codo con ventanas',41.5,22,42.7,2,[0,48,L1-32],.30)
stock(R2,'Motor J3 GX3270BLS',90,[0,48,L1-32],[40.5,47,21],'Servo','Desplazado24mm para reductor28/20; par maximo55kgfcm6V conservador en variante PWM')
stock(R2,'Par608 codo',25,[0,0,L1],[22,47,22],'Rodamiento')
stock(R2,'Reductor codo1.4a1 y fijaciones',9,[0,30,L1-12],[32,5,50],'Al6061','28/20T modulo1, centro24mm; geometria a modelar, eta.85 supuesta')
stock(R2,'Fijaciones brazo',7,[0,0,L1/2],[30,30,L1],'Acero')
ring(R3,'Eje codo acero8mm',8,0,70,[0,0,0],'Acero')
for z in [15,L2-15]:ring(R3,'Manguito PVC20 '+str(z),26,20.3,25,[0,0,z],'PLA')
for s in [-1,1]:plate(R3,'Puente proximal codo'+str(s),25,35,3,[0,s*14,9],'Al6061',[(8,10)],[(8,1),(3.4,2)])
ring(R3,'Tubo20x1.5 corte',20,17,L2-30,[0,0,L2/2],'PVC')
for s in [-1,1]:plate(R3,'Placa J4'+str(s),30,24,2,[0,s*12,L2-2],'Al6061',[(8,6)],[(5,1),(3.4,2)])
housing(R3,'Housing J4 con ventanas',23,12,27,2,[28,0,L2-18],.25)
stock(R3,'Motor J4 MG90S',13.4,[28,0,L2-18],[23,12,29],'Servo','Motor desplazado fuera del tubo ID17; transmision1:1, giro180 por confirmar variante')
stock(R3,'Soporte y pinon J4 desplazado',6,[23,0,L2-3],[45,30,8],'PLA','Reserva nueva para apoyo continuo y engrane28T; ejes paralelos separados28mm')
stock(R4,'Rueda conducida J4 28T',2,[0,0,0],[30,30,4],'PLA','Modulo1,28T,1:1. No sustituye trazado de dientes ni tolerancias')
stock(R3,'Par626 J4',18,[0,0,L2],[19,19,16],'Rodamiento','2x626-2Z,6x19x6; presupuesto9g/unidad vs8.8g de catalogo SKF')
stock(R3,'Fijaciones antebrazo',6,[0,0,L2/2],[25,25,L2],'Acero')
ring(R4,'Eje J4 acero6',6,0,28,[0,0,0],'Acero')
for s in [-1,1]:plate(R4,'Horquilla J5'+str(s),60,30,2.5,[0,s*16,32],'Al6061',[(12,12)],[(5,1),(3.4,2)])
plate(R4,'Travesano horquilla J5',35,38,2.5,[0,0,7],'Al6061',holes=[(5,1),(3.4,4)])
stock(R4,'Separadores union horquilla',6,[0,0,15],[35,35,20],'Al6061','Separadores/escuadras a concretar para union atornillada')
housing(R4,'Housing J5 ventanas',41.5,22,42.7,2,[0,45,55],.35)
stock(R4,'Motor J5 MG946R',55,[0,45,55],[41,43,21],'Servo','13kgfcm6V de TowerPro; no nominal continuo publicado')
stock(R4,'Par625 J5',10,[0,0,55],[16,38,16],'Rodamiento')
stock(R4,'Bosses y fijaciones J5',5,[0,0,15],[35,35,25],'PLA')
stock(R4,'Tornilleria J5',5,[0,0,20],[30,35,20],'Acero')
ring(R5,'Eje J5 acero5',5,0,44,[0,0,0],'Acero')
housing(R5,'Housing J6 abierto',23,12,27,2,[0,0,49],.35)
stock(R5,'Motor J6 MG90S',13.4,[0,0,49],[23,12,29],'Servo','1.8kgfcm4.8V; giro180 variante pendiente')
stock(R5,'Par625 J6',10,[0,0,65],[16,16,15],'Rodamiento')
stock(R5,'Nervios J6',3,[0,0,15],[28,25,25],'PLA')
stock(R5,'Tornilleria J6',4,[0,0,15],[20,25,20],'Acero')
stock('90','Pieza50mm200g',200,[0,0,0],[50,50,50],'Carga','Usuario')
parts+=json.loads((H/'pinza_reutilizada.json').read_text(encoding='utf-8'))
out=dict(scope='Primitivas propuestas salvo pinza v09 reutilizada, sin modificar CAD ni asignar resistencia FDM. Reticula/ventanas son sustracciones de volumen y deben materializarse fuera de asientos y tornillos.',
    lengths_mm=dict(base_top=74,tower_h=40,link1=L1,link2=L2,wrist45=55,wrist56=65,payload_z=88),
    materials_density_g_mm3=rho,parts=parts)
(H/'prediseno_componentes.json').write_text(json.dumps(out,ensure_ascii=False,indent=2),encoding='utf-8')
from collections import defaultdict
tot=defaultdict(float);roots=defaultdict(float)
for p in parts:tot[p['material']]+=p['mass_g'];roots[p['root']]+=p['mass_g']
(H/'presupuesto_masa.json').write_text(json.dumps(dict(total_with_payload_g=sum(tot.values()),without_payload_g=sum(tot.values())-200,by_material_g=dict(tot),by_root_g=dict(roots)),indent=2),encoding='utf-8')
print('TOTAL',round(sum(tot.values()),1),'SIN CARGA',round(sum(tot.values())-200,1),dict(tot))
