"""Informe de preseleccion; no aprueba fabricacion ni modifica Fusion."""
from pathlib import Path
import json,csv,hashlib
H=Path(__file__).resolve().parent
ds=json.loads((H/'resultados.json').read_text(encoding='utf-8'))
u=json.loads((H/'uniones_resultados.json').read_text(encoding='utf-8'))
s=json.loads((H/'STS_resultados.json').read_text(encoding='utf-8'))['results'][0]
su=json.loads((H/'STS_uniones_resultados.json').read_text(encoding='utf-8'))['rows'][0]
diag=json.loads((H/'diagnostico_velocidades_catalogo.json').read_text(encoding='utf-8'))
g=ds['results'][-1]
lines=['# Busqueda v11: par, masa y resistencia simultaneos','',
'AFR2-20261028-07. Preseleccion externa sobre inventario v09; Fusion no modificado. Objetivo activo, sin seleccion final de compra ni aptitud fisica declarada.','',
'## Resultado y criterio','',
'No se encontro todavia una solucion cerrada que acredite simultaneamente motor, transmision, PLA impreso y estabilidad del auto. Si hay dos rutas numericamente viables bajo condiciones diferentes:','',
'- DS51150: supera el filtro de par y masa a30 grados/s por eje. No satisface la frenada desde las velocidades de catalogo. Su ficha da bloqueo, no par nominal continuo ni curva de frenado.',
'- ST-3215-C047 con reducciones: supera el filtro frente al par nominal documentado, incluso desde el maximo de velocidad de salida de cada reduccion +10%. Esto requiere fabricar/selectar reductores dentro del peso presupuestado, aceptar velocidades menores en hombro/codo y acreditar las uniones. No es aun un conjunto mecanico seleccionado.',
'',
'No sustituir la condicion original por30 grados/s sin explicarlo. Tampoco confundir velocidad sin carga, velocidad bajo par nominal y aceleracion nominal: esta ultima no esta documentada para estos actuadores. La frenada usada tiene duracion50ms, perfil cubico de velocidad y aceleracion pico1.5*velocidad_inicial/0.05. No representa una detencion instantanea ni un choque contra tope.','',
'## DS51150 y masa realimentada','',
'Se conservaron2:1 y eficiencia0.85 en J2; J3 directo; eficiencia0.90 en J4. J1/J4/J5 usan GX3270BLS a7.4V, J6 MG90S y pinza MG92B posicional. DS51150 se examina a10V y12V, con175g y sensibilidad206g; el valor125kgfcm es una sensibilidad de variante distinta, no la especificacion ANNIMOS.','',
'| Caso | Masa con carga (g) | Menor cociente bloqueo/demanda | Menor sensibilidad lineal par-velocidad |','|---|---:|---:|---:|']
for r in ds['results'][1:]:
    j=r['scenarios']['50'];lines.append(f"|{r['name']}|{r['mass_g']:.1f}|{min(x['stall_peak_ratio'] for x in j):.2f}|{min(x['linear_speed_margin_sensitivity'] for x in j):.2f}|")
