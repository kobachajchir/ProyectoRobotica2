"""Informe explicativo desde resultados mecanicos ya calculados; no ensayos."""
from pathlib import Path
from collections import defaultdict
import json

H=Path(__file__).resolve().parent


def main():
    actual=json.loads((H/'cargas_cad_actuales.json').read_text())
    proposed=json.loads((H/'actuadores_integrados_cargas_cad_actuales.json').read_text())
    selected=json.loads((H/'preseleccion_actuadores_integrados.json').read_text())
    geometry=json.loads((H/'geometria_v17.json').read_text())
    parts=json.loads((H/'masas_cad_actuales.json').read_text())
    if actual['geometry_sha256']!=proposed['geometry_sha256']:raise RuntimeError('Resultados de revisiones diferentes')
    material=defaultdict(float)
    for part in parts:
      if part['material']!='Carga':material[part['material']]+=part['mass_g']
    native=['J3','J6','pinza']+[j for j in ['J1','J2','J4','J5'] if geometry.get('integrated_'+j+'_mount')]
    missing=[j for j in ['J1','J2','J4','J5'] if j not in native]
    lines=[
      '# Informe de masas, centros de masa y esfuerzos del brazo v17',
      '',
      'AFR2-20261029-03. Resultados de la revision identificada porSHA, no de un brazo fabricado.',
      '',
      '## Por que se itera el diseno',
      '',
      'Se mantiene el antropomorfo y la pinzaV7. Se acortan los ejes a70/60mm,',
      'se usan tubos de PVC de 25 x 1,2 y 20 x 1 mm y se lleva el motor de J3 junto al hombro. Una',
      'correa con poleas de 20 y 50 dientes transmite hasta el codo, sin cruzar el giro axial J4.',
      'El cambio se decide por masa y palanca, incluyendo el peso de los',
      'actuadores nuevos antes de modificar cada alojamiento.',
      '',
      'El motor de J3 esta a 19,533 mm sobre J2. Comparando el mismo',
      'motor de 75,5 g situado a 70 mm y a 19,533 mm, su contribucion gravitatoria',
      'al par de J2 con el brazo horizontal baja aproximadamente de 0,0518 a 0,0145 Nm.',
      'Esta comparacion aislada no sustituye el calculo del conjunto: la',
      'polea, correa, soportes, ejes, herrajes y sus inercias tambien cuentan.',
      '',
      'Los tubos pesan aproximadamente5.46 y2.47g. El grueso de la masa',
      'permanece en piezas impresas, metales, motores y base; hacer solamente',
      'los tubos mas finos no resuelve por si solo el peso total.',
      '',
      '## Modelo y masas actuales',
      '',
      f"CAD actual sin pieza: **{actual['mass_without_payload_g']:.1f}g**. ",
      f"Propuesta con actuadores/reservas restantes: **{selected['mass_budget_without_payload_g']:.1f}g**, sin pieza.",
      'La pieza de referencia mide 50 x 50 x 50 mm y se calcula con 200 g.',
      'No se incluye el auto ni las referencias80/81/82 en esas masas.',
      '',
      'Actuadores nativos: '+', '.join(native)+'.',
      'Montajes nuevos pendientes: '+(', '.join(missing) if missing else 'ninguno deJ1/J2/J4/J5')+'.',
      'No tomar las reservas como piezas ya construidas.',
      '',
      '| Grupo actual | Masa g | CG local X,Y,Z mm |',
      '|---|---:|---|',
    ]
    for group in actual['groups']:
      if group['root'].startswith('90_'):continue
      cg=', '.join(f'{v:.2f}' for v in group['cg_local_mm'])
      lines.append(f"|{group['root']}|{group['mass_g']:.2f}|{cg}|")
    lines+=['','LosCG se expresan respecto al marco de cada grupo, no todos respecto',
      'al suelo. Para elCG global se transforman mediante la cadena cinematica',
      'y se ponderan por masa: rG=sum(mi*ri)/sum(mi).','',
      '| Material o masa comercial | Total actual g |','|---|---:|']
    for kind,mass in sorted(material.items(),key=lambda row:-row[1]):lines.append(f'|{kind}|{mass:.2f}|')
    lines+=['','PLA macizo rho1.24g/cm3,Al6061=2.70,Al2024=2.78,PVC=1.45 supuesto.',
      'La masa impresa real requiere laminado, perfil y orientacion todavia',
      'no definidos. Motores, rodamientos y electronica usan masas comerciales',
      'o estimadas por ocurrencia; compartir una definicion no divide su masa.',
      'ElCG/inercia de un producto comercial se aproxima distribuyendo su masa',
      'sobre la envolvente, sin representar sus interiores exactos.','',
      '## Caso de carga y limites del modelo','',
      'Se calcula gravedad, aceleracion lineal, aceleracion angular y termino',
      'giroscopico. Se proyectan fuerzas/momentos mediante trabajo virtual',
      'sobre cada eje. Se incluyen relaciones4 enJ2,2.5 enJ3 y1 enJ4.',
      'El caso base tiene J2=90,J3=0 y variaciones de muneca; se muestrean',
      f"{proposed['samples']}estados con combinaciones de signos de movimiento.",
      '',
      'La frenada usada es una HIPOTESIS finita:30grados/s por eje y50ms con',
      'perfil cubico de velocidad. Su desaceleracion pico es900grados/s2.',
      'No es una parada instantanea ni una aceleracion maxima nominal medida',
      'del fabricante. No se conoce la inercia interna del rotor ni el frenado',
      'real del servo. El muestreo no demuestra un maximo global de6ejes.',
      '',
      '## Pares para la propuesta con masas y reservas','',
      '| Eje | Actuador | Reduccion | Pico externo Nm | Capacidad estimada Nm | Margen |',
      '|---|---|---:|---:|---:|---:|']
    for row in selected['axes']:
      lines.append(f"|{row['joint']}|{row['motor']}|{row['ratio']}|{row['external_peak_Nm']:.4f}|{row['capacity_Nm']:.4f}|{row['margin']:.3f}|")
    lines+=['','Capacidades con par NOMINAL16kgfcm a12V paraSTS3250 y1.5kgfcm a6V',
      'paraSTS3032, no con par de bloqueo. Eficiencias0.95directas y0.8en',
      'transmisiones son hipotesis. Margen de preseleccion pedido:1.2.',
      'La relacion par/velocidad se trata por separado en capacidad_actuadores_resultados.json.',
      '',
      '## Cargas sobre base y primer eslabon','',
      'La fuerza resultante y el momento resultante incluyen cortante, flexion',
      'y torsion. No son solamente el par que tiene que entregar el motor.',
      'Componentes maximas individuales, no necesariamente simultaneas:',
      '',
      '| Seccion propuesta | Fuerza resultante maxima N | Momento resultante maximo Nm | max Fx,Fy,Fz N | max Mx,My,Mz Nm |',
      '|---|---:|---:|---|---|']
    for joint,row in proposed['section_peaks'].items():
      f=', '.join(f'{v:.3f}' for v in row['local_components_abs'][:3]);m=', '.join(f'{v:.3f}' for v in row['local_components_abs'][3:])
      lines.append(f"|{joint}|{row['force_N']:.3f}|{row['moment_Nm']:.3f}|{f}|{m}|")
    lines+=['','EnJ1 el eje actuado esZ; Mx/My son flexion que toman eje, apoyos y',
      'montura. EnJ2 el eje actuado esY. No usar el maximo de un motor para',
      'aprobar automaticamente un cartucho, horquilla, manguito o piezaFDM.',
      'La carga de una seccion de tubo requiere cortar el camino de cargas',
      'correcto, incluyendo reacciones de correa y motores: no sustituirla',
      'por un momento articular agregado donde puedan cancelarse cargas.',
      '',
      '## Que falta para aprobar piezas','',
      'Las comprobaciones anteriores seleccionan actuadores y proporcionan',
      'cargas; no son un analisis de tensiones de las uniones. Se necesitan',
      'condiciones reales de impresionPLA, tolerancias, apriete, interfaces y',
      'propiedades resistentes verificadas. Los materiales nativos marcados',
      'SOLO MASA/SOLO DENSIDAD se excluyen de FEA. ElPLA existente declara',
      'E1500hipotetico, sin resistencia intercapas certificada.',
      '',
      'Shape Optimization queda preparado para una pasada posterior, con',
      'interfaces/rodamientos/agujeros preservados. Su resultado requiere',
      'redisenar CAD fabricable y comprobar tensiones por separado. No se',
      'resolvio un estudio en nube. Por indicacion del usuario no se continua',
      'calculando vuelco. La interfazTTL y alimentaciones12V/6V siguen pendientes.',
      '',
      '## Trazabilidad','',
      'Geometria fuente: `geometria_v17.json`.',
      f"SHA256: `{actual['geometry_sha256']}`.",
      'Calculos: calcular_cad_v17.py y preseleccionar_actuadores_integrados.py.',
      'Revision de geometria: REVISION_MONTAJE_J2.md y JSON brutos asociados.',
      'No se ejecutaron tests de software, compilaciones, flasheos ni ensayos fisicos.',
    ]
    (H/'INFORME_PESOS_Y_ESFUERZOS_v17.md').write_text('\n'.join(lines)+'\n',encoding='utf-8')
    print('Informe actualizado desde resultados mecanicos existentes.')


if __name__=='__main__':main()
