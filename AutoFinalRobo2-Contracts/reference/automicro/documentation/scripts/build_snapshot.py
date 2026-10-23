"""Publica cobertura de las fichas ya extraídas; no relee ni valida el código."""
from pathlib import Path
from collections import Counter
import json
BASE=Path(__file__).resolve().parents[1]
def load(path):return json.loads((BASE/path).read_text(encoding='utf-8-sig'))
def main():
    files=[];counts={};origins=Counter()
    for root in ['f4','esp','web']:
        source_map=load(f'published/{root}/source-map.json');index=load(f'published/{root}/index.json')
        for row in source_map:files.append({'rootId':root,'path':row['path'],'sha256':row['sha256'],'modules':row['modules'],'analysisLevel':row['coverage']})
        counts[root]={'modules':len(index),'files':len(source_map),'filesWithOwner':sum(bool(x['modules']) for x in source_map)}
        for m in index:
            doc=load(f'published/{root}/'+m['json'])
            for f in doc['functions']+doc['internalFunctions']:origins[root+':'+f['descriptionOrigin']]+=1
    model={'schemaVersion':1,'date':'2026-10-20','revisionPolicy':'working-copy-content-hashes; clean-HEAD-not-established','files':files,
        'counts':counts,'functionDescriptionOrigins':dict(origins),'verification':'not-run-user-instruction',
        'excluded':['HAL/CMSIS y middleware ST','Dependencias vendor/node_modules','Outputs/binarios y firmware F103','Configuración privada y credenciales','Internos Qt pendientes'],
        'limitations':['La cobertura es del inventario estructural, no cobertura semántica exhaustiva ni prueba de runtime.','Un archivo de soporte puede estar inventariado sin ficha narrativa propia.','Descripciones inferred-name y cuerpos no extraídos requieren profundización; AST no ejecuta typechecker y C/C++ no usa preprocesador.']}
    (BASE/'published/source-snapshot.json').write_text(json.dumps(model,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    lines=['# Cobertura y límites de la documentación','',
        'Fotografía del 20-10-2026, archivos de trabajo con hash. Estos números describen extracción documental; no acreditan corrección, ejecución ni interoperabilidad.','',
        '| Raíz | Fichas de unidad | Archivos en inventario propio | Con ficha asignada |','| --- | --- | --- | --- |']
    lines += ['| '+r+' | '+str(c['modules'])+' | '+str(c['files'])+' | '+str(c['filesWithOwner'])+' |' for r,c in counts.items()]
    lines += ['','## Profundidad editorial de las funciones','',
        'Las fichas conservan firmas y una línea por función localizada. Las etiquetas describen de dónde viene esa explicación; completar la extracción no equivale a leer semánticamente cada closure.','',
        '| Raíz / origen de descripción | Entradas |','| --- | --- |',*['| '+k+' | '+str(v)+' |' for k,v in sorted(origins.items())],'',
        '`editorial`: explicación escrita de API; `structural`: descripción a partir de delegación/cuerpo; `inferred-name`: semántica pendiente de profundizar, no confirmada. C/C++ puede omitir sintaxis compleja, macros o sobrecargas; AST TS conserva retornos sin anotación como inferidos, sin typechecker.','',
        'F4, ESP y Web tienen fichas de alcance, handles/estado, modularidad, tipos, callbacks, funciones e interacciones. Los fuentes de soporte se conservan en source-map. Qt es consumidor del contrato, con manual interno equivalente pendiente.','',
        'El inventario general anterior de la skill y su caché de relaciones son una fotografía separada; el snapshot publicado actual se obtiene de estas fichas, incluyendo fragmentos .inc y estilos agrupados. No presentar tokens no resueltos como fallos de software.','',
        'Se incorporaron 147 fichas de identidad de pantalla con familias/limitaciones y referencias candidatas; no se resolvió exhaustivamente reachability/estado por cada código ni se ejecutaron comparaciones visuales. Ver [sistema de pantallas](shared/SCREEN_SYSTEM.md).','',
        'No se ejecutaron tests, compilaciones, validadores, navegador, despliegues ni pruebas físicas. Las correcciones de código y el port de la documentación al producto siguen pendientes.']
    (BASE/'published/coverage.md').write_text('\n'.join(lines)+'\n',encoding='utf-8')
    print(json.dumps({'operation':'documentation-generation','counts':counts}))
if __name__=='__main__':main()
