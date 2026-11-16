"""Composición documental D2, sin importar ni ejecutar firmware o CAD.
Base de contenido D1: Koba Chajchir. Adaptación por archivo en README.md.
"""
from pathlib import Path
import json, re, math
from xml.sax.saxutils import escape
from PIL import Image as PILImage, ImageDraw, ImageFont
from reportlab.platypus import (BaseDocTemplate, PageTemplate, Frame, Paragraph, Spacer, Flowable,
    Table, TableStyle, Image, PageBreak, KeepTogether, NextPageTemplate, CondPageBreak)
from reportlab.platypus.tableofcontents import TableOfContents
from reportlab.lib.styles import ParagraphStyle
from reportlab.lib import colors
from reportlab.lib.units import mm
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from docx import Document
from docx.shared import Cm, Pt, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.oxml import OxmlElement
from docx.oxml.ns import qn

HERE=Path(__file__).resolve().parent
DATA=json.loads((HERE/'contenido_informe.json').read_text(encoding='utf-8'))
B=DATA['blocks']
OUT=HERE/'entrega'; OUT.mkdir(exist_ok=True)
QA=HERE/'revision_visual'; QA.mkdir(exist_ok=True)
NAVY='#182B42'; ACCENT='#9C2945'; INK='#263344'; MUTED='#66717F'; PALE='#F1F4F7'
FONT=Path('C:/Windows/Fonts')
pdfmetrics.registerFont(TTFont('AF',(FONT/'arial.ttf').as_posix()))
pdfmetrics.registerFont(TTFont('AFB',(FONT/'arialbd.ttf').as_posix()))
pdfmetrics.registerFont(TTFont('AFI',(FONT/'ariali.ttf').as_posix()))
pdfmetrics.registerFont(TTFont('AFU',(FONT/'ARIALUNI.ttf').as_posix()))
pdfmetrics.registerFontFamily('AF',normal='AF',bold='AFB',italic='AFI',boldItalic='AFB')

def diagram(name,title,rows,foot,sequential=True):
    """Esquema original, no dibujo mecánico ni edición de una imagen existente."""
    W=2000; H=220+len(rows)*210
    im=PILImage.new('RGB',(W,H),'white'); d=ImageDraw.Draw(im)
    fonts={s:ImageFont.truetype(str(FONT/'arial.ttf'),s) for s in [30,32,36,42]}
    bold=ImageFont.truetype(str(FONT/'arialbd.ttf'),44)
    d.text((65,35),title,font=bold,fill=NAVY)
    for j,row in enumerate(rows):
        y=130+j*210; count=len(row); gap=55; width=(W-130-gap*(count-1))/count
        for k,(head,body) in enumerate(row):
            x=65+k*(width+gap)
            d.rounded_rectangle((x,y,x+width,y+145),radius=17,fill=PALE,outline='#CCD5DE',width=3)
            d.rounded_rectangle((x,y,x+8,y+145),radius=4,fill=ACCENT)
            d.text((x+28,y+22),head,font=fonts[36],fill=NAVY)
            d.text((x+28,y+75),body,font=fonts[30],fill=INK)
            if k<count-1 and (sequential or j==0):
                ax=x+width+10; ay=y+73; bx=ax+gap-20
                d.line((ax,ay,bx,ay),fill=ACCENT,width=5)
                d.polygon([(bx,ay),(bx-15,ay-10),(bx-15,ay+10)],fill=ACCENT)
        if j<len(rows)-1 and sequential:
            x=W//2; d.line((x,y+155,x,y+195),fill=ACCENT,width=5)
            d.polygon([(x,y+195),(x-10,y+180),(x+10,y+180)],fill=ACCENT)
    d.text((65,H-55),foot,font=fonts[30],fill=MUTED)
    im.save(HERE/'figuras'/name)

