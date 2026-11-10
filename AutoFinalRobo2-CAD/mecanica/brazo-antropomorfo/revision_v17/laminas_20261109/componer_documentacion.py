"""Composicion A3 desde vistas reales. No asigna ajustes ni tolerancias."""
from pathlib import Path
import json,csv,math,importlib.util
from reportlab.pdfgen import canvas
from reportlab.lib.units import mm
H=Path(__file__).resolve().parent
spec=importlib.util.spec_from_file_location('base_compositor',str(H/'componer_base.py'))
base=importlib.util.module_from_spec(spec);spec.loader.exec_module(base)
text,rule,wrapped,imagebox,view=base.text,base.rule,base.wrapped,base.imagebox,base.view
ACCENT='#981b39'
data=json.loads((H/'vistas_completas.json').read_text(encoding='utf-8'))
native=json.loads((H/'interfaces_nativas.json').read_text(encoding='utf-8'))

def fmt(v):return ('%.2f'%v).rstrip('0').rstrip('.')

def interfaces(part):
 out={};bounds=part['bounds_mm'];mid=[(bounds[i]+bounds[i+3])/2 for i in range(3)]
 for body in native['parts'][part['name']]:
  for cy in body['cylinders']:
   axis=cy['axis'][:];k=max(range(3),key=lambda i:abs(axis[i]))
   if axis[k]<0:axis=[-v for v in axis]
   org=cy['origin_mm'];dot=sum(org[i]*axis[i] for i in range(3));perp=[org[i]-dot*axis[i] for i in range(3)]
   key=(tuple(round(x,5) for x in axis),tuple(round(x,2) for x in perp),cy['internal'])
   if key not in out:
    q=[org[i]+axis[i]*sum((mid[j]-org[j])*axis[j] for j in range(3))-bounds[i] for i in range(3)]
    out[key]={'axis':['X','Y','Z'][k] if abs(axis[k])>.99999 else 'OBL','xyz':q,'internal':cy['internal'],'diameters':set(),'lengths':set(),'direction':axis}
   out[key]['diameters'].add(round(cy['diameter_mm'],3))
   interval=cy['axial_interval_mm']
   if interval:out[key]['lengths'].add(round(abs(interval[1]-interval[0]),3))
 vals=sorted(out.values(),key=lambda q:(not q['internal'],q['axis'],tuple(q['xyz'])))
 for i,a in enumerate(vals,1):a['id']=('H' if a['internal'] else 'E')+str(i).zfill(2);a['diameters']=sorted(a['diameters']);a['lengths']=sorted(a['lengths'])
 return vals

def footer(c,code,title,n,total,notes,material='Conjunto mixto',scale='segun vista'):
 c.setStrokeColorRGB(0,0,0);c.setLineWidth(.2*mm);c.rect(10*mm,10*mm,194*mm,47*mm);c.rect(210*mm,10*mm,200*mm,47*mm)
 text(c,13,51,'NOTAS Y ALCANCE',8,True,ACCENT);y=46
 for note in notes:y=wrapped(c,note,13,y,187,7.3,3.6,maxlines=8)
 text(c,214,51,'AutoFinalRobo2 | Documentacion mecanica',9,True,ACCENT);rule(c,210,47,410,47)
 wrapped(c,title,214,41,190,9.2,4,maxlines=2)
 text(c,214,30,code,8.5,True);text(c,321,30,'REV D | 09/11/2026',7.5)
 rule(c,210,27,410,27)
 text(c,214,23,material[:65],7.2);text(c,214,18,'Koba Chajchir | Cotas nominales CAD',7.5)
 text(c,214,12,'Ajustes / proceso pendientes de cierre; sin aprobacion',7.1,True)
 text(c,332,23,'A3 | mm | Hoja %s/%s'%(n,total),7.3)
 text(c,332,18,'Vistas '+scale+' | 3D sin escala',7.2)
 text(c,332,12,'Proyeccion: primer diedro',7.2)

def header(c,code,title):
 c.setLineWidth(.25*mm);c.rect(5*mm,5*mm,410*mm,287*mm)
 text(c,11,284,code,10.5,True,ACCENT)
 wrapped(c,title,65,284,343,10.3,4.5,maxlines=2)
 text(c,11,273,'Fuente F3D 02/11/2026 | Revision V17 | Extraccion BRep 09/11/2026 | No Drawing asociativo',7.3,color='#555555')

