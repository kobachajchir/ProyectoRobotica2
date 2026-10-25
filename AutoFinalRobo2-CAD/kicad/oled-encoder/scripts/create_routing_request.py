"""Polilíneas de diseño B.Cu; genera órdenes MCP, no ejecuta verificaciones."""
from pathlib import Path
import json,sys
routes=[
 ('SDA',.4,[(2,18.54),(.75,17.29),(.75,7.44),(6.19,2)]),
 ('SCL',.4,[(2,16),(5,13),(5,5.73),(8.73,2)]),
 ('ENC_SW',.4,[(2,21.08),(5.08,18),(9.625,18),(13.5,14.125)]),
 ('ENC_DT',.4,[(2,23.62),(4,23.62),(5.68,25.3),(12,25.3),(13.5,26.8),(13.5,28)]),
 ('ENC_CLK',.4,[(2,26.16),(5.66,26.16),(7.5,28),(8.5,28)]),
 ('+3V3',.65,[(11.5,8),(11.5,2.23),(11.27,2)]),
 ('+3V3',.65,[(11.5,6.5),(12.5,5.5),(16,5.5)]),
 ('+3V3',.65,[(11.5,8),(10.5,8),(8,10.5)])]
commands=[]
for net,width,points in routes:
    commands.append(dict(name='kicad_create_track_segments',arguments=dict(net_name=net,width_mm=width,layer='B.Cu',points=[dict(x_mm=round(100+x,6),y_mm=round(100+y,6)) for x,y in points],dry_run=False,commit_message='AFR2 UI A: '+net)))
commands.extend([dict(name='kicad_refill_zones',arguments=dict(dry_run=False,commit_message='AFR2 UI A: plano GND B.Cu')),dict(name='kicad_save_board',arguments=dict(dry_run=False))])
Path(sys.argv[1]).write_text(json.dumps(commands,indent=2),encoding='utf-8')
