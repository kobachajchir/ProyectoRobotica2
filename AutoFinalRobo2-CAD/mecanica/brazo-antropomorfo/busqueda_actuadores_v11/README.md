# Busqueda v11: par, masa y resistencia simultaneos

AFR2-20261028-07. Preseleccion externa sobre inventario v09; Fusion no modificado. Objetivo activo, sin seleccion final de compra ni aptitud fisica declarada.

## Resultado y criterio

No se encontro todavia una solucion cerrada que acredite simultaneamente motor, transmision, PLA impreso y estabilidad del auto. Si hay dos rutas numericamente viables bajo condiciones diferentes:

- DS51150: supera el filtro de par y masa a30 grados/s por eje. No satisface la frenada desde las velocidades de catalogo. Su ficha da bloqueo, no par nominal continuo ni curva de frenado.
- ST-3215-C047 con reducciones: supera el filtro frente al par nominal documentado, incluso desde el maximo de velocidad de salida de cada reduccion +10%. Esto requiere fabricar/selectar reductores dentro del peso presupuestado, aceptar velocidades menores en hombro/codo y acreditar las uniones. No es aun un conjunto mecanico seleccionado.

No sustituir la condicion original por30 grados/s sin explicarlo. Tampoco confundir velocidad sin carga, velocidad bajo par nominal y aceleracion nominal: esta ultima no esta documentada para estos actuadores. La frenada usada tiene duracion50ms, perfil cubico de velocidad y aceleracion pico1.5*velocidad_inicial/0.05. No representa una detencion instantanea ni un choque contra tope.

## DS51150 y masa realimentada

Se conservaron2:1 y eficiencia0.85 en J2; J3 directo; eficiencia0.90 en J4. J1/J4/J5 usan GX3270BLS a7.4V, J6 MG90S y pinza MG92B posicional. DS51150 se examina a10V y12V, con175g y sensibilidad206g; el valor125kgfcm es una sensibilidad de variante distinta, no la especificacion ANNIMOS.

| Caso | Masa con carga (g) | Menor cociente bloqueo/demanda | Menor sensibilidad lineal par-velocidad |
|---|---:|---:|---:|
|A_150_10V_GX|2752.4|2.83|2.76|
|B_150_bajo_GX|2752.4|2.83|2.69|
|C_206g_bajo_GX|2814.4|2.81|2.67|
|D_150_12V_GX|2752.4|2.83|2.76|
|E_150_bajo_DS35|2692.4|2.93|2.80|
|F_206g_bajo_DS35|2754.4|2.91|2.78|
|G_reforzado_206g_GX|3519.4|2.55|2.42|

G incluye1119g adicionales al v09: motores,255g de montajes/cable,330g de acero adicional y200g de potencia fija. No descuenta los engranajes impresos sustituidos. Masa3519.4g, CG horizontal[-20.59, 100.79, 150.39]mm. El peso nuevo no elimina por si solo el margen de par a30 grados/s.

El DS51150 ocupa65x30x48mm de carcasa; el plano muestra75mm entre centros longitudinales de fijacion,17mm entre filas,31mm desde fondo a plano inferior de orejas y56.8mm de altura con horn representado. Falta cota de posicion de spline y patron/tolerancias completos. Offset17mm del calculo es inferido; no es una cota del eje de salida del plano. Horn18T y nueva fijacion: no reutilizar25T.

### Contraejemplo a velocidades de catalogo

En G, con velocidades iniciales por eje600/125/250/600/600/600 grados/s, hay demanda local de motor:

| Eje | Demanda (kgfcm) | Bloqueo usado (kgfcm) |
|---|---:|---:|
|J1|440.23|63.00|
|J2|177.02|125.00|
|J3|187.26|125.00|
|J4|19.74|63.00|
|J5|64.22|63.00|
|J6|1.83|1.80|

Basta este contraejemplo para descartar la combinacion para esa condicion. No es una envolvente global y no demuestra que el resto del espacio sea menos exigente. La sensibilidad lineal par-velocidad anterior tampoco sustituye una curva de frenado regenerativo.

### Transmision y J1

La rueda del hombro tiene modulo1.5,Z40; pinonZ20 y ancho6mm. Para G, tau_J2=7.40Nm y Ft=246.79N en radio30mm. Lewis con Y20=0.322 da85.16MPa nominales en el pinon; incluso el valor orientativo de53MPa del material base PrintaLot no habilita esta pieza. No usar ese valor como resistencia FDM. Aumentar solo el motor no resuelve el diente.

Un reemplazo de acero de ancho10mm reduce esa referencia a51.10MPa; con n2 y factor adicional2 se requiere204.4MPa. Es solo filtro de flexion, no aprobacion de contacto Hertz, fatiga, chaveta/horn ni mecanizado. La reserva330g corresponde a discos macizos aproximados OD63/33x10mm y margen pequeno de cubos; no es el peso de un reductor multietapa.

