"""Compara solo PLA de BRep actual con medicion previa y 3D previo terminado."""
from pathlib import Path
from collections import defaultdict
import json,hashlib,csv
HERE=Path(__file__).resolve().parent
current=json.loads((HERE/'pesos_impresion_20261031.json').read_text(encoding='utf-8'))
finished=json.loads((HERE/'pesos_impresion_pre_optimizacion_6041204.json').read_text(encoding='utf-8'))
historic_path=HERE.parent/'geometria_v17.json'
historic=json.loads(historic_path.read_text(encoding='utf-8'))
rho=current['density_g_cm3']
old_parts=[p for p in historic['parts'] if (p.get('material_v17') or '').startswith('PLA') or p['appearance'] in ['AFR2_PLA_PrintALot_Rojo','AFR2_PETG_Rojo']]
last=sum(p['volume_cm3']*rho for p in old_parts)
now=current['current']['total_solid_g'];before=finished['total_solid_g']
a=defaultdict(float);b=defaultdict(float)
for p in finished['parts']:a[p['component']]+=p['mass_solid_g']
for p in current['current']['parts']:b[p['component']]+=p['mass_solid_g']
rows=[dict(component=n,before_finished_g=a[n],current_g=b[n],saving_g=a[n]-b[n]) for n in sorted(a.keys()|b.keys(),key=lambda n:b[n]-a[n])]
result=dict(scope='Solo impresos PLA macizos, sin auto, carga, metales, motores, rodamientos, electronica, PVC ni laminadoFDM.',density_g_cm3=rho,last_measurement_g=last,last_geometry_sha256=hashlib.sha256(historic_path.read_bytes()).hexdigest(),finished_before_user_g=before,finished_source_commit=finished['source_commit'],current_g=now,saving_vs_last_measurement_g=last-now,saving_vs_last_measurement_pct=100*(last-now)/last,saving_vs_finished_g=before-now,saving_vs_finished_pct=100*(before-now)/before,components=rows)
(HERE/'comparacion_pesos_impresion.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
with (HERE/'masas_impresos_por_pieza.csv').open('w',encoding='utf-8-sig',newline='') as fp:
 writer=csv.DictWriter(fp,fieldnames=list(rows[0]));writer.writeheader();writer.writerows(rows)
lines=['# Masa de las piezas impresas: PLA PrintALot blanco','',
 'AFR2-20261031-03. Solo masa geometrica de los impresos del brazo abierto, sin modificarlo.','',
 f"**Actual: {now:.1f} g**, en {current['current']['total_volume_cm3']:.2f} cm3 de material modelado; {current['current']['instances']} piezas impresas.",
 '', '| Referencia | Masa PLA maciza | Ahorro actual |', '|---|---:|---:|',
 f'| Ultima medicion guardada, antes del cierre J1/J4/J5 | {last:.1f} g | {last-now:.1f} g ({100*(last-now)/last:.1f} %) |',
 f'| 3D terminado antes de las optimizaciones manuales, archivo6041204 | {before:.1f} g | {before-now:.1f} g ({100*(before-now)/before:.1f} %) |',
 f'| Modelo abierto actual | {now:.1f} g | — |','',
 'La segunda referencia se mide ahora desde el F3D terminado anterior: no era la ultima medicion guardada. Permite separar los cambios de los nuevos housings de las optimizaciones manuales posteriores. Se abrio una copia temporal y se restituyo el documento actual sin guardarlo ni cambiar su geometria.','',
 '## Por pieza: antes de optimizar frente a actual','',
 '| Componente | Anterior terminado (g) | Actual (g) | Ahorro (g) |','|---|---:|---:|---:|']
for row in rows:lines.append(f"| {row['component']} | {row['before_finished_g']:.2f} | {row['current_g']:.2f} | {row['saving_g']:.2f} |")
lines+=['','## Metodo y limites','',
 'Masa = volumen neto del solido CAD en cm3 x1.24g/cm3. Se cuentan las ocurrencias, no solo definiciones compartidas. Se incluyen los vaciados, ventanas y agujeros modelados. La apariencia blanca no altera la densidad ni convierte otras piezas en PLA.',
 '', 'Densidad tomada de PrintALot_PLA_TDS.pdf, tabla Densidad/ASTMD792:1.24g/cm3. Copia local verificada por SHA256 80aa99ebd931b2533735f68aade3b886916bf8f4a4b5975b61df2f484a618e08; procedencia en revision_v09/README.md.',
 '', 'Es una estimacion maciza: no es el consumo de filamento del laminador ni un pesaje fisico. Paredes, relleno, tapas, soportes, brim y desperdicio aun sin definir. No calcular la masa real multiplicando este total por el porcentaje de relleno: las paredes y tapas tambien consumen material.',
 '', 'No incluye caños PVC, metal, motores, rodamientos, electronica ni pieza transportada. No se recalculan esfuerzos, pares, vuelco ni resistencia. No se usan propiedades fisicas nativas que quedaron pendientes de sincronizar; se aplica la misma densidad externamente a las tres referencias.',
 '', 'La reduccion del GLB a596.484bytes es independiente de esta reduccion fisica de material. Se mide BRep deFusion; no el volumen de las mallas simplificadas deBlender.',
 '', 'Evidencia: pesos_impresion_20261031.json, pesos_impresion_pre_optimizacion_6041204.json, comparacion_pesos_impresion.json y masas_impresos_por_pieza.csv.']
(HERE/'MASA_IMPRESION_PLA.md').write_text('\n'.join(lines)+'\n',encoding='utf-8')
print(json.dumps({k:v for k,v in result.items() if k!='components'}))
