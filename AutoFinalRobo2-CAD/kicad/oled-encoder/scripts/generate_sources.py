"""Genera la revisiÃ³n de diseÃ±o OLED/encoder, sin ejecutar verificadores.

Conserva UUID de los seis componentes del esquema aportado. No ejecutar sobre
archivos abiertos con cambios pendientes. El ruteo se aplica despuÃ©s por MCP.
"""
from pathlib import Path
import copy,json,uuid,math
from sexpr import parse,dump,Quoted as Q,child,children

CAD=Path(__file__).resolve().parents[3]
DEST=Path(__file__).resolve().parents[1]
LIB=CAD/'kicad/libraries/AFR2_UI.pretty'
NAME='PlacaOLED-Encoder-AutoRobo2'
SOURCE=CAD/'reference/supplied/oled-encoder-20261025'/f'{NAME}.kicad_sch'
if (DEST/f'{NAME}.kicad_pcb').exists():
    raise SystemExit('La revisiÃ³n ya existe. Este generador inicial no sobrescribe PCB ruteadas; editar por MCP o crear otra revisiÃ³n.')
ROOT=parse(SOURCE.read_text(encoding='utf-8'))
ROOT_ID=child(ROOT,'uuid')[1]
LIBROOT=child(ROOT,'lib_symbols')
CACHE={str(s[1]):s for s in children(LIBROOT,'symbol')}
UUID=lambda:Q(str(uuid.uuid4()))
CONFIG=json.loads((DEST/'design-parameters.json').read_text(encoding='utf-8'))
PITCH=CONFIG['power_pitch_mm']
def setprop(n,key,val):
    p=next((v for v in children(n,'property') if v[1]==key),None)
    if p:p[2]=Q(val)
    else:n.append(['property',Q(key),Q(val),['at','0','0','0'],['effects',['font',['size','1.27','1.27']]],['hide','yes']])
def textnode(text,x,y,size=1.27):
    return ['text',Q(text),['at',x,y,'0'],['effects',['font',['size',size,size]],['justify','left']],['uuid',UUID()]]
def pinrows(c):
    return [p for sub in children(c,'symbol') for p in children(sub,'pin')]

original={next(p[2] for p in children(n,'property') if p[1]=='Reference'):n for n in children(ROOT,'symbol')}
device=parse(Path('C:/Program Files/KiCad/10.99/share/kicad/symbols/Device.kicad_sym').read_text(encoding='utf-8'))
csmall=copy.deepcopy(next(n for n in children(device,'symbol') if n[1]=='C_Small'))
csmall[1]=Q('Device:C_Small')
CACHE['Device:C_Small']=csmall
oled=copy.deepcopy(CACHE['Display_Graphic:ER_OLEDM0.91_1x-I2C'])
oled[1]=Q('AFR2:OLED096_I2C')
for sub in children(oled,'symbol'):sub[1]=Q(str(sub[1]).replace('ER_OLEDM0.91_1x-I2C','OLED096_I2C'))
setprop(oled,'Value','OLED096_I2C')
setprop(oled,'Description','OLED I2C 0.96in 128x64; socket 1x04; GND VCC SCL SDA confirmados por usuario')
setprop(oled,'Datasheet','')
CACHE[str(oled[1])]=oled

placements={
 'J2':(55.88,55.88,'3V3 / GND','AFR2_UI:Power_1x02'),
 'C1':(91.44,55.88,'100nF 16V','AFR2_UI:C_Disc_2.5'),
 'C2':(116.84,55.88,'4.7uF 10V','AFR2_UI:CP_D5_P2.5'),
 'SW1':(193.04,55.88,'EC11 / KY040 sin breakout','AFR2_UI:Encoder_KY040_Bare'),
 'U1':(101.6,116.84,'OLED 0.96 128x64 / SOCKET 1x04','AFR2_UI:OLED096_Socket4'),
 'J1':(193.04,116.84,'DATOS CLK DT SW SDA SCL','AFR2_UI:Signals_Socket5')}