En J1 de G: P0=579.1N; requisito nominal de fluencia del husillo con n2/Kt2=376.1MPa; resistencia normal FDM requerida en cuna=16.36MPa. El acero4140 propuesto con certificado Rp>=600MPa supera el filtro del husillo; no hay certificado de la pieza ni resistencia FDM asignada.

## Ruta multivuelta con par nominal: ST-3215-C047

Ficha FEETECH A/0,2023-07-20:12V,55+-1g,45.2x24.7x35mm,par nominal10kgfcm, bloqueo30kgfcm+-10%, velocidad sin carga0.222s/60grados+-10%, eje25T/OD5.9mm. Se presupuestaron80g netos por unidad en la envolvente, usando conservadoramente el peso de bulto local como masa, sin afirmarlo como neto.

El modelo local Starware SKU3116 se anuncia STS3215,12V,30kgfcm y disponible. No publica sufijoC047, par nominal ni multivuelta. Por eso es oferta comercial argentina compatible por familia, NO correspondencia de variante/lote confirmada. El precio consultado66.999ARS/unidad es orientativo y no una cotizacion; seis suman401.994ARS antes de reductores, potencia, pinza y envio.

Propuesta de presupuestos, no piezas compradas ni CAD:

| Eje | Reduccion externa | Presupuesto reductor completo (g) | Velocidad salida maxima+10% (grados/s) | Par requerido motor (kgfcm) | Nominal/demanda |
|---|---:|---:|---:|---:|---:|
|J1|12:1|300|24.75|4.052|2.47|
|J2|28:1|300|10.61|4.739|2.11|
|J3|24:1|300|12.38|2.938|3.40|
|J4|4:1|75|74.25|1.113|8.98|
|J5|12:1|150|24.75|1.372|7.29|
|J6|4:1|75|74.25|0.091|110.47|

Eficiencia supuesta0.50 por reductor completo. Masa total3919.0g con200g de carga, CG horizontal[-13.19, 110.02, 148.02]mm. Las masas de reductores incluyen engranajes,housing,ejes y rodamientos, repartidas50/50 entre soporte padre y salida. Estudio de262trayectorias/5502estados fuente y99378orientaciones expandidas para J1. Son finales heredados de la familia v10 con frenadas50ms; no todo el workspace continuo.

El caso uniforme24:1 muestra por que el peso importa: con300g por reductor no alcanza margen2 nominal aun a eta0.65. La distribucion de masas y relaciones de la tabla es parte necesaria de la propuesta. Un proveedor que exceda esos presupuestos obliga a recalcular; no se puede comprar primero cualquier caja y suponer que cumple.

La ficha permite+-7 vueltas de posicion,5040grados totales. Las relaciones propuestas permiten420/180/210/1260/420/1260grados totales de salida; mantienen los recorridos requeridos180/135/180/180/180/180. Sin embargo, el contador de vueltas se pierde al cortar energia. Hay que definir inicializacion de postura para que no haya ambiguedad multivuelta. El encoder integrado forma parte del servo; no se propone un sensor externo. PWM no informa la posicion real: envia una consigna. Este STS usa busTTL semiduplex y requiere interfaz/control distinto del PCA9685.

Seis motores a2.7A de bloqueo requieren considerar16.2A simultaneos a12V mas auxiliares. Par nominal10kgfcm no acredita frenado, energia regenerada, duty ni temperatura de todo el ensamble. El presupuesto200g de potencia no selecciona bateria/regulador ni valida el auto.

### Reaccion sobre la estructura actual

Con STS y estos presupuestos: J1 P0=507.0N; C0/P0=3.87; husillo nominal equivalente=82.71MPa y requisito Rp n2/Kt2=330.8MPa; cuna requiere14.73MPa de resistencia normal FDM efectiva. Desplazamiento calculado de tapa0.108mm bajo E1500MPa supuesto. No es FEA ni prueba de holgura real.

| Eje | Fuerza maxima (N) | Momento resultante maximo (Nm) | Par minimo a acreditar en salida con n2 (Nm) |
|---|---:|---:|---:|
|J1|34.97|6.871|4.768|
|J2|27.21|6.805|13.013|
|J3|20.13|3.646|6.916|
|J4|14.30|1.610|0.437|
|J5|9.57|0.855|1.614|
|J6|5.86|0.386|0.036|

Las columnas son maximos independientes, no un unico estado simultaneo. Las salidas deben resistir tambien flexion,corte y reacciones; los rodamientos externos deben descargar los ejes de servo. La rigidez de los nuevos housings, aprietes,cargas de dientes y acoples todavia no estan calculados. Los tensores asignados a reductores son presupuestos isotropicosR50mm; no son una cota demostrada de inercias internas/reflejadas.

## Condiciones que impiden cerrar el objetivo