diagram('arquitectura.png','Arquitectura funcional del proyecto',[
 [('Operador','Web en PC o teléfono'),('ESP8266','HTTP · WebSocket · LittleFS'),('ATmega2560','Control · sensores · salidas')],
 [('Interfaz local','OLED 128×64 + encoder'),('Plataforma','Motores · Hall · IR · radar')],
 [('Contracts y CAD','Definiciones y modelos'),('Loader','Diagnóstico y mantenimiento')]],
 'Manipulador: integración del bus STS y potencia pendiente de cierre.',sequential=False)
diagram('cadena.png','Referencias espaciales del manipulador móvil',[
 [('Mundo W','Pose estimada (x, y, ψ)'),('Base B','Transformación planar')],
 [('Montura M','Transformación fija T_BM'),('Brazo 6R','Z · Y · Y · Z · Y · Z')],
 [('Herramienta TCP','Offset distal d = 0,1435 m'),('Pose mundial','T_WB · T_BM · T_MTCP')]],
 'Esquema de composición: aplicar una única vez cada cambio de marco.')
diagram('odometria.png','De las cuentas Hall a la pose estimada',[
 [('Cuatro ruedas','Δn · CPR · signos'),('Distancias','Δs = π D Δn / N')],
 [('Cuerpo de la base','Inversa Mecanum A+ y S'),('Escala direccional','k_x · k_y · k_ψ')],
 [('Rotación global','Rumbo de punto medio'),('Pose publicada','x · y · ψ · validez')]],
 'La pose es relativa al origen y acumula deriva; no es posición absoluta.')

S={
 'body':ParagraphStyle('body',fontName='AF',fontSize=10.2,leading=14.4,textColor=colors.HexColor(INK),spaceAfter=7),
 'chapter':ParagraphStyle('chapter',fontName='AFB',fontSize=23,leading=29,textColor=colors.HexColor(NAVY),spaceAfter=18,keepWithNext=True),
 'heading':ParagraphStyle('heading',fontName='AFB',fontSize=12.5,leading=17,textColor=colors.HexColor(ACCENT),spaceBefore=11,spaceAfter=7,keepWithNext=True),
 'small':ParagraphStyle('small',fontName='AF',fontSize=8.6,leading=12.1,textColor=colors.HexColor(INK),spaceAfter=0),
 'th':ParagraphStyle('th',fontName='AFB',fontSize=8.6,leading=12,textColor=colors.white),
 'eq':ParagraphStyle('eq',fontName='AFU',fontSize=10.5,leading=17,textColor=colors.HexColor(NAVY),spaceAfter=0),
 'matrix':ParagraphStyle('matrix',fontName='AFU',fontSize=10,leading=17,alignment=1,textColor=colors.HexColor(NAVY)),
 'caption':ParagraphStyle('caption',fontName='AF',fontSize=8.3,leading=11.5,textColor=colors.HexColor(MUTED),spaceAfter=12),
 'note':ParagraphStyle('note',fontName='AFB',fontSize=10,leading=15,textColor=colors.HexColor(ACCENT)),
}
SUP={'ᵀ':'T','ˣ':'×','⁰':'0','¹':'1','²':'2','³':'3','⁴':'4','⁵':'5','⁶':'6','⁷':'7','⁸':'8','⁹':'9','⁻':'−','⁺':'+'}
SUB={'ᵢ':'i','ⱼ':'j','₀':'0','₁':'1','₂':'2','₃':'3','₄':'4','₅':'5','₆':'6','₇':'7','₈':'8','₉':'9'}
def rich(t):
    t=escape(str(t))
    for char,val in SUP.items(): t=t.replace(char,'<super>'+val+'</super>')
    for char,val in SUB.items(): t=t.replace(char,'<sub>'+val+'</sub>')
    return t
def mathrich(t):
    t=rich(t)
    t=re.sub(r'([^\W\d_]+)_\(([^)]+)\)',r'\1<sub>\2</sub>',t)
    t=re.sub(r'([^\W\d_]+)_([\w,]+)',r'\1<sub>\2</sub>',t)
    t=re.sub(r'\b([qLrcs])([1-6][0-9]?)\b',r'\1<sub>\2</sub>',t)
    return t
