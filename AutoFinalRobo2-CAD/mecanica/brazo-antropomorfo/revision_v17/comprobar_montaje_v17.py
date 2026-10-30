"""Comprobaciones mecanicas expresamente solicitadas. No tests de software.

Uso: python comprobar_montaje_v17.py extraer|historial|J2
No ejecutar mientras haya una fase de modelado nativa en curso.
"""
from pathlib import Path
import json,sys,subprocess,os

H=Path(__file__).resolve().parent
mode=sys.argv[1] if len(sys.argv)>1 else None
files={'extraer':'extraer_v17.py','historial':'inspeccionar_historial_v17.py','J2':'revisar_geometria_v17.py','J2barrido':'revisar_geometria_v17.py'}
if mode not in files:raise SystemExit('Indicar extraer, historial o J2')
script=''
if mode in ['J2','J2barrido']:
    samples=[
      dict(name='extendido',degrees=[0,90,0,0,0,0]),
      dict(name='hombro_minimo',degrees=[0,-10,0,0,0,0]),
      dict(name='hombro_maximo',degrees=[0,125,0,0,0,0]),
      dict(name='codo_menos90',degrees=[0,90,-90,0,0,0]),
      dict(name='codo_mas90',degrees=[0,90,90,0,0,0]),
      dict(name='muneca_plegada_uno',degrees=[0,90,0,90,90,90]),
      dict(name='muneca_plegada_otro',degrees=[0,90,0,-90,-90,-90]),
      dict(name='pinza_cerrada',degrees=[0,90,0,0,0,0],gripper_absolute_deg=0,exclude_payload=True),
      dict(name='pinza_abierta',degrees=[0,90,0,0,0,0],gripper_absolute_deg=180),
    ]
    name='interferencias_montaje_J2'
    if mode=='J2barrido':
      samples=[dict(name='hombro_'+str(q),degrees=[0,q,0,0,0,0]) for q in range(-10,126,5)]
      name='interferencias_barrido_J2'
    script=f"SCOPE17='arm'\nAUDIT_NAME17={name!r}\nGRIPPER_ABS17='pieza50'\nPOSES17={samples!r}\n"
script+=f"exec(compile(open({str(H/files[mode])!r},encoding='utf-8').read(),{files[mode]!r},'exec'))"
request=H.parents[2]/('.downloads/revision_v17_'+mode+'.json')
request.write_text(json.dumps(dict(name='fusion_mcp_execute',arguments=dict(featureType='script',object=dict(script=script,readOnly=mode not in ['J2','J2barrido'])))),encoding='utf-8')
env=os.environ.copy();env['AFR2_FUSION_TIMEOUT']='1200'
result=subprocess.run([sys.executable,str(H.parent/'revision_v15/conexion_fusion.py'),str(request)],capture_output=True,text=True,env=env)
print(result.stdout)
if result.stderr:print(result.stderr,file=sys.stderr)
if result.returncode:raise SystemExit(result.returncode)
reply=json.loads(result.stdout)
if reply.get('error') or reply.get('result',{}).get('isError'):raise SystemExit(2)
for item in reply.get('result',{}).get('content',[]):
    if item.get('type')!='text':continue
    try:payload=json.loads(item['text'])
    except ValueError:continue
    if payload.get('success') is False or payload.get('error'):raise SystemExit(2)