1.30grados/s es una referencia de los calculos anteriores, no una velocidad minima especificada por el usuario. DS cumple el filtro a30 pero falla desde catalogo; STS con reductores pasa el filtro nominal desde las velocidades maximas del conjunto propuesto, con J2/J3 mas lentos. No se descarta STS solo por ese cambio de prestaciones, ni se presenta como un brazo de30grados/s en todos los ejes.
2. Identificar la variante local exacta STS y verificar fichaC047/multivuelta/par nominal. No se enviaron mensajes a vendedores.
3. Seleccionar o disenar reductores reales con las masas,eficiencias,capacidades y recorridos calculados. Las reservas no prueban fabricabilidad ni cumplimiento de una caja inexistente.
4. Acreditar freno/par-velocidad bajo carga, accionamiento de emergencia, inercia interna y alimentacion. Una parada por corte de energia no garantiza sostener la carga.
5. Disenar/recalcular dientes,horns,ejes y housings con esos reductores; acreditar PLA PrintaLot por orientacion/perimetros/temperatura o sacar las cargas principales del PLA mediante refuerzos metalicos calculados. El perfil de impresion sigue sin definir.
6. Verificar estabilidad con masa/CG y poligono reales del auto. Los umbrales de vuelco del JSON son sensibilidades hipoteticas, no datos de ruedas ni margen dinamico confirmado.
7. Repetir choques/recorridos sobre la nueva geometria. Los barridos v09 no sirven para housings nuevos no modelados. La pinza requiere acreditar friccion, pads y momento de contacto; no alcanza elegir un servo de mayor bloqueo.

## Fuentes y recuperacion

Consultadas2026-10-28. Los datos tecnicos vienen de fichas de fabricante; las tiendas prueban oferta comercial, no desempeno.

- [ANNIMOS DS51150-12V RED, ficha](https://m.media-amazon.com/images/I/81atmTkZyeL.pdf). SHA25670071c21a8bd91a71bf65e46793d81270e1026ad24eb0f4a408ee92bd7910a4b. Descarga excluida:.downloads/actuadores/ANNIMOS_DS51150.pdf. Licencia de redistribucion no identificada.
- [Tiendamia Argentina, familia con opcion150KG-270](https://tiendamia.com.ar/p/amz/b07ktscn4j/annimos-132-3-lbs-digital-servo-8-4v-engranaje-de-acero-inoxidable). La pagina principal es60kg; opcion150 no demuestra stock/precio del SKU exacto. No usar precio de60kg como cotizacion150kg.
- [MercadoLibre Argentina, oferta ANNIMOS](https://listado.mercadolibre.com.ar/juegos-juguetes/hobbies/nuevo/_BRAND_27255673): apareceDS51150/270, no se recupero publicacion individual comprable. Oferta a importar, no stock nacional acreditado.
- [FEETECH ST-3215-C047 A/0, ficha primaria alojada porSeeed](https://files.seeedstudio.com/products/Feetech/108090003_FEETECH_ST-3215-C047-Datasheet.pdf). SHA256ddef0f5d8e72a06e3a2e46f84c0d5b314ef09388f6f97c0f1bd708c376d53808. Descarga excluida:.downloads/actuadores/FEETECH_ST3215_C047.pdf. No mezclar con STS3215/7.4V/19.5kg.
- [Starware SKU3116, STS3215 12V30kg](https://tienda.starware.com.ar/producto/servo-motor-smart-starware-sts3215-360g-30kg-12v-encoder-magnetico-ttl/): oferta nacional, varianteC047 no identificada.
- [Zennix DS51150](https://www.zennixtek.com/products/dsservo-ds51150-servo-motor): variante de menor par usada solo como sensibilidad. [ZOSKAY35kg Argentina](https://tiendamia.com.ar/p/amz/b07s9xzyn2/zoskay-servo-de-motor-sin-nucleo-de-35-kg-de): alternativa descartada para recomendacion definitiva por correspondencia dimensional/variante abierta.
- [Martin, Gear Catalog,pG83-G84](https://www.martinsprocket.com/docs/catalogs/power%20transmission/4_gear%20catalog/gear-catalog.pdf) y [Jonkopping University, Gear strength](https://mechanics.ju.se/MachineElements/gear-strength.html): metodo Lewis y factor20dientes0.322, flexion separada de contacto. El factor no valida el filete poligonal CAD ni fatiga.
- [Engramaq Argentina, engranajes modulares](https://engramaq.com.ar/products/engranajes/): oferta mecanica desde modulo1, acero4140 entre opciones; NO acredita suministro del modulo0.5 ni una caja24/28:1 de este estudio. [Parot, catalogo](https://www.parot.com.ar/catalogo/06_engranajes.pdf): tampoco es una caja multietapa seleccionada.

## Reproducibilidad y limites

Ejecutar conPython3.12/numpy; fuentes externas a Fusion:

```powershell
python iterar.py
python uniones.py
python iterar.py --rated-probe
python estudio_STS.py --envelope
python uniones_STS.py
python informar.py
```

Los NPZ regenerables quedan en AutoFinalRobo2-CAD/exports/analisis-actuadores-v11 y analisis-actuadores-v11-STS, excluidos por politica del repo. Conservar los scripts, parametros y resumenesJSON versionados. El calculo importa v09 y v10 por rutas relativas. No hubo CAD nuevo, ensayos fisicos, FEA, compilaciones ni tests de software. Calculos mecanicos pedidos expresamente por el usuario.
