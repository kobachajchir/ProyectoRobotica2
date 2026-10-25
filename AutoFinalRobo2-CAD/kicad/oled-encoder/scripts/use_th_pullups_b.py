"""Sustituye sólo huella/descripción de R2/R3 con el esquema cerrado."""
from pathlib import Path
from sexpr import parse,dump,child,children,Quoted as Q
root=Path(__file__).resolve().parents[1];cad=root.parents[1]
p=root/'PlacaOLED-Encoder-AutoRobo2.kicad_sch';n=parse(p.read_text(encoding='utf-8'))
for s in children(n,'symbol'):
 pr={p[1]:p for p in children(s,'property')}
 if pr['Reference'][2] in ['R2','R3']:
  pr['Footprint'][2]=Q('AFR2_UI:R_Axial_TH_7.62_Lift1.5')
  pr['Description'][2]=Q('Pull-up 10k axial TH DIN0207; cara inferior; cuerpo separado 1.5mm; sin SMD')
p.write_text(dump(n)+'\n',encoding='utf-8')
src=Path('C:/Program Files/KiCad/10.99/share/kicad/footprints/Resistor_THT.pretty/R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal.kicad_mod')
fp=parse(src.read_text(encoding='utf-8'));fp[1]=Q('R_Axial_TH_7.62_Lift1.5')
for pad in children(fp,'pad'):child(pad,'size')[1:]=[1.4,1.4]
for m in children(fp,'model'):child(child(m,'offset'),'xyz')[3]=1.5
(cad/'kicad/libraries/AFR2_UI.pretty/R_Axial_TH_7.62_Lift1.5.kicad_mod').write_text(dump(fp)+'\n',encoding='utf-8')
print('R2/R3 axiales TH; modelo con separacion de 1.5mm respecto a PCB.')
