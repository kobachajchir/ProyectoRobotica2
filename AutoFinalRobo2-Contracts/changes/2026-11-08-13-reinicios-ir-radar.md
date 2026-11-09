# AFR2-20261108-13 - Vista Reinicios en el monitor, IR sin motores y radar acoplado

Pedidos del usuario (08/11/2026):

1. Los reinicios aparecían en Eventos y errores; el Monitor serie debe tener un botón que muestre una
   tabla específica de reinicios: el reset, por qué fue y el HELLO.
2. Los sensores de línea deben poder activarse sin habilitar motores.
3. El radar debe encender servo y HC-SR04; si se apaga cualquiera de los dos, se apaga el radar.

Versión: 1.7.0 (subversión). Loader 2.4.0.

## Loader 2.4.0: vista Reinicios
- Botón **Reinicios (n)** en la barra del Monitor serie (muestra "· k nuevos"); abre una vista propia
  en lugar de Enviar/Intérprete. La pestaña de arranques sale del intérprete.
- Tabla: Nº, hora PC, ms AVR, equipo, **razón**, **sesión nueva (HELLO)** (ms desde el arranque y
  sesión), **antes** (enlace perdido: "el ESP no pidió nada en N ms"), excepción, epc1, boot ID,
  fuente, detalle. Detalle con **línea de tiempo** (ROM, trama BOOT, ESP BOOT, LINK hello, tramas
  HELLO/EXT_HELLO con TAP) y el log de antes y después.
- Un HELLO sin reinicio del ESP queda como fila "Enlace: sesión nueva sin reinicio visto del ESP"; uno
  que sigue a un reinicio del ATmega se asigna a ese reinicio.
- Ejemplo de la captura del usuario: `ESP ROM probable` (272 ms de silencio) → BOOT `rst=6
  external-reset` → HELLO a +251 ms → EXT_HELLO; queda en una sola fila.

## ATmega + Web: línea y radar
- Acción nueva `AFR2_A_IR_TOGGLE = 108` y ítem `extra[49]` "Sensores de linea". La página 5 de
  Controles rápidos (Línea) ahora enciende o apaga la **lectura IR** (LEYENDO / APAGADO), sin requerir
  motores. El seguimiento de línea sigue en Modo y en Seguimiento línea (`AFR2_A_LINE_TOGGLE`).
- `line_valid` solo se publica con el IR encendido o con una pantalla de línea abierta (Línea IR1..IR5,
  IR valores, Calibrar línea). `line_valid_raw` conserva si el sensor se puede leer: lo usan los
  requisitos de seguimiento y la capacidad `AFR2_EXT_F_LINE` para la Web. Iniciar el seguimiento
  enciende el IR; apagar el IR durante el seguimiento lo detiene. IR apagado tras cada reset.
- Radar: `Afr2Head_Start` guarda el estado del HC-SR04 y lo enciende; el servo ya se activa al
  barrer. `Afr2Head_Stop` lo restaura. Apagar el HC-SR04 (`Afr2Head_SetSampling(0)`) o el servo detiene
  el radar. Vale para la tarjeta, el menú Radar automático y el comando `RADAR_START` del ESP.
- Dashboard: el ítem IR de la tarjeta muestra la lectura IR (bit 5 de `quick_flags`).
- Web: `firmwareRenderer.ts`, `uiSimulation.ts` y `generated/firmwareLibrary.ts` (símbolo 108,
  `extra[49]`, hashes) con el mismo comportamiento.

## Estado

atmega=**compilado** (`make -B firmware communication-firmware library`): sin errores, advertencia previa
en `afr2_product.c:214`; flash 149984 B (57,2 %), SRAM 7016 B (85,6 %). HEX con 1.7.0. **Sin cargar ni
probar.** web=fuente editada, sin build. esp=solo versión. loader=2.4.0 compilado e iniciado, sin
abrir puertos.
