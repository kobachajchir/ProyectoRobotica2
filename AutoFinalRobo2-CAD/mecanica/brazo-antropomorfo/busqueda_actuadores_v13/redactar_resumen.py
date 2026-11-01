"""Publica tablas del calculo mecanico; no ejecuta tests ni CAD."""
from pathlib import Path
import json,csv,math
from collections import defaultdict
H=Path(__file__).resolve().parent
d=json.loads((H/'resultados_liviano.json').read_text(encoding='utf-8'))
s=json.loads((H/'secciones_preliminares.json').read_text(encoding='utf-8'))
candidate=next(r for r in d['rows'] if r['name']=='centrado_PWM_L90_75')
reserved=next(r for r in d['rows'] if r['name']=='centrado_PWM_L90_75_reserva25')
parts=json.loads((H/'candidato_compacto_componentes.json').read_text(encoding='utf-8'))['parts']
mat=defaultdict(float)
for p in parts:mat[p['material']]+=p['mass_g']
lines=['# Resultados numericos v13','',
    'AFR2-20261028-09. Prediseno externo, sin nuevo CAD ni resistencia fisica aprobada. Masa del auto1.5kg aproximada; CG centrado supuesto.', '',
    '## Comparacion de alternativas','',
    '| Caso | Masa con200g y reservas (g) | Minimo par disponible/demanda en sensibilidad | Vuelco ejes inferidos | Vuelco pies320 |',
    '|---|---:|---:|---:|---:|']
for r in d['rows']:
    lines.append(f"| {r['name']} | {r['total_with_payload_g']:.1f} | {min(r['linear_speed_sensitivity_min_ratio']):.3f} | {r['ground_support'][0]['maximum_overturning_utilization']:.3f} | {r['ground_support'][-1]['maximum_overturning_utilization']:.3f} |")
lines+=['','Par>=2 es filtro preliminar; utiliza pares maximos y recta par/velocidad SUPUESTA, no pares continuos acreditados. Vuelco<1 indica centro de presiones dentro del rectangulo, sin demostrar toda la distribucion/friccion. Los casos PWM incluyen250g de pies; reserva25 aumenta estructura y pies25%, manteniendo masas de motores/electronica/bateria.', '',
    '## Candidato compacto90/75','',
    f"- Masa sin carga, con pies: **{candidate['without_payload_g']:.1f}g**; con carga: **{candidate['total_with_payload_g']:.1f}g**.",
    f"- Sin carga ni pies propuestos: **{candidate['without_payload_g']-250:.1f}g**. Pies250g ya incluidos arriba, no masa gratuita.",
    f"- Comparacion v09 sin carga2200.43g: ahorro con pies **{100*(1-candidate['without_payload_g']/2200.4327):.1f}%**, previo a definir todo el montaje.",
    f"- Con auto1.5kg y carga200g: **{1.5+candidate['total_with_payload_g']/1000:.3f}kg** totales.",
    f"- Reserva estructural25%: **{reserved['without_payload_g']:.1f}g** sin carga, no es una cota de masa real garantizada.",
    '- Alcance al centro de la pieza373mm frente a458mm nativos; hombro100mm frente a209mm. Se mantiene toda la cadena distal55/65/88mm.',
    '', '| Material/funcion | Masa presupuestada(g) |','|---|---:|']
for k,v in mat.items():lines.append(f'| {k} | {v:.2f} |')
lines+=['','Los servos suman401.0g: son mas livianos que los RS01 descartados, pero no menos que los375.2g de servos v09. El ahorro total procede principalmente de PLA/bridas/control; esas piezas y los77g de electronica todavia no son un BOM comprado ni un CAD definitivo.', '',
    '## Demandas por eje','',
    '| Eje | Demanda pico motor(kgfcm) | Par maximo usado(kgfcm) | Minimo cociente con recta supuesta | Fuerza resultante(N) | Momento resultante(Nm) |',
    '|---|---:|---:|---:|---:|---:|']
for j in range(6):lines.append(f"| J{j+1} | {candidate['peak_motor_kgfcm'][j]:.3f} | {candidate['capacity_kgfcm'][j]:.2f} | {candidate['linear_speed_sensitivity_min_ratio'][j]:.3f} | {candidate['peak_joint_force_N'][j]:.3f} | {candidate['peak_joint_moment_Nm'][j]:.4f} |")
lines+=['','Fuerza y momento son normas maximas de envolvente, no componentes simultaneas de un unico estado. No hay inercias internas conocidas de servos.', '',
    '## Masa y centro de masa por grupo','',
    '| Grupo | Masa(g) | CG local X,Y,Z(mm) |','|---|---:|---|']