def para(t,sty='body'):
    txt=mathrich(t) if sty in ['eq','matrix'] else rich(t)
    if re.match(r'^R[1-4]\. ',str(t)):
        txt=re.sub(r'https?://[^\s]+',lambda mt:'<link href="'+mt.group(0).rstrip('.')+'" color="'+ACCENT+'">Consultar fuente oficial</link>',txt)
    return Paragraph(txt,S[sty])
WIDTH=170*mm
def table_pdf(b):
    n=len(b['head'])
    if n==2: widths=[55*mm,115*mm]
    elif n==3: widths=([12*mm,95*mm,63*mm] if b['head'][0]=='ID' else [48*mm,48*mm,74*mm])
    elif n==5: widths=[14*mm,55*mm,34*mm,34*mm,33*mm]
    else: widths=[WIDTH/n]*n
    rows=[[para(x,'th') for x in b['head']]]+[[para(x,'small') for x in row] for row in b['rows']]
    t=Table(rows,colWidths=widths,hAlign='LEFT',repeatRows=1,spaceAfter=12)
    cmds=[('BACKGROUND',(0,0),(-1,0),colors.HexColor(NAVY)),('VALIGN',(0,0),(-1,-1),'TOP'),
          ('LEFTPADDING',(0,0),(-1,-1),8),('RIGHTPADDING',(0,0),(-1,-1),8),
          ('TOPPADDING',(0,0),(-1,-1),7),('BOTTOMPADDING',(0,0),(-1,-1),7),
          ('LINEBELOW',(0,1),(-1,-1),0.25,colors.HexColor('#DCE2E8'))]
    for i in range(1,len(rows)):
        if i%2: cmds.append(('BACKGROUND',(0,i),(-1,i),colors.HexColor(PALE)))
    t.setStyle(TableStyle(cmds));return KeepTogether([t]) if b['head'][0]=='Elemento' else t
def matrix_pdf(b):
    rows=[[para(x,'matrix') for x in row] for row in b['rows']]
    n=len(rows[0]); sizes=[]
    for col in range(n):
        sizes.append(max(pdfmetrics.stringWidth(str(row[col]),'AFU',10) for row in b['rows'])+22)
    if sum(sizes)>WIDTH-12:
        sizes=[(WIDTH-12)/n]*n
    t=Table(rows,colWidths=sizes,hAlign='CENTER')
    t.setStyle(TableStyle([('VALIGN',(0,0),(-1,-1),'MIDDLE'),
        ('TOPPADDING',(0,0),(-1,-1),5),('BOTTOMPADDING',(0,0),(-1,-1),5)]))
    class BracketMatrix(Flowable):
        def __init__(self,table):
            Flowable.__init__(self);self.table=table;w,h=table.wrap(WIDTH,1000);self.width=w+16;self.height=h;self.hAlign='CENTER'
        def draw(self):
            c=self.canv;c.saveState();c.setStrokeColor(colors.HexColor(ACCENT));c.setLineWidth(0.9)
            for x,sgn in [(2,1),(self.width-2,-1)]:
                c.line(x,0,x,self.height);c.line(x,0,x+sgn*5,0);c.line(x,self.height,x+sgn*5,self.height)
            self.table.drawOn(c,8,0);c.restoreState()
    return KeepTogether([para(b['label'],'small'),Spacer(1,6),BracketMatrix(t),Spacer(1,11)])

chapter_pages=[]
class Report(BaseDocTemplate):
    def beforeDocument(self):
        self.current_chapter='Informe técnico';self.ch=0;chapter_pages.clear()
    def afterFlowable(self,f):
        if isinstance(f,Paragraph) and f.style.name=='chapter':
            label=f.getPlainText();key='chapter_'+str(self.ch);self.ch+=1
            self.canv.bookmarkPage(key);self.canv.addOutlineEntry(label,key,level=0)
            if label!='Contenido': self.notify('TOCEntry',(0,label,self.page,key))
            chapter_pages.append({'title':label,'page':self.page})
            self.current_chapter=label

