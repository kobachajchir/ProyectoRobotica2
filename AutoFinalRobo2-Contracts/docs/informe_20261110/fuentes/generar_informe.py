"""Genera el informe D2 (DOCX editable con ecuaciones OMML) desde AFR2_Informe_D2.md.

Uso: python generar_informe.py <carpeta_salida>
Requiere pandoc en PATH y python-docx. El PDF se exporta despues con Word (exportar_pdf.ps1).
Los enlaces a planos se escriben relativos a Revision/R02_AAAAMMDD/01_Informe/.
"""
import csv, re, subprocess, sys, shutil
from pathlib import Path
from docx import Document
from docx.shared import Pt, Cm, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT
from docx.oxml import OxmlElement
from docx.oxml.ns import qn

HERE = Path(__file__).resolve().parent.parent           # .../docs/informe_20261110
ROOT = HERE.parents[2]                                   # D:/ProyectoRobotica2
PLANOS = ROOT / 'Revision/R01_20261110/02_Planos'
PLANO_REL = '../../R01_20261110/02_Planos/AFR2_Planos_Brazo_V17_Documentacion_Dimensional_A3.pdf'
OUT = Path(sys.argv[1]).resolve()
OUT.mkdir(parents=True, exist_ok=True)
NAME = 'AFR2_Informe_D2'
ACC = RGBColor(0x9b, 0x1b, 0x30); INK = RGBColor(0x1f, 0x29, 0x33); GREY = RGBColor(0x5f, 0x6b, 0x7a)


def annex_table():
    rows = []
    for line in (PLANOS / 'INDICE.md').read_text(encoding='utf-8').splitlines():
        m = re.match(r'\|\s*(\d+)\s*\|\s*(AFR2-[A-Z]\d+)\s*\|\s*([^|]+)\|', line)
        if m:
            rows.append((int(m.group(1)), m.group(2), m.group(3).strip()))
    bom = {r['codigo']: r for r in csv.DictReader(open(PLANOS / 'BOM_FABRICADOS.csv', encoding='utf-8-sig'))}
    out = [': Tabla A1. Láminas principales de conjuntos y piezas (la hoja de interfaces sigue a cada pieza).', '',
           '| Código | Descripción | Hoja | Cant. | Material |', '|---|---|---:|---:|---|']
    for hoja, code, desc in rows:
        b = bom.get(code)
        mat = b['material'].split(';')[0].replace(' segun clasificacion documental', '') if b else 'conjunto'
        cant = b['cantidad'] if b else '—'
        out.append(f'| [{code}](PLANO#{hoja}) | {desc} | {hoja} | {cant} | {mat} |')
    return '\n'.join(out)


def build_markdown():
    md = (HERE / f'{NAME}.md').read_text(encoding='utf-8')
    md = md.replace('<!--ANEXO_PLANOS-->', annex_table())
    md = re.sub(r'\(PLANO#(\d+)\)', lambda m: f'({PLANO_REL}#page={m.group(1)})', md)
    tmp = OUT / f'{NAME}.build.md'
    tmp.write_text(md, encoding='utf-8')
    return tmp


def set_font(style, size=None, bold=None, color=None, name='Arial', italic=None):
    f = style.font; f.name = name
    rpr = style.element.get_or_add_rPr(); fonts = rpr.find(qn('w:rFonts'))
    if fonts is None:
        fonts = OxmlElement('w:rFonts'); rpr.append(fonts)
    for a in ('w:ascii', 'w:hAnsi', 'w:cs', 'w:eastAsia'):
        fonts.set(qn(a), name)
    for a in ('w:asciiTheme', 'w:hAnsiTheme', 'w:cstheme', 'w:eastAsiaTheme'):
        if fonts.get(qn(a)) is not None:
            del fonts.attrib[qn(a)]
    if size: f.size = Pt(size)
    if bold is not None: f.bold = bold
    if italic is not None: f.italic = italic
    if color is not None: f.color.rgb = color


