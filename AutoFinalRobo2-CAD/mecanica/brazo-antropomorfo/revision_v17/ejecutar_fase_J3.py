"""Envia una fase explicita al MCP local. No repetir tras timeout sin inspeccion."""
from pathlib import Path
import json,sys,subprocess,os
H=Path(__file__).resolve().parent
request=H.parents[2]/'.downloads/fusion_request.json'
env=os.environ.copy();env['AFR2_FUSION_TIMEOUT']='1200'
if len(sys.argv)<2:raise SystemExit('Indicar una o mas fases explicitas')
for phase in sys.argv[1:]:
    script=f"FASE17={phase!r}\nexec(compile(open({str(H/'modelar_transmision_J3.py')!r},encoding='utf-8').read(),'modelar_transmision_J3.py','exec'))"
    request.write_text(json.dumps(dict(name='fusion_mcp_execute',arguments=dict(featureType='script',object=dict(script=script)))),encoding='utf-8')
    result=subprocess.run([sys.executable,str(H.parent/'revision_v15/conexion_fusion.py'),str(request)],capture_output=True,text=True,env=env)
    print(result.stdout,flush=True)
    if result.stderr:print(result.stderr,file=sys.stderr,flush=True)
    if result.returncode:raise SystemExit(result.returncode)
    reply=json.loads(result.stdout)
    if reply.get('error') or reply.get('result',{}).get('isError'):raise SystemExit(2)
    for item in reply.get('result',{}).get('content',[]):
        if item.get('type')!='text':continue
        try:payload=json.loads(item['text'])
        except (ValueError,KeyError):continue
        if payload.get('success') is False or payload.get('error'):raise SystemExit(2)