lines += ['',
f"G incluye{g['delta_mass_g']:.0f}g adicionales al v09: motores,255g de montajes/cable,330g de acero adicional y200g de potencia fija. No descuenta los engranajes impresos sustituidos. Masa{g['mass_g']:.1f}g, CG horizontal{[round(x,2) for x in g['cg_mm']]}mm. El peso nuevo no elimina por si solo el margen de par a30 grados/s.",
'',
'El DS51150 ocupa65x30x48mm de carcasa; el plano muestra75mm entre centros longitudinales de fijacion,17mm entre filas,31mm desde fondo a plano inferior de orejas y56.8mm de altura con horn representado. Falta cota de posicion de spline y patron/tolerancias completos. Offset17mm del calculo es inferido; no es una cota del eje de salida del plano. Horn18T y nueva fijacion: no reutilizar25T.',
'',
'### Contraejemplo a velocidades de catalogo','',
'En G, con velocidades iniciales por eje600/125/250/600/600/600 grados/s, hay demanda local de motor:',
'',
'| Eje | Demanda (kgfcm) | Bloqueo usado (kgfcm) |','|---|---:|---:|']
for j,(d,c) in enumerate(zip(diag['peak_motor_kgfcm'],diag['catalog_stall_kgfcm']),1):lines.append(f'|J{j}|{d:.2f}|{c:.2f}|')
lines += ['',
'Basta este contraejemplo para descartar la combinacion para esa condicion. No es una envolvente global y no demuestra que el resto del espacio sea menos exigente. La sensibilidad lineal par-velocidad anterior tampoco sustituye una curva de frenado regenerativo.','',
'### Transmision y J1','',
'La rueda del hombro tiene modulo1.5,Z40; pinonZ20 y ancho6mm. Para G, tau_J2=7.40Nm y Ft=246.79N en radio30mm. Lewis con Y20=0.322 da85.16MPa nominales en el pinon; incluso el valor orientativo de53MPa del material base PrintaLot no habilita esta pieza. No usar ese valor como resistencia FDM. Aumentar solo el motor no resuelve el diente.',
'',
'Un reemplazo de acero de ancho10mm reduce esa referencia a51.10MPa; con n2 y factor adicional2 se requiere204.4MPa. Es solo filtro de flexion, no aprobacion de contacto Hertz, fatiga, chaveta/horn ni mecanizado. La reserva330g corresponde a discos macizos aproximados OD63/33x10mm y margen pequeno de cubos; no es el peso de un reductor multietapa.',
'',
f"En J1 de G: P0={u['rows'][-1]['bearing_P0_N']:.1f}N; requisito nominal de fluencia del husillo con n2/Kt2={u['rows'][-1]['shaft_yield_requirement_n2_Kt2_MPa']:.1f}MPa; resistencia normal FDM requerida en cuna={u['rows'][-1]['FDM_normal_requirement_n2_Kt2_MPa']:.2f}MPa. El acero4140 propuesto con certificado Rp>=600MPa supera el filtro del husillo; no hay certificado de la pieza ni resistencia FDM asignada.",
'',
'## Ruta multivuelta con par nominal: ST-3215-C047','',
'Ficha FEETECH A/0,2023-07-20:12V,55+-1g,45.2x24.7x35mm,par nominal10kgfcm, bloqueo30kgfcm+-10%, velocidad sin carga0.222s/60grados+-10%, eje25T/OD5.9mm. Se presupuestaron80g netos por unidad en la envolvente, usando conservadoramente el peso de bulto local como masa, sin afirmarlo como neto.','',
'El modelo local Starware SKU3116 se anuncia STS3215,12V,30kgfcm y disponible. No publica sufijoC047, par nominal ni multivuelta. Por eso es oferta comercial argentina compatible por familia, NO correspondencia de variante/lote confirmada. El precio consultado66.999ARS/unidad es orientativo y no una cotizacion; seis suman401.994ARS antes de reductores, potencia, pinza y envio.','',
'Propuesta de presupuestos, no piezas compradas ni CAD:','',
'| Eje | Reduccion externa | Presupuesto reductor completo (g) | Velocidad salida maxima+10% (grados/s) | Par requerido motor (kgfcm) | Nominal/demanda |','|---|---:|---:|---:|---:|---:|']
for j in range(6):lines.append(f"|J{j+1}|{s['ratios'][j]:g}:1|{s['reducer_mass_budget_g'][j]:g}|{s['initial_output_speed_deg_s'][j]:.2f}|{s['peak_motor_kgfcm'][j]:.3f}|{s['rated_torque_ratio'][j]:.2f}|")
lines += ['',
f"Eficiencia supuesta0.50 por reductor completo. Masa total{s['mass_g']:.1f}g con200g de carga, CG horizontal{[round(x,2) for x in s['cg_mm']]}mm. Las masas de reductores incluyen engranajes,housing,ejes y rodamientos, repartidas50/50 entre soporte padre y salida. Estudio de{s['trajectories']}trayectorias/{s['states']}estados fuente y{s['expanded_J1_states']}orientaciones expandidas para J1. Son finales heredados de la familia v10 con frenadas50ms; no todo el workspace continuo.",
'',
'El caso uniforme24:1 muestra por que el peso importa: con300g por reductor no alcanza margen2 nominal aun a eta0.65. La distribucion de masas y relaciones de la tabla es parte necesaria de la propuesta. Un proveedor que exceda esos presupuestos obliga a recalcular; no se puede comprar primero cualquier caja y suponer que cumple.',
'',
'La ficha permite+-7 vueltas de posicion,5040grados totales. Las relaciones propuestas permiten420/180/210/1260/420/1260grados totales de salida; mantienen los recorridos requeridos180/135/180/180/180/180. Sin embargo, el contador de vueltas se pierde al cortar energia. Hay que definir inicializacion de postura para que no haya ambiguedad multivuelta. El encoder integrado forma parte del servo; no se propone un sensor externo. PWM no informa la posicion real: envia una consigna. Este STS usa busTTL semiduplex y requiere interfaz/control distinto del PCA9685.','',
'Seis motores a2.7A de bloqueo requieren considerar16.2A simultaneos a12V mas auxiliares. Par nominal10kgfcm no acredita frenado, energia regenerada, duty ni temperatura de todo el ensamble. El presupuesto200g de potencia no selecciona bateria/regulador ni valida el auto.','',
'### Reaccion sobre la estructura actual','',
f"Con STS y estos presupuestos: J1 P0={su['bearing_P0_N']:.1f}N; C0/P0={su['bearing_C0_over_P0']:.2f}; husillo nominal equivalente={su['shaft_vm_nominal_bbox_bound_MPa']:.2f}MPa y requisito Rp n2/Kt2={su['shaft_yield_requirement_n2_Kt2_MPa']:.1f}MPa; cuna requiere{su['FDM_normal_requirement_n2_Kt2_MPa']:.2f}MPa de resistencia normal FDM efectiva. Desplazamiento calculado de tapa{su['cap_translation_mm']:.3f}mm bajo E1500MPa supuesto. No es FEA ni prueba de holgura real.",
'',
'| Eje | Fuerza maxima (N) | Momento resultante maximo (Nm) | Par minimo a acreditar en salida con n2 (Nm) |','|---|---:|---:|---:|']
for j in range(6):lines.append(f"|J{j+1}|{s['max_joint_force_N'][j]:.2f}|{s['max_joint_moment_Nm'][j]:.3f}|{2*s['peak_motor_kgfcm'][j]*s['ratios'][j]*s['eta']*.0980665:.3f}|")
lines += ['',
'Las columnas son maximos independientes, no un unico estado simultaneo. Las salidas deben resistir tambien flexion,corte y reacciones; los rodamientos externos deben descargar los ejes de servo. La rigidez de los nuevos housings, aprietes,cargas de dientes y acoples todavia no estan calculados. Los tensores asignados a reductores son presupuestos isotropicosR50mm; no son una cota demostrada de inercias internas/reflejadas.','',
'## Condiciones que impiden cerrar el objetivo','',
'1.30grados/s es una referencia de los calculos anteriores, no una velocidad minima especificada por el usuario. DS cumple el filtro a30 pero falla desde catalogo; STS con reductores pasa el filtro nominal desde las velocidades maximas del conjunto propuesto, con J2/J3 mas lentos. No se descarta STS solo por ese cambio de prestaciones, ni se presenta como un brazo de30grados/s en todos los ejes.',
'2. Identificar la variante local exacta STS y verificar fichaC047/multivuelta/par nominal. No se enviaron mensajes a vendedores.',
'3. Seleccionar o disenar reductores reales con las masas,eficiencias,capacidades y recorridos calculados. Las reservas no prueban fabricabilidad ni cumplimiento de una caja inexistente.',
'4. Acreditar freno/par-velocidad bajo carga, accionamiento de emergencia, inercia interna y alimentacion. Una parada por corte de energia no garantiza sostener la carga.',
'5. Disenar/recalcular dientes,horns,ejes y housings con esos reductores; acreditar PLA PrintaLot por orientacion/perimetros/temperatura o sacar las cargas principales del PLA mediante refuerzos metalicos calculados. El perfil de impresion sigue sin definir.',
'6. Verificar estabilidad con masa/CG y poligono reales del auto. Los umbrales de vuelco del JSON son sensibilidades hipoteticas, no datos de ruedas ni margen dinamico confirmado.',
'7. Repetir choques/recorridos sobre la nueva geometria. Los barridos v09 no sirven para housings nuevos no modelados. La pinza requiere acreditar friccion, pads y momento de contacto; no alcanza elegir un servo de mayor bloqueo.','',
'## Fuentes y recuperacion','',
'Consultadas2026-10-28. Los datos tecnicos vienen de fichas de fabricante; las tiendas prueban oferta comercial, no desempeno.','',
'- [ANNIMOS DS51150-12V RED, ficha](https://m.media-amazon.com/images/I/81atmTkZyeL.pdf). SHA25670071c21a8bd91a71bf65e46793d81270e1026ad24eb0f4a408ee92bd7910a4b. Descarga excluida:.downloads/actuadores/ANNIMOS_DS51150.pdf. Licencia de redistribucion no identificada.',
'- [Tiendamia Argentina, familia con opcion150KG-270](https://tiendamia.com.ar/p/amz/b07ktscn4j/annimos-132-3-lbs-digital-servo-8-4v-engranaje-de-acero-inoxidable). La pagina principal es60kg; opcion150 no demuestra stock/precio del SKU exacto. No usar precio de60kg como cotizacion150kg.',
'- [MercadoLibre Argentina, oferta ANNIMOS](https://listado.mercadolibre.com.ar/juegos-juguetes/hobbies/nuevo/_BRAND_27255673): apareceDS51150/270, no se recupero publicacion individual comprable. Oferta a importar, no stock nacional acreditado.',
'- [FEETECH ST-3215-C047 A/0, ficha primaria alojada porSeeed](https://files.seeedstudio.com/products/Feetech/108090003_FEETECH_ST-3215-C047-Datasheet.pdf). SHA256ddef0f5d8e72a06e3a2e46f84c0d5b314ef09388f6f97c0f1bd708c376d53808. Descarga excluida:.downloads/actuadores/FEETECH_ST3215_C047.pdf. No mezclar con STS3215/7.4V/19.5kg.',
'- [Starware SKU3116, STS3215 12V30kg](https://tienda.starware.com.ar/producto/servo-motor-smart-starware-sts3215-360g-30kg-12v-encoder-magnetico-ttl/): oferta nacional, varianteC047 no identificada.',
'- [Zennix DS51150](https://www.zennixtek.com/products/dsservo-ds51150-servo-motor): variante de menor par usada solo como sensibilidad. [ZOSKAY35kg Argentina](https://tiendamia.com.ar/p/amz/b07s9xzyn2/zoskay-servo-de-motor-sin-nucleo-de-35-kg-de): alternativa descartada para recomendacion definitiva por correspondencia dimensional/variante abierta.',
'- [Martin, Gear Catalog,pG83-G84](https://www.martinsprocket.com/docs/catalogs/power%20transmission/4_gear%20catalog/gear-catalog.pdf) y [Jonkopping University, Gear strength](https://mechanics.ju.se/MachineElements/gear-strength.html): metodo Lewis y factor20dientes0.322, flexion separada de contacto. El factor no valida el filete poligonal CAD ni fatiga.',
'- [Engramaq Argentina, engranajes modulares](https://engramaq.com.ar/products/engranajes/): oferta mecanica desde modulo1, acero4140 entre opciones; NO acredita suministro del modulo0.5 ni una caja24/28:1 de este estudio. [Parot, catalogo](https://www.parot.com.ar/catalogo/06_engranajes.pdf): tampoco es una caja multietapa seleccionada.','',
'## Reproducibilidad y limites','',
'Ejecutar conPython3.12/numpy; fuentes externas a Fusion:','',
'```powershell',
'python iterar.py',
'python uniones.py',
'python iterar.py --rated-probe',
'python estudio_STS.py --envelope',
'python uniones_STS.py',
'python informar.py',
'```','',
'Los NPZ regenerables quedan en AutoFinalRobo2-CAD/exports/analisis-actuadores-v11 y analisis-actuadores-v11-STS, excluidos por politica del repo. Conservar los scripts, parametros y resumenesJSON versionados. El calculo importa v09 y v10 por rutas relativas. No hubo CAD nuevo, ensayos fisicos, FEA, compilaciones ni tests de software. Calculos mecanicos pedidos expresamente por el usuario.','']
(H/'README.md').write_text('\n'.join(lines),encoding='utf-8')
with (H/'comparacion.csv').open('w',newline='',encoding='utf-8-sig') as f:
    writer=csv.writer(f);writer.writerow(['caso','masa_g','eje','demanda_motor_kgfcm','capacidad_kgfcm','tipo_capacidad','cociente'])
    for r in ds['results']:
        for x in r['scenarios']['50']:writer.writerow([r['name'],r['mass_g'],x['joint'],x['peak_servo_kgfcm'],x['advertised_peak_kgfcm'],'bloqueo',x['stall_peak_ratio']])
    for j in range(6):writer.writerow([s['name'],s['mass_g'],'J'+str(j+1),s['peak_motor_kgfcm'][j],10,'nominal',s['rated_torque_ratio'][j]])
manifest=[]
for p in sorted(H.glob('*')):
    if p.is_file() and p.name!='manifiesto.json':manifest.append(dict(file=p.name,bytes=p.stat().st_size,sha256=hashlib.sha256(p.read_bytes()).hexdigest()))
(H/'manifiesto.json').write_text(json.dumps(dict(files=manifest,scope='Integridad de entrega; no validacion fisica ni prueba de software.'),indent=2),encoding='utf-8')
