"""Relaciona OLED con torretas comerciales15mm y doble socket de5mm.
Aplicar sólo con PCB cerrado; no cambia posiciones de componentes ni cobre.
"""
from pathlib import Path
import copy,json
from sexpr import parse,dump,child,children,Quoted as Q
root=Path(__file__).resolve().parents[1];cad=root.parents[1]
def update(fp):
 headers=[m for m in children(fp,'model') if '4-Pin Header' in m[1]]
 if len(headers)==1:fp.append(copy.deepcopy(headers[0]));headers=[m for m in children(fp,'model') if '4-Pin Header' in m[1]]
 for i,m in enumerate(headers):child(child(m,'offset'),'xyz')[3]=i*5
 for m in children(fp,'model'):
  if 'OLED0.96CAD' in m[1]:child(child(m,'offset'),'xyz')[3]=13
p=cad/'kicad/libraries/AFR2_UI.pretty/OLED096_Socket4.kicad_mod';fp=parse(p.read_text(encoding='utf-8'));update(fp);p.write_text(dump(fp)+'\n',encoding='utf-8')
p=root/'PlacaOLED-Encoder-AutoRobo2.kicad_pcb';board=parse(p.read_text(encoding='utf-8'))
for fp in children(board,'footprint'):
 if next(p[2] for p in children(fp,'property') if p[1]=='Reference')=='U1':update(fp)
p.write_text(dump(board)+'\n',encoding='utf-8')
p=root/'design-parameters.json';j=json.loads(p.read_text(encoding='utf-8'))
j['placements_mm'].update(R2=[9,29.11],R3=[11.3,18.3]);j['encoder_pullups']={'R2':'10k axial TH DIN0207 P7.62 cara inferior, cuerpo separado1.5mm','R3':'10k axial TH DIN0207 P7.62 cara inferior, cuerpo separado1.5mm'}
j['oled_socket']='Dos headers hembra1x04 de5mm apilados o zocalo10mm equivalente; OLEDoffsetZ13; soporte15mm desdeplanoPCBbottom'
p.write_text(json.dumps(j,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print('OLED sobre torretas15mm mediante socket de altura10mm; encoder conserva montaje directo.')
