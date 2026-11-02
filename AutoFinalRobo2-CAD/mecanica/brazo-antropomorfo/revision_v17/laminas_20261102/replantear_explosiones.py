"""Explosion documental oblicua, preservando orientaciones/ubicacion relativa."""
from pathlib import Path
H=Path(__file__).resolve().parent;source=H/'renderizar.py'
exec(compile(source.read_text(encoding='utf-8').split('\nparts=[')[0],str(source),'exec'))
data=json.loads((H/'vistas.json').read_text(encoding='utf-8'))
ins=[i for i in g['instances'] if i['root']=='05_Inclinacion_muneca']
tramos=[]
for start in range(0,len(data['assembly']),8):
 clear();objects=[];labels={};items=data['assembly'][start:start+8];offsets=[]
 for index,item in enumerate(items):
  cn=item['component'];oo=[make(i,True) for i in ins if i['path']==item['path']]
  # Traslaciones documentales. No recentrar cada pieza en una fila plana.
  delta=Vector((0,0,0))
  if 'Nucleo' in cn:pass
  elif 'M06_MG90S' in cn:delta.y=-.050
  elif 'Tapa_STS3032' in cn:delta.z=.060;delta.y=-.025
  elif 'R06_625' in cn:delta.z=.025 if 'inferior' in cn else .055
  elif 'Tapa_625' in cn:delta.z=.095
  elif 'Tornillo_tapa' in cn:delta.z=.120
  elif 'Inserto' in cn:delta.z=.035
  elif 'M2x35' in cn:delta.y=-.075
  elif 'Tuerca_M2' in cn:delta.y=.045
  elif 'Eje_y_brida' in cn:delta.y=-.040
  elif 'Separador_axial' in cn:delta.y=-.020
  elif 'Separador_trasero' in cn:delta.y=.035
  elif 'Arandela_trasera' in cn:delta.y=.055
  elif 'axial_trasero' in cn:delta.y=.075
  elif 'Horn_STS3250' in cn:delta.y=-.075
  elif 'horn' in cn:delta.y=-.105
  else:delta.y=-.035
  for o in oo:o.location+=delta
  objects.extend(oo);labels[str(item['balloon'])]=oo[0]
  offsets.append({'position':item['balloon'],'component':cn,'translation_mm':[1000*x for x in delta]})
 name='AFR2-A500-E'+str(len(tramos)+1)+'-ISO.png'
 coords=camera(objects,name,direction=(1,-1,1),labels=labels)
 tramos.append({'render':name,'items':items,'labels':coords,'offsets':offsets})
data['exploded_sections']=tramos
data['exploded_note']='Explosion isometrica documental; separaciones visuales, no secuencia de desmontaje validada.'
(H/'vistas.json').write_text(json.dumps(data),encoding='utf-8')
(H/'explosiones_isometricas.json').write_text(json.dumps(tramos,indent=2),encoding='utf-8')
print('EXPLOSIONES_ISOMETRICAS_LISTAS',flush=True)
