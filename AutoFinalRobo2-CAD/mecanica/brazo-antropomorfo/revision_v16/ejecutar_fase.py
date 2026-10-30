"""Ejecutar una fase autorizada de CAD en MCP local. No repetir tras timeout."""
from pathlib import Path
import json,sys,subprocess
H=Path(__file__).resolve().parent
request=H.parents[2]/'.downloads/fusion_request.json'
script=f"PHASE16={sys.argv[1]!r}\nexec(compile(open({str(H/'modelar_conjunto.py')!r},encoding='utf-8').read(),'modelar_conjunto.py','exec'))"
request.write_text(json.dumps(dict(name='fusion_mcp_execute',arguments=dict(featureType='script',object=dict(script=script)))),encoding='utf-8')
subprocess.run([sys.executable,str(H.parent/'revision_v15/conexion_fusion.py'),str(request)],check=True)
