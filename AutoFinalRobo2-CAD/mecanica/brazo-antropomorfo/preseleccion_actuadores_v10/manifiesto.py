"""Registrar origen y hashes de la evidencia numerica, sin recalcularla."""
from pathlib import Path
import hashlib,json
H=Path(__file__).resolve().parent;V=H.parent/'revision_v09'
export=H.parents[2]/'exports'/'analisis-actuadores-v10'/'cargas_comparadas.npz'
def sha(p):
    with p.open('rb') as fp:return hashlib.file_digest(fp,'sha256').hexdigest()
r=json.loads((H/'resultados.json').read_text(encoding='utf-8'))
out=dict(record='AFR2-20261028-06',date='2026-10-28',baseline_commit='172b6d0',CAD_modified=False,
    baseline_geometry_sha256=sha(V/'geometria_v09.json'),
    native_v09_sha256=sha(H.parent/'AFR2_Brazo_PVC32_AGV_v09_J1_GX3270BLS.f3d'),
    evidence_counts=r['baseline'],
    regenerable_export=dict(path='AutoFinalRobo2-CAD/exports/analisis-actuadores-v10/cargas_comparadas.npz',size_bytes=export.stat().st_size,sha256=sha(export),tracked=False),
    sources={name:sha(H/name) for name in ['comparar.py','uniones.py','informar.py','candidatos.json']},
    execution_order=['comparar.py','uniones.py','informar.py','manifiesto.py'],
    dependencies=['Python3.12 usado','numpy','matplotlib'],
    note='Solo calculos y documentacion. Sin Fusion nuevo, FEA, ensayo fisico ni aceptacion de compra/fabricacion. La exportacion completa es regenerable y se excluye de Git; JSON/CSV publican los resultados.')
(H/'evidencia_calculo.json').write_text(json.dumps(out,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps({'record':out['record'],'export_MB':export.stat().st_size/1e6,'cases':r['baseline']['cases'],'source_states':r['baseline']['source_states']}))
