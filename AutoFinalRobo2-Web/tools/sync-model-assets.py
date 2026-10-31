"""Copia activos3D sin compilar la Web ni sustituir sus HTML/JS/CSS.

Pedido AFR2-20261031-05: public -> dist -> paquetes gzip Web/ESP.
Actualiza solo entradas de modelos/decodificadores y sus totales.
"""
from pathlib import Path
import json,gzip,hashlib,datetime,shutil,math

WEB=Path(__file__).resolve().parents[1];ROOT=WEB.parent
STAMP=datetime.datetime.now(datetime.timezone.utc).isoformat()
MODELS=[('auto','AutoFinalRobo2.glb'),('brazo','AFR2_Brazo_v17_Blanco.glb'),('cubo','AFR2_Cubo_50mm.glb')]
catalog=dict(schema='afr2-model-assets-1',revision='AFR2-20261031-05',units='m',models=[])
for ident,name in MODELS:
 source=WEB/'public/models'/name;raw=source.read_bytes()
 catalog['models'].append(dict(id=ident,url='/models/'+name,bytes=len(raw),sha256=hashlib.sha256(raw).hexdigest(),auxiliary=ident=='cubo'))
(WEB/'public/models/catalog.json').write_text(json.dumps(catalog,indent=2),encoding='utf-8')
routes=['models/'+name for _,name in MODELS]+['models/catalog.json','draco/draco_decoder.wasm','draco/draco_wasm_wrapper.js']
entries=[];copies=[]
for route in routes:
 source=WEB/'public'/route;raw=source.read_bytes();packed=gzip.compress(raw,compresslevel=9,mtime=0)
 dst=WEB/'dist'/route;dst.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source,dst);copies.append(str(dst.relative_to(ROOT)))
 for base in [WEB/'artifacts/littlefs/dist',ROOT/'AutoFinalRobo2-ESP/data/dist']:
  target=base/(route+'.gz');target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(packed);copies.append(str(target.relative_to(ROOT)))
 content='model/gltf-binary' if route.endswith('.glb') else 'application/wasm' if route.endswith('.wasm') else 'application/json; charset=utf-8' if route.endswith('.json') else 'application/javascript; charset=utf-8'
 entries.append(dict(route='/'+route,servedPath='/'+route+'.gz',contentType=content,contentEncoding='gzip',rawBytes=len(raw),gzipBytes=len(packed),sha256=hashlib.sha256(raw).hexdigest()))
packages=[]
for base in [WEB/'artifacts/littlefs/dist',ROOT/'AutoFinalRobo2-ESP/data/dist']:
 path=base/'manifest.json'
 if not path.exists():raise RuntimeError('Falta manifest del paquete existente: '+str(path))
 manifest=json.loads(path.read_text(encoding='utf-8'));replacements={e['route']:e for e in entries}
 files=[e for e in manifest['files'] if e['route'] not in replacements]+entries
 files.sort(key=lambda e:e['route'])
 # Misma politica de tools/web-budget.mjs, sin ejecutar builds/validadores.
 estimate=65536+sum(math.ceil((e['gzipBytes']+512)/8192)*8192 for e in files)
 if estimate>manifest['filesystemBudgetBytes']:raise RuntimeError('Activos no caben en presupuesto del paquete')
 manifest.update(files=files,totalRawBytes=sum(e['rawBytes'] for e in files),totalGzipBytes=sum(e['gzipBytes'] for e in files),estimatedFilesystemBytes=estimate,
  modelAssetsUpdatedAt=STAMP,modelAssetsRevision='AFR2-20261031-05',modelAssetsOnlyUpdate='Solo modelos, catalogo y Draco. HTML/JS/CSS conservan build anterior; interfaz nueva requiere compilacion posterior.')
 path.write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf-8')
 packages.append(dict(path=str(base.relative_to(ROOT)),estimated_filesystem_bytes=estimate,gzip_bytes=manifest['totalGzipBytes'],app_rebuilt=False))
report=dict(models=catalog['models'],copies=copies,packages=packages,build=False,flashed=False)
target=ROOT/'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/web_blanco_completo/copias_modelos.json'
target.write_text(json.dumps(report,indent=2),encoding='utf-8');print(json.dumps(dict(models=catalog['models'],packages=packages,copies=len(copies))))