NETS={'J2':{'1':'+3V3','2':'GND'},'C1':{'1':'+3V3','2':'GND'},'C2':{'1':'+3V3','2':'GND'},
 'SW1':{'1':'GND','2':'GND','3':'ENC_SW','4':'ENC_CLK','5':'ENC_DT'},
 'U1':{'1':'GND','2':'+3V3','3':'SCL','4':'SDA'},
 'J1':{'1':'ENC_CLK','2':'ENC_DT','3':'ENC_SW','4':'SDA','5':'SCL'}}
new=['kicad_sch',['version','20261015'],['generator',Q('eeschema')],['generator_version',Q('10.99')],['uuid',ROOT_ID],['paper',Q('A4')],
 ['title_block',['title',Q('AutoFinalRobo2 - Interfaz OLED / encoder')],['date',Q('2026-10-25')],['rev',Q('A')],['company',Q('AFR2')]]]
newcache=['lib_symbols']
used=set()
for ref,(x,y,value,footprint) in placements.items():
    symbol=copy.deepcopy(original[ref]); libid=str(child(symbol,'lib_id')[1])
    if ref=='U1':libid='AFR2:OLED096_I2C'
    if ref=='C1':libid='Device:C_Small'
    child(symbol,'lib_id')[1]=Q(libid)
    old=child(symbol,'at'); dx=x-float(old[1]);dy=y-float(old[2]);old[1:]=[x,y,'0']
    child(symbol,'unit')[1]='1'
    for prop in children(symbol,'property'):
        at=child(prop,'at')
        if at:at[1:3]=[round(float(at[1])+dx,4),round(float(at[2])+dy,4)]
    setprop(symbol,'Value',value);setprop(symbol,'Footprint',footprint)
    if ref=='U1':setprop(symbol,'Datasheet','');setprop(symbol,'Description','OLED 0.96in; orden frente GND VCC SCL SDA; conector inferior en montaje')
    if ref=='C1':setprop(symbol,'Description','Unpolarized decoupling capacitor')
    for p in children(symbol,'property'):
        if p[1]=='Reference':child(p,'at')[1:]=[x+8.89,y-4.445,'0']
        if p[1]=='Value':child(p,'at')[1:]=[x+8.89,y,'0']
    cachekey=str(child(symbol,'lib_name')[1]) if child(symbol,'lib_name') else libid
    if cachekey not in used:newcache.append(copy.deepcopy(CACHE[cachekey]));used.add(cachekey)
    new.append(symbol)
    for p in pinrows(CACHE[cachekey]):
        num=str(child(p,'number')[1]);at=child(p,'at'); px=x+float(at[1]);py=y-float(at[2]);angle=int(at[3])
        vec={0:(-1,0),180:(1,0),90:(0,1),270:(0,-1)}[angle]
        ex=round(px+vec[0]*5.08,4);ey=round(py+vec[1]*5.08,4)
        new.append(['wire',['pts',['xy',px,py],['xy',ex,ey]],['stroke',['width','0'],['type','default']],['uuid',UUID()]])
        labelangle={0:0,180:180,90:90,270:270}[angle]
        new.append(['global_label',Q(NETS[ref][num]),['shape','bidirectional'],['at',ex,ey,labelangle],['effects',['font',['size','1.27','1.27']],['justify','left']],['uuid',UUID()],
            ['property',Q('Intersheetrefs'),Q('${INTERSHEET_REFS}'),['at',ex,ey,'0'],['effects',['font',['size','1.27','1.27']]],['hide','yes']]])
new.insert(6,newcache)
new.extend([textnode('Alimentacion comun 3,3 V. C1 desacople / C2 reserva local.',35.56,30.48),
 textnode('Encoder pasivo a GND. Pull-ups en Mega (firmware vigente).',152.4,30.48),
 textnode('J1: 1 CLK/D2; 2 DT/D3; 3 SW/D42; 4 SDA/D20; 5 SCL/D21.',35.56,152.4),
 textnode('Una cara de cobre B.Cu; FR4 1,5 mm; PCB 20 x 35 mm.',35.56,160.02),
 textnode('OLED enchufado en hembra 1x04; orden visto de frente confirmado.',35.56,167.64),
 textnode('Sin ERC/DRC. Adaptacion de niveles I2C con Mega 5V: pendiente externa.',35.56,175.26),
 ['sheet_instances',['path',Q('/'),['page',Q('1')]]],['embedded_fonts','no']])
