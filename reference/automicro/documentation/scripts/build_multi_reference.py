"""AMDOC 1 ESP/Web: generación estática, sin ejecutar aplicaciones ni validadores.

--root esp|web --module slug permite actualizar solo los archivos de una ficha.
TypeScript se lee del AST generado por extract_web.cjs, sin importar la Web.
"""
from pathlib import Path
from fnmatch import fnmatchcase
import argparse
import hashlib
import importlib.util
import json
import re

BASE=Path(__file__).resolve().parents[1]
ROOTS={'esp':BASE.parent.parent/'Firmware ESP01/FirmESP01MicroArduino2025',
       'web':BASE.parent.parent/'Web ESP01/WebESP01AutoMicro2025'}
spec=importlib.util.spec_from_file_location('amdoc_f4',BASE/'scripts/build_reference.py')
common=importlib.util.module_from_spec(spec);spec.loader.exec_module(common)

def read(path):return path.read_text(encoding='utf-8-sig',errors='replace')
def write(path,data):
    path.parent.mkdir(parents=True,exist_ok=True)
    path.write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
def ev(root,path,text,offset):return {'rootId':root,'path':path,'line':text.count('\n',0,offset)+1}
def cpp_functions(root,path,text):
    clean=common.masked(text)
    pattern=r'(?m)^[ \t]*((?:(?:[A-Za-z_]\w*(?:::\w+)*(?:<[^\n;{}]*>)?|[*&])[ \t\n]+)+[*& \t]*)([A-Za-z_]\w*(?:::\w+)*)\s*\(([^;{}]*?)\)\s*(?:const\s*)?([;{])'
    found=[]
    for m in re.finditer(pattern,clean):
        prefix,name,args,tail=m.groups()
        if name in {'if','while','switch','for','sizeof','__attribute__'} or set(prefix.split())&{'return','typedef','define','case','else'}:continue
        end=common.close_brace(clean,m.end()-1) if tail=='{' else m.end()
        body=clean[m.end():end] if tail=='{' else ''
        signature=re.sub(r'\s+',' ',clean[m.start():m.end()-1]).strip()
        signature=re.sub(r'\s*=\s*[^,)]+','',signature)
        found.append({'name':name,'signature':signature,'evidence':ev(root,path,text,m.start()),
            'definition':tail=='{','private':'static' in prefix.split() and not path.endswith('.h'),
            'calls':sorted(set(re.findall(r'\b([\w:]+)\s*\(',body))-{'if','for','while','switch','sizeof','return'}),
            'writes':sorted(set(re.findall(r'\b\w+(?:->|\.)(\w+)\s*=(?!=)',body))),'body':body})
    return found