def page(c,doc):
    c.saveState()
    if doc.page>1:
        c.setFont('AFB',8);c.setFillColor(colors.HexColor(NAVY))
        c.drawString(20*mm,282*mm,'AUTOFINALROBO2')
        c.setFont('AF',8);c.setFillColor(colors.HexColor(MUTED))
        c.drawRightString(190*mm,282*mm,'Presentación y desarrollo del proyecto')
        c.setFont('AF',8);c.drawString(20*mm,13*mm,'Robótica  |  Revisión D2  |  12 noviembre 2026')
        c.setFillColor(colors.HexColor(ACCENT));c.setFont('AFB',9);c.drawRightString(190*mm,13*mm,str(doc.page))
    else:
        c.setFillColor(colors.HexColor(ACCENT));c.roundRect(20*mm,278*mm,25*mm,3*mm,1.5*mm,fill=1,stroke=0)
    c.restoreState()

pdf=OUT/'AutoFinalRobo2_Informe_Proyecto_D2.pdf'
docpdf=Report(str(pdf),pagesize=(210*mm,297*mm),leftMargin=20*mm,rightMargin=20*mm,topMargin=25*mm,bottomMargin=23*mm,
    title='AutoFinalRobo2 Informe de presentación y desarrollo',author='Koba Chajchir')
docpdf.addPageTemplates(PageTemplate(id='main',frames=[Frame(20*mm,23*mm,170*mm,249*mm,leftPadding=0,rightPadding=0,topPadding=0,bottomPadding=0)],onPage=page))
story=[]
cover_title=ParagraphStyle('cover_title',fontName='AFB',fontSize=36,leading=41,textColor=colors.black,spaceAfter=11)
cover_sub=ParagraphStyle('cover_sub',fontName='AF',fontSize=17,leading=23,textColor=colors.HexColor(NAVY),spaceAfter=8)
story.extend([Spacer(1,12*mm),Paragraph('AutoFinalRobo2',cover_title),
    Paragraph('Informe de presentación y desarrollo<br/>del proyecto de robótica',cover_sub),
    para('Plataforma móvil Mecanum y manipulador antropomorfo de seis ejes'),Spacer(1,6*mm)])
hero=Image(str(HERE/'figuras/robot_ensamblado.png'));hero.drawWidth=130*mm;hero.drawHeight=112*mm
story.extend([hero,Spacer(1,3*mm),para('Representación CAD v17 del conjunto. Geometría de diseño.','caption')])
story.append(table_pdf(table:= {'head':['Datos académicos','Completar en la versión editable'], 'rows':[
 ['Institución y asignatura','[Completar]'],['Docente y comisión','[Completar]'],['Integrantes','[Completar]']], 'widths':None}))
story.extend([para('Autoría documental de base: Koba Chajchir.','small'),Spacer(1,3*mm),
    para('Revisión D2 · 12 de noviembre de 2026','small'),PageBreak(),para('Contenido','chapter')])
toc=TableOfContents();toc.levelStyles=[ParagraphStyle('toc',fontName='AF',fontSize=9.4,leading=11.5,leftIndent=0,firstLineIndent=0,spaceBefore=2,spaceAfter=0,textColor=colors.HexColor(INK))]
toc.tableStyle=TableStyle([('VALIGN',(0,0),(-1,-1),'TOP'),('TOPPADDING',(0,0),(-1,-1),0),('BOTTOMPADDING',(0,0),(-1,-1),0)])
story.extend([para('Proyecto, modelos matemáticos, desarrollo, resultados y continuidad. Las páginas corresponden a esta edición PDF.','caption'),toc])
fig_count=0; eq_count=0
for b in B:
    if b is B[0]: pass
