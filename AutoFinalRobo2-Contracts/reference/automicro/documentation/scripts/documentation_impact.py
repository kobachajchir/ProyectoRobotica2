"""Consulta enlaces documentales; no relee código ni ejecuta validaciones.

--file web:src/ruta.ts o --finding AMDOC-WEB-004. El índice publicado es una
fotografía: actualizar build_connections.py tras cambiar ownership/enlaces.
"""
from pathlib import Path
import argparse
import json
BASE=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser(description=__doc__)
group=parser.add_mutually_exclusive_group(required=True)
group.add_argument('--file');group.add_argument('--finding')
options=parser.parse_args()
model=json.loads((BASE/'published/shared/connections.json').read_text(encoding='utf-8'))
key=options.file or options.finding
entry=(model['sourceToDocumentation'] if options.file else model['findingToDocumentation']).get(key)
print(json.dumps({'selector':key,'basis':'published-documentation-index','entry':entry,
    'note':'Un resultado vacío no acredita ausencia de dependencia. Includes/imports y referencias de pantalla son candidatos estáticos.'},ensure_ascii=False,indent=2))