for row in candidate['groups']:lines.append(f"| {row['root']} | {row['mass_g']:.2f} | {', '.join(f'{x:.2f}' for x in row['cg_local_mm'])} |")
lines+=['','00 es fijo al auto;01 es base movil antes de hombro;02 brazo;03 antebrazo;04/05 muñeca;06 herramienta;90 carga. Coordenadas locales de cada articulacion; tensor completo enJSON. El CG global horizontal se guarda en resultados_liviano.json, no es el CG medido del auto completo.', '',
    '## Resistencia minima requerida','',
    '| Caso | Maximo Rp requerido eje(MPa) | Maximo Rp requerido placas(MPa) | Rp montura(MPa) | C0 maximo requerido(N) | Mu de pads requerido para factor2 |',
    '|---|---:|---:|---:|---:|---:|']
for r in s:
    lines.append(f"| {r['name']} | {max(x['required_Rp_n2_Kt2_MPa'] for x in r['shafts']):.1f} | {max(x['required_Rp_n2_Kt2_MPa'] for x in r['plates']):.1f} | {r['base_plate']['required_Rp_n2_Kt2_MPa']:.1f} | {max(x['required_C0_upper_n2_N'] for x in r['bearings']):.1f} | {r['gripper']['minimum_pad_mu_for_factor2']:.3f} |")
lines+=['','n=2/Kt=2 provisionales en ejes/placas; rodamientos usan cotaFr+Fa con n2 y separaciones propuestas. Requiere acero certificadoRp>=600MPa, aluminio certificadoRp>=240MPa y ligamentos netos declarados. No basta con esa tabla: faltan acoples/ranuras, fijaciones, asientos, acrilico y apoyos. El coeficiente de pads es requisito dinamico, no un dato medido; la pinza usa par de bloqueo y eficiencia.7 supuesta.', '',
    '| Tubo del candidato | Sigma envolvente(MPa) | Tau envolvente(MPa) | Resistencia efectiva requerida con n2/Kt2(MPa) |',
    '|---|---:|---:|---:|']
ss=next(r for r in s if r['name']==candidate['name'])
for x in ss['tubes']:lines.append(f"| {x['name']} | {x['envelope_sigma_MPa']:.3f} | {x['envelope_tau_MPa']:.3f} | {x['required_effective_strength_n2_Kt2_MPa']:.3f} |")
lines+=['','Al PVC no se le asigna aqui una resistencia medida. Al PLA PrintaLot tampoco se le asigna el valor de la resina como resistencia FDM. Fluencia diferida, temperatura, taladros y contacto permanecen pendientes.', '',
    '## Traccion con la masa de este candidato','',
    'Mismo escenario de sensibilidad: bateria7.4V, motorV=7.4-1.2x.49, eficiencia.6, a=.5m/s2,Crr=.05 y masa rotante equivalente1kg. La transferencia par/tension es una hipotesis de cribado.', '',
    '| Caso | Masa total(kg) | Pendiente(deg) | Fuerza requerida(N) | Fuerza diagonal disponible(N) | Cociente |','|---|---:|---:|---:|---:|---:|']
for r in [candidate,reserved]:
    mass=1.5+r['total_with_payload_g']/1000
    for angle in [0,5]:
        th=math.radians(angle);req=mass*(.5+9.80665*math.sin(th)+.05*9.80665*math.cos(th))+.5
        avail=4*.2941995/.04*((7.4-1.2*.49)/9)*.6/math.sqrt(2)
        lines.append(f"| {r['name']} | {mass:.3f} | {angle} | {req:.3f} | {avail:.3f} | {avail/req:.3f} |")
lines+=['','Estos resultados no prueban arranque, desplazamiento real ni frenada del AGV. No se usa par de bloqueo para declarar una carga maxima enkg.', '',
    '**Conclusion:** candidata de predimensionamiento con condicion de apoyos; compra y nuevos housings siguen sin cerrar. No se ha demostrado aun el cumplimiento fisico simultaneo del objetivo.']
(H/'RESUMEN_NUMERICO.md').write_text('\n'.join(lines)+'\n',encoding='utf-8')
with (H/'candidato_masas_CG.csv').open('w',newline='',encoding='utf-8') as f:
    w=csv.writer(f);w.writerow(['root','component','material','mass_g','cg_local_x_mm','cg_local_y_mm','cg_local_z_mm'])
    for p in parts:w.writerow([p['root'],p['name'],p['material'],p['mass_g'],*p['cg_local_mm']])
print('Candidato',candidate['total_with_payload_g'],'g con carga; margen',min(candidate['linear_speed_sensitivity_min_ratio']))
