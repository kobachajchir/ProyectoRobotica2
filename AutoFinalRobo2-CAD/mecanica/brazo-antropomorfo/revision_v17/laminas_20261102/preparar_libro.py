"""Adaptador AFR2 de captura nativa a libro reproducible de laminas."""
from pathlib import Path
import json,csv,hashlib,shutil
H=Path(__file__).resolve().parent
d=json.loads((H/'vistas.json').read_text(encoding='utf-8'));inv=json.loads((H/'inventario.json').read_text(encoding='utf-8'));native=json.loads((H/'interfaces.json').read_text(encoding='utf-8'))
parts={p['component']:p for p in d['parts']};sheets=[]
def row(code,label,qty=1):return {'id':code,'label':label,'qty':qty}
roots=[('A100','Montura y control fijo'),('A200','Giro de base J1 y torre'),('A300','Primer eslabon y motor remoto J3'),('A400','Antebrazo y giro J4'),('A500','Inclinacion J5 y soporte J6'),('A600','Herramienta y pinza V7')]
sheets.append({'code':'AFR2-A000','title':'Brazo antropomorfo - conjunto general','kind':'general','render':str(H/'renders/AFR2-A000.png'),'general_rows':[row('AFR2-'+code,label) for code,label in roots], 'notes':['Modelo actual capturado del maestro Fusion; auto y cubo excluidos.','Los codigos A100-A600 organizan la serie; solo A500 se desarrolla en esta primera tanda.','Vista de identificacion, no plano de implantacion ni comprobacion de movilidad.'],'material':'Conjunto mixto'})
sheets.append({'code':'AFR2-A500','title':'Inclinacion J5 y soporte J6 - armado','kind':'assembly','render':str(H/'renders/AFR2-A500.png'),'views':d['assembly_views'],'notes':['Conjunto extraido de 05_Inclinacion_muneca. Componentes comerciales completos.','Cotas envolventes de referencia; interfaces y ajustes requieren acotado funcional.','Ver A500-LP y A500-E1..E4 para identificacion de cada ocurrencia.'],'material':'PLA blanco, acero, aluminio y comerciales'})
def shortname(cn):
 if cn in parts:return parts[cn]['title']
 if 'R06_625' in cn:return 'Rodamiento 625 (5 x 16 x 5 mm)'
 if 'M06_MG90S' in cn:return 'Servo FEETECH STS3032 A0 (nombre CAD heredado)'
 if 'M2x35' in cn:return 'Tornillo M2 x 35 - envolvente CAD'
 if 'Tuerca_M2' in cn:return 'Tuerca M2 - envolvente CAD'
 if 'M3x6_ISO' in cn:return 'Tornillo avellanado M3 x 6'
 if 'Inserto_M2' in cn:return 'Inserto M2 - referencia'
 if 'Tornillo_tapa' in cn:return 'Tornillo de tapa J6'
 if 'Horn_STS3250' in cn:return 'Horn de salida STS3250'
 return cn.replace('V03_','').replace('_',' ')
bom=[row(str(i['balloon']).zfill(2),shortname(i['component'])) for i in d['assembly']]
sheets.append({'code':'AFR2-A500-LP','title':'J5 / J6 - lista de piezas','kind':'assembly','render':str(H/'renders/AFR2-A500.png'),'bom':bom,'notes':['Cada posicion es una ocurrencia de componente; los solidos internos de un rodamiento no son piezas adicionales.','Nombres originales y rutas completas en BOM_A500.csv.','P501-P506 son piezas fabricadas documentadas por separado.'],'material':'Conjunto mixto'})
for n,section in enumerate(d['exploded_sections'],1):
 sheets.append({'code':'AFR2-A500-E'+str(n),'title':'J5 / J6 - explosion isometrica, tramo '+str(n)+' de 4','kind':'exploded','render':str(H/'renders'/section['render']),'bom':[row(str(x['balloon']).zfill(2),shortname(x['component'])) for x in section['items']],'balloons':section['labels'],'revision':'B','notes':['Explosion isometrica documental: se conservan orientaciones y relaciones espaciales.','Separaciones de visualizacion; no constituyen una secuencia de desmontaje validada.','Globos vinculados a A500-LP. Se conservan todas las ocurrencias de la tornilleria.'],'material':'Conjunto mixto'})
