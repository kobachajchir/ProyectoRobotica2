"""Informe/figura desde los resultados numericos; no ejecuta CAD ni ensayos."""
from pathlib import Path
import json,math,csv,zipfile,sys
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
H=Path(__file__).resolve().parent
load=lambda n:json.loads((H/n).read_text(encoding='utf-8'))
r=load('resultados.json');u=load('uniones_resultados.json');cfg=load('candidatos.json');grip=load('pinza_carga.json')
rr={x['name']:x for x in r['results']};uu={x['name']:x for x in u['rows']}
EXPORT=H.parents[2]/'exports'/'analisis-actuadores-v10'
data=np.load(EXPORT/'cargas_comparadas.npz')
f=lambda x:f'{x:.2f}'.replace('.',',')

# Exact affine scaling of the declared reserve masses within this rigid model.
# A change of unknown shape/CG is not represented by this interpolation.
cap=np.array([70,70,70,1.8,24,1.8])/2
i0=list(data['case_names']).index('GX70_J2_DS8930_J5_8V4')
i1=list(data['case_names']).index('GX70_DS8930_montajes_8V4')
trajectory_labels=data['source_trajectory_labels'];source_case_index=data['case_index']
indices=[i for i,c in enumerate(source_case_index) if str(trajectory_labels[c]).endswith('_50ms')]
axis=[2,1,1,2,1,2];denom=np.array([1,1.7,1,.9,1,1])*.0980665
selected=set(indices);small=[]
with zipfile.ZipFile(EXPORT/'cargas_comparadas.npz') as archive:
    with archive.open('F_M_local.npy') as fp:
        version=np.lib.format.read_magic(fp)
        shape,fortran,dtype=np.lib.format.read_array_header_1_0(fp)
        if fortran:raise ValueError('Se esperaba orden C para lectura por filas')
        row_size=int(np.prod(shape[1:]))*dtype.itemsize
        for i in range(shape[0]):
            buf=fp.read(row_size)
            if i not in selected:continue
            row=np.frombuffer(buf,dtype=dtype).reshape(shape[1:])
            small.append([[row[j,k,1,axis[j]]/denom[j] for k in [i0,i1]] for j in range(6)])
signed=np.transpose(np.array(small),(1,0,2))
nom=signed[:,:,0];delta=signed[:,:,1]-nom
allow=np.inf;limiter=None
for j in [1,2]:
    for k in range(len(indices)):
        if abs(delta[j,k])<1e-12:continue
        upper=((cap[j] if delta[j,k]>0 else -cap[j])-nom[j,k])/delta[j,k]
        if upper<allow:allow=float(upper);limiter='J'+str(j+1)
budget=dict(scope='Solo J2/J3, 8.4V, parada50ms, candidato DS8930 sin cambiar J4. Escalado uniforme de50g de reservas; no valida los otros ejes ni compra.',
            uniform_reserve_fraction_max=max(0,allow),reserve_mass_max_g=max(0,allow)*50,limiter=limiter)
(H/'presupuesto_masa_reserva.json').write_text(json.dumps(budget,ensure_ascii=False,indent=2),encoding='utf-8')
data.close();del data,small,signed,nom,delta

show=[('v09_actual','50','Actual · 50 ms'),('GX70_J2_GX55_J5_7V4','50','Muñeca GX55 · 50 ms'),
      ('ligero5_montajes_7V4','50','Opción ligera + reservas · 50 ms'),('ligero5_montajes_7V4','100','Opción ligera + reservas · 100 ms')]
fig,ax=plt.subplots(figsize=(11,5.4),layout='constrained')
x=np.arange(5);width=.18
for i,(name,stop,label) in enumerate(show):
    values=[2*j['peak_servo_kgfcm']/j['advertised_peak_kgfcm'] for j in rr[name]['scenarios'][stop]][:5]
    ax.bar(x+(i-1.5)*width,values,width,label=label,color=['#526779','#d47b34','#be4545','#3d8b78'][i])
ax.axhline(1,color='#222',lw=1.2);ax.axhspan(1,2.6,color='#ce4d4d',alpha=.07)
ax.set(xticks=x,xticklabels=['J1 base','J2 hombro','J3 codo','J4 giro muñeca','J5 inclinación'],
       ylabel='Exigencia preliminar 2 × par calculado / pico anunciado',ylim=(0,2.6),
       title='El cambio de motor debe incluir la masa que agrega a los ejes anteriores')
