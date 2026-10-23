"""Genera catálogo de metadatos y mapa de fuentes. No compara paridad ni valida software."""
from pathlib import Path
from collections import Counter
import hashlib
import json
import re

BASE=Path(__file__).resolve().parents[1]
PROJECT=BASE.parent.parent
ROOTS={'f4':PROJECT/'FirmwareF4','esp':PROJECT/'Firmware ESP01/FirmESP01MicroArduino2025','web':PROJECT/'Web ESP01/WebESP01AutoMicro2025'}

def read(root,path):
    return (ROOTS[root]/path).read_text(encoding='utf-8-sig',errors='replace')

def ev(root,path,text,offset):
    return {'rootId':root,'path':path,'line':text.count('\n',0,offset)+1,
            'sha256':hashlib.sha256((ROOTS[root]/path).read_bytes()).hexdigest()}

def run():
    shared=BASE/'published/shared';shared.mkdir(parents=True,exist_ok=True)
    aliases={}
    for p in ['Core/Inc/wifi_bridge_types.h','Core/Inc/uner_app.h']:
        aliases.update(re.findall(r'(?m)^#define\s+(\w+)\s+(\w+)\s*$',read('f4',p)))
    def number(token):
        seen=set()
        while token in aliases and token not in seen:
            seen.add(token);token=aliases[token]
        try:return int(token.rstrip('uUlL'),0)
        except ValueError:return None
    f4_text=read('f4','Core/Src/uner_app.c')
    table=f4_text.split('static const UNER_CommandSpec uner_commands[] = {',1)[1].split('\n};',1)[0]
    f4=[]
    for m in re.finditer(r'\{\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^}]+)\}',table):
        token,lo,hi,flags,timeout,handler=[x.strip() for x in m.groups()]
        f4.append({'id':number(token),'symbol':token,'requestMin':lo,'requestMax':hi,
                   'flags':flags,'timeoutMs':timeout,'handler':handler,
                   'evidence':ev('f4','Core/Src/uner_app.c',f4_text,f4_text.index(table)+m.start())})
    esp_text=read('esp','src/f4_command_registry.c')
    esp=[]
    for m in re.finditer(r'(?m)^\s*SPEC\((0x[\da-fA-F]+u?),\s*"([^"]+)"\s*,([^\n]+?)\)',esp_text):
        args=[x.strip() for x in m.group(3).split(',')]
        if len(args)!=11:continue
        owner,reqlo,reqhi,resplo,resphi,policy,timeout,idem,priv,impl,sensitive=args
        esp.append({'id':number(m.group(1)),'name':m.group(2),'owner':owner,
                    'requestMin':reqlo,'requestMax':reqhi,'responseMin':resplo,'responseMax':resphi,
                    'completionPolicy':policy,'timeoutMs':timeout,'idempotent':idem,
                    'privilege':priv,'implementedMetadata':impl,'sensitive':sensitive,
                    'evidence':ev('esp','src/f4_command_registry.c',esp_text,m.start())})
    web_text=read('web','src/protocol/wsApi.ts')
    web=[]
    for m in re.finditer(r'(?m)^\s*(\w+):\s*"([^"]+)"',web_text.split('} as const',1)[0]):
        web.append({'constant':m.group(1),'name':m.group(2),'evidence':ev('web','src/protocol/wsApi.ts',web_text,m.start())})
    dispatch_text=read('esp','src/ws_bridge/dispatch.inc')
    routes=[]
    for m in re.finditer(r'\{\s*"([^"]+)"\s*,\s*"([^"]+)"\s*,\s*(BRIDGE_ROUTE_\w+)\s*,\s*(\w+)\s*,\s*(\w+)\s*,\s*"([^"]+)"\s*\}',dispatch_text):
        name,target,route,validator,handler,schema=m.groups()
        routes.append({'name':name,'target':target,'route':route,'validator':validator,'handler':handler,'schema':schema,
                       'webDeclared':any(x['name']==name for x in web),
                       'evidence':ev('esp','src/ws_bridge/dispatch.inc',dispatch_text,m.start())})
    model={'schemaVersion':1,'status':'source-metadata-only','date':'2026-10-20',
           'f4Commands':f4,'espRegistry':esp,'webDeclarations':web,'espJsonRoutes':routes,
           'limitations':['Sin ejecución ni comparación automática de paridad.',
                          'implementedMetadata no demuestra handler/efecto físico.',
                          'La relación numérica/JSON requiere seguir handler y serializador; no se deduce del parecido del nombre.']}
    (shared/'command-registry.json').write_text(json.dumps(model,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    f4ids={x['id']:x for x in f4 if x['id'] is not None};espids={x['id']:x for x in esp}
    lines=['# Catálogo documental de registros de comandos','',
           'Extraído de `f4:Core/Src/uner_app.c`, `esp:src/f4_command_registry.c`, `esp:src/ws_bridge/dispatch.inc` y `web:src/protocol/wsApi.ts`. Metadatos de fuente, sin comprobación automática de paridad ni ejecución.','',
           'La tabla conserva por separado F4 y ESP. `impl=true` es una declaración del registro ESP; no certifica handler activo. Las filas no implican compatibilidad. La respuesta efectiva se fija leyendo el handler.','',
           '| ID | Nombre ESP / símbolo F4 | F4 request min..max; flags; timeout | ESP request / response bytes; política; timeout | Dueño / impl / privilegio ESP |','| --- | --- | --- | --- | --- |']
    for cmd in sorted(set(f4ids)|set(espids)):
        a=f4ids.get(cmd);b=espids.get(cmd)
        name=(b['name'] if b else '')+(' / '+a['symbol'] if a else '')
        left=f"{a['requestMin']}..{a['requestMax']}; {a['flags']}; {a['timeoutMs']}" if a else 'sin entrada extraída'
        right=f"{b['requestMin']}..{b['requestMax']} / {b['responseMin']}..{b['responseMax']}; {b['completionPolicy']}; {b['timeoutMs']}" if b else 'sin entrada extraída'
        extra=f"{b['owner']}; {b['implementedMetadata']}; {b['privilege']}" if b else '—'
        lines.append(f'| <a id="command-0x{cmd:02x}"></a>`0x{cmd:02X}` | {name} | {left.replace("|","/")} | {right} | {extra} |')
    lines += ['', 'La fila 0x74 tiene una contradicción observada entre metadata F4 y handler: [AMDOC-F4-006](../../review/FINDINGS.md#amdoc-f4-006). Los timeouts aquí son de registro; no son el timeout global de la Promise Web.','',
              '## Rutas JSON ESP y declaraciones Web','',
              'Una ruta en la tabla tiene validador y handler registrados. `Web declarada` indica una constante encontrada, no que todas las pantallas la invoquen. El ID UNER se construye dentro del handler indicado.','',
              '| Comando JSON | Destino / ruta | Validador → handler | Esquema declarado | Web declarada |','| --- | --- | --- | --- | --- |']
    for r in routes:
        lines.append(f"| `{r['name']}` | {r['target']} / {r['route']} | `{r['validator']}` → `{r['handler']}` | `{r['schema']}` | {'sí' if r['webDeclared'] else 'no encontrada'} |")
    (shared/'command-registry.md').write_text('\n'.join(lines)+'\n',encoding='utf-8')
    from build_snapshot import main as publish_snapshot
    publish_snapshot()
    print(json.dumps({'operation':'documentation-generation','f4Entries':len(f4),'espEntries':len(esp),'jsonRoutes':len(routes),'webDeclarations':len(web)}))

if __name__=='__main__':run()
