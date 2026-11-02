"""Numeros locales con lider corto; sustituye solo paginas de despiece."""
from pathlib import Path
import json,math,importlib.util
import numpy as np
from PIL import Image
from pypdf import PdfReader,PdfWriter
H=Path(__file__).resolve().parent
book=json.loads((H/'libro.json').read_text(encoding='utf-8'))
report={};changed=[]
def distance(a,b):return math.hypot(a[0]-b[0],a[1]-b[1])
def cross(a,b,c):return (b[0]-a[0])*(c[1]-a[1])-(b[1]-a[1])*(c[0]-a[0])
def intersects(a,b,c,d):return cross(a,b,c)*cross(a,b,d)<0 and cross(c,d,a)*cross(c,d,b)<0
for index,s in enumerate(book['sheets']):
 if s['kind']!='exploded':continue
 im=Image.open(s['render']).convert('RGBA');ow,oh=im.size;alpha=im.getchannel('A');bb=alpha.getbbox();pad=max(3,int(max(bb[2]-bb[0],bb[3]-bb[1])*.035));bb=(max(0,bb[0]-pad),max(0,bb[1]-pad),min(ow,bb[2]+pad),min(oh,bb[3]+pad))
 cw,ch=bb[2]-bb[0],bb[3]-bb[1];scale=min(272/cw,163/ch);dx=132+(272-cw*scale)/2;dy=89+(163-ch*scale)/2
 ix=dx-bb[0]*scale;iy=dy-(oh-bb[3])*scale;iw=ow*scale;ih=oh*scale
 mask=np.asarray(alpha)>40;integral=np.pad(mask.astype(np.int32),((1,0),(1,0))).cumsum(0).cumsum(1)
 def occupied(x,y):
  px=int((x-ix)/iw*ow);py=int(oh-(y-iy)/ih*oh)
  return bool(mask[py,px]) if 0<=px<ow and 0<=py<oh else False
 def empty_badge(x,y,r=3.5):
  if not (136<x<405 and 78<y<257):return False
  x1=max(0,min(ow,int((x-r-ix)/iw*ow)));x2=max(0,min(ow,int((x+r-ix)/iw*ow)+1))
  y1=max(0,min(oh,int(oh-(y+r-iy)/ih*oh)));y2=max(0,min(oh,int(oh-(y-r-iy)/ih*oh)+1))
  return integral[y2,x2]-integral[y1,x2]-integral[y2,x1]+integral[y1,x1]==0
 placed={};segments=[]
 for label,p in s['balloons'].items():
  center=(ix+p[0]*iw,iy+p[1]*ih);candidates=[]
  for angle_i in range(72):
   angle=angle_i*math.tau/72;ux,uy=math.cos(angle),math.sin(angle);hit=False;edge=None
   for step in range(0,401):
    r=step*.25;x=center[0]+ux*r;y=center[1]+uy*r
    if occupied(x,y):hit=True
    elif hit:edge=(x-ux*.2,y-uy*.2);break
    elif r>12:break
   if edge is None:continue
   for gap in [5.5,6.5,8.]:
    badge=(edge[0]+ux*gap,edge[1]+uy*gap)
    if not empty_badge(*badge):continue
    if any(distance(badge,x['badge_mm'])<7 for x in placed.values()):continue
    if any(occupied(edge[0]+ux*t,edge[1]+uy*t) for t in np.arange(.5,gap-2.7,.3)):continue
    if any(intersects(edge,badge,a,b) for a,b in segments):continue
    # Un lider no atraviesa un globo ya colocado.
    if any(distance(edge,x['badge_mm'])<4 for x in placed.values()):continue
    score=gap+.035*distance(center,edge)
    candidates.append((score,edge,badge));break
  if not candidates:raise RuntimeError('Ubicacion local pendiente: '+s['code']+' / '+label)
  _,edge,badge=min(candidates,key=lambda x:x[0]);placed[label]={'anchor_mm':list(edge),'badge_mm':list(badge)};segments.append((edge,badge))
 s['local_balloons']=placed;report[s['code']]=placed;changed.append(index)
composer=H.parents[4]/'AutoFinalRobo2-Contracts/.agents/skills/fusion-technical-sheets/scripts/componer.py'
spec=importlib.util.spec_from_file_location('laminas',composer);m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
temporary=H/'cache/llamadas_locales_libro.pdf';m.compose(book,temporary)
target=H/'AFR2_Laminas_Iniciales_A3.pdf';original=PdfReader(target);replacement=PdfReader(temporary);writer=PdfWriter()
# Conservar los objetos de pagina originales para todas las hojas ajenas.
for i,page in enumerate(original.pages):writer.add_page(replacement.pages[i] if i in changed else page)
if original.metadata:writer.add_metadata({k:str(v) for k,v in original.metadata.items() if v is not None})
out=H/'cache/libro_solo_despieces_actualizados.pdf'
with out.open('wb') as fp:writer.write(fp)
out.replace(target)
(H/'libro.json').write_text(json.dumps(book,indent=2),encoding='utf-8')
(H/'llamadas_cortas.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps({'modified_pdf_pages':[i+1 for i in changed],'local_references':sum(len(x) for x in report.values()),'other_pages':'originales conservadas'}))
