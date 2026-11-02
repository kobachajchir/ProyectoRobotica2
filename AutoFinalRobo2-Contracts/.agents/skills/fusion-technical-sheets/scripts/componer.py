"""Compositor A3: vistas CAD vectoriales y render, notas separadas del rotulo."""
import json,sys,math
from pathlib import Path
from reportlab.pdfgen import canvas
from reportlab.lib.units import mm
from reportlab.lib.utils import ImageReader
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from PIL import Image

# Incrustar fuentes cuando estan disponibles para conservar acentos y diametros.
for normal,bold in [('C:/Windows/Fonts/arial.ttf','C:/Windows/Fonts/arialbd.ttf'),('/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf','/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf')]:
 if Path(normal).exists() and Path(bold).exists():
  pdfmetrics.registerFont(TTFont('Helvetica',normal));pdfmetrics.registerFont(TTFont('Helvetica-Bold',bold));break

ACCENT='#a91843'
def text(c,x,y,s,size=8,bold=False,color='#111111'):
 c.setFillColor(color);c.setFont('Helvetica-Bold' if bold else 'Helvetica',size);c.drawString(x*mm,y*mm,str(s))
def rule(c,x1,y1,x2,y2,width=.2,color='#111111'):
 c.setStrokeColor(color);c.setLineWidth(width*mm);c.line(x1*mm,y1*mm,x2*mm,y2*mm)
def wrapped(c,s,x,y,w,size=8,leading=4,maxlines=20):
 lines=[];line=''
 for word in s.split():
  trial=(line+' '+word).strip()
  if c.stringWidth(trial,'Helvetica',size)>w*mm and line:lines.append(line);line=word
  else:line=trial
 if line:lines.append(line)
 if len(lines)>maxlines:raise ValueError('Texto excede su contenedor: '+s[:80])
 for i,l in enumerate(lines):text(c,x,y-i*leading,l,size)
 return y-len(lines)*leading
def imagebox(c,path,x,y,w,h):
 source=Image.open(path).convert('RGBA');ow,oh=source.size
 bb=source.getchannel('A').getbbox() or (0,0,ow,oh)
 pad=max(3,int(max(bb[2]-bb[0],bb[3]-bb[1])*.035))
 bb=(max(0,bb[0]-pad),max(0,bb[1]-pad),min(ow,bb[2]+pad),min(oh,bb[3]+pad))
 crop=source.crop(bb);iw,ih=crop.size;scale=min(w/iw,h/ih);dw,dh=iw*scale,ih*scale
 dx=x+(w-dw)/2;dy=y+(h-dh)/2
 c.drawImage(ImageReader(crop),dx*mm,dy*mm,dw*mm,dh*mm,mask='auto')
 # Afin para globos expresados en coordenadas normalizadas del PNG original.
 return dx-bb[0]*scale,dy-(oh-bb[3])*scale,ow*scale,oh*scale
def dim(c,a,b,offset,label,horizontal=True):
 x1,y1=a;x2,y2=b
 if horizontal:
  yd=min(y1,y2)-offset;rule(c,x1,y1-1,x1,yd-1,.12);rule(c,x2,y2-1,x2,yd-1,.12);rule(c,x1,yd,x2,yd,.15)
  for x in [x1,x2]:rule(c,x-.7,yd-.7,x+.7,yd+.7,.18)
  tw=c.stringWidth(label,'Helvetica',7.2)/mm;c.setFillColorRGB(1,1,1);c.rect(((x1+x2)/2-tw/2-1)*mm,(yd-.9)*mm,(tw+2)*mm,3*mm,stroke=0,fill=1);text(c,(x1+x2)/2-tw/2,yd+.2,label,7.2)
 else:
  xd=min(x1,x2)-offset;rule(c,x1-1,y1,xd-1,y1,.12);rule(c,x2-1,y2,xd-1,y2,.12);rule(c,xd,y1,xd,y2,.15)
  for y in [y1,y2]:rule(c,xd-.7,y-.7,xd+.7,y+.7,.18)
  c.saveState();c.translate((xd-1)*mm,(y1+y2)/2*mm);c.rotate(90);c.setFillColorRGB(1,1,1);tw=c.stringWidth(label,'Helvetica',7.2);c.rect(-tw/2-2,-1,tw+4,9,fill=1,stroke=0);c.setFillColorRGB(0,0,0);c.setFont('Helvetica',7.2);c.drawCentredString(0,0,label);c.restoreState()
