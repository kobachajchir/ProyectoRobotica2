# AFR2-20261110-09 — Alertas, Fallo1 y Armado visual sin narración

Versión: **1.17.0 (subversión)**.

Pedidos: corregir Play en la Web servida desde el ESP, retirar narración y su envío, revisar «Fallo1 · lazo de control demorado» y sumar una pill superior de advertencias con contador, modal y acciones de recuperación.

## Origen de Play observado en archivos

El paquete local `AutoFinalRobo2-ESP/data/dist/manifest.json` identifica Web **1.15.0**, fecha `2026-11-10T04:16:51.405Z`, y aún contiene `/audio/armado-narracion.opus`. Al leer el HTML comprimido se observa `<audio>`, el texto «Reproducir recorrido con narración» y el avance tomado de `audio.currentTime` después de `audio.play()`. Es coherente con el síntoma informado: cambia a Pausar y no avanza si el audio no progresa.

Esto describe el paquete local leído, no una lectura de la flash del ESP ni una reproducción del fallo en navegador. La fuente ya tenía una retirada de audio previa, pero esa fuente no correspondía al paquete preparado.

## Armado y envío

- `assemblyPlaybackTimeline.ts` conserva 43,885714 s y las marcas de etapas; reemplaza el nombre histórico assemblyNarration. Play usa `requestAnimationFrame` y `performance.now`, sin elemento de audio, `play()` de media ni dependencia de `currentTime`.
- Las grabaciones únicas se trasladan de `public/audio/` a `reference/supplied/assembly-audio/`; fuera del servidor público y del listado explícito de Vite.
- Vite identifica la versión real de compilación en `.afr2-build.json`. El empaquetador exige que coincida con la versión actual antes de empaquetar.
- `assembly-audio-policy.mjs` identifica las dos grabaciones retiradas y rechaza HTML con el reproductor anterior. El empaquetador no incluye esas grabaciones aunque queden en un dist viejo.
- La promoción al ESP rechaza paquetes de otra versión, manifiestos con narración e índices antiguos con dependencia de audio. Un paquete nuevo conserva la limpieza de activos retirados del manifiesto anterior.

No se altera manualmente el HTML minificado existente ni se cambia la etiqueta de versión de un paquete antiguo para presentarlo como nuevo. Hace falta una compilación nueva para obtener la corrección en el equipo.

## Fallo1

`afr2_product.c` conserva el corte por retraso >100 ms cuando ya hay PWM aplicado a una rueda. Ya no convierte una demora previa, con las cuatro salidas aún a cero, en un fallo de marcha al aceptar la primera consigna. La limpieza local restablece los acumuladores sin arrancar motores ni limpiar automáticamente por recibir movimiento. Conserva juntos el intervalo de odometría y sus cuentas anteriores para no atribuir a unos pocos milisegundos los pulsos acumulados mientras había un fallo.

No se aumenta el umbral ni se suprime un fallo real con salidas activas. La causa física de una demora real sigue pendiente de diagnóstico con el log de bucle/render del ATmega; no se atribuye sin medición al OLED, a la red o al navegador.

## Centro de alertas

- `RobotAlerts.tsx`: pill siempre visible en la cabecera, icono del registro y contador. El modal reutiliza `FloatingModal`, origen de apertura, cierre y foco como Wi-Fi.
- Cuenta avisos activos de enlace Web, enlace ATmega, estado ausente/vencido, fallo del controlador y STOP no confirmado. Un código de fallo no es un contador ni una máscara: el controlador informa uno a la vez. No cuenta mensajes históricos de diagnóstico ni inventa varios fallos a partir de un código.
- Usa el lector compartido de `robot.state` y la caducidad de 6 s existente; no crea un segundo sondeo. Desconexión o lectura vencida no equivalen a «sin fallos».
- Incluye ayuda por Fallos1–4, acceso a Puesta a punto, Conexión y visor OLED, STOP y actualización explícita de información.
- Recuperación desde Web con el comando existente `system.reboot`, confirmación y salidas apagadas verificadas por un estado reciente, sin cambios de configuración pendientes ni escritura de preferencias. Una aceptación de reinicio no se presenta como fallo solucionado; se espera el estado posterior.
- Limpiar sin reiniciar sigue siendo una acción física local: Menú principal → Sistema → Limpiar fallo. El modal lo indica. No se habilita arbitrariamente esa acción a través del encoder remoto ni se cambia el contrato UART.
- `robotFaults.ts` centraliza textos/ayuda para el modal y la barra operativa. La cabecera permite distribuir las pills en más de una línea en móvil, conservando STOP.

## Estado de esta etapa

ATmega: corrección en fuente. Web: fuente y herramientas de paquete modificadas. ESP: solo versión compartida, sin cambios de lógica de firmware. Contracts: ficha, versión y cobertura. CAD/Loader: sin cambios.

Sin tests, compilaciones, validadores, navegador, empaquetado, promoción, imagen LittleFS ni flasheos en esta etapa. Se consultó si se autorizaba compilar y preparar LittleFS; el usuario indicó expresamente «Dejar solo los cambios en fuente». El paquete preparado antiguo permanece identificado como antiguo y sin modificar. Los cambios concurrentes ajenos se conservan. No hay commit/push nuevos solicitados en esta tarea.
