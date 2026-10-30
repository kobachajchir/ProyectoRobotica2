"""Cliente MCP local alternativo para Fusion, sin editar configuracion del equipo."""
import json,urllib.request,sys,os
URL='http://127.0.0.1:27182/mcp'
headers={'Content-Type':'application/json','Accept':'application/json, text/event-stream'}
def call(payload):
    req=urllib.request.Request(URL,json.dumps(payload).encode(),headers)
    with urllib.request.urlopen(req,timeout=float(os.environ.get('AFR2_FUSION_TIMEOUT','300'))) as response:
        session=response.headers.get('Mcp-Session-Id')
        if session:headers['Mcp-Session-Id']=session
        raw=response.read().decode()
    if not raw:return {}
    if raw.startswith('event:') or raw.startswith('data:'):
        raw=next(x[5:].strip() for x in raw.splitlines() if x.startswith('data:'))
    return json.loads(raw)
call({'jsonrpc':'2.0','id':1,'method':'initialize','params':{'protocolVersion':'2024-11-05','capabilities':{},'clientInfo':{'name':'codex-local','version':'1'}}})
call({'jsonrpc':'2.0','method':'notifications/initialized','params':{}})
if len(sys.argv)==1:
    result=call({'jsonrpc':'2.0','id':2,'method':'tools/list','params':{}})
    for t in result.get('result',{}).get('tools',[]):print(t['name'],json.dumps(t['inputSchema']))
else:
    from pathlib import Path
    args=json.loads(Path(sys.argv[1]).read_text(encoding='utf-8-sig'))
    print(json.dumps(call({'jsonrpc':'2.0','id':3,'method':'tools/call','params':args}),ensure_ascii=False))