ax.grid(axis='y',alpha=.18);ax.set_axisbelow(True);ax.legend(ncol=2,loc='upper left',fontsize=8.5)
fig.text(.5,-.025,'≤ 1 pasa esta comparación nominal. No acredita par continuo, frenado real, impresión o estabilidad.',ha='center',fontsize=9)
if '--skip-plot' not in sys.argv:fig.savefig(H/'comparacion_margenes.png',dpi=180,bbox_inches='tight')
plt.close(fig)

lines=['# Comprobación de motores y masas antes de modelar','',
'28/10/2026 · AFR2-20261028-06. CAD v09 intacto. Se estudian masas, posiciones y tensores alternativos sobre las piezas actuales; no se modelan motores ni housings nuevos.','',
'**Resultado: no se justifica un reemplazo masivo por motores más pesados para prometer la parada de50 ms.** Hay una opción ligera que pasa la comparación preliminar de pares×2 en100 ms, con reservas de montaje, pero todavía faltan recorrido documentado de los dos Dualsky, curvas de servicio/frenado, agarre, alimentación y estabilidad. Es preselección por cálculo, no selección de compra o capacidad física aprobada.','',
'## Qué pesa hoy y quién lo soporta','',
'El conjunto actual se estima en2.400,43 g con carga200 g y sin el auto. Incluye1.014,88 g de PLA PrintaLot macizo. Se conserva su geometría, masa de PVC, herrajes y electrónica. Los100 g del servo J2 son un presupuesto conservador anterior, no una pesada neta confirmada; sus diferencias de masa se expresan respecto de ese presupuesto.','',
'| Motor | Masa actual usada | Dónde está fijado | A qué articulaciones agrega carga por su propio peso |','| --- | ---: | --- | --- |',
'| J2, GX3240 | 100 g supuestos | Torre giratoria, antes de J2 | Principalmente J1/base; su peso no actúa como carga distal sobre su propio J2 |',
'| J3, GX3270 | 90 g de catálogo | Primer eslabón, antes de J3 | J1 y J2; no se suma como carga distal de su propio J3 |',
'| J4, MG90S | 13,4 g | Antebrazo, antes de J4 | J1/J2/J3 |',
'| J5, MG946R | 55 g | Rotor de J4, antes de J5 | J1/J2/J3/J4; por eso un motor pesado de muñeca perjudica al MG90S de J4 |',
'| Pinza, MG90S | 13,4 g | Herramienta, después de J6 | Todos los ejes anteriores |','',
'Se incluye la inercia de traslación y de giro de los cuerpos. Las masas nuevas se representan por prismas uniformes de dimensiones publicadas, conservando el plano delantero y orientación de montaje como hipótesis; la distribución interna real, salientes/estriado y CG exacto siguen pendientes. El desplazamiento del eje dentro de la carcasa se infiere y no se presenta como cota comercial confirmada.','',
'## Candidatos encontrados','',
'| Candidato | Masa neta adoptada | Pico usado y tensión | Situación antes de fabricar |','| --- | ---: | --- | --- |']
for key in ['GX70','GX55','DS8930','DS9401','DS3225','MG92B']:
    m=cfg['motors'][key]
    torque='; '.join(str(v)+' kgf·cm @ '+k+' V' for k,v in m['torque_kgfcm'].items())
    lines.append(f"| {m['name']} | {m['mass_g']} g | {torque} | {m['availability']} |")