def inspect_file(root,path,ast):
    absolute=ROOTS[root]/path;text=read(absolute)
    if root=='web' and path in ast:return dict(ast[path],text=text)
    is_c=absolute.suffix in {'.h','.c','.cpp','.inc'}
    functions=cpp_functions(root,path,text) if is_c else []
    types=common.declarations(path,text) if is_c else []
    for typ in types:
        typ['evidence']['rootId']=root
        cleaned=[]
        for field in typ['fields']:
            sig=re.sub(r'\s*=.*$','',field['declaration']).strip()
            if '(' in sig and not field['callback']:continue
            field['declaration']=sig
            if not field['callback']:
                target=re.sub(r'\{[^}]*\}','',sig)
                target=re.sub(r'\[[^]]*\]','',target)
                identifiers=re.findall(r'[A-Za-z_]\w*',target)
                if identifiers:field['name']=identifiers[-1]
            field['evidence']['rootId']=root;cleaned.append(field)
        typ['fields']=cleaned
    return {'path':path,'text':text,'sha256':hashlib.sha256(absolute.read_bytes()).hexdigest(),
            'imports':re.findall(r'(?m)^\s*#\s*include\s+[<"]([^>"\n]+)',text),
            'functions':functions,'types':types,'hooks':[],
            'analysisLevel':'c-cpp-structural' if is_c else 'support-inventory'}

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root',choices=['esp','web'],required=True)
    parser.add_argument('--module',action='append',default=[])
    options=parser.parse_args();root=options.root
    overlay=json.loads(read(BASE/f'overlays/{root}-libraries.json'))
    notes_path=BASE/f'overlays/{root}-callback-notes.json'
    callback_notes=json.loads(read(notes_path)) if notes_path.exists() else {'types':{},'fields':{}}
    modules=overlay['modules'];selected=[m for m in modules if not options.module or m['slug'] in options.module or m['id'] in options.module]
    findings=json.loads(read(BASE/'review/findings.json'))['findings']
    if root=='web':
        ast={f['path']:f for f in json.loads(read(BASE.parent/'.cache/amdoc-web-ast.json'))['files']}
    else:ast={}
    all_paths=sorted(p.relative_to(ROOTS[root]).as_posix() for p in (ROOTS[root]/'src').rglob('*') if p.is_file() and p.suffix in {'.h','.c','.cpp','.inc','.ts','.tsx','.js','.jsx','.json','.css'})
    owned={m['id']:[p for p in all_paths if p in m.get('files',[]) or any(fnmatchcase(p,g) for g in m.get('patterns',[]))] for m in modules}
    needed={p for m in selected for p in owned[m['id']]}
    files={p:inspect_file(root,p,ast) for p in sorted(needed)}
    out=BASE/f'published/{root}';(out/'libraries').mkdir(parents=True,exist_ok=True)
    index=[]
    for m in selected:
        m['files']=owned[m['id']]
        functions={};types=[];hooks=[]
        for p in m['files']:
            f=files[p];types.extend(f['types']);hooks.extend(dict(h,path=p) for h in f['hooks'])
            for original in f['functions']:
                fn=original.copy();fn.setdefault('evidence',{'rootId':root,'path':p,'line':fn.get('line',1)})
                key=(fn['name'],p) if fn.get('private') else (fn['name'],)
                if key not in functions or fn['definition']:functions[key]=fn
        api=[];internal=[]
        for f in sorted(functions.values(),key=lambda f:(f['name'],f['evidence']['path'])):
            short=f.get('displayName',f['name'].split('.')[-1])
            description=m.get('functions',{}).get(f['name']) or m.get('functions',{}).get(short)
            if description:origin='editorial'
            else:description,origin=common.describe(short,m,f)
            f.pop('body',None);f.update(description=description,descriptionOrigin=origin)
            (internal if f.get('private') else api).append(f)
        for t in types:
            if t.get('callbackType'):
                t['description']=callback_notes['types'].get(t['name'],'Propósito/contexto de invocación pendiente de explicación editorial; conservar la firma sin asumir llamada activa.')
            for field in t['fields']:
                key=t['name']+'.'+field['name']
                field['description']=m.get('attributes',{}).get(key) or callback_notes['fields'].get(key) or common.field_description(field,m)
        model={'schemaVersion':1,'format':'amdoc-1','id':m['id'],'slug':m['slug'],'title':m['title'],'target':root,
            'kind':m['kind'],'status':'source-documented','snapshotDate':'2026-10-20','verification':'not-run-user-instruction',
            **{k:m[k] for k in ['responsibility','scope','state','modularity','lifecycle','interactions','limitations']},
            'relationships':m.get('relationships',[]),'findings':[f['id'] for f in findings if m['id'] in f['modules']],
            'files':[{'rootId':root,'path':p,'sha256':files[p]['sha256'],'imports':files[p]['imports'],'analysisLevel':files[p]['analysisLevel']} for p in m['files']],
            'types':types,'functions':api,'internalFunctions':internal,'hooks':hooks}
        write(out/f'libraries/{m["slug"]}.json',model)
        lines=[f'# {m["title"]}','','ID: `'+m['id']+'` · Capa: **'+m['kind']+'** · Fotografía: 2026-10-20.','',
            'Documentación de fuente; sin tests, compilación, navegador ni validación física.','',
            '## Responsabilidad y alcance','',m['responsibility'],'',m['scope'],'',
            '<a id="modularity"></a>','## Modularidad observada','',m['modularity'],'',
            '<a id="state"></a>','## Handle, estado y atributos externos','',m['state'],'',
            '<a id="lifecycle"></a>','## Ciclo de vida y contexto','',*['- '+x for x in m['lifecycle']],'',
            '<a id="interactions"></a>','## Interacciones','',*['- '+x for x in m['interactions']],'',
            '<a id="limitations"></a>','## Precauciones y límites','',*['- '+x for x in m['limitations']],'',
            '## Hallazgos conectados','',*['- ['+f['id']+': '+f['title']+'](../../../review/FINDINGS.md#'+f['id'].lower()+')' for f in findings if m['id'] in f['modules']],'',
            '## Archivos y evidencia','', '| Archivo | Análisis | Hash SHA-256 |','| --- | --- | --- |']
        lines += ['| `'+f['path']+'` | '+f['analysisLevel']+' | `'+f['sha256']+'` |' for f in model['files']]
        lines += ['','## Tipos, puertos, props y callbacks','',
            'En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.','']
        for t in types:
            lines += ['### `'+t['name']+'`','', '`'+common.ref(t['evidence'])+'`','']
            if t.get('callbackType'):lines+=['Callback: `'+t['callbackType']+'`. '+t['description'],'']
            if t.get('type'):lines+=['Tipo: `'+common.esc(t['type'])+'`.','']
            if t['fields']:
                lines+=['| Atributo / firma | Propósito |','| --- | --- |']
                lines+=['| `'+common.esc(f['declaration'])+'` | '+common.esc(f['description'])+' |' for f in t['fields']]
                lines+=['']
        for title,rows in [('Funciones públicas / componentes / hooks',api),('Funciones y callbacks internos',internal)]:
            lines+=['','## '+title,'','Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.','',
                    '| Función / firma | Descripción de una línea | Evidencia / origen |','| --- | --- | --- |']
            for f in rows:
                anchor='fn-'+re.sub(r'[^a-z0-9_.:-]','-',f['name'].lower())
                lines+=['| <a id="'+anchor+'"></a>`'+common.esc(f['signature'])+'` | '+common.esc(f['description'])+' | `'+common.ref(f['evidence'])+'`; '+f['descriptionOrigin']+' |']
        if hooks:
            lines+=['','## Estado y efectos React encontrados','', '| Hook | Función/closure | Evidencia |','| --- | --- | --- |']
            lines+=['| `'+h['callee']+'` | `'+h['scope']+'` | `web:'+h['path']+':'+str(h['line'])+'` |' for h in hooks]
        (out/f'libraries/{m["slug"]}.md').write_text('\n'.join(lines)+'\n',encoding='utf-8')
        index.append({'id':m['id'],'slug':m['slug'],'title':m['title'],'kind':m['kind'],'files':m['files'],
            'markdown':'libraries/'+m['slug']+'.md','json':'libraries/'+m['slug']+'.json','functions':len(api),'internalFunctions':len(internal)})
    if options.module and (out/'index.json').exists():
        old=json.loads(read(out/'index.json'));ids={m['id'] for m in index};index=[m for m in old if m['id'] not in ids]+index
    index.sort(key=lambda m:m['id']);write(out/'index.json',index)
    source_map=[]
    oldmap={f['path']:f for f in json.loads(read(out/'source-map.json'))} if (out/'source-map.json').exists() else {}
    for p in all_paths:
        row={'rootId':root,'path':p,'modules':[m['id'] for m in modules if p in owned[m['id']]],
            'sha256':files[p]['sha256'] if p in files else oldmap.get(p,{}).get('sha256'),
            'coverage':files[p]['analysisLevel'] if p in files else oldmap.get(p,{}).get('coverage','inventory-only')}
        source_map.append(row)
    write(out/'source-map.json',source_map)
    lines=['# '+('Firmware ESP01' if root=='esp' else 'Cliente Web')+' — manual de unidades','',
        'Metodología [AMDOC 1](../../METHOD.md). Fotografía de fuente: 2026-10-20. Las inferencias y limitaciones se indican por ficha.','',
        'Ver [arquitectura](architecture.md), [conexiones entre manuales](../shared/CONNECTIONS.md) y [pantallas F4 ↔ ESP ↔ Web](../shared/SCREEN_SYSTEM.md).','',
        '| Unidad | Capa | Funciones públicas | Internas |','| --- | --- | --- |']
    lines += [f'| [{m["title"]}]({m["markdown"]}) | {m["kind"]} | {m["functions"]} | {m["internalFunctions"]} |' for m in index]
    (out/'README.md').write_text('\n'.join(lines)+'\n',encoding='utf-8')
    print(json.dumps({'operation':'documentation-generation','root':root,'modules':len(selected),'filesRead':len(files),'inventoryFiles':len(all_paths)}))

if __name__=='__main__':main()
