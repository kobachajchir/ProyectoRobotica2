"""Genera el registro documental de códigos y referencias, sin ejecutar paridad visual.

Es una extracción de declaraciones/candidatos del checkout, no un test ni una
certificación de fidelidad. No modifica constantes o renderers de los repositorios.
"""
from pathlib import Path
from collections import Counter
import argparse
import hashlib
import json
import re
BASE=Path(__file__).resolve().parents[1]
PROJECT=BASE.parent.parent
F4=PROJECT/'FirmwareF4';WEB=PROJECT/'Web ESP01/WebESP01AutoMicro2025';ESP=PROJECT/'Firmware ESP01/FirmESP01MicroArduino2025'

def read(p):return p.read_text(encoding='utf-8-sig',errors='replace')
def line(text,pos):return text.count('\n',0,pos)+1
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--screen',action='append',default=[],help='Símbolo SCREEN_CODE_* o hex 0x030205; repetible.')
    options=parser.parse_args()
    header=read(F4/'Core/Inc/screen_codes.h');web=read(WEB/'src/screens/screenCodes.ts')
    registry=read(WEB/'src/screens/screenRenderRegistry.ts');remediated=read(WEB/'src/screens/remediatedScreens.ts')
    meta=read(WEB/'src/types/ScreenTypes.ts')
    f4={};webcodes={}
    for m in re.finditer(r'(?m)^\s*#define\s+(SCREEN_CODE_\w+)\s+SCREEN_CODE\(\s*(0x[\da-fA-F]+|\d+)[uUlL]*\s*,\s*(0x[\da-fA-F]+|\d+)[uUlL]*\s*,\s*(0x[\da-fA-F]+|\d+)[uUlL]*\s*\)',header):
        name,*parts=m.groups();parts=[int(x,0) for x in parts]
        f4[name]={'value':(parts[0]<<16)|(parts[1]<<8)|parts[2],'line':line(header,m.start())}
    for m in re.finditer(r'export\s+const\s+(SCREEN_CODE_\w+)\s*(?::\s*ScreenCode)?\s*=\s*(0x[\da-fA-F]+|\d+)',web):
        webcodes[m.group(1)]={'value':int(m.group(2),0),'line':line(web,m.start())}
    metadata={int(m.group(1),16) for m in re.finditer(r'(?m)^\s*(0x[\da-fA-F]+)\s*:',meta)}
    cases={m.group(1):m for m in re.finditer(r'case\s+(SCREEN_CODE_\w+)\s*:',registry)}
    namedtables={m.group(1):m for m in re.finditer(r'\[ScreenCodes\.(SCREEN_CODE_\w+)\]\s*:',registry)}
    remcodes={int(m.group(1),16) for m in re.finditer(r'\[(0x[\da-fA-F]+)\]\s*:',remediated)}
    requested={name for name,a in f4.items() if name in options.screen or f'0x{a["value"]:06x}' in [s.lower() for s in options.screen]}
    previous_path=BASE/'published/shared/screen-manifest.json'
    previous=json.loads(read(previous_path)) if options.screen and previous_path.exists() else {'screens':[]}
    old={s['symbol']:s for s in previous['screens']}
    references={name:list(old.get(name,{}).get('f4References',[])) if options.screen and name not in requested else [] for name in f4}
    # En modo focalizado se refrescan las referencias ya conocidas del código.
    # Nuevas referencias/códigos requieren extracción general explícita.
    known_paths={e['path'] for name in requested for e in old.get(name,{}).get('f4References',[])}
    source_paths=sorted(F4/p for p in known_paths) if options.screen and previous_path.exists() else sorted((F4/'Core').rglob('*'))
    for absolute in source_paths:
        if absolute.suffix not in {'.c','.h'} or absolute.name=='screen_codes.h':continue
        text=read(absolute);path=absolute.relative_to(F4).as_posix()
        for match in re.finditer(r'\bSCREEN_CODE_\w+\b',text):
            if match.group() in references and (not options.screen or match.group() in requested):references[match.group()].append({'repo':'f4','path':path,'line':line(text,match.start()),'kind':'lexical-reference'})
    families=json.loads(read(BASE/'overlays/screen-families.json'))['families']
    overrides_path=BASE/'overlays/screen-overrides.json'
    overrides=json.loads(read(overrides_path))['screens'] if overrides_path.exists() else {}
    findings=json.loads(read(BASE/'review/findings.json'))['findings']
    records=[]
    for name,a in sorted(f4.items(),key=lambda pair:(pair[1]['value'],pair[0])):
        value=a['value'];b=webcodes.get(name)
        override=overrides.get(name,{})
        family=next((f for f in families if f['id']==override.get('family')),None) or next((f for f in families if any(re.search(pattern,name) for pattern in f['symbolPatterns'])),families[-1])
        observation='same-literal' if b and b['value']==value else ('different-literal' if b else 'web-symbol-not-extracted')
        binding=[]
        if value in remcodes:binding.append({'kind':'remediated-priority','path':'src/screens/remediatedScreens.ts','availability':'identity-only-or-reported-menu'})
        if name in namedtables:binding.append({'kind':'identified-table','path':'src/screens/screenRenderRegistry.ts','line':line(registry,namedtables[name].start()),'availability':'generic-builder-candidate'})
        if name in cases:
            start=cases[name].end();nextcase=re.search(r'\bcase\s+SCREEN_CODE_',registry[start:]);end=start+nextcase.start() if nextcase else len(registry)
            tokens=sorted(set(re.findall(r'\b(build\w+)\s*\(',registry[start:end])))
            binding.append({'kind':'switch-case','path':'src/screens/screenRenderRegistry.ts','line':line(registry,cases[name].start()),'builders':tokens,'availability':'parameterized-candidate'})
        ids=[f['id'] for f in findings if f['id'] in family.get('findings',[])]
        rec={'schemaVersion':1,'id':'shared:screen/'+f'{value:06x}','symbol':name,'code':value,'hex':f'0x{value:06X}',
            'decomposition':{'menu':value>>16&255,'submenu':value>>8&255,'page':value&255},'revision':'working-copy-hashes','confidence':'manual-review',
            'family':family['id'],'numericObservation':observation,'webDeclaration':b,'metadataEntryObserved':value in metadata,
            'evidence':[{'repo':'f4','path':'Core/Inc/screen_codes.h','line':a['line']},*([{'repo':'web','path':'src/screens/screenCodes.ts','line':b['line']}] if b else [])],
            'renderers':binding,'callGraph':{'status':'lexical-candidates-only','edges':[]},'f4References':references[name],
            'variants':family['variants'],'fields':family['fields'],'actions':family['actions'],'transitions':family['transitions'],
            'missingData':family['missingData'],'localOnly':family.get('localOnly',[]),'findings':ids,
            'exactness':'not-established','notes':family['notes']}
        for field in ['role','variants','fields','actions','transitions','missingData','notes']:
            if field in override:rec[field]=override[field]
        rec.setdefault('role','not-resolved')
        rec['findings']=sorted(set(rec['findings']+[fid for fid in override.get('findings',[]) if any(f['id']==fid for f in findings)]))
        if options.screen and name not in requested and name in old:rec=old[name]
        records.append(rec)
    out=BASE/'published/shared';(out/'screens').mkdir(parents=True,exist_ok=True)
    source_files={'f4':['Core/Inc/screen_codes.h'],'web':['src/screens/screenCodes.ts','src/screens/screenRenderRegistry.ts','src/screens/remediatedScreens.ts','src/types/ScreenTypes.ts'],'esp':['src/ws_bridge/reports.inc']}
    roots={'f4':F4,'web':WEB,'esp':ESP}
    sources=[{'rootId':root,'path':p,'sha256':hashlib.sha256((roots[root]/p).read_bytes()).hexdigest()} for root,paths in source_files.items() for p in paths]
    model={'schemaVersion':1,'date':'2026-10-20','basis':'source-declarations-and-manual-contract-review','runtimeParityExecuted':False,
        'sources':sources,'screens':records,'webOnlyDeclarations':[{'symbol':n,**v} for n,v in webcodes.items() if n not in f4],
        'limitations':['Mismo literal no acredita renderer, variante, datos ni acción.',
                       'Referencias e imports no prueban reachability; el grafo no se considera resuelto.',
                       'Las reglas de familia indican disponibilidad general, no un snapshot completo por cada código.']}
    (out/'screen-manifest.json').write_text(json.dumps(model,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    for r in records:
        if options.screen and r['symbol'] not in requested:continue
        key=f'{r["code"]:06x}'
        (out/f'screens/{key}.json').write_text(json.dumps(r,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
        lines=['# '+r['hex']+' — '+r['symbol'],'', 'ID: `'+r['id']+'` · Familia: `'+r['family']+'`.','',
            'Fidelidad exacta: **no establecida**. Rol localizado: `'+r['role']+'`. Declaración, renderer candidato, datos y acciones son dimensiones separadas.','',
            '| Dato de registro | Observación de fuente |','| --- | --- |',
            '| Declaración Web | '+r['numericObservation']+' |',
            '| Metadata ScreenTypes | '+('entrada literal encontrada' if r['metadataEntryObserved'] else 'entrada literal no extraída')+' |',
            '| Resolución Web | '+(', '.join(b['kind'] for b in r['renderers']) or 'sin binding candidato extraído')+' |','',
            '## Variantes y estado','',*['- '+x for x in r['variants']],'',
            '## Campos y disponibilidad','', '| Campo | Tipo / unidad | Disponibilidad |','| --- | --- | --- |']
        lines += ['| `'+f['name']+'` | '+f['type']+' / '+f['units']+' | '+f['availability']+'; '+f['note']+' |' for f in r['fields']]
        lines += ['','## Acciones y transiciones','',*['- '+a for a in r['actions']],*['- '+t for t in r['transitions']],'',
            '## Datos que impiden una copia exacta','',*['- '+x for x in r['missingData']],'',*['- '+x for x in r['notes']],'',
            '## Hallazgos conectados','',*['- ['+i+'](../../../review/FINDINGS.md#'+i.lower()+')' for i in r['findings']],'',
            '## Referencias de código','',*['- `'+e['repo']+':'+e['path']+':'+str(e['line'])+'`' for e in r['evidence']],
            *['- `f4:'+e['path']+':'+str(e['line'])+'` (referencia candidata)' for e in r['f4References']],
            '', '[Volver al sistema de pantallas](../SCREEN_SYSTEM.md).']
        (out/f'screens/{key}.md').write_text('\n'.join(lines)+'\n',encoding='utf-8')
    lines=['# Registro documental de códigos de pantalla','',
        'Declaraciones leídas, sin compilar ni ejecutar comparación de píxeles. Un mismo literal no acredita equivalencia de render ni contrato completo de interacción.','',
        '| Código | Símbolo F4 | Literal Web | Metadata | Camino Web candidato |','| --- | --- | --- | --- | --- |']
    lines += ['| ['+r['hex']+'](screens/'+f'{r["code"]:06x}'+'.md) | `'+r['symbol']+'` | '+r['numericObservation']+' | '+('sí' if r['metadataEntryObserved'] else 'no extraída')+' | '+(', '.join(x['kind'] for x in r['renderers']) or 'no extraído')+' |' for r in records]
    (out/'SCREEN_CODE_REGISTER.md').write_text('\n'.join(lines)+'\n',encoding='utf-8')
    counts=Counter(r['numericObservation'] for r in records)
    print(json.dumps({'operation':'documentation-extraction','f4Declarations':len(records),'webDeclarations':len(webcodes),'literalObservations':dict(counts),'metadataLiteralEntries':len(metadata),'screensWithoutBindingCandidate':sum(not r['renderers'] for r in records),'mode':'focused-known-references' if options.screen else 'all-declarations'}))

if __name__=='__main__':main()
