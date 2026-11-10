"""Tres partes de lectura del borrador, conservando la numeracion general."""
from pathlib import Path
import json
import pypdfium2 as pdfium
H=Path(__file__).resolve().parent
index=json.loads((H/'indice_paginas.json').read_text(encoding='utf-8'))
src=pdfium.PdfDocument(str(H/'AFR2_Informe_Borrador_D1.pdf'))
groups=[(0,6,'01_Proyecto_y_mecanica'),(6,16,'02_Modelos_matematicos'),(16,len(index),'03_Sistema_resultados_y_fuentes')]
dest=H/'partes';dest.mkdir(exist_ok=True)
result=[]
for start,end,name in groups:
 first=index[start]['page']-1;last=index[end]['page']-1 if end<len(index) else len(src)
 out=pdfium.PdfDocument.new();out.import_pages(src,pages=list(range(first,last)))
 file='AFR2_Informe_D1_'+name+'.pdf';out.save(str(dest/file));out.close()
 result.append({'file':'partes/'+file,'first_page':first+1,'last_page':last,'pages':last-first})
metadata={'pages':len(src),'parts':result};src.close()
(H/'entrega.json').write_text(json.dumps(metadata,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(metadata,ensure_ascii=False))
