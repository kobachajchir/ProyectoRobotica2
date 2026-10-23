"""Relaciona fichas publicadas; no relee firmware ni ejecuta validadores."""
from pathlib import Path, PurePosixPath
from collections import defaultdict
from posixpath import relpath
import json
BASE=Path(__file__).resolve().parents[1]

def load(path):return json.loads((BASE/path).read_text(encoding='utf-8-sig'))
def write(path,data):(BASE/path).write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
def main():
    nodes=[];edges=[];modules={};sources=defaultdict(lambda:{'modules':[],'pages':[],'screens':[],'findings':[]})
    by_path={};by_basename=defaultdict(list)
    for root in ['f4','esp','web']:
        for m in load(f'published/{root}/index.json'):
            page=f'published/{root}/'+m['markdown'];data=load(f'published/{root}/'+m['json']);modules[m['id']]=(root,page,data)
            nodes.append({'id':m['id'],'kind':'module','root':root,'title':m['title'],'page':page})
            for f in data['files']:
                key=root+':'+f['path'];row=sources[key]
                row['modules'].append(m['id']);row['pages'].append(page)
                by_path[(root,f['path'])]=m['id'];by_basename[(root,PurePosixPath(f['path']).name)].append(m['id'])
    # Estas aristas son candidatas de compilación, nunca llamadas ejecutadas.
    for mid,(root,page,data) in modules.items():
        for f in data['files']:
            for imp in f.get('imports',f.get('includes',[])):
                targets=[]
                if root=='web' and imp.startswith('.'):
                    normalized=[]
                    for part in (PurePosixPath(f['path']).parent/PurePosixPath(imp)).parts:
                        if part=='..':
                            if normalized:normalized.pop()
                        elif part!='.':normalized.append(part)
                    stem='/'.join(normalized)
                    targets=[by_path[(root,p)] for p in [stem,stem+'.ts',stem+'.tsx',stem+'/index.ts',stem+'/index.tsx'] if (root,p) in by_path]
                elif root!='web':targets=by_basename.get((root,PurePosixPath(imp).name),[])
                for target in set(targets)-{mid}:
                    edges.append({'from':mid,'to':target,'kind':'import-or-include-candidate','evidence':root+':'+f['path'],'token':imp})
    manual=load('overlays/connections.json')
    nodes+=manual.get('nodes',[])
    edges+=manual['edges']
    contract_pages={n['id']:n['page'] for n in manual.get('nodes',[])}
    for e in manual['edges']:
        if e['from'] in modules and e['to'] in contract_pages:
            for f in modules[e['from']][2]['files']:
                sources[modules[e['from']][0]+':'+f['path']]['pages'].append(contract_pages[e['to']])
    screens=load('published/shared/screen-manifest.json')['screens']
    for s in screens:
        page='published/shared/screens/'+f'{s["code"]:06x}'+'.md'
        nodes.append({'id':s['id'],'kind':'screen','symbol':s['symbol'],'page':page})
        for e in s['evidence']+s['f4References']:
            row=sources[e['repo']+':'+e['path']];row['screens'].append(s['id']);row['pages'].append(page)
        for e in s['renderers']:
            row=sources['web:'+e['path']];row['screens'].append(s['id']);row['pages'].append(page)
    findings=load('review/findings.json')['findings'];finding_map={}
    for f in findings:
        docs=f['documentation'];finding_map[f['id']]={'modules':f['modules'],'code':f['code'],'documentation':docs,'status':f['status']}
        nodes.append({'id':f['id'],'kind':'finding','page':'review/FINDINGS.md#'+f['id'].lower()})
        for mid in f['modules']:edges.append({'from':f['id'],'to':mid,'kind':'documents-code-change-impact'})
        for code in f['code']:
            parts=code.split(':',2)
            if len(parts)>=2:
                row=sources[parts[0]+':'+parts[1]];row['findings'].append(f['id']);row['pages']+= [d['path'] for d in docs]
    for row in sources.values():
        for key in row:row[key]=sorted(set(row[key]))
    model={'schemaVersion':1,'date':'2026-10-20','basis':'published-records-and-editorial-edges','nodes':nodes,'edges':edges,
        'sourceToDocumentation':dict(sorted(sources.items())),'findingToDocumentation':finding_map,
        'limitations':['Grafo documental, no grafo de ejecución resuelto.','Una búsqueda por archivo conserva candidatos estáticos y no detecta dependencias externas no registradas.','Actualizar fichas/manifest antes de reconstruir enlaces.']}
    write('published/shared/connections.json',model)
    pages={n['id']:n['page'] for n in nodes}
    def link(identifier):
        return '[`'+identifier+'`]('+relpath(pages[identifier],'published/shared')+')' if identifier in pages else '`'+identifier+'`'
    lines=['# Conexiones e impacto entre documentaciones','',
        'Este índice conecta módulo, mensaje, pantalla, archivo y hallazgo mediante IDs estables. Sus relaciones de fuente son candidatas; no acreditan ejecución ni compatibilidad.','',
        '## Recorridos compartidos','', '| Productor | Consumidor | Relación y datos |','| --- | --- | --- |']
    lines+=['| '+link(e['from'])+' | '+link(e['to'])+' | '+e['kind']+': '+e['description']+' |' for e in manual['edges']]
    lines+=['','## Uso para una corrección local','',
        '1. Buscar archivo o ID con `documentation_impact.py`; la consulta solo lee `connections.json`.',
        '2. Abrir las fichas/secciones indicadas y resolver el cambio de código en su repositorio dueño.',
        '3. Editar overlays/contratos afectados, conservar la historia del hallazgo y regenerar solo las unidades indicadas.',
        '4. Publicar el registro con `render_findings.py` y actualizar este índice con `build_connections.py`. No hace falta releer todos los repositorios.','',
        '```text','python documentation/scripts/documentation_impact.py --finding AMDOC-WEB-004',
        'python documentation/scripts/documentation_impact.py --file web:src/protocol/screenMenuState.ts','```','',
        '## Archivos publicados','',
        '- [Modelo de conexiones y búsqueda inversa](connections.json).',
        '- [Contrato binario y JSON](wire-contracts.md).',
        '- [Modelo, acciones y límites de pantallas](SCREEN_SYSTEM.md).',
        '- [Fichas por código](SCREEN_CODE_REGISTER.md).',
        '- [Registro de hallazgos](../../review/FINDINGS.md).','',
        'Cada registro incluye fuente relativa, ID y selector; las líneas son ayudas de lectura. La ausencia en el índice se conserva como desconocida. Los consumidores Qt siguen pendientes de una revisión interna equivalente.']
    (BASE/'published/shared/CONNECTIONS.md').write_text('\n'.join(lines)+'\n',encoding='utf-8')
    print(json.dumps({'operation':'documentation-generation','modules':len(modules),'screens':len(screens),'findings':len(findings)}))
if __name__=='__main__':main()