lines+=['',
'El título local del DS8930 dice64 g, pero la tabla y PDF oficial recuperado indican68 g: se toma68 g. Para DS9401 se usa3,5 kgf·cm a6 V y26 g del PDF, con7,4 V marcadoN/C. No se adopta el título local de4 kg/7,4 V. El GX55 se limita a7,4 V, según su ficha, aunque el título del anuncio incluye8,4 V.','',
'El MG92B posicional160° no sustituye1:1 un J4 que requiere180°. Sí alcanza el recorrido funcional de la pinza para el cubo50 mm: agarre en109,8° y apertura60 mm en≈133,7°, por debajo de160°. El máximo geométrico con160° sería≈71,03 mm. Exige confirmar la variante, horn, pulsos y límites, sin extrapolar automáticamente los180° del MG90S.','',
'Se descarta SPT5325LV-360 porque la fuente del fabricante identifica rotación continua sin realimentación angular accesible: PWM controla giro, no una posición. El DYM70 no se selecciona porque la página mezcla masa neta/bruta y dimensiones de empaque; tampoco su variante180° cubre el recorrido270° requerido por J2 con reducción2:1.','',
'## Cálculo reproducible y alcance','',
f"Se usan **{r['baseline']['source_states']:,} estados fuente**, {r['baseline']['trajectory_cases']} trayectorias/casos y{r['baseline']['cases']} combinaciones de masas/selección, entre ellas ocho sensibilidades del CG de J5 y una masa hipotética200 g en J3. Las cargas de J1 se transportan a **{r['baseline']['expanded_J1_states']:,} estados por alternativa** al orientar la base entre−90° y+90° cada10°. No son nuevos ensayos CAD.",'',
'Se conserva200 g,30°/s por eje y paradas suaves de200/100/50 ms con21 instantes por trayectoria. Además de los casos originales, se ensayan orientaciones finales J4/J6 en−90°,0°,+90°, manteniendo J2=90°,J3=0°,J5=0° y el brazo extendido. Se excluyen trayectorias que salen de los límites angulares actuales. Las muestras no prueban todo el espacio ni los máximos reales de velocidad/aceleración de un servo cargado.','',
'La suma por bases de masa/inercia se contrasta contra la suma directa por cuerpos y contra los4.411 estados originales de J1: las discrepancias máximas están en el orden de10⁻⁸ N y10⁻⁹ N·m. Se mantienen reducción2:1/eficiencia0,85 en J2 y eficiencia0,9 en J4. Los resultados ya están referidos al motor: no hay que volver a dividirlos por la reducción.','',
'No se dispone de curvas de frenado/regeneración ni de torque continuo. La sensibilidad linealτ≈τbloqueo(1−|velocidad|/velocidad_sin_carga) solo ilustra pérdida de capacidad con velocidad, no caracteriza estos servos. Tampoco se conocen rotor/inercia reflejada, flexibilidad, backlash, aceleración inicial arbitraria, temperaturas, tolerancias o parada del AGV. Una frenada instantánea no tiene un esfuerzo finito que pueda cerrarse con este modelo.','',
'## Cuánto empeora agregar masa','',
f"El centro de masa del conjunto actual, en la postura horizontal de inventario J1=90°/J2=90°, es **{[round(v,2) for v in rr['v09_actual']['cg_mm']]} mm**. Con la opción ligera completa y reservas pasa a **{[round(v,2) for v in rr['ligero5_montajes_7V4']['cg_mm']]} mm**, respecto del origen global del auto utilizado en v09.",'',
f"Ampliar las orientaciones de muñeca/herramienta lleva el pico de J1 actual a **{f(rr['v09_actual']['scenarios']['50'][0]['peak_servo_kgfcm'])} kgf·cm**, frente a16,30 del muestreo anterior más restringido. Ese incremento es por los casos añadidos, no por cambiar el CAD o la masa de partida.",'',
'| Variante de cálculo | Masa total | Cambio | J2 en50 ms | J3 en50 ms | J4 en50 ms |','| --- | ---: | ---: | ---: | ---: | ---: |']
names=['v09_actual','solo_J5_GX55','GX70_J2_DS8930_J5_8V4','ligero4_8V4','ligero5_montajes_7V4','sensibilidad_J3_200g']
for name in names:
    c=rr[name];jj=c['scenarios']['50']
    lines.append(f"| {name} | {f(c['mass_g'])} g | {f(c['delta_mass_g'])} g | {f(jj[1]['peak_servo_kgfcm'])} | {f(jj[2]['peak_servo_kgfcm'])} | {f(jj[3]['peak_servo_kgfcm'])} |")