DEST.mkdir(parents=True,exist_ok=True);LIB.mkdir(parents=True,exist_ok=True)
(DEST/f'{NAME}.kicad_sch').write_text(dump(new)+'\n',encoding='utf-8')
symroot=['kicad_symbol_lib',['version','20261015'],['generator',Q('kicad_symbol_editor')],copy.deepcopy(oled)]
symroot[-1][1]=Q('OLED096_I2C')
(CAD/'kicad/libraries/AFR2.kicad_sym').write_text(dump(symroot)+'\n',encoding='utf-8')
(DEST/'sym-lib-table').write_text('(sym_lib_table (version 7) (lib (name "AFR2")(type "KiCad")(uri "${KIPRJMOD}/../libraries/AFR2.kicad_sym")(options "")(descr "OLED del proyecto")))\n')
(DEST/'fp-lib-table').write_text('(fp_lib_table (version 7) (lib (name "AFR2_UI")(type "KiCad")(uri "${KIPRJMOD}/../libraries/AFR2_UI.pretty")(options "")(descr "Huellas de interfaz AFR2")))\n')
pro=json.loads((CAD/'reference/supplied/oled-encoder-20261025'/f'{NAME}.kicad_pro').read_text())
pro.setdefault('net_settings',{})['classes']=[dict(name='Default',clearance=.35,track_width=.4,via_diameter=.8,via_drill=.4),dict(name='POWER_3V3',clearance=.4,track_width=.65,via_diameter=.8,via_drill=.4)]
pro['net_settings']['netclass_patterns']=[{'netclass':'POWER_3V3','pattern':'+3V3'}]
(DEST/f'{NAME}.kicad_pro').write_text(json.dumps(pro,indent=2)+'\n',encoding='utf-8')

def model(path,offset=(0,0,0),rotate=(0,0,0)):
    return ['model',Q('${KIPRJMOD}/../libraries/3d/fusion-ui/'+path+'.step'),['offset',['xyz',*offset]],['scale',['xyz','1','1','1']],['rotate',['xyz',*rotate]]]
def pad(num,x,y,net=None,shape='circle',size=1.8,drill=1):
    n=['pad',Q(str(num)),'thru_hole',shape,['at',x,y],['size',size,size],['drill',drill],['layers',Q('B.Cu'),Q('B.Mask')]]
    if net:n.append(['pinfunction',Q(net)])
    return n
def fp(name,pads,outline,models):
    n=['footprint',Q(name),['version','20260206'],['generator',Q('pcbnew')],['layer',Q('F.Cu')],['attr','through_hole'],
        ['property',Q('Reference'),Q('REF**'),['at','0','-2.5','0'],['layer',Q('F.SilkS')],['effects',['font',['size','0.8','0.8'],['thickness','.12']]]],
        ['property',Q('Value'),Q(name),['at','0','3','0'],['layer',Q('F.Fab')],['effects',['font',['size','.8','.8'],['thickness','.12']]]]]
    if outline:
        x1,y1,x2,y2=outline
        n.append(['fp_rect',['start',x1,y1],['end',x2,y2],['stroke',['width','.12'],['type','solid']],['fill','none'],['layer',Q('F.SilkS')]])
    n.extend(pads);n.extend(models)
    (LIB/(name+'.kicad_mod')).write_text(dump(n)+'\n',encoding='utf-8')

# Las alturas son de montaje; CAD aportados sin escala global para forzar cotas.
fp('OLED096_Socket4',[pad(4,-3.81,0),pad(3,-1.27,0),pad(2,1.27,0),pad(1,3.81,0)],[-5.2,-1.4,5.2,1.4],
 [model('4-Pin Header - Female - 2.54mm',(-3.81,0,0),(-90,0,0)),model('OLED0.96CAD',(0,12.2,7.55),(0,0,180))])
fp('Signals_Socket5',[pad(i,0,-(i-1)*2.54) for i in range(1,6)],[-1.35,-11.56,1.35,1.4],
 [model('5-Pin Header - Female - 2.54mm',(0,0,0),(-90,0,-90))])
