"""Render the authored AFR2 pinout report; no firmware tools or checks are run."""
from pathlib import Path
import re
from xml.sax.saxutils import escape
from reportlab.pdfgen import canvas
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle, PageBreak, Preformatted, KeepTogether
from reportlab.lib.styles import ParagraphStyle
from reportlab.lib import colors
from reportlab.lib.enums import TA_LEFT
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'hardware/INFORME_PINOUT_ROBOT_20261106.md'
OUTPUT = ROOT / 'hardware/output/pdf/INFORME_PINOUT_ROBOT_20261106.pdf'
for name, file in [('Body', 'arial.ttf'), ('Bold', 'arialbd.ttf'), ('Italic', 'ariali.ttf')]:
    pdfmetrics.registerFont(TTFont(name, 'C:/Windows/Fonts/' + file))
pdfmetrics.registerFontFamily('Body', normal='Body', bold='Bold', italic='Italic', boldItalic='Bold')
INK = colors.HexColor('#172838')
ACCENT = colors.HexColor('#007D83')
styles = {
    'body': ParagraphStyle('body', fontName='Body', fontSize=9.1, leading=12.5, textColor=INK, spaceAfter=7),
    'h1': ParagraphStyle('h1', fontName='Bold', fontSize=25, leading=29, textColor=INK, spaceAfter=14),
    'h2': ParagraphStyle('h2', fontName='Bold', fontSize=16, leading=20, textColor=ACCENT, spaceAfter=12),
    'h3': ParagraphStyle('h3', fontName='Bold', fontSize=11.2, leading=15, textColor=INK, spaceBefore=7, spaceAfter=8),
    'cell': ParagraphStyle('cell', fontName='Body', fontSize=8, leading=10.6, textColor=INK),
    'source': ParagraphStyle('source', fontName='Body', fontSize=7.6, leading=10, textColor=INK, spaceAfter=4),
    'head': ParagraphStyle('head', fontName='Bold', fontSize=8, leading=10.6, textColor=colors.white),
    'code': ParagraphStyle('code', fontName='Courier', fontSize=8, leading=11, backColor=colors.HexColor('#EDF5F5'), borderPadding=8, spaceAfter=10),
}

def fmt(text):
    text = escape(text)
    text = re.sub(r'\*\*(.+?)\*\*', r'<b>\1</b>', text)
    text = re.sub(r'`([^`]+)`', r'\1', text)
    return text

def para(text, kind='body'):
    return Paragraph(fmt(text), styles[kind])

class NumberedCanvas(canvas.Canvas):
    def __init__(self, *args, **kwargs):
        canvas.Canvas.__init__(self, *args, **kwargs)
        self.saved = []
    def showPage(self):
        self.saved.append(dict(self.__dict__))
        self._startPage()
    def save(self):
        total = len(self.saved)
        for state in self.saved:
            self.__dict__.update(state)
            self.setStrokeColor(ACCENT)
            self.setLineWidth(1)
            self.line(38, 801, 557, 801)
            self.setFont('Bold', 8)
            self.setFillColor(INK)
            self.drawString(38, 812, 'AFR2  /  PINOUT DEL ROBOT')
            self.setFont('Body', 7.5)
            self.drawRightString(557, 812, '06.11.2026')
            self.setFillColor(colors.HexColor('#566978'))
            self.drawString(38, 25, 'AFR2-20261106-15  |  Documentación / sin nuevas pruebas físicas')
            self.drawRightString(557, 25, f'{self._pageNumber} / {total}')
            canvas.Canvas.showPage(self)
        canvas.Canvas.save(self)

story = []
lines = SOURCE.read_text(encoding='utf-8').splitlines()
i = 0
while i < len(lines):
    line = lines[i].strip()
    if not line:
        i += 1
        continue
    if line == '<!-- pagebreak -->':
        story.append(PageBreak()); i += 1; continue
    if line.startswith('```'):
        block = []; i += 1
        while i < len(lines) and not lines[i].startswith('```'):
            block.append(lines[i]); i += 1
        story.append(Preformatted('\n'.join(block), styles['code'])); i += 1; continue
    if line.startswith('|'):
        rows = []
        while i < len(lines) and lines[i].strip().startswith('|'):
            parts = [v.strip() for v in lines[i].strip().strip('|').split('|')]
            if not all(re.fullmatch(r'[-: ]+', v) for v in parts):
                rows.append(parts)
            i += 1
        count = len(rows[0])
        ratios = {3:[0.27,0.47,0.26],4:[0.25,0.25,0.25,0.25],5:[0.20,0.25,0.17,0.17,0.21],6:[0.26,0.15,0.15,0.14,0.15,0.15]}.get(count,[1/count]*count)
        data = [[para(c, 'head' if r == 0 else 'cell') for c in row] for r,row in enumerate(rows)]
        table = Table(data, colWidths=[519*r for r in ratios], repeatRows=1, hAlign='LEFT')
        table.setStyle(TableStyle([
            ('BACKGROUND',(0,0),(-1,0),ACCENT), ('VALIGN',(0,0),(-1,-1),'TOP'),
            ('ROWBACKGROUNDS',(0,1),(-1,-1),[colors.HexColor('#F0F5F7'),colors.white]),
            ('LINEBELOW',(0,1),(-1,-1),0.3,colors.HexColor('#D9E2E8')),
            ('LEFTPADDING',(0,0),(-1,-1),7),('RIGHTPADDING',(0,0),(-1,-1),7),
            ('TOPPADDING',(0,0),(-1,-1),5),('BOTTOMPADDING',(0,0),(-1,-1),5),
        ]))
        story.extend([table,Spacer(1,10)]); continue
    if line.startswith('# '): story.append(para(line[2:],'h1'))
    elif line.startswith('## '): story.append(para(line[3:],'h2'))
    elif line.startswith('### '): story.append(para(line[4:],'h3'))
    elif line.startswith('- '): story.append(para('• '+line[2:], 'source'))
    else: story.append(para(line))
    i += 1

OUTPUT.parent.mkdir(parents=True, exist_ok=True)
doc = SimpleDocTemplate(str(OUTPUT), pagesize=(595.28,841.89), rightMargin=38,leftMargin=38,topMargin=58,bottomMargin=45,
                        title='Pinout completo del robot - AutoFinalRobo2',author='Koba Chajchir / asistencia Codex')
doc.build(story, canvasmaker=NumberedCanvas)
print(OUTPUT)