lines+=['','Pares en kgf·cm. La variante J3 de200 g es una sensibilidad de masa, **no un producto comercial elegido**. Muestra por qué importa la posición de la masa:35 g en la muñeca pueden perjudicar más a J2 que110 g adicionales en el motor del codo, más próximo.','',
'La opción ligera completa usa GX70 en J2, mantiene GX70 en J1/J3, estudia DS9401 en J4, DS8930 en J5 y MG92B posicional en la pinza. Cambiar solo esos motores suma16 g frente al inventario. Se reservan15 g para cada uno de los montajes de J2/J3/J4/J5/pinza y5 g de cable en herramienta: **80 g de reserva y96 g netos de incremento**, hasta2.496,43 g. La reserva es una hipótesis localizada, no el peso de housings diseñados; no incluye una batería o regulación nuevas.','',
f"Incluso ignorando el problema de J4, la combinación mínima J2GX70/J5DS8930 a8,4 V deja aproximadamente **{f(budget['reserve_mass_max_g'])} g** de reserva uniforme del modelo antes de perder×2 en{budget['limiter']} a50 ms. Al agregar el motor de J4 y sus soportes, esa aparente holgura desaparece. Por eso no se dimensiona contra70 kgf·cm usando únicamente las piezas antiguas.",'',
'## Resultado de la opción ligera completa','',
'| Eje | Pico anunciado usado | Demanda50 ms | Pico/demanda50 ms | Demanda100 ms | Pico/demanda100 ms |','| --- | ---: | ---: | ---: | ---: | ---: |']
c=rr['ligero5_montajes_7V4']
for a,b in zip(c['scenarios']['50'],c['scenarios']['100']):
    lines.append(f"| {a['joint']} | {f(a['advertised_peak_kgfcm'])} | {f(a['peak_servo_kgfcm'])} | {f(a['stall_peak_ratio'])} | {f(b['peak_servo_kgfcm'])} | {f(b['stall_peak_ratio'])} |")
lines+=['',
'La comparación×2 falla a50 ms. A100 ms, los seis ejes pasan esta comparación nominal en la opción ligera, con J2 como limitante. La sensibilidad lineal torque–velocidad también queda por encima de2 en esos seis ejes a100 ms. **Eso no convierte100 ms en una frenada garantizada ni implementada**: la curva real de los actuadores y la electrónica debe demostrarlo.','',
'Elevar los GX a8,4 V tampoco cierra el caso50 ms con la reserva completa: J2/J3/J4 continúan debajo del objetivo. Se mantienen circuitos separados para los servos de6 V. Los MG90S se comparan conservadoramente contra1,8 kgf·cm a4,8 V; incluso tomando favorablemente2,2 kgf·cm, el J4 actual no llega a×2 en50 ms. La variante y tensión efectiva deben confirmarse.','',
'![Comparación de márgenes nominales](comparacion_margenes.png)','',
'## Uniones y tubos con las masas alternativas','',
'Se aplican las cargas a las **secciones y cuna actuales de v09**. Los housings futuros no existen y no se declara su resistencia. Se conservan el modelo de dos paredes de sección variable, puente rígido, apoyos6000 separados13 mm y las hipótesis de placa rígida de las fijaciones. E=1.500 MPa/ν0,38 del PLA son supuestos de rigidez, no propiedades FDM certificadas.','',
'| Magnitud nominal | Actual, mismas orientaciones | Opción ligera completa y reserva |','| --- | ---: | ---: |']
a=uu['v09_actual'];b=uu['ligero5_montajes_7V4']
for key,label,unit in [('bearing_P0_N','Carga equivalente de referencia6000','N'),('shaft_vm_nominal_bbox_bound_MPa','Cota nominal equivalente de referencia del eje','MPa'),('FDM_normal_requirement_n2_Kt2_MPa','Resistencia normal efectiva FDM requerida, n2/Kt2','MPa'),('anchor_tension_increment_N','Tracción incremental máxima en anclaje al auto','N'),('cart_M3_tension_increment_N','Tracción incremental máxima de tirante de cartucho','N'),('cart_washer_pressure_MPa','Presión nominal bajo arandela de tirante','MPa')]:
    lines.append(f"| {label} | {f(a[key])} {unit} | {f(b[key])} {unit} |")