def scale_for(v,box):
 w=max(v['bounds'][2]-v['bounds'][0],.1);h=max(v['bounds'][3]-v['bounds'][1],.1)
 lim=min((box[2]-16)/w,(box[3]-22)/h)
 return next((a for a in [4,2,1,.5,.25,.2,.1,.05,.025] if a<=lim),.01)

def scale_label(s):return fmt(s)+':1' if s>=1 else '1:'+fmt(1/s)

def section_plot(c,s,box):
 if not s['lines']:return
 factor=scale_for(s,box);view(c,s,box,factor)
 # Rayado documental: trazas horizontales dentro de los intervalos del corte.
 lines=s['lines'];bb=s['bounds'];step=3.2/max(factor,.01);start=math.ceil(bb[1]/step)*step
 tr=lambda p:(box[0]+box[2]/2+(p[0]-(bb[0]+bb[2])/2)*factor,box[1]+box[3]/2+(p[1]-(bb[1]+bb[3])/2)*factor)
 y=start
 while y<bb[3]:
  xs=[]
  for l in lines:
   a,b=l['a'],l['b']
   if (a[1]<=y<b[1]) or (b[1]<=y<a[1]):xs.append(a[0]+(b[0]-a[0])*(y-a[1])/(b[1]-a[1]))
  xs.sort()
  for x1,x2 in zip(xs[::2],xs[1::2]):
   if x2-x1>.05:
    a=tr((x1,y));b=tr((x2,y));rule(c,*a,*b,.08,'#9a9a9a')
  y+=step
 text(c,box[0],box[1]-8,'Escala corte '+scale_label(factor)+' | malla CAD tol. 0,12 mm',7.2)

def feature_table(c,features,x=218,y=244):
 cols=[x,x+17,x+35,x+54,x+73,x+92,x+144]
 labels=['EJE','DIR','XQ','YQ','ZQ','DIAMETROS','L TRAMOS']
 for a,t in zip(cols,labels):text(c,a,y,t,7.2,True)
 rule(c,x,y-3,405,y-3);y-=10
 for f in features:
  vals=[f['id'],f['axis']]+[fmt(v) for v in f['xyz']]
  for a,t in zip(cols[:5],vals):text(c,a,y,t,7.2)
  dia=' / '.join(fmt(v) for v in f['diameters']);length=' / '.join(fmt(v) for v in f['lengths'])
  yy=wrapped(c,dia,cols[5],y,47,7.2,3.5,maxlines=5)
  zz=wrapped(c,length,cols[6],y,40,7.2,3.5,maxlines=5)
  y=min(y-8,yy-3,zz-3);rule(c,x,y+2,405,y+2,.08,'#cccccc')

def bom_table(c,items,x=14,y=248,width=188):
 for it in items:
  text(c,x,y,str(it['position']).zfill(3),7.3,True)
  label=it.get('part_code','')+' | '+it['label'].replace('_',' ')
  end=wrapped(c,label,x+14,y,width-17,7.2,3.5,maxlines=3)
  y=min(y-7,end-2);rule(c,x,y+1,x+width,y+1,.08,'#cccccc')

def line_count(s,width,size=7.2):
 count=1;line=''
 for word in s.split():
  trial=(line+' '+word).strip()
  if base.pdfmetrics.stringWidth(trial,'Helvetica',size)>width*mm and line:count+=1;line=word
  else:line=trial
 return count

def pack(items,height_fn,available,maxcount):
 chunks=[];current=[];height=0
 for item in items:
  add=height_fn(item)
  if current and (height+add>available or len(current)>=maxcount):chunks.append(current);current=[];height=0
  current.append(item);height+=add
 if current or not chunks:chunks.append(current)
 return chunks

sheets=[];manifest=[]
for assembly in data['assemblies']:
 sheets.append(('assembly',assembly,None))
 for rows in pack(assembly['items'],lambda it:max(7,3.5*line_count(it.get('part_code','')+' | '+it['label'].replace('_',' '),171)+2),170,18):sheets.append(('bom',assembly,rows))
 for exp in assembly['exploded']:sheets.append(('exploded',assembly,exp))