# La portada del contenido no se repite.
first_chapter=next(i for i,b in enumerate(B) if b['kind']=='chapter' and b['text'].startswith('1.'))
for b in B[first_chapter:]:
    k=b['kind']
    if k=='chapter':story.extend([PageBreak(),para(b['text'],'chapter')])
    elif k=='heading':story.append(para(b['text'],'heading'))
    elif k=='paragraph':story.append(para(b['text']))
    elif k=='equation':
        eq_count+=1
        row=Table([[para(b['text'],'eq'),para(f'({eq_count:02d})','caption')]],colWidths=[WIDTH-38,38],hAlign='LEFT',spaceAfter=8,spaceBefore=3)
        row.setStyle(TableStyle([('BACKGROUND',(0,0),(-1,-1),colors.HexColor(PALE)),('VALIGN',(0,0),(-1,-1),'MIDDLE'),('TOPPADDING',(0,0),(-1,-1),7),('BOTTOMPADDING',(0,0),(-1,-1),7),('LEFTPADDING',(0,0),(-1,-1),8)]))
        story.append(row)
    elif k=='table':story.append(table_pdf(b))
    elif k=='matrix':story.append(matrix_pdf(b))
    elif k=='callout':
        t=Table([[para(b['text'],'note')]],colWidths=[WIDTH],spaceAfter=12,spaceBefore=4)
        t.setStyle(TableStyle([('BACKGROUND',(0,0),(-1,-1),colors.HexColor('#FAEFF2')),('BOX',(0,0),(-1,-1),0,colors.white),('LEFTPADDING',(0,0),(-1,-1),12),('RIGHTPADDING',(0,0),(-1,-1),12),('TOPPADDING',(0,0),(-1,-1),10),('BOTTOMPADDING',(0,0),(-1,-1),10)]));story.append(t)
    elif k=='image':
        fig_count+=1
        im=Image(str(HERE/b['path']));scale=min(WIDTH/im.imageWidth,105*mm/im.imageHeight)
        im.drawWidth=im.imageWidth*scale;im.drawHeight=im.imageHeight*scale
        story.append(KeepTogether([im,Spacer(1,5),para(f'Figura {fig_count}. '+b['caption'],'caption')]))
docpdf.multiBuild(story)
(HERE/'indice_paginas.json').write_text(json.dumps(chapter_pages,ensure_ascii=False,indent=2),encoding='utf-8')

# Word conserva texto, tablas, matrices OMML y campos académicos editables.
doc=Document();sec=doc.sections[0]
sec.page_width=Cm(21);sec.page_height=Cm(29.7);sec.top_margin=Cm(2.2);sec.bottom_margin=Cm(2.2);sec.left_margin=Cm(2);sec.right_margin=Cm(2)
sec.different_first_page_header_footer=True
normal=doc.styles['Normal'];normal.font.name='Arial';normal.font.size=Pt(10.5)
normal.paragraph_format.line_spacing=1.16;normal.paragraph_format.space_after=Pt(8)
for name,size,col in [('Title',34,'000000'),('Subtitle',16,'182B42'),('Heading 1',23,'182B42'),('Heading 2',12.5,'9C2945')]:
    s=doc.styles[name];s.font.name='Arial';s.font.size=Pt(size);s.font.color.rgb=RGBColor.from_string(col)
    s.paragraph_format.keep_with_next=True
doc.styles['Heading 1'].paragraph_format.space_after=Pt(16)
doc.styles['Heading 2'].paragraph_format.space_before=Pt(12)
doc.styles['Caption'].font.name='Arial';doc.styles['Caption'].font.size=Pt(8.5);doc.styles['Caption'].font.color.rgb=RGBColor.from_string('66717F')
hp=sec.header.paragraphs[0];hp.text='AUTOFINALROBO2     /     Presentación y desarrollo del proyecto'
for r in hp.runs:r.font.size=Pt(8);r.font.color.rgb=RGBColor.from_string('66717F')
fp=sec.footer.paragraphs[0];fp.text='Robótica  |  Revisión D2  |  12 noviembre 2026                              '
fld=OxmlElement('w:fldSimple');fld.set(qn('w:instr'),'PAGE');fp._p.append(fld)
for r in fp.runs:r.font.size=Pt(8)
doc.add_paragraph('AutoFinalRobo2','Title')
doc.add_paragraph('Informe de presentación y desarrollo\ndel proyecto de robótica','Subtitle')
doc.add_paragraph('Plataforma móvil Mecanum y manipulador antropomorfo de seis ejes')
pp=doc.add_paragraph();pp.alignment=1;pp.add_run().add_picture(str(HERE/'figuras/robot_ensamblado.png'),height=Cm(10.5))
doc.add_paragraph('Representación CAD v17 del conjunto. Geometría de diseño.','Caption')