def reference_doc():
    ref = OUT / 'referencia.docx'
    subprocess.run(['pandoc', '-o', str(ref), '--print-default-data-file', 'reference.docx'], check=True)
    d = Document(ref); st = d.styles
    for s in ('Normal', 'Body Text', 'First Paragraph', 'Compact'):
        if s in [x.name for x in st]:
            set_font(st[s], 10, color=INK)
            pf = st[s].paragraph_format; pf.space_after = Pt(5); pf.line_spacing = 1.12
            if s != 'Compact': pf.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY
    for s, size in (('Heading 1', 15), ('Heading 2', 12), ('Heading 3', 10.5)):
        set_font(st[s], size, bold=True, color=ACC if s != 'Heading 3' else INK)
        pf = st[s].paragraph_format; pf.space_before = Pt(14 if s == 'Heading 1' else 9); pf.space_after = Pt(5); pf.keep_with_next = True
    set_font(st['Heading 1'], 15, bold=True, color=ACC)
    for s in ('Image Caption', 'Table Caption'):
        set_font(st[s], 8.5, italic=False, color=GREY)
        st[s].paragraph_format.alignment = WD_ALIGN_PARAGRAPH.CENTER if s == 'Image Caption' else WD_ALIGN_PARAGRAPH.LEFT
        st[s].paragraph_format.space_after = Pt(8)
    if 'Captioned Figure' in [x.name for x in st]:
        st['Captioned Figure'].paragraph_format.alignment = WD_ALIGN_PARAGRAPH.CENTER
    for s in ('Figure', 'Captioned Figure'):
        if s in [x.name for x in st]:
            st[s].paragraph_format.alignment = WD_ALIGN_PARAGRAPH.CENTER; st[s].paragraph_format.keep_with_next = True
    from docx.enum.style import WD_STYLE_TYPE
    for name, size, bold, color, before in (('Portada Institucion', 13, True, INK, 30), ('Portada Titulo', 30, True, ACC, 8),
                                            ('Portada Subtitulo', 13, False, INK, 4), ('Portada Datos', 10.5, False, INK, 4),
                                            ('Recuadro', 9.5, False, INK, 0), ('Titulo Indice', 15, True, ACC, 0),
                                            ('Titulo Indice 2', 12, True, ACC, 12)):
        s = st.add_style(name, WD_STYLE_TYPE.PARAGRAPH) if name not in [x.name for x in st] else st[name]
        s.base_style = st['Normal']; set_font(s, size, bold=bold, color=color)
        s.paragraph_format.alignment = WD_ALIGN_PARAGRAPH.CENTER if name.startswith('Portada') else WD_ALIGN_PARAGRAPH.LEFT
        if name.startswith('Titulo'): s.paragraph_format.keep_with_next = True
        s.paragraph_format.space_before = Pt(before); s.paragraph_format.space_after = Pt(4)
    rec = st['Recuadro']; ppr = rec.element.get_or_add_pPr()
    shd = OxmlElement('w:shd'); shd.set(qn('w:val'), 'clear'); shd.set(qn('w:fill'), 'F4F1F2'); ppr.append(shd)
    bdr = OxmlElement('w:pBdr')
    for side in ('left',):
        e = OxmlElement(f'w:{side}'); e.set(qn('w:val'), 'single'); e.set(qn('w:sz'), '18'); e.set(qn('w:color'), '9B1B30'); e.set(qn('w:space'), '8'); bdr.append(e)
    ppr.append(bdr)
    rec.paragraph_format.left_indent = Cm(0.4); rec.paragraph_format.right_indent = Cm(0.4)
    if 'Hyperlink' in [x.name for x in st]:
        st['Hyperlink'].font.color.rgb = RGBColor(0x2b, 0x6c, 0xb0)
    sec = d.sections[0]
    sec.page_width = Cm(21); sec.page_height = Cm(29.7)
    for m in ('left_margin', 'right_margin'): setattr(sec, m, Cm(2.3))
    sec.top_margin = Cm(2.2); sec.bottom_margin = Cm(2.0)
    d.save(ref)
    return ref


def add_field(par, instr):
    for kind, text in (('begin', None), (None, instr), ('separate', None), (None, '1'), ('end', None)):
        r = par.add_run()
        if kind:
            fc = OxmlElement('w:fldChar'); fc.set(qn('w:fldCharType'), kind); r._r.append(fc)
        elif text == instr:
            it = OxmlElement('w:instrText'); it.set(qn('xml:space'), 'preserve'); it.text = f' {instr} '; r._r.append(it)
        else:
            r.text = text