def fmt(v):return ('%.2f'%v).rstrip('0').rstrip('.')
def view(c,v,box,scale):
 x,y,w,h=box;b=v['bounds'];cx=(b[0]+b[2])/2;cy=(b[1]+b[3])/2
 tr=lambda p:(x+w/2+(p[0]-cx)*scale,y+h/2+(p[1]-cy)*scale)
 text(c,x,y+h+5,v['title'],8,True,ACCENT)
 c.saveState()
 for hidden in [True,False]:
  c.setDash(1.1*mm,.8*mm) if hidden else c.setDash()
  c.setStrokeColorRGB(.55,.55,.55) if hidden else c.setStrokeColorRGB(0,0,0)
  c.setLineWidth((.09 if hidden else .15)*mm)
  p=c.beginPath()
  for line in v['lines']:
   if line['hidden']!=hidden:continue
   a=tr(line['a']);bb=tr(line['b']);p.moveTo(a[0]*mm,a[1]*mm);p.lineTo(bb[0]*mm,bb[1]*mm)
  c.drawPath(p)
 c.restoreState()
 mn=tr(b[:2]);mx=tr(b[2:]);dim(c,(mn[0],mn[1]),(mx[0],mn[1]),5,fmt(b[2]-b[0]));dim(c,(mn[0],mn[1]),(mn[0],mx[1]),5,fmt(b[3]-b[1]),False)
def footer(c,book,sheet,n,total,scale):
 # Contenedores hermanos y disjuntos: notas10..204; rotulo210..410.
 c.setStrokeColorRGB(0,0,0);c.setLineWidth(.2*mm);c.rect(10*mm,10*mm,194*mm,47*mm);c.rect(210*mm,10*mm,200*mm,47*mm)
 text(c,13,51,'NOTAS / ALCANCE',8,True,ACCENT);y=46
 for note in sheet.get('notes',[]):y=wrapped(c,note,13,y,187,7.5,3.7,maxlines=9)
 if y<12:raise ValueError('Notas invaden margen: '+sheet['code'])
 text(c,214,51,book['project'],10,True,ACCENT);text(c,338,51,book.get('author','Autor sin asignar'),8);rule(c,210,47,410,47)
 text(c,214,41,sheet['title'],11,True);text(c,214,35,sheet['code'],9,True)
 text(c,331,35,'REV '+sheet.get('revision','A')+'  |  '+book['date'],8)
 rule(c,210,31,410,31);rule(c,328,10,328,31)
 text(c,214,27,'MATERIAL / PROCESO',6.8,color='#555555');text(c,214,22,sheet.get('material','Conjunto mixto'),8)
 text(c,214,17,'ESTADO: DOCUMENTACION PRELIMINAR',7.3,True);text(c,214,12,'Unidades: mm  |  Proyeccion: primer diedro',7.3)
 text(c,332,26,'Escala vistas: '+scale,7.6);text(c,332,21,'Isometrica: sin escala',7.6);text(c,332,16,'Formato A3  |  Hoja '+str(n)+' / '+str(total),7.6);text(c,332,11,'AutoFinalRobo2  |  Sin firma de aprobacion',6.8)
