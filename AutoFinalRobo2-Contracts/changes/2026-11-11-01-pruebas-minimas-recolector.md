# AFR2-20261111-01 - Pruebas mínimas (Conducir) y meta de cajas del simulador del recolector

Versión: 1.24.0 (subversión: función nueva compatible). Subida en `afr2_build_info.h` y `Web/package.json` (+ `package-lock.json`).
Estado: **atmega** solo versión (sin cambio de fuente, sin compilar ni cargar); **esp** solo versión (sin compilar ni flashear);
**web** fuente editada, sin `tsc`, build ni prueba en navegador ni en el auto. Sin CMD nuevo: no cambian `wire_catalog.json`,
`commands.json` ni Protocol Studio.

## Parte 1 · Conducir → «Pruebas mínimas»

Panel nuevo en el área Conducir (`ControlPage.tsx`, debajo de Conducción manual). Cada envío es **una repetición**; las cinco las
toma el usuario una por una y no hay repetición automática. El panel de odometría con confirmación por encoder
(`OdometryTestPanel`) vive en Puesta a punto > Calibración y no se tocó.

### Ensayos y envío

| Ensayo | Orden al auto |
|---|---|
| 1 Avance 1,0 m | un GOTO relativo `x=1000, y=0, theta=0` |
| 2 Lateral 1,0 m (izquierda/derecha) | `x=0, y=±1000` (izquierda positiva) |
| 3 Giro 90° sobre el centro (CW/CCW) | GOTO completo `x=y=0`, `theta=±1571 mrad` (CCW positivo) |
| 4 Trayectoria de la tarea | la lista de la pantalla GOTO (misma lista, editable allí) enviada como cadena de GOTO relativos (`useGotoRouteRunner`, AFR2-20261110-16) |
| 5 La misma con carga | igual a 4; el registro marca «con carga» (200 g) |

- «Cargar ir a la caja y volver al drop» siembra la lista de GOTO con *adelante X mm* y *atrás X mm* (X editable, 1000 mm por defecto) y
  «Editar en GOTO» abre esa pantalla. Si la lista está vacía o inválida el ensayo 4/5 queda bloqueado.
- Velocidad de misión: 100 mm/s por defecto, de 10 mm/s hasta el límite configurado (`mecanum_drive.limits.maxSpeedMmS`).
- Repetición 1 a 5 y tanda «Ajuste (odometría sin calibrar)» / «Verificación (calibrada)» son etiquetas del registro. Reenviar la misma
  combinación ensayo/lado/tanda/repetición **reemplaza** ese registro (el botón lo avisa).
- STOP siempre visible; el runner lo detecta y cancela la repetición.
- `useGotoRouteRunner.run` ahora devuelve un resultado (`completed`, `stop`, `cancelled`, `fault`, `failed`, `timeout`, `rejected`, con el
  número de falla) y acepta `thetaMrad` por tramo. Los llamadores anteriores lo ignoran: sin cambio de comportamiento.

### Lista «Preparación del ensayo»

Requisitos en tarjetas que pasan de rojo/pendiente a verde en vivo; «Enviar al robot» se habilita solo con todos los obligatorios en
verde («N de M listos»): conexión con el auto, GOTO disponible, **motores habilitados** (`motorsEnabled` de `robot.state`, ahora leído en
`parseRobotState`), **sensores Hall configurados**, auto detenido y sin falla F1–F4, pose de partida y ensayo/velocidad (o trayectoria) válidos.
«Odometría calibrada» es obligatoria solo en la tanda de verificación; en la de ajuste figura como informativa.

### Registro, resumen y exportación

- Por repetición se guarda en el navegador (`localStorage`, clave `afr2.minimumTests.v1`): ensayo, lado, tanda, repetición, fecha, versión de
  producto (la de `package.json`), velocidad, batería (mV si hay lectura válida), carga, consigna, odometría de partida y final (final
  expresada en el sistema del auto al partir: X adelante, Y izquierda, ψ CCW), pose medida cargada a mano (x, y, ψ), **error final = medida − consigna**,
  **deriva = odometría − medida**, duración, PWM máximo, causa de fin (completado, STOP, cancelado, falla Fn, no llegó, tiempo agotado, rechazado) y notas.
- Falla F1–F4 o corte del auto (incluido el de 1,5 s sin pulso Web, que el auto informa como misión cancelada): se corta la repetición y queda registrada con su causa.
- Descargas: CSV (32 columnas, `;` y BOM para Excel) y JSON; resumen por ensayo y tanda con media ± desvío muestral de las repeticiones
  completadas **y medidas**; exportación «Tabla 24» en CSV.

### Límites declarados