def style_tables(d):
    for t in d.tables:
        t.alignment = WD_TABLE_ALIGNMENT.CENTER
        tblPr = t._tbl.tblPr
        for old in tblPr.findall(qn('w:tblBorders')):
            tblPr.remove(old)
        borders = OxmlElement('w:tblBorders')
        for side, sz, color in (('top', '8', '9B1B30'), ('bottom', '8', '9B1B30'), ('insideH', '2', 'CBD2D9')):
            e = OxmlElement(f'w:{side}'); e.set(qn('w:val'), 'single'); e.set(qn('w:sz'), sz); e.set(qn('w:color'), color); borders.append(e)
        tblPr.append(borders)
        w = tblPr.find(qn('w:tblW'))
        if w is None:
            w = OxmlElement('w:tblW'); tblPr.append(w)
        w.set(qn('w:type'), 'pct'); w.set(qn('w:w'), '5000')
        for ri, row in enumerate(t.rows):
            trPr = row._tr.get_or_add_trPr()
            cant = OxmlElement('w:cantSplit'); trPr.append(cant)
            if ri == 0:
                h = OxmlElement('w:tblHeader'); trPr.append(h)
            for cell in row.cells:
                if ri == 0:
                    tcPr = cell._tc.get_or_add_tcPr(); shd = OxmlElement('w:shd')
                    shd.set(qn('w:val'), 'clear'); shd.set(qn('w:fill'), 'F4F1F2'); tcPr.append(shd)
                for p in cell.paragraphs:
                    p.paragraph_format.space_after = Pt(1); p.paragraph_format.space_before = Pt(1)
                    p.paragraph_format.alignment = WD_ALIGN_PARAGRAPH.LEFT
                    for r in p._p.iter(qn('w:r')):   # incluye runs dentro de hipervinculos
                        rpr = r.find(qn('w:rPr'))
                        if rpr is None:
                            rpr = OxmlElement('w:rPr'); r.insert(0, rpr)
                        for tag in ('w:sz', 'w:szCs'):
                            e = rpr.find(qn(tag))
                            if e is None:
                                e = OxmlElement(tag); rpr.append(e)
                            e.set(qn('w:val'), '17')
                        if ri == 0 and rpr.find(qn('w:b')) is None:
                            rpr.append(OxmlElement('w:b'))
        column_widths(t)


def column_widths(t, total_cm=16.4):
    """Ancho de columnas proporcional al largo del texto, con minimo legible."""
    ncol = len(t.columns)
    if ncol == 0:
        return
    lens = [[] for _ in range(ncol)]
    for row in t.rows:
        for i, c in enumerate(row.cells[:ncol]):
            lens[i].append(len(c.text.strip()))
    weights = []
    for i, l in enumerate(lens):
        avg = sum(l) / len(l); mx = max(l)
        words = [len(w) for row in t.rows for w in row.cells[i].text.split()] or [1]
        weights.append(max(max(words) * 0.95 + 1.5, 5.0, min(60.0, 0.6 * avg + 0.4 * min(mx, 45))))
    s = sum(weights); widths = [Cm(total_cm * w / s) for w in weights]
    tblPr = t._tbl.tblPr
    lay = tblPr.find(qn('w:tblLayout'))
    if lay is None:
        lay = OxmlElement('w:tblLayout'); tblPr.append(lay)
    lay.set(qn('w:type'), 'fixed')
    grid = t._tbl.tblGrid
    for gc, wv in zip(grid.findall(qn('w:gridCol')), widths):
        gc.set(qn('w:w'), str(int(wv.twips)))
    for row in t.rows:
        for c, wv in zip(row.cells[:ncol], widths):
            c.width = wv


def headers(d):
    sec = d.sections[0]; sec.different_first_page_header_footer = True
    hp = sec.header.paragraphs[0]; hp.text = ''
    hp.paragraph_format.tab_stops.add_tab_stop(Cm(16.4), alignment=2)  # derecha
    r = hp.add_run('Robótica II · AutoFinalRobo2 — Manipulador móvil'); r.font.size = Pt(8); r.font.color.rgb = GREY
    hp.add_run('\t'); add_field(hp, 'PAGE')
    for run in hp.runs: run.font.size = Pt(8)
    fp = sec.footer.paragraphs[0]; fp.text = ''
    r = fp.add_run('Ingeniería en Mecatrónica · UNER · Revisión D2, 10/11/2026'); r.font.size = Pt(8); r.font.color.rgb = GREY
    fp.alignment = WD_ALIGN_PARAGRAPH.CENTER


def main():
    md = build_markdown(); ref = reference_doc()
    docx = OUT / f'{NAME}.docx'
    subprocess.run(['pandoc', str(md), '-f', 'markdown+raw_attribute+fenced_divs+pipe_tables+tex_math_dollars',
                    '-o', str(docx), '--reference-doc', str(ref), '--resource-path', str(HERE)], check=True)
    d = Document(docx); style_tables(d); headers(d); d.save(docx)
    shutil.copyfile(HERE / f'{NAME}.md', OUT / f'{NAME}.md')
    md.unlink(); ref.unlink()
    print(docx)


if __name__ == '__main__':
    main()