for p in data['parts']:
 # Cotas envolventes desde BRep, no desde el muestreo de silueta.
 b=p['bounds_mm']
 for v,bb in zip(p['views'],[[b[0],b[2],b[3],b[5]],[-b[4],b[2],-b[1],b[5]],[b[0],b[1],b[3],b[4]]]):v['bounds']=bb
 p['features']=interfaces(p);sheets.append(('part',p,None))
 # Dividir las tablas antes de reducir texto o esconder interfaces.
 height=lambda f:max(8,3.5*max(line_count(' / '.join(fmt(v) for v in f['diameters']),47),line_count(' / '.join(fmt(v) for v in f['lengths']),40))+3)
 for rows in pack(p['features'],height,120,12):sheets.append(('detail',p,rows))

out=H/'AFR2_Planos_Brazo_V17_Documentacion_Dimensional_A3.pdf'
c=canvas.Canvas(str(out),pagesize=(420*mm,297*mm),pageCompression=1)
c.setTitle('AutoFinalRobo2 | Brazo V17 | Documentacion dimensional revision D');c.setAuthor('Koba Chajchir')
sequences={}
for no,(kind,p,extra) in enumerate(sheets,1):
 title=p.get('title','');code=p['code']
 if kind in ('detail','bom'):
  key=(code,kind);sequences[key]=sequences.get(key,0)+1
 if kind=='detail':code+='-I%02d'%sequences[(code,kind)];title='Interfaces y corte | '+title
 elif kind=='bom':code+='-LP%02d'%sequences[(code,kind)];title='Lista de ocurrencias | '+title
 elif kind=='exploded':code+='-E'+str(p['exploded'].index(extra)+1);title='Despiece isometrico | '+title
 header(c,code,title);scale='--'
 c.bookmarkPage(code);c.addOutlineEntry(code+' | '+title,code,0)
 if kind=='part':
  boxes=[(28,159,88,99),(140,159,88,99),(28,81,88,65)]
  factor=min(scale_for(v,b) for v,b in zip(p['views'],boxes));scale=scale_label(factor)
  for v,b in zip(p['views'],boxes):view(c,v,b,factor)
  text(c,249,262,'ISOMETRICA DEL SOLIDO REAL',8,True,ACCENT);imagebox(c,p['render'],246,165,160,90)
  section_plot(c,p['section'],(253,84,143,60))
  dims=[p['bounds_mm'][i+3]-p['bounds_mm'][i] for i in range(3)]
  text(c,139,133,'EXTENSIONES NOMINALES',7.5,True)
  for i,d in enumerate(dims):text(c,139,125-i*5,['X','Y','Z'][i]+' = '+fmt(d)+' mm',7.3)
  text(c,139,104,'Cantidad: '+str(p['occurrences']),7.5)
  text(c,139,96,'Ver hoja -I y CSV de interfaces',7.2)
  notes=['Geometria BRep; visibilidad/siluetas de malla CAD (0,12 mm); aristas a 0,03 mm.','Referencias documentales: Xmin, Ymin, Zmin. No son datums GD&T aprobados.','Cotas nominales e interfaces extraidas; ajustes, tolerancias y proceso no definidos quedan pendientes.']
 elif kind=='detail':
  section_plot(c,p['section'],(30,138,165,100))
  factor=scale_for(p['views'][2],(30,80,165,45));view(c,p['views'][2],(30,80,165,45),factor)
  scale='por vista';feature_table(c,extra)
  text(c,218,262,'COORDENADAS / SUPERFICIES CILINDRICAS',8,True,ACCENT)
  wrapped(c,'Q es un punto del eje en el plano medio del cuerpo. Coordenadas respecto a Xmin/Ymin/Zmin. H: interior; E: exterior. Un eje puede reunir varios escalones.',218,108,185,7.2,3.6,maxlines=5)
  notes=['Los diametros y posiciones proceden de superficies BRep, no de pixels.','Longitudes: tramos cilindricos del CAD; no equivalen a profundidad taladrada ni rosca.','No se cuentan caras como agujeros. Ver datos completos por superficie en interfaces_nativas.json.']
 elif kind=='assembly':
  imagebox(c,p['render'],30,85,365,170)
  notes=['Conjunto armado desde la captura nativa, con sus orientaciones y ubicaciones guardadas.','Lista de ocurrencias en hojas -LP y despieces por tramos -E.','No se modifica la geometria ni se afirma secuencia de montaje/desmontaje comprobada.']
 elif kind=='bom':
  bom_table(c,extra);imagebox(c,p['render'],220,92,185,163)
  notes=['Cada posicion identifica una ocurrencia; los solidos de un mismo comercial se mantienen agrupados.','Las placas con hijos Nano/PCA9685/XL4016 se agrupan como unidades comerciales.','Codigo COMERCIAL/REFERENCIA no implica plano de fabricacion propio. Rutas completas en CSV.']
 else:
  text(c,14,262,'POSICIONES DEL TRAMO',8,True,ACCENT);bom_table(c,extra['items'],14,248,110)
  ix,iy,iw,ih=imagebox(c,extra['render'],136,86,269,170)
  used=[]
  for key,xy in extra['balloons'].items():
   tx,ty=ix+xy[0]*iw,iy+xy[1]*ih;dx,dy=tx-270,ty-170;dist=max(math.hypot(dx,dy),1);dx/=dist;dy/=dist
   bx,by=tx+dx*11,ty+dy*11
   for attempt in range(12):
    if all(math.hypot(bx-x,by-y)>7 for x,y in used):break
    bx+=-dy*5;by+=dx*5
   bx=min(401,max(140,bx));by=min(252,max(80,by));used.append((bx,by))
   rule(c,tx,ty,bx,by,.13,'#555555');c.setFillColorRGB(1,1,1);c.circle(bx*mm,by*mm,3*mm,stroke=1,fill=1)
   c.setFillColorRGB(0,0,0);c.setFont('Helvetica',7.2);c.drawCentredString(bx*mm,(by-.8)*mm,key.zfill(3))
  notes=['Despiece isometrico documental; conserva orientaciones del ensamble.','Las separaciones son desplazamientos de visualizacion, no una trayectoria de desmontaje aprobada.','Todas las posiciones del tramo aparecen en la BOM; piezas pequenas conservan escala relativa.']
 material=p.get('material_note','Conjunto mixto')
 footer(c,code,title,no,len(sheets),notes,material,scale);c.showPage()
 manifest.append({'sheet':no,'code':code,'kind':kind,'title':title,'component':p.get('name'),'status':'cotas nominales extraidas; ajustes/tolerancias pendientes' if kind in ['part','detail'] else 'documentacion de conjunto'})