- **PWM máximo**: ninguna orden existente informa el PWM del auto; el campo queda vacío (no se inventó un CMD).
- **Tabla 24**: su texto no está en el repositorio (`Revision/R02_20261110` no existe; solo hay `R01_20261110`). El CSV usa estas columnas supuestas:
  ensayo, tanda, carga, repeticiones medidas/enviadas, velocidad, error x/y/ψ, deriva x/y/ψ y duración. Hay que ajustar `TABLE24_HEADER` cuando
  se disponga de la tabla real.
- **Sin confirmación por encoder** en el auto (a diferencia de la prueba de odometría): la propuesta por acción 102 no transporta velocidad, giro
  ni cadenas. El envío lo autoriza el botón (con la lista de requisitos) y lo corta STOP.
- La batería sale del último valor que ya tiene la Web; no se fuerza una medición nueva.

## Parte 2 · Simulador conjunto del recolector

Archivos: `collectorSimulation.ts`, `CollectorPage.tsx` (y `styles/collector.css`). Antecedentes: AFR2-20261110-03 y -05.

### Hallazgos de la revisión (script del scratchpad, fuera del repositorio; esbuild + Node, como en -05)

1. Línea base (código anterior): 240/240 escenas (semillas 1–30, cajas 3 a 10) entregan todo y pasan por patrullar, barrer, acercarse, encarar,
   centrar, ajustar distancia, tomar, transportar, entregar y salir del drop.
2. **Bucle de giro** (semilla 35 con 9 y 10 cajas, fuera del rango de -05): el auto, cargando una caja hacia el drop, no podía girar sin tocar otras;
   la maniobra de despeje elegía su dirección en cada cuadro y oscilaba para siempre en «Despejando el auto…». Ahora elige una dirección una vez
   (mirando hasta 0,8 m adelante) y la mantiene; además la fase de alineación tiene tope de 60 s con el mismo criterio que la navegación.
3. **Choque al girar** (semilla 67, 6 a 10 cajas): un giro que rozaba una caja con 0 mm de holgura chocaba por la deriva de la cinemática y
   cortaba la misión («Trayectoria bloqueada»). Los giros se planifican con 6 mm de holgura.
4. `generateCubes` podía dejar menos cajas que las pedidas; ahora relaja la separación (0,15 → 0,13 → 0,11 m) hasta cumplir la cantidad.
5. La misión terminaba con «Todas las cargas se entregaron» o fallaba con «Dos vueltas de patrulla sin cajas», sin noción de meta.

### Cambios

- Slider «Cajas a recolectar» (3 a 10, paso 1, marcas visibles en cada valor, 5 por defecto): `initialCollector(seed, count = 5)` genera esa
  cantidad (se sustituye el 6 fijo) y `requestedCount` es la **meta**. El generador de «Posiciones y cajas» queda en el mismo rango.
- Progreso «entregadas / meta» en el panel de misión y en la lectura del espacio de simulación. Con una misión en curso, cambiar el slider
  pide confirmación **Sí/No**; al confirmar se detiene y se regenera la escena (con la semilla actual); sin misión se regenera al instante.
- Éxito solo cuando `stored ≥ meta`. Mientras falten, sin detección el auto sigue patrullando **en ciclo**; después de cada vuelta completa sin
  detección reintenta las cajas apartadas. Cada caja cuenta sus intentos fallidos (4 como máximo; hasta entonces se reintenta en cada vuelta).
- Pide intervención solo si **ninguna** caja restante es alcanzable (cuatro intentos agotados, o sin toma frontal válida, sin cara libre o sin
  ruta libre) y lo explica caja por caja; también si no quedan cajas en el suelo y falta para la meta, o si el auto no puede desplazarse.

### Resultados del diagnóstico (después de los cambios)

- **800/800** escenas entregan todas las cajas: semillas 1–100 × cada cantidad de 3 a 10 (100 por valor). Tiempo simulado máximo: 255 s (3 cajas) a
  1 250 s (10 cajas).
- Casos dirigidos: caja encerrada por otras cuatro (meta 10) → completa 10/10; caja ya abandonada → entrega las otras 4 y pide ayuda explicando
  «C5: 4 intentos fallidos (…)»; caja en la esquina (-1,9; -1,9) → completa 6/6; solo 4 cajas con meta 5 → pide ayuda («faltan 1 para la meta»).
- No es una prueba del proyecto ni una compilación de producto; el simulador se ejecutó sin interfaz.

## Pendientes

`tsc -b` y `vite build` no se ejecutaron (AGENTS.md); la interfaz nueva (panel, lista de requisitos, slider) no se vio en navegador. Los
5 ensayos físicos, con el auto, siguen sin hacerse.

## Compilación (11/11/2026)

`tsc -b` sin errores y `vite build` correcto (36 s) sobre el árbol con 1.24.0. No se ejecutaron `postbuild`, `package-web` ni `build:esp`
(no se empaquetó ni se copió al ESP), ni el ATmega ni el ESP (solo cambia su versión). Sin prueba en navegador ni en el auto.
