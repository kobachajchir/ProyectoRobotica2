"""Publica el registro editorial sin leer o ejecutar firmware ni regenerar fichas."""
from pathlib import Path
import json

BASE=Path(__file__).resolve().parents[1]

def render():
    findings=json.loads((BASE/'review/findings.json').read_text(encoding='utf-8-sig'))['findings']
    lines=['# Registro de inconsistencias y revisiones de código','',
        'Fuente editorial: `findings.json`. IDs persistentes; no se eliminan al cerrar. Fotografía inicial: 2026-10-20. Lectura de código, sin tests, compilación ni ejecución de paridad. Los fallos condicionales y revisiones no equivalen a incidentes observados en hardware.','',
        '| ID | Prioridad | Estado / certeza | Hallazgo |','| --- | --- | --- | --- |']
    lines += [f"| [{f['id']}](#{f['id'].lower()}) | {f['priority']} | {f['status']} / {f['certainty']} | {f['title']} |" for f in findings]
    for f in findings:
        lines += ['', '<a id="'+f['id'].lower()+'"></a>', '## '+f['id']+' — '+f['title'], '',
            '**Clasificación:** '+f['category']+'. **Estado:** '+f['status']+'. **Certeza:** '+f['certainty']+'.','',
            '**Observación:** '+f['observation'],'','**Disparador y efecto:** '+f['impact'],'',
            '**Cambio a considerar:** '+f['proposal'],'','**Código que habría que revisar:**','',
            *['- `'+p+'`' for p in f['code']],'','**Puntos documentales que deben cambiar junto con el código:**','']
        lines += ['- ['+d['label']+'](../'+d['path']+') · selector estable: `'+d['selector']+'`.' for d in f['documentation']]
        lines += ['','**Repositorios relacionados:** '+', '.join(f['repositories'])+'.','','**Cierre:** '+f['closure']]
        if f.get('history'):
            lines+=['','**Historia conservada:**','']
            for h in f['history']:
                commits='; '.join(k+'='+h[k] for k in ['codeCommit','documentationCommit'] if h.get(k))
                lines+=['- '+h['date']+': '+h.get('action',h.get('note','Cambio registrado'))+(' ('+commits+')' if commits else '')]
    (BASE/'review/FINDINGS.md').write_text('\n'.join(lines)+'\n',encoding='utf-8')
    return len(findings)

if __name__=='__main__':
    print(json.dumps({'operation':'documentation-generation','findings':render()}))
