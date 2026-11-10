"""Convierte los enlaces URI relativos '...pdf#page=N' del PDF exportado por Word en
acciones GoToR, que abren el libro de planos directamente en la hoja N.
Uso: python enlaces_pdf.py <informe.pdf>
"""
import re, sys
from pypdf import PdfReader, PdfWriter
from pypdf.generic import NameObject, DictionaryObject, ArrayObject, NumberObject, TextStringObject, BooleanObject

src = sys.argv[1]
reader = PdfReader(src)
writer = PdfWriter(clone_from=reader)
count = 0
for page in writer.pages:
    for ref in page.get('/Annots') or []:
        a = ref.get_object()
        act = a.get('/A')
        if a.get('/Subtype') != '/Link' or act is None:
            continue
        act = act.get_object()
        uri = act.get('/URI')
        if not uri:
            continue
        m = re.match(r'^(\.\./[^#]+\.pdf)#page=(\d+)$', str(uri))
        if not m:
            continue
        a[NameObject('/A')] = DictionaryObject({
            NameObject('/S'): NameObject('/GoToR'),
            NameObject('/F'): TextStringObject(m.group(1)),
            NameObject('/D'): ArrayObject([NumberObject(int(m.group(2)) - 1), NameObject('/Fit')]),
            NameObject('/NewWindow'): BooleanObject(True),
        })
        count += 1
with open(src, 'wb') as fh:
    writer.write(fh)
print(f'{count} enlaces a planos convertidos')