for p in d['parts']:
 info=next(x for x in inv['components'] if x['name']==p['component']);printed=info['printed'];rho=1.24 if printed else 7.85
 # Las cotas envolventes proceden del BRep y no de los extremos de la malla.
 bb=native[p['component']][0]['bounds_mm']
 for v,axes in zip(p['views'],[(0,2),(1,2),(0,1)]):
  a,b=axes;v['bounds']=[bb[a],bb[b],bb[a+3],bb[b+3]]
  if v['title']=='PERFIL IZQUIERDO':v['bounds']=[-bb[a+3],bb[b],-bb[a],bb[b+3]]
 diameters=sorted(set(round(x['diameter_mm'],3) for x in p['cylinders']))
 sheets.append({'code':p['code'],'title':p['title'],'kind':'part','component':p['component'],'render':str(H/'renders'/p['render']),'views':p['views'],'diameters':diameters,'mass_note':'Masa CAD estimada: %.2f g; densidad %.2f g/cm3. Cantidad en el brazo: %s.'%(p['volume_cm3']*rho,rho,info['occurrences']), 'material':'PLA blanco PrintALot / FDM' if printed else 'Acero / proceso pendiente','notes':['Cotas en mm desde BRep; contornos visibles y ocultos derivados de geometria nativa.','Acotado envolvente preliminar. Faltan datums, posiciones de agujeros, cortes y ajustes funcionales.','Los diametros listados son superficies cilindricas; no indican cantidad de perforaciones.']})
if (H/'fichas_conjuntos.json').exists():
 fichas=json.loads((H/'fichas_conjuntos.json').read_text(encoding='utf-8'))
 sheets[0]={'code':'AFR2-A000','title':'Brazo completo y sus subconjuntos','kind':'general','render':fichas['arm_render'],'render_cards':fichas['arm_cards'],'cards_title':'SUBCONJUNTOS DEL BRAZO','main_label':'BRAZO COMPLETO','revision':'C','material':'Conjunto mixto','notes':['Ficha visual del brazo con sus seis subconjuntos funcionales.','Los grupos incluyen todas las425 instancias de solidos de la captura; no son piezas comerciales contadas.','Esta ficha no sustituye los planos individuales; solo seis piezas tienen hoja preliminar en esta tanda.']}
 sheets.insert(1,{'code':'AFR2-R000','title':'Robot integrado y conjuntos del auto','kind':'general','render':fichas['robot_render'],'render_cards':fichas['car_cards'],'cards_title':'SUBCONJUNTOS DEL AUTO','main_label':'AUTO CON BRAZO','revision':'C','material':'Conjunto mixto','notes':['Auto base desde el modelo editable20261025 y brazo desde la captura20261102.','Renders agrupados por funcion; no se modifico el CAD ni se realizo una nueva comprobacion de montaje.','Los subconjuntos mostrados no implican que sus planos de fabricacion esten terminados.']})
for sheet in sheets:sheet.setdefault('revision','B')
if (H/'llamadas_cortas.json').exists():
 locales=json.loads((H/'llamadas_cortas.json').read_text(encoding='utf-8'))
 for sheet in sheets:
  if sheet['kind']=='exploded' and sheet['code'] in locales:sheet['local_balloons']=locales[sheet['code']]
book={'project':'AutoFinalRobo2 - Robotica II','author':'Koba Chajchir','date':'02/11/2026','source':'Brazo v17 / captura nativa 02-11-2026','layout':'ortogonales izquierda / render adaptable / notas inferiores izquierda / rotulo inferior derecha','sheets':sheets}
(H/'libro.json').write_text(json.dumps(book,indent=2),encoding='utf-8')
with (H/'BOM_A500.csv').open('w',encoding='utf-8-sig',newline='') as fp:
 w=csv.writer(fp);w.writerow(['posicion','codigo_pieza','descripcion','cantidad','componente_CAD','ruta_CAD'])
 for x in d['assembly']:w.writerow([x['balloon'],parts.get(x['component'],{}).get('code','COMERCIAL/REFERENCIA'),shortname(x['component']),1,x['component'],x['path']])
text=['# Indice de la primera tanda de laminas','','Documento fuente: '+inv['document'],'','| Hoja | Codigo | Contenido |','| ---: | --- | --- |']
for n,s in enumerate(sheets,1):text.append('| %d | %s | %s |'%(n,s['code'],s['title']))
(H/'INDICE.md').write_text('\n'.join(text)+'\n',encoding='utf-8')
print(json.dumps({'sheets':len(sheets),'parts':len(d['parts']),'assembly_positions':len(d['assembly'])}))