c.save()
(H/'libro.json').write_text(json.dumps({'date':'2026-11-09','source':data['source'],'source_id':data['source_id'],'sheets':manifest},ensure_ascii=False,indent=2),encoding='utf-8')
with (H/'BOM_FABRICADOS.csv').open('w',newline='',encoding='utf-8-sig') as f:
 w=csv.writer(f);w.writerow(['codigo','componente_CAD','descripcion','cantidad','material','conjuntos','estado'])
 for p in data['parts']:w.writerow([p['code'],p['name'],p['title'],p['occurrences'],p['material_note'],';'.join(p['roots']),'documentado nominal; cierre de proceso/ajuste pendiente'])
with (H/'INTERFACES.csv').open('w',newline='',encoding='utf-8-sig') as f:
 w=csv.writer(f);w.writerow(['codigo','eje_documental','interior','direccion','XQ_mm','YQ_mm','ZQ_mm','diametros_mm','longitudes_tramos_mm','vector_eje_x','vector_eje_y','vector_eje_z'])
 for p in data['parts']:
  for a in p['features']:w.writerow([p['code'],a['id'],a['internal'],a['axis'],*a['xyz'],';'.join(map(str,a['diameters'])),';'.join(map(str,a['lengths'])),*a['direction']])
with (H/'BOM_CONJUNTOS.csv').open('w',newline='',encoding='utf-8-sig') as f:
 w=csv.writer(f);w.writerow(['conjunto','posicion','codigo','componente','ruta_CAD','cantidad'])
 for a in data['assemblies']:
  for it in a['items']:w.writerow([a['code'],it['position'],it['part_code'],it['component'],it['path'],1])
lines=['# Indice de documentacion dimensional V17','','Revision D | 09/11/2026','','| Hoja | Codigo | Contenido |','|---:|---|---|']
for x in manifest:lines.append('| %s | %s | %s |'%(x['sheet'],x['code'],x['title']))
(H/'INDICE.md').write_text('\n'.join(lines)+'\n',encoding='utf-8')
print(json.dumps({'pdf':str(out),'sheets':len(manifest),'parts':len(data['parts']),'assemblies':len(data['assemblies'])}))
