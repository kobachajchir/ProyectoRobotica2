"""Deriva seis tomos del PDF, sin alterar la documentacion original."""
from pathlib import Path
import json
import pypdfium2 as pdfium
H=Path(__file__).resolve().parent
manifest=json.loads((H/'libro.json').read_text(encoding='utf-8'))
src=pdfium.PdfDocument(str(H/'AFR2_Planos_Brazo_V17_Documentacion_Dimensional_A3.pdf'))
folder=H/'tomos';folder.mkdir(exist_ok=True)
titles=['Montura_y_control','Base_y_hombro','Brazo_y_correa','Antebrazo_y_J4','Muneca_J5_J6','Herramienta_y_pinza']
contents=[]
for n,title in enumerate(titles,1):
 prefix_a='AFR2-A'+str(n);prefix_p='AFR2-P'+str(n)
 sheets=[s for s in manifest['sheets'] if s['code'].startswith((prefix_a,prefix_p))]
 out=pdfium.PdfDocument.new();out.import_pages(src,pages=[s['sheet']-1 for s in sheets])
 file='AFR2_Tomo_%s_%s_A3.pdf'%(n,title);out.save(str(folder/file));out.close()
 contents.append({'tomo':n,'title':title.replace('_',' '),'file':'tomos/'+file,'sheets':len(sheets),'contents':[{'page':i+1,'book_page':s['sheet'],'code':s['code']} for i,s in enumerate(sheets)]})
src.close()
(H/'tomos.json').write_text(json.dumps(contents,ensure_ascii=False,indent=2),encoding='utf-8')
lines=['# Seis tomos de consulta','','Las hojas conservan la numeracion del libro general.','','| Tomo | Hojas | PDF |','|---:|---:|---|']
for t in contents:lines.append('| %s | %s | [%s](%s) |'%(t['tomo'],t['sheets'],t['title'],Path(t['file']).name))
for t in contents:
 lines.extend(['','## Tomo '+str(t['tomo']),'','| Página del tomo | Hoja general | Código |','|---:|---:|---|'])
 for s in t['contents']:lines.append('| %s | %s | %s |'%(s['page'],s['book_page'],s['code']))
(folder/'INDICE.md').write_text('\n'.join(lines)+'\n',encoding='utf-8')
print(json.dumps({'tomos':len(contents),'sheets':sum(t['sheets'] for t in contents),'files':[t['file'] for t in contents]}))