lines+=['',
f"La referencia metálica con n2/Kt2 exige **{f(b['shaft_yield_requirement_n2_Kt2_MPa'])} MPa** frente al certificado propuesto≥600 MPa. La relación C₀/P₀ del6000 es≈{f(b['bearing_C0_over_P0'])}; no es un factor de seguridad de la impresión ni vida a fatiga. El eje usa una envolvente superior nominal por caja de sección; por eso no debe compararse su cifra directamente con los62,45 MPa de la evaluación puntual v09 anterior. La fila actual de esta tabla usa exactamente el mismo método y las mismas orientaciones que la alternativa.",'',
'| Tubo, sección libre a25 mm | Normal actual | Normal alternativa completa | Cortante/torsión alternativa |','| --- | ---: | ---: | ---: |']
for a,b in zip(rr['v09_actual']['pipes'],rr['ligero5_montajes_7V4']['pipes']):
    lines.append(f"| {a['section']} | {f(a['normal_nominal_MPa'])} MPa | {f(b['normal_nominal_MPa'])} MPa | {f(b['shear_envelope_MPa'])} MPa |")
lines+=['','Los tubos conservanØ32/Ø28,3 mm, pared1,85 mm. Se corta el grafo de cargas por unión y centroides; no se modela el contacto de manguitos. Se reportan axial+flexión y envolvente de torsión+cortante; no se asigna un admisible a un grado de PVC no identificado. Las uniones J2–J6 quedan acompañadas de sus fuerzas y momentos por eje enresultados/NPZ, sin aprobación local de asientos, insertos, engranajes o impresión.','',
'La comprobación de continuidad/interferencias v09 sigue siendo antecedente. **No se repite ni se hereda como prueba de montaje de los nuevos motores**: sus planos, housings y recorrido real todavía deben confirmar que el paquete es realizable.','',
'## Pinza y orientación de la carga','',
'La prueba con una sola orientación de pinza subestima la fricción necesaria: si una mordaza queda debajo de la pieza, gran parte del peso va por reacción normal. Se incluyen giros J4/J6 para estudiar la pieza sostenida también por fricción. La pieza sigue solidaria en el cálculo de movimiento; el análisis siguiente es una condición necesaria independiente.','',
'| Parada | Fuerza tangencial máxima de la pieza | Par necesario con μ=0,3 | μ mínimo con MG90S1,8 | μ mínimo con MG92B3,5 |','| --- | ---: | ---: | ---: | ---: |']
for g in grip['rows']:
    lines.append(f"| {g['stop_ms']} ms | {f(g['max_tangential_force_N'])} N | {f(g['required_servo_kgfcm_by_mu']['0.3'])} kgf·cm | {f(g['minimum_mu_translation_only']['1.8'])} | {f(g['minimum_mu_translation_only']['3.5'])} |")