def bom(c,rows,x=15,y=258,w=182):
 text(c,x,y+10,'LISTA DE PIEZAS',9,True,ACCENT);rule(c,x,y+6,x+w,y+6)
 text(c,x,y,'POS. / CODIGO',7.3,True);text(c,x+33,y,'DESCRIPCION',7.3,True);text(c,x+w-9,y,'CANT.',7.3,True)
 y-=7
 for row in rows:
  if y<75:raise ValueError('BOM larga: dividir hojas')
  text(c,x,y,row['id'],7.2);label=row['label'];space=w-49
  if c.stringWidth(label,'Helvetica',7.2)>space*mm:
   while c.stringWidth(label+'...','Helvetica',7.2)>space*mm:label=label[:-1]
   label+='...'
  text(c,x+33,y,label,7.2);text(c,x+w-6,y,row.get('qty',1),7.2);rule(c,x,y-2,x+w,y-2,.08,'#cccccc');y-=5.2
def compose(book,out):
 c=canvas.Canvas(str(out),pagesize=(420*mm,297*mm),pageCompression=1);c.setTitle(book['project']+' - laminas preliminares');c.setAuthor(book.get('author','AutoFinalRobo2'))
 for n,s in enumerate(book['sheets'],1):
  c.setLineWidth(.3*mm);c.rect(5*mm,5*mm,410*mm,287*mm)
  text(c,11,284,s['code']+'  /  '+s['title'].upper(),11,True,ACCENT)
  text(c,11,277,book.get('source','CAD nativo')+'  |  Cotas de geometria; no de imagen.',7.1,color='#555555')
  scale='--'
  if s.get('views'):
   boxes=[(24,156,74,99),(119,156,74,99),(24,80,74,62)]
   limit=min(min((b[2]-16)/max(v['bounds'][2]-v['bounds'][0],1),(b[3]-20)/max(v['bounds'][3]-v['bounds'][1],1)) for v,b in zip(s['views'],boxes))
   candidates=[4,2,1,.5,.25,.2,.1,.05];factor=next((a for a in candidates if a<=limit),.025)
   scale=(fmt(factor)+':1') if factor>=1 else ('1:'+fmt(1/factor))
   for v,b in zip(s['views'],boxes):view(c,v,b,factor)
  elif s.get('bom'):bom(c,s['bom'],w=103 if s['kind']=='exploded' else 182)
  elif s.get('general_rows'):
   bom(c,s['general_rows']);text(c,15,82,'Referencia de conjuntos; ver inventario completo adjunto.',8)
  exploded=s['kind']=='exploded'
  text(c,132 if exploded else 222,263,'VISTA 3D / '+('EXPLOSION ISOMETRICA' if exploded else 'CONJUNTO' if s['kind']!='part' else 'ISOMETRICA'),8,True,ACCENT)
  image_rect=imagebox(c,s['render'],132,89,272,163) if exploded else imagebox(c,s['render'],211,105,195,149)
  if s.get('balloons'):
   ix,iy,iw,ih=image_rect
   # Dos rieles para globos; no colocar texto sobre el render.
   for i,(label,p) in enumerate(s['balloons'].items()):
    tx=ix+p[0]*iw;ty=iy+p[1]*ih;bx=(142+i*35) if exploded else (220+i*23);by=253 if i%2==0 else 80
    rule(c,bx,by,tx,ty,.12,'#555555');c.setFillColorRGB(1,1,1);c.setStrokeColorRGB(0,0,0);c.circle(bx*mm,by*mm,3*mm,stroke=1,fill=1);c.setFillColorRGB(0,0,0);c.setFont('Helvetica',7.5);c.drawCentredString(bx*mm,(by-.8)*mm,label)
  y=95
  if s.get('diameters'):
   text(c,218,y,'SUPERFICIES CILINDRICAS - DIAMETROS GEOMETRICOS',7.7,True);y-=5
   y=wrapped(c,' / '.join('Ø '+fmt(x) for x in s['diameters']),218,y,184,7.5,3.7,maxlines=4)
  if s.get('mass_note'):wrapped(c,s['mass_note'],218,y-2,184,7.5,3.7,maxlines=3)
  footer(c,book,s,n,len(book['sheets']),scale);c.showPage()
 c.save()
if __name__=='__main__':
 book=json.loads(Path(sys.argv[1]).read_text(encoding='utf-8'));compose(book,Path(sys.argv[2]))