fp('Power_1x02',[pad(1,0,0),pad(2,PITCH,0)],[-2.4,-2.5,5.3,3.6],
 [model('jst2pins_P2mm_adaptado',(-27.7258,-28.45,0))])
encoderpads=[pad(4,-2.5,7,size=2.2,drill=1.2),pad(2,0,7,size=2.2,drill=1.2),pad(5,2.5,7,size=2.2,drill=1.2),
 pad(1,-2.5,-6.875,size=2.2,drill=1.2),pad(3,2.5,-6.875,size=2.2,drill=1.2)]
for x in [-4.7,4.7]:encoderpads.append(['pad',Q(''),'np_thru_hole','oval',['at',x,0],['size','1.5','2.5'],['drill','oval','1.5','2.5'],['layers',Q('*.Cu'),Q('*.Mask')]])
fp('Encoder_KY040_Bare',encoderpads,[-6,-6.6,6,6.6],
 [model('KY040RotaryEncoderCAD'),model('KY040RotaryEncoderKnobCAD',(0,0,12.5),(-90,0,0))])
def stock_cap(name):
    n=model(name)
    n[1]=Q('${KICAD10_3DMODEL_DIR}/Capacitor_THT.3dshapes/'+name+'.step')
    return n
fp('C_Disc_2.5',[pad(1,0,0),pad(2,2.5,0)],[-1.2,-1,3.7,1],[stock_cap('C_Disc_D5.0mm_W2.5mm_P2.50mm')])
fp('CP_D5_P2.5',[pad(1,0,0,shape='rect'),pad(2,2.5,0)],[-1.3,-2.6,3.8,2.6],[stock_cap('CP_Radial_D5.0mm_P2.50mm')])

board=['kicad_pcb',['version','20260206'],['generator',Q('pcbnew')],['generator_version',Q('10.99')],
 ['general',['thickness','1.5']],['paper',Q('A4')],['title_block',['title',Q('AFR2 UI 20x35 - single side')],['rev',Q('A')]],
 ['layers',['0',Q('F.Cu'),'signal'],['2',Q('B.Cu'),'signal'],['9',Q('F.Adhes'),'user',Q('F.Adhesive')],['11',Q('B.Paste'),'user'],['13',Q('F.Paste'),'user'],['5',Q('B.SilkS'),'user',Q('B.Silkscreen')],['7',Q('F.SilkS'),'user',Q('F.Silkscreen')],['1',Q('B.Mask'),'user'],['3',Q('F.Mask'),'user'],['25',Q('Edge.Cuts'),'user'],['27',Q('Margin'),'user'],['29',Q('B.CrtYd'),'user',Q('B.Courtyard')],['31',Q('F.CrtYd'),'user',Q('F.Courtyard')],['33',Q('B.Fab'),'user'],['35',Q('F.Fab'),'user']],
 ['setup',['pad_to_mask_clearance','0']]]
for a,b in [((100,100),(120,100)),((120,100),(120,135)),((120,135),(100,135)),((100,135),(100,100))]:
    board.append(['gr_line',['start',*a],['end',*b],['stroke',['width','.05'],['type','solid']],['layer',Q('Edge.Cuts')],['uuid',UUID()]])
for i,x in enumerate([103,117],1):
    board.append(['footprint',Q('MountingHole_3.2mm_M3'),['layer',Q('F.Cu')],['uuid',UUID()],['at',x,132],
        ['property',Q('Reference'),Q('H'+str(i)),['at','0','3','0'],['layer',Q('F.SilkS')],['effects',['font',['size','.7','.7'],['thickness','.12']]]],
        ['attr','board_only','exclude_from_pos_files','exclude_from_bom'],['pad',Q(''),'np_thru_hole','circle',['at','0','0'],['size','3.2','3.2'],['drill','3.2'],['layers',Q('*.Cu'),Q('*.Mask')]],
        ['fp_circle',['center','0','0'],['end','3','0'],['stroke',['width','.1'],['type','solid']],['fill','none'],['layer',Q('F.CrtYd')]]])
(DEST/f'{NAME}.kicad_pcb').write_text(dump(board)+'\n',encoding='utf-8')
print('Fuentes generadas en',DEST)