lines+=['','Se usa2μN≥Ft, piñón de radio12 mm y eficiencia0,8 supuesta: ambos racks consumen par. Los mínimos deμ anteriores **no incluyen margen×2**; para ese criterio se duplican. Tampoco resuelven momentos de contacto, presión de almohadillas, flexión del dedo ni las curvas reales del servo. Debe medirse el agarre de las almohadillas elegidas. El MG92B es una alternativa por masa/par/recorrido funcional, no una prueba de agarre físico.','',
'## Alimentación y estabilidad','',
'Los tres GX3270 anuncian hasta7 A de bloqueo cada uno a7,4 V: suma21 A, antes de J4/J5/pinza/lógica. A8,4 V anuncian9 A cada uno: suma27 A. No es un consumo continuo calculado ni una fuente aprobada; se desconocen corrientes de bloqueo confirmadas de los Dualsky. Los reguladores, batería, cableado, protección y disipación actuales no quedan dimensionados por esta comparación de masas. Una batería2S no sostiene8,4 V regulados con un buck durante toda su descarga; cambiar la alimentación agrega otra masa que habría que incorporar.','',
'| Semihuella hipotética hacia el brazo | Masa mínima de auto sin brazo, CG centrado |','| --- | ---: |']
t=rr['ligero5_montajes_7V4']['static_tipping_sensitivity']['chassis_mass_min_g_for_halfspan']
for b,m in t.items():lines.append(f'| {b} mm | {f(m)} g |')
lines+=['','Es solo una sensibilidad estática sin factor de margen: m_auto ≥ m_brazo(y_CG−b)/b, con CG del auto en0. **Esas semihuellas no fueron medidas** y no identifican el polígono real de apoyo Mecanum. Falta masa/CG del auto y batería, posición de contactos y aceleración del AGV; por ello no se declara estabilidad.','',
'## Decisión antes de modelar','',
'1. **GX3270 en J2 merece la preselección por par/masa**. Está antes de J2 y su masa no se vuelve contra su propio hombro; sí carga J1. No hace falta reemplazar J3 por uno más pesado para la hipótesis100 ms: el GX3270 actual conserva margen nominal.','2. **No elegiría GX55 en J5 por defecto**: sus35 g extra elevan cargas de J2/J3 y especialmente J4. DS8930 es mejor candidato de masa, sujeto a recorrido y curva reales.','3. **J4 también necesita revisión para×2 en50 ms**. DS9401 es una alternativa de masa/par para estudiar, no compra cerrada: faltan recorrido180° y plano. El MG92B160° no conserva ese recorrido1:1, aunque sí sirve como candidato para la apertura funcional de pinza.','4. **Con el requisito50 ms mantenido, la combinación propuesta no se justifica como solución terminada**. Harían falta más capacidad real, compensación/reducción con el recorrido correcto, reducción de masas distales o una envolvente de movimiento distinta. No se oculta ese déficit aumentando solo el rótulo de torque.','5. **Para100 ms y30°/s existe una opción ligera que pasa el cribado nominal con reservas**, pero esa parada es una hipótesis distinta, no permiso para asegurar que cualquier emergencia frenará así. Confirmar los datos de los candidatos, agarre y alimentación antes de crear sus housings.','',
'## Archivos y fuentes','',
'[Inventario de partida](../revision_v09/masas_por_cuerpo_v09.csv) · [Candidatos/URLs](candidatos.json) · [Cálculo](comparar.py) · [Uniones](uniones.py) · [Resultados](resultados.json) · [Resumen CSV](comparacion_resumen.csv) · [Pinza](pinza_carga.json) · [Presupuesto de reserva](presupuesto_masa_reserva.json) · [Manifest de evidencia](evidencia_calculo.json)','',
'Los estados completos son una exportación regenerable de≈182 MB en`AutoFinalRobo2-CAD/exports/analisis-actuadores-v10/cargas_comparadas.npz`, excluida de Git según la política del repositorio. Se publican resultados, fuentes y SHA256. Para reconstruir en otra PC, ejecutar con Python/NumPy/Matplotlib, en este orden: `comparar.py`, `uniones.py`, `informar.py`. No requieren Fusion, hardware o compilación de firmware.','',
'El PDF Dualsky enlazado por el fabricante se recuperó en`.downloads/actuadores/DUALSKY_SERVO-spec-202605.pdf`, excluido de Git; SHA256`'+cfg['DS8930_pdf_sha256']+'`. Los recortes de lectura tampoco son modelos CAD. Las fuentes siguientes respaldan sus datos comerciales, no los resultados numéricos propios:','']
for k,url in cfg['sources'].items():lines.append(f'- [{k}]({url})')
(H/'README.md').write_text('\n'.join(lines)+'\n',encoding='utf-8')
with (H/'comparacion_resumen.csv').open('w',encoding='utf-8-sig',newline='') as fp:
    fields=['variante','masa_g','delta_g','parada_ms','J1','J2','J3','J4','J5','J6','min_ratio_pico_demanda']
    writer=csv.DictWriter(fp,fieldnames=fields);writer.writeheader()
    for c in r['results']:
        for t,jj in c['scenarios'].items():writer.writerow(dict(variante=c['name'],masa_g=c['mass_g'],delta_g=c['delta_mass_g'],parada_ms=t,**{x['joint']:x['peak_servo_kgfcm'] for x in jj},min_ratio_pico_demanda=min(x['stall_peak_ratio'] for x in jj)))
print(json.dumps(dict(cases=len(rr),source_states=r['baseline']['source_states'],expanded_J1_states=r['baseline']['expanded_J1_states'],reserve_budget=budget),ensure_ascii=False))
