"""Genera la hoja Markdown y el PDF del mismo pinout, sin pruebas de hardware.

Uso: python tools/generate_pinout_auto.py
Dependencia de autoría: reportlab. El JSON conserva tablas, notas y referencias.
"""
import json
from pathlib import Path
from xml.sax.saxutils import escape

from reportlab.lib import colors
from reportlab.lib.enums import TA_LEFT
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import ParagraphStyle
from reportlab.pdfgen.canvas import Canvas
from reportlab.platypus import Paragraph, Table, TableStyle

ROOT = Path(__file__).resolve().parents[1]
HARDWARE = ROOT / "hardware"
DATA = json.loads((HARDWARE / "PINOUT_DEL_AUTO.json").read_text(encoding="utf-8"))
PDF = HARDWARE / "output" / "pdf" / "PINOUT_DEL_AUTO.pdf"
PDF.parent.mkdir(parents=True, exist_ok=True)


def markdown():
    lines = [f'# {DATA["title"]}', '', DATA['subtitle'], '',
             f'**{DATA["revision"]} - {DATA["date"]}**', '',
             '[Hoja PDF para imprimir](output/pdf/PINOUT_DEL_AUTO.pdf). '
             'Fuente editable: [PINOUT_DEL_AUTO.json](PINOUT_DEL_AUTO.json); '
             'generador: [generate_pinout_auto.py](../tools/generate_pinout_auto.py).', '',
             DATA['orientation'], '']
    for section in DATA['sections']:
        lines.extend([f'## {section["title"]}', '',
                      '| ' + ' | '.join(section['columns']) + ' |',
                      '| ' + ' | '.join('---' for _ in section['columns']) + ' |'])
        lines.extend('| ' + ' | '.join(row) + ' |' for row in section['rows'])
        lines.extend(['', section['note'], ''])
    lines.extend(['## Compatibilidad del nuevo agrupamiento Hall', '',
                  'A8-A11 son PK0-PK3 y A12-A15 son PK4-PK7. Las ocho señales '
                  'pertenecen a PINK y al grupo PCINT2; se conserva una captura '
                  'simultánea del puerto. No se necesita adyacencia entre cada A y B.', '',
                  'El firmware usa x1: cuenta flancos ascendentes de A y lee B '
                  'para el sentido. La máscara cambia de 0x55 a **0x0F** para '
                  'interrumpir solo con PK0-PK3. Los contadores y el protocolo '
                  'mantienen el orden lógico **FL, FR, RL, RR**; el orden físico '
                  'de los headers es **FR, FL, RR, RL**. No cambia CPR ni formato wire.', '',
                  '## Estado de esta entrega', '', DATA['status'], '',
                  'El usuario confirma alimentación del panel OLED/encoder a 5 V. '
                  'Se registra como montaje comunicado, sin certificar tolerancia '
                  'del OLED, pull-ups, continuidad ni rendimiento físico. No se '
                  'modifica el CAD que conserva la red +3V3. El build '
                  'AFR2-20261027-10 habilita captura Hall por pedido humano; '
                  'la preferencia Hall conectado y la EEPROM se conservan. '
                  'La versión flasheada previa puede usar otro mapa.', '',
                  'D0/D1 corresponden a USB/USART0; no son el enlace operativo ESP. '
                  'La carga temporal tiene cableado propio en [FLASHING.md](FLASHING.md). '
                  'Para el selector y niveles, ver [WIFI_SHIELD.md](WIFI_SHIELD.md). '
                  'Para IRQ y límites, ver [INTERRUPCIONES_ENCODER_HALL.md]'
                  '(INTERRUPCIONES_ENCODER_HALL.md).', '',
                  '## Referencias', ''])
    lines.extend(f'- [{r["label"]}]({r["target"]}): {r["detail"]}' for r in DATA['references'])
    lines.extend(['', f'**{DATA["author"]}**', ''])
    (HARDWARE / 'PINOUT.md').write_text('\n'.join(lines), encoding='utf-8')


INK = colors.HexColor('#172733')
ACCENT = colors.HexColor('#164F63')
MUTED = colors.HexColor('#475569')
CELL = ParagraphStyle('cell', fontName='Helvetica', fontSize=9.5, leading=12,
                      textColor=INK, alignment=TA_LEFT)
