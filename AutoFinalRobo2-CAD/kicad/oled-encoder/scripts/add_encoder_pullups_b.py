"""Adapta el esquema guardado a los pull-ups de la imagen del usuario.

Ejecutar con el esquemático cerrado. Conserva las posiciones/UUID existentes.
"""
from pathlib import Path
import copy,uuid
from sexpr import parse,dump,child,children,Quoted as Q
P=Path(__file__).resolve().parents[1]/'PlacaOLED-Encoder-AutoRobo2.kicad_sch'
n=parse(P.read_text(encoding='utf-8'))
uid=lambda:Q(str(uuid.uuid4()))
props=lambda s:{p[1]:p for p in children(s,'property')}
if any(props(s)['Reference'][2] in ['R2','R3'] for s in children(n,'symbol')):
    raise SystemExit('R2/R3 ya existen; no se duplican.')
device=parse(Path('C:/Program Files/KiCad/10.99/share/kicad/symbols/Device.kicad_sym').read_text(encoding='utf-8'))
lib=copy.deepcopy(next(s for s in children(device,'symbol') if s[1]=='R'));lib[1]=Q('Device:R')
child(n,'lib_symbols').append(lib)
template=next(s for s in children(n,'symbol') if props(s)['Reference'][2]=='C1')
def wire(a,b):return ['wire',['pts',['xy',*a],['xy',*b]],['stroke',['width',0],['type','default']],['uuid',uid()]]
def label(text,x,y):return ['global_label',Q(text),['shape','bidirectional'],['at',x,y,0],['effects',['font',['size',1.27,1.27]],['justify','left']],['uuid',uid()],['property',Q('Intersheetrefs'),Q('${INTERSHEET_REFS}'),['at',x,y,0],['effects',['font',['size',1.27,1.27]]],['hide','yes']]]
for ref,x,net in [('R2',160.02,'ENC_CLK'),('R3',177.8,'ENC_DT')]:
    s=copy.deepcopy(template);child(s,'lib_id')[1]=Q('Device:R')
    if child(s,'lib_name'):s.remove(child(s,'lib_name'))
    child(s,'uuid')[1]=uid();child(s,'at')[1:]=[x,82.55,0]
    for pin in children(s,'pin'):child(pin,'uuid')[1]=uid()
    for p in children(s,'property'):
        at=child(p,'at')
        if at:at[1:]=[x+2.54,82.55,0]
    pr=props(s);pr['Reference'][2]=Q(ref);child(pr['Reference'],'at')[1:]=[x+3.81,80.01,0]
    pr['Value'][2]=Q('10k');pr['Footprint'][2]=Q('AFR2_UI:R_Axial_TH_7.62_Lift1.5')
    pr['Description'][2]=Q('Pull-up externo 10k a +3V3; axial TH DIN0207; cara inferior separado1.5mm')
    for project in children(child(s,'instances'),'project'):
        for path in children(project,'path'):child(path,'reference')[1]=Q(ref)
    n.extend([s,wire((x,78.74),(x,74.93)),label(net,x,74.93),wire((x,86.36),(x,91.44))])
n.extend([wire((160.02,91.44),(177.8,91.44)),wire((177.8,91.44),(190.5,91.44)),label('+3V3',190.5,91.44)])
for t in children(n,'text'):
    t[1]=Q(str(t[1]).replace('Encoder pasivo a GND. Pull-ups en Mega (firmware vigente).','Encoder A/CLK y B/DT: pull-ups externos R2/R3 de 10k a +3V3; C y pulsador a GND.').replace('FR4 1,5 mm','FR4 2 mm'))
child(child(n,'title_block'),'rev')[1]=Q('B')
P.write_text(dump(n)+'\n',encoding='utf-8')
print('Añadidos R2/R3 10k; el resto del esquema conserva posiciones y UUID.')
