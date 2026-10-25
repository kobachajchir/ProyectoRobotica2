"""Transfiere a PCB la netlist exportada por KiCad y sus huellas de proyecto.

Sólo genera fuentes. Las rutas se dibujan después mediante KiPilot MCP.
"""
from pathlib import Path
import xml.etree.ElementTree as ET
import json,copy,uuid,math
from sexpr import parse,dump,child,children,Quoted as Q
DEST=Path(__file__).resolve().parents[1]
CAD=DEST.parents[1]
name='PlacaOLED-Encoder-AutoRobo2'
config=json.loads((DEST/'design-parameters.json').read_text())
netlist=ET.parse(CAD/'exports/oled-encoder'/f'{name}.net').getroot()
board=parse((DEST/f'{name}.kicad_pcb').read_text(encoding='utf-8'))
if children(board,'segment') or len(children(board,'footprint'))>2:
    raise SystemExit('PCB ya poblada/ruteada; no se sobrescribe ni se duplican componentes.')
rootid=child(parse((DEST/f'{name}.kicad_sch').read_text(encoding='utf-8')),'uuid')[1]
uid=lambda:Q(str(uuid.uuid4()))
nets={};padnets={}
for code,net in enumerate(netlist.findall('nets/net'),1):
    netname=net.attrib['name'];nets[netname]=code
    board.append(['net',code,Q(netname)])
    for node in net.findall('node'):padnets[(node.attrib['ref'],node.attrib['pin'])]=(code,netname)
for comp in netlist.findall('components/comp'):
    ref=comp.attrib['ref'];footprint=comp.findtext('footprint')
    lib,part=footprint.split(':',1)
    fp=parse((CAD/'kicad/libraries'/(lib+'.pretty')/(part+'.kicad_mod')).read_text(encoding='utf-8'))
    fp[1]=Q(footprint)
    fp[:]=[v for v in fp if not (isinstance(v,list) and v[0] in ['version','generator'])]
    x,y=config['placements_mm'][ref]
    fp.append(['at',x+100,y+100])
    fp.append(['uuid',uid()])
    fp.append(['path',Q('/'+str(rootid)+'/'+comp.findtext('tstamps'))])
    for prop in children(fp,'property'):
        if prop[1]=='Reference':prop[2]=Q(ref);child(prop,'at')[1:]=[0,-2.5,0]
        if prop[1]=='Value':prop[2]=Q(comp.findtext('value'));prop.append(['hide','yes'])
    for p in children(fp,'pad'):
        if (ref,str(p[1])) in padnets:
            code,netname=padnets[(ref,str(p[1]))]
            p.append(['net',code,Q(netname)])
        p.append(['uuid',uid()])
    board.append(fp)

board.append(['zone',['net',nets['GND']],['net_name',Q('GND')],['layer',Q('B.Cu')],['uuid',uid()],['hatch','edge','.5'],
 ['connect_pads',['clearance','.4']],['min_thickness','.25'],['fill','yes',['thermal_gap','.35'],['thermal_bridge_width','.5']],
 ['polygon',['pts',['xy','100.5','100.5'],['xy','119.5','100.5'],['xy','119.5','134.5'],['xy','100.5','134.5']]]])
for x,y in config['mounting_holes_mm']:
    pts=[['xy',round(100+x+3*math.cos(i*math.pi/16),6),round(100+y+3*math.sin(i*math.pi/16),6)] for i in range(32)]
    board.append(['zone',['net','0'],['net_name',Q('')],['layer',Q('B.Cu')],['uuid',uid()],['hatch','edge','.5'],
      ['connect_pads',['clearance','0']],['min_thickness','.25'],['keepout',['tracks','not_allowed'],['vias','not_allowed'],['pads','not_allowed'],['copperpour','not_allowed'],['footprints','allowed']],['polygon',['pts',*pts]]])
for text,x,y in [('AFR2 A',110,134),('3V3',110.8,107.3),('GND',115.6,108),('CLK',104,126.2),('DT',104,123.62),('SW',104,121.08),('SDA',104,118.54),('SCL',104,116)]:
    board.append(['gr_text',Q(text),['at',x,y],['layer',Q('F.SilkS')],['uuid',uid()],['effects',['font',['size','.65','.65'],['thickness','.12']]]])
(DEST/f'{name}.kicad_pcb').write_text(dump(board)+'\n',encoding='utf-8')
print('PCB poblada desde netlist KiCad;',len(netlist.findall('components/comp')),'componentes; sin verificadores')