HEAD = ParagraphStyle('head', parent=CELL, fontName='Helvetica-Bold', textColor=colors.white)
NOTE = ParagraphStyle('note', fontName='Helvetica', fontSize=9, leading=11, textColor=MUTED)
FOOT = ParagraphStyle('foot', parent=NOTE, fontSize=7.4, leading=9.0)


def paragraph(canvas, text, x, top, width, style=NOTE):
    item = Paragraph(escape(text), style)
    _, height = item.wrap(width, 1000)
    item.drawOn(canvas, x, top - height)
    return top - height


def section(canvas, index, x, top, width):
    data = DATA['sections'][index]
    canvas.setFillColor(ACCENT)
    canvas.setFont('Helvetica-Bold', 10.2)
    canvas.drawString(x, top - 11, data['title'])
    top -= 19
    rows = [[Paragraph(escape(t), HEAD) for t in data['columns']]]
    rows += [[Paragraph(escape(t), CELL) for t in row] for row in data['rows']]
    table = Table(rows, colWidths=[width * w for w in data['widths']], hAlign='LEFT')
    table.setStyle(TableStyle([
        ('BACKGROUND', (0, 0), (-1, 0), ACCENT),
        ('ROWBACKGROUNDS', (0, 1), (-1, -1), [colors.HexColor('#F0F4F6'), colors.white]),
        ('VALIGN', (0, 0), (-1, -1), 'TOP'),
        ('LEFTPADDING', (0, 0), (-1, -1), 5),
        ('RIGHTPADDING', (0, 0), (-1, -1), 5),
        ('TOPPADDING', (0, 0), (-1, -1), 4),
        ('BOTTOMPADDING', (0, 0), (-1, -1), 4),
        ('LINEBELOW', (0, 0), (-1, 0), 0.5, ACCENT),
    ]))
    _, height = table.wrap(width, 1000)
    table.drawOn(canvas, x, top - height)
    top -= height + 5
    top = paragraph(canvas, data['note'], x, top, width)
    return top - 12


def pdf():
    width, height = A4
    canvas = Canvas(str(PDF), pagesize=(width, height), pageCompression=1)
    canvas.setTitle(DATA['title'])
    canvas.setAuthor('Koba Chajchir')
    canvas.setSubject(DATA['subtitle'] + ' - ' + DATA['revision'])
    margin = 30
    content_width = width - 2 * margin
    for page, indices in enumerate(([0, 1, 2, 3], [4, 5, 6, 7]), start=1):
        canvas.setFillColor(INK)
        canvas.setFont('Helvetica-Bold', 25)
        canvas.drawString(margin, height - 39, DATA['title'])
        canvas.setFont('Helvetica', 9)
        canvas.drawString(margin, height - 55, DATA['subtitle'])
        canvas.setFillColor(MUTED)
        canvas.setFont('Helvetica', 8)
        canvas.drawRightString(width - margin, height - 32, DATA['date'])
        canvas.drawRightString(width - margin, height - 45, DATA['revision'])
        top = paragraph(canvas, DATA['orientation'], margin, height - 66, content_width) - 11
        for index in indices:
            top = section(canvas, index, margin, top, content_width)
        canvas.setStrokeColor(colors.HexColor('#CBD5E1'))
        canvas.line(margin, 83, width - margin, 83)
        paragraph(canvas, DATA['status'], margin, 77, content_width, FOOT)
        paragraph(canvas,
                  'Referencias: Manual OSOYOO del kit (PDF pp. 5, 14-17, 26-27, 33-34, 43); '
                  'board_pinmap.h; Arduino Mega 2560 y Microchip ATmega2560; '
                  'guías OSOYOO de Hall. Enlaces: AutoFinalRobo2-Contracts/hardware/PINOUT.md.',
                  margin, 48, content_width, FOOT)
        canvas.setFillColor(INK)
        canvas.setFont('Helvetica-Bold', 10)
        canvas.drawString(margin, 17, DATA['author'])
        canvas.setFont('Helvetica', 8)
        canvas.drawRightString(width - margin, 17, f'AutoFinalRobo2 | {page} / 2')
        canvas.showPage()
    canvas.save()


if __name__ == '__main__':
    markdown()
    pdf()
    print('Guardados: hardware/PINOUT.md y hardware/output/pdf/PINOUT_DEL_AUTO.pdf')
