"""Ficha del brazo y ficha del robot con renders por subconjunto real."""
from pathlib import Path
H=Path(__file__).resolve().parent;source=H/'renderizar.py'
exec(compile(source.read_text(encoding='utf-8').split('\nparts=[')[0],str(source),'exec'))
armsets=[
 ('AFR2-A100','Montura y control fijo',['00_Base_control']),
 ('AFR2-A200','Giro J1, torre y entrada J2',['01_Giro_base','09_Pinion_hombro']),
 ('AFR2-A300','Eslabon y accionamiento remoto J3',['02_Brazo_PVC','12_Polea_entrada_J3']),
 ('AFR2-A400','Antebrazo y giro de muneca J4',['03_Antebrazo_PVC','04_Giro_muneca','11_Pinion_J4']),
 ('AFR2-A500','Inclinacion J5 y soporte J6',['05_Inclinacion_muneca']),
 ('AFR2-A600','Herramienta y pinza V7',['06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','10_Pinion_pinza'])]
cards=[]
for code,title,roots in armsets:
 clear();selected=[i for i in g['instances'] if i['root'] in roots];objects=[make(i) for i in selected]
 filename=code+'-FICHA.png';camera(objects,filename,direction=(1,-1,1))
 cards.append({'code':code,'title':title,'render':str(H/'renders'/filename),'roots':roots,'solid_instances':len(selected)})
 print('FICHA_SUBCONJUNTO_LISTO',code,flush=True)

carfile=H.parents[2]/'auto-completo/AutoFinalRobo2-Final-20261025.blend'
with bpy.data.libraries.load(str(carfile),link=False) as (src,dst):dst.collections=['AFR2_Final_Detallado_20261025']
car=[o for o in dst.collections[0].all_objects if o.type=='MESH']
def stored_world(ob):
 return stored_world(ob.parent)@ob.matrix_parent_inverse@ob.matrix_basis if ob.parent else ob.matrix_basis.copy()
# Guardar transforms de las fuentes antes de construir cualquier escena derivada.
sources=[(ob,stored_world(ob)) for ob in car]
car_to_arm=Matrix.Translation((0,-.032,-.07905134505))
def carobjects(substrings):
 objects=[]
 for ob,M in sources:
  if not any(key in ob.name for key in substrings):continue
  new=bpy.data.objects.new('FICHA_'+ob.name,ob.data);s.collection.objects.link(new);new.matrix_world=car_to_arm@M;objects.append(new)
 return objects
carsets=[
 ('AFR2-C100','Chasis, separadores y fijaciones',['CapaInferior','CapaSuperior','Separadores_Montaje','Fijaciones_Placas']),
 ('AFR2-C200','Traccion: motores, ruedas y soportes',['GM25_370','Rueda80','Soportes_Motores','Casquillos_Ruedas','Prisioneros_Ruedas']),
 ('AFR2-C300','Control: Mega, Wi-Fi y Model Y',['Mega2560','WifiShield','MotorDriver_ModelY']),
 ('AFR2-C400','Alimentacion y voltimetro',['Celda_18650','Portabaterias','Voltimetro']),
 ('AFR2-C500','Sensores, servo y soporte frontal',['SG90','HC_SR04','Soporte_HCSR04','SensorLinea']),
 ('AFR2-C600','Panel OLED y mando local',['AFR2_UI_B2'])]
carcards=[]
for code,title,keys in carsets:
 clear();objects=carobjects(keys)
 if not objects:raise RuntimeError('Subconjunto sin objetos: '+code)
 filename=code+'-FICHA.png';camera(objects,filename,direction=(1,-1,1))
 carcards.append({'code':code,'title':title,'render':str(H/'renders'/filename),'source_objects':[o.name for o in objects]})
 print('FICHA_SUBCONJUNTO_LISTO',code,flush=True)
clear();objects=[make(i) for i in g['instances']]+carobjects(['FINAL_RAW_'])
camera(objects,'AFR2-R000-ROBOT-COMPLETO.png',direction=(1,-2,1))
data={'arm_cards':cards,'car_cards':carcards,'arm_render':str(H/'renders/AFR2-A000.png'),'robot_render':str(H/'renders/AFR2-R000-ROBOT-COMPLETO.png'),'arm_source':'AFR2_Fuente_Laminas_20261102.f3d','car_source':str(carfile),'car_collection':'AFR2_Final_Detallado_20261025','car_to_arm_translation_m':[0,-.032,-.07905134505],'native_cad_modified':False}
(H/'fichas_conjuntos.json').write_text(json.dumps(data,indent=2),encoding='utf-8')
print('FICHAS_GENERALES_LISTAS',flush=True)