def control(paragraph,label):
    sdt=OxmlElement('w:sdt');pr=OxmlElement('w:sdtPr');alias=OxmlElement('w:alias');alias.set(qn('w:val'),label);pr.append(alias);pr.append(OxmlElement('w:text'));sdt.append(pr)
    content=OxmlElement('w:sdtContent');rr=OxmlElement('w:r');tt=OxmlElement('w:t');tt.text='[Completar '+label.lower()+']';rr.append(tt);content.append(rr);sdt.append(content);paragraph._p.append(sdt)
for label in ['Institución','Asignatura','Docente','Integrantes','Comisión y ciclo lectivo']:
    pp=doc.add_paragraph(label+': ');control(pp,label);pp.paragraph_format.space_after=Pt(5)
doc.add_paragraph('Autoría documental de base: Koba Chajchir.\nRevisión D2 · 12 de noviembre de 2026.')
doc.add_page_break();doc.add_heading('Contenido',1)
doc.add_paragraph('Índice de capítulos. La paginación del PDF está fijada para lectura; Word ajusta las páginas según el entorno de edición.')
for b in B[first_chapter:]:
    if b['kind']=='chapter':
        pp=doc.add_paragraph(b['text']);pp.paragraph_format.space_after=Pt(4)
        for rr in pp.runs:rr.font.size=Pt(10)

def mr(t):
    r=OxmlElement('m:r');rpr=OxmlElement('w:rPr');ff=OxmlElement('w:rFonts');ff.set(qn('w:ascii'),'Cambria Math');ff.set(qn('w:hAnsi'),'Cambria Math');rpr.append(ff);r.append(rpr)
    tx=OxmlElement('m:t');tx.text=str(t);r.append(tx);return r
def shade(cell,color):
    pr=cell._tc.get_or_add_tcPr();el=OxmlElement('w:shd');el.set(qn('w:fill'),color);pr.append(el)
eq_count=0;fig_count=0
for b in B[first_chapter:]:
    k=b['kind']
    if k=='chapter':doc.add_page_break();doc.add_heading(b['text'],1)
    elif k=='heading':doc.add_heading(b['text'],2)
    elif k in ['paragraph','callout']:
        pp=doc.add_paragraph(b['text'])
        if k=='callout':
            for rr in pp.runs:rr.bold=True;rr.font.color.rgb=RGBColor.from_string('9C2945')
    elif k=='equation':
        eq_count+=1;pp=doc.add_paragraph();om=OxmlElement('m:oMath');om.append(mr(b['text']));pp._p.append(om)
        pp.add_run(f'    ({eq_count:02d})').font.size=Pt(8)
        pr=pp._p.get_or_add_pPr();sh=OxmlElement('w:shd');sh.set(qn('w:fill'),'F1F4F7');pr.append(sh)
    elif k=='matrix':
        pp=doc.add_paragraph(b['label']);pp.paragraph_format.keep_with_next=True
        pp=doc.add_paragraph();pp.alignment=1;om=OxmlElement('m:oMath');de=OxmlElement('m:d');dp=OxmlElement('m:dPr')
        for key,char in [('begChr','['),('endChr',']')]:el=OxmlElement('m:'+key);el.set(qn('m:val'),char);dp.append(el)
        de.append(dp);en=OxmlElement('m:e');mat=OxmlElement('m:m')
        for row in b['rows']:
            rr=OxmlElement('m:mr')
            for val in row:el=OxmlElement('m:e');el.append(mr(val));rr.append(el)
            mat.append(rr)
        en.append(mat);de.append(en);om.append(de);pp._p.append(om)
    elif k=='table':
        t=doc.add_table(rows=1,cols=len(b['head']));t.autofit=False
        n=len(b['head']);widths=[5.5,11.5] if n==2 else (([1.2,9.5,6.3] if b['head'][0]=='ID' else [4.8,4.8,7.4]) if n==3 else [1.4,5.5,3.4,3.4,3.3] if n==5 else [17/n]*n)
        for c,w in zip(t.columns,widths):c.width=Cm(w)
        for c,txt in zip(t.rows[0].cells,b['head']):c.text=txt;shade(c,'182B42')
        hdr=OxmlElement('w:tblHeader');t.rows[0]._tr.get_or_add_trPr().append(hdr)
        for i,row in enumerate(b['rows']):
            cells=t.add_row().cells
            for c,txt in zip(cells,row):
                c.text=str(txt)
                if i%2==0:shade(c,'F1F4F7')
        for i,row in enumerate(t.rows):
            row._tr.get_or_add_trPr().append(OxmlElement('w:cantSplit'))
            for c,w in zip(row.cells,widths):
                c.width=Cm(w)
                for pp in c.paragraphs:
                    pp.paragraph_format.space_after=Pt(5);pp.paragraph_format.space_before=Pt(5)
                    for rr in pp.runs:
                        rr.font.size=Pt(8.5)
                        if i==0:rr.font.color.rgb=RGBColor(255,255,255);rr.bold=True
        doc.add_paragraph().paragraph_format.space_after=Pt(2)
    elif k=='image':
        fig_count+=1
        with PILImage.open(HERE/b['path']) as im:w,h0=im.size
        scale=min(16/w,10.5/h0)
        pp=doc.add_paragraph();pp.alignment=1;pp.paragraph_format.keep_with_next=True
        pp.add_run().add_picture(str(HERE/b['path']),width=Cm(w*scale))
        doc.add_paragraph(f'Figura {fig_count}. '+b['caption'],'Caption')
doc.core_properties.title='AutoFinalRobo2 Informe de presentación y desarrollo del proyecto'
doc.core_properties.author='Koba Chajchir'
doc.core_properties.subject='Robótica móvil, manipulación, modelos matemáticos y desarrollo de proyecto'
doc.core_properties.comments='Revisión D2. Adaptación documental del borrador D1. Campos académicos editables.'
doc.save(OUT/'AutoFinalRobo2_Informe_Proyecto_D2.docx')

# Fuente de contenido íntegra para posteriores revisiones.
md=['# AutoFinalRobo2\n\nInforme de presentación y desarrollo del proyecto de robótica\n\nRevisión D2 · 12/11/2026\n']
for b in B[first_chapter:]:
    k=b['kind']
    if k=='chapter':md.append('\n# '+b['text']+'\n')
    elif k=='heading':md.append('\n## '+b['text']+'\n')
    elif k in ['paragraph','callout']:md.append(b['text']+'\n')
    elif k=='equation':md.append('```text\n'+b['text']+'\n```\n')
    elif k=='matrix':md.append(b['label']+'\n\n```text\n'+'\n'.join('[ '+' | '.join(row)+' ]' for row in b['rows'])+'\n```\n')
    elif k=='table':md.append('| '+' | '.join(b['head'])+' |\n| '+' | '.join(['---']*len(b['head']))+' |\n'+'\n'.join('| '+' | '.join(map(str,row))+' |' for row in b['rows'])+'\n')
    elif k=='image':md.append('!['+b['caption']+']('+b['path']+')\n')
(HERE/'AutoFinalRobo2_Informe_Proyecto_D2.md').write_text('\n'.join(md),encoding='utf-8')
print(json.dumps({'pdf':str(pdf),'docx':str(OUT/'AutoFinalRobo2_Informe_Proyecto_D2.docx'),'ecuaciones':eq_count,'matrices':sum(x['kind']=='matrix' for x in B),'tablas':sum(x['kind']=='table' for x in B),'figuras':fig_count,'paginas':max(x['page'] for x in chapter_pages)},ensure_ascii=False))
