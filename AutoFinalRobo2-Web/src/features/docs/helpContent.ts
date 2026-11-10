/* AFR2-20261102-23 / AFR2-20261103-26. Ayuda técnica por módulo, en orden ATmega → ESP → Web → OLED.
 * Cada dato sale de la fuente de su repo (board_pinmap.h, afr2_sensors.c, afr2_board.c,
 * afr2_preferences.c, afr2_link.c, broker.cpp, mega_link.cpp, platformio.ini, afr2Client.ts,
 * package-web.mjs, afr2_oled.c, afr2_encoder_ui.c) o de su contrato en Contracts.
 * Fuente revisada ≠ comportamiento físico comprobado: cada capítulo lo aclara donde importa.
 *
 * Convención de texto: lo que va entre `comillas invertidas` (archivos, carpetas, rutas, comandos,
 * pines y mensajes) se muestra resaltado en celeste. Cada viñeta expresa una sola idea. */

import type { UiIconName } from '../../components/ui/UiIcon';
import { version as WEB_VERSION } from '../../../package.json';

export interface HelpChapter {
  title: string;
  /** Una idea por viñeta. */
  text: string[];
  /** Tabla breve de datos técnicos: [dato, valor]. */
  facts?: [string, string][];
}
export interface HelpGroup { title: string; chapters: HelpChapter[] }
export interface HelpPinGroup { label: string; rows: [string, string][] }
export interface HelpPinout { title: string; groups: HelpPinGroup[] }
export interface HelpVisual { src: string; alt: string; width: number; height: number; kind: 'render' | 'screen' }
export interface HelpModule {
  /** Nombre del componente y su función en el sistema (encabezan la tarjeta del módulo). */
  name: string; role: string; icon: UiIconName;
  intro: string;
  /** Especificaciones clave, en celdas bajo la descripción. */
  specs: [string, string][];
  /** Cuarto derecho de la tarjeta: pines o puntos de acceso más consultados. */
  pinout: HelpPinout;
  /** Imagen junto a la descripción: render del CAD en Fusion (fondo transparente) o captura de pantalla. */
  visual?: HelpVisual;
  groups: HelpGroup[];
}
export type HelpModuleId = 'atmega' | 'esp' | 'web' | 'oled';

export const HELP: Record<HelpModuleId, HelpModule> = {
  atmega: {
    name: 'ATmega2560', role: 'Controlador de tiempo real', icon: 'board',
    intro: 'Es la autoridad del auto: genera el PWM de las cuatro ruedas Mecanum, cuenta los pulsos Hall, mide distancia y línea, ejecuta secuencias, guarda la configuración en EEPROM y dibuja la OLED. Trabaja en un bucle cooperativo alimentado por interrupciones, sin RTOS ni esperas bloqueantes. Solo responde pedidos del ESP, y ninguna orden externa saltea sus guardas ni su STOP.',
    specs: [['Núcleo', 'AVR 8 bits · 16 MHz'], ['Memoria', '256 KiB flash · 8 KiB SRAM · 4 KiB EEPROM'], ['Código', 'C gnu11 + avr-libc'], ['Enlace', 'ROBO2 · USART1 115200']],
    visual: { src: '/help/atmega2560.webp', alt: 'Render CAD de la placa Arduino Mega 2560 del auto', width: 900, height: 584, kind: 'render' },
    pinout: { title: 'Pinout', groups: [
      { label: 'Motores · IN1 / IN2 / PWM', rows: [['Del. der. (FR)', 'D22 D24 D9'], ['Del. izq. (FL)', 'D26 D28 D10'], ['Tras. der. (RR)', 'D5 D6 D11'], ['Tras. izq. (RL)', 'D7 D8 D12']] },
      { label: 'Hall de ruedas', rows: [['Canal A', 'A8–A11'], ['Canal B', 'A12–A15']] },
      { label: 'Sensores', rows: [['Línea IR1 (izq.)–IR5 (der.)', 'A4–A0'], ['Batería (divisor)', 'A5'], ['HC-SR04 TRIG / ECHO', 'D30 D31'], ['Servo radar', 'D13']] },
      { label: 'Panel', rows: [['OLED SDA / SCL', 'D20 D21'], ['Encoder CLK / DT', 'D2 D3'], ['Encoder SW', 'D42']] },
      { label: 'Enlace con el ESP', rows: [['TX1 → E_RX', 'D18'], ['E_TX → RX1', 'D19']] },
      { label: 'Servicio ESP (agregado)', rows: [['AutoKEY → GPIO0', 'D32 · PC5'], ['AutoRESET → RST', 'D33 · PC4']] },
    ] },
    groups: [
      { title: 'Arquitectura y compilación', chapters: [
        { title: 'Estructura del firmware',
          text: [
            '`platform/` es la única capa que toca registros: placa, motores, sensores, TWI, USART1, watchdog y preferencias.',
            '`services/` guarda la lógica sin hardware: enlace ROBO2, producto EX01, secuencias, red y gestor I2C.',
            '`ui/` y `display/` dibujan menús, dashboard, QR y radar sobre un framebuffer; `input/` decodifica el encoder.',
            '`main.c` es un bucle cooperativo. Las ISR solo capturan datos (pulsos Hall, flancos del encoder, ticks, bytes UART) y el trabajo ocurre en `main`.',
            'Ninguna tarea espera al hardware: los plazos se comparan siempre contra el tick de 1 ms.',
            'El `Makefile` genera el firmware de producto, el de comunicación, `libafr2_ui.a` (el renderer compartido con el Loader), un diagnóstico I2C/encoder por USART0 y el servicio puente USB↔ESP.',
            'El Loader compila y carga; la Web nunca flashea.',
          ],
          facts: [
            ['Compilador', '`avr-gcc -mmcu=atmega2560 -std=gnu11 -Os`'],
            ['Advertencias', '`-Wall -Wextra -fstack-usage`'],
            ['Enlace', 'Una sección por función + `--gc-sections`'],
            ['Último build registrado', 'AFR2-20261029-20 · flash 80 066 B · SRAM estática 7 085 B de 8 192 B'],
            ['Pila', 'Solo hay `.su` por función; el máximo dinámico con ISR no está certificado'],
          ] },
        { title: 'Temporización e interrupciones',
          text: [
            '`Timer0` en modo CTC genera el tick de 1 ms que sirve de reloj de todo el firmware.',
            '`INT4` e `INT5` capturan el giro del encoder; `PCINT2` captura los ocho canales Hall.',
            '`USART1` y TWI trabajan por interrupción, de modo que `main` nunca espera un byte.',
            'El watchdog arranca con 500 ms y lo alimenta `main`: si el bucle se cuelga, el chip se reinicia solo.',
            'Un reinicio pedido por la Web o la OLED se difiere hasta enviar la respuesta y entonces fuerza `WDTO_15MS`.',
            'Tras cualquier reinicio las salidas quedan apagadas y no se reanudan secuencias ni misiones.',
          ],
          facts: [
            ['Tick', '`Timer0` CTC · prescaler 64 · `OCR0A` = 249 → 1 ms'],
            ['Watchdog', '500 ms en marcha · 15 ms en reinicio controlado'],
            ['ISR', '`INT4`/`INT5` encoder · `PCINT2` Hall · `TIMER3`/`TIMER5` sensores · `USART1` RX/UDRE · TWI'],
          ] },
      ] },
      { title: 'Hardware y pines', chapters: [
        { title: 'Motores Mecanum',
          text: [
            'Cada rueda tiene un puente H del Model Y con dos pines de dirección (`IN1`, `IN2`) y una entrada PWM de velocidad.',
            'El PWM es de hardware: `Timer2` (`D9`, `D10`) y `Timer1` (`D11`, `D12`) en Fast PWM de 8 bits.',
            'Al detener, se desconectan las salidas de comparación y se ponen `IN1`/`IN2` en cero, para que el puente quede realmente apagado.',
            'Convención del chasis: X positivo hacia adelante, Y positivo hacia la izquierda y ángulo positivo antihorario visto desde arriba.',
            'La mezcla Mecanum del firmware y la cinemática de la Web usan esa misma referencia. La polaridad y el sentido de cada rueda se corrigen por calibración, no por cableado.',
          ],
          facts: [
            ['Del. der. (FR)', '`IN1` `D22` · `IN2` `D24` · PWM `D9`'],
            ['Del. izq. (FL)', '`IN1` `D26` · `IN2` `D28` · PWM `D10`'],
            ['Tras. der. (RR)', '`IN1` `D5` · `IN2` `D6` · PWM `D11`'],
            ['Tras. izq. (RL)', '`IN1` `D7` · `IN2` `D8` · PWM `D12`'],
            ['Frecuencia PWM', '16 MHz / (64 · 256) ≈ 977 Hz'],
          ] },
        { title: 'Sensores Hall',
          text: [
            'Hay dos headers de cuatro señales: el canal A en `A8`–`A11` y el canal B en `A12`–`A15`, en orden FR, FL, RR, RL.',
            'Los ocho pines son `PK0`–`PK7`, así que un solo grupo `PCINT2` los captura a la vez.',
            'El firmware cuenta flancos ascendentes del canal A y lee el B para saber el sentido.',
            'La distancia de cada rueda es π · diámetro · pulsos ÷ pulsos por vuelta, con el valor calibrado.',
            'Los contadores y el protocolo usan el orden lógico FL, FR, RL, RR; el físico de los headers es FR, FL, RR, RL.',
          ],
          facts: [
            ['Canal A', '`A8` `A9` `A10` `A11` (D62–D65)'],
            ['Canal B', '`A12` `A13` `A14` `A15` (D66–D69)'],
            ['Interrupción', '`PCINT2` · máscara `0x0F` (`PK0`–`PK3`)'],
            ['Alimentación', '+5 V regulados y GND común; nunca la tensión de batería'],
          ] },
        { title: 'Ultrasonido, servo, línea y batería',
          text: [
            'HC-SR04: `TRIG` en `D30` y `ECHO` en `D31`. Se mide sin bloquear con `Timer3` a 25 µs por tick.',
            'Si el eco no sube o no baja en 32 ms se informa `no-rise` o `no-fall`. El rango válido es de 2 cm a 4 m.',
            'Servo del radar en `D13`: `Timer5` a 0,5 µs alterna el pin por comparación, con período de 20 ms y pulso de 1000–2000 µs. Sin pulso, el servo queda libre.',
            'Línea `IR1`–`IR5` en `A4`–`A0` (`PF4`–`PF0`), entradas digitales leídas cada 5 ms con mayoría de 3 lecturas. La barra OSOYOO da LOW sobre negro y HIGH sobre blanco. `IR1` está a la izquierda e `IR5` a la derecha. La calibración es opcional y solo pide una muestra sobre blanco.',
            'Batería en `A5`, tomada después de la llave a través de un divisor de 10 kΩ/10 kΩ con 100 nF a GND: `A5` recibe la mitad de la tensión. La referencia es AVcc (5 V nominal, sin calibrar). Por ejemplo, 7,4 V dan unos 3,7 V en el pin (raw 758), y el ADC satura cerca de 10 V.',
          ],
          facts: [
            ['HC-SR04', '`D30` / `D31` · `Timer3` 25 µs · tope 32 ms'],
            ['Servo', '`D13` (`PB7`) · `Timer5` · 1000–2000 µs cada 20 ms'],
            ['Línea', '`A4`–`A0` digitales · LOW = negro · `IR1` a la izquierda'],
            ['ADC', 'AVcc · prescaler 128 (125 kHz) · solo batería `A5`'],
          ] },
        { title: 'Panel OLED y encoder',
          text: [
            'La OLED usa el TWI del Mega en `D20` (SDA) y `D21` (SCL). `A4` y `A5` no sirven como I2C porque ya están ocupados por `IR1` y la batería.',
            'El encoder KY-040 sin breakout cierra contra GND: `CLK` en `D2`, `DT` en `D3` y `SW` en `D42`.',
            'El panel se conecta por `J1` (hembra 1×5 a 2,54 mm: CLK, DT, SW, SDA, SCL) y `J2` (JST 1×2 a 2,00 mm: +5 V, GND).',
            'La PCB y la pieza nueva siguen pendientes de planos.',
          ],
          facts: [
            ['OLED', '`D20` SDA · `D21` SCL · +5 V'],
            ['Encoder', '`D2` CLK (`INT4`) · `D3` DT (`INT5`) · `D42` SW'],
            ['`J1` / `J2`', '1×5 de 2,54 mm para señales · 1×2 de 2,00 mm para alimentación'],
          ] },
      ] },
      { title: 'Comunicación con el ESP', chapters: [
        { title: 'Enlace ROBO2 y sesión',
          text: [
            'El Mega habla con el ESP por `USART1` (TX1 `D18`, RX1 `D19`) a través de `E_RX`/`E_TX` del shield.',
            'Una trama ROBO2 lleva la cabecera ASCII `ROBO2`, longitud N ≤ 64, tokens `3A A5`, versión `01`, ruta, comando, payload y un XOR de todos los bytes anteriores. Mide como máximo 76 bytes.',
            'El XOR comprueba la estructura; no es un CRC ni autentica al emisor.',
            'Solo existen dos rutas: ESP→Mega (`31`) y Mega→ESP (`13`). El Mega nunca envía por su cuenta: solo responde.',
            'Cada payload empieza con sesión u32 y transacción u32 en little-endian; el Mega rechaza las transacciones repetidas o anteriores.',
            'Si la sesión negociada pasa 1500 ms sin pedidos, el Mega detiene el movimiento, revoca la sesión y exige un nuevo `hello`. Un error de recepción produce lo mismo.',
          ],
          facts: [
            ['UART', '`USART1` 115200 8N1 · `U2X`, `UBRR` = 16 (error nominal +2,12 %)'],
            ['Trama', '12 + N bytes, N ≤ 64 · XOR final'],
            ['Resultados', '0 OK · 1 BAD_REQUEST · 2 CAPABILITY_UNAVAILABLE · 3 SESSION_REQUIRED · 4 INTERNAL_ERROR'],
            ['Supervisión', '1500 ms sin pedidos → STOP y revocación de sesión'],
            ['Extensión', '`EX01` v9 sobre el perfil 2'],
          ] },
      ] },
      { title: 'Movimiento, secuencias y memoria', chapters: [
        { title: 'Guardas de movimiento y STOP',
          text: [
            'El control es directo desde la Web, sin pedir permiso previo, pero el ATmega rechaza la orden si hay un fallo, una medición Hall, una escritura EEPROM, el radar o una secuencia en curso.',
            'También la rechaza si el control local está activo en la OLED o si se está editando el servo.',
            'Los motores deben estar habilitados en Calibración → Motores.',
            'Las consignas caducan si no se renuevan, de modo que cortar la comunicación detiene el auto.',
            'STOP tiene prioridad en todas las colas. La Web solo lo muestra como confirmado cuando el ATmega lo informa como productor autoritativo.',
          ] },
        { title: 'Ejecutor de secuencias',
          text: [
            'Un programa admite hasta 32 instrucciones de rueda y 120 s de duración, en memoria estática.',
            'Cada bloque termina por tiempo o por distancia de rueda; en modo Chasis cuenta la rueda de mayor recorrido.',
            'Si se agota el watchdog de ejecución de 120 s, el resultado es un fallo, nunca una llegada inventada.',
            'Las etiquetas viajan como IDs compactos y nombres constantes en flash hasta la pantalla de ejecución.',
            'La ejecución física sigue sujeta a la aprobación de SRAM/pila, Hall y calibración.',
          ],
          facts: [['Límites', '32 instrucciones · 120 s · distancia medida por pulsos Hall']] },
        { title: 'EEPROM y preferencias',
          text: [
            'La configuración se guarda en un journal propio de dos registros de 128 bytes que se alternan.',
            'Cada registro lleva la firma `AFR2`, versión, longitud, CRC16-CCITT y un marcador de commit `0xA5` que se escribe al final: un corte de energía a mitad de escritura deja el registro inválido.',
            'Al arrancar se lee el último registro válido. Si ninguno valida, se usan valores por defecto, sin escribir nada automáticamente.',
            'Guardar y Descartar son operaciones distintas, cada una con resultado autoritativo.',
            'Las migraciones de versiones anteriores ocurren en RAM y preservan las calibraciones: CPR por rueda, polaridad, Hall conectado, perfil IR, pulsación larga y servo.',
          ],
          facts: [
            ['Formato', '2 × 128 B · little-endian · CRC16-CCITT · commit `0xA5`'],
            ['Versión actual', 'v8 (agrega pulsación larga y preferencia de servo)'],
          ] },
      ] },
    ],
  },
  esp: {
    name: 'ESP8266 · ESP-12S', role: 'Pasarela Wi-Fi y servidor', icon: 'wifi',
    intro: 'Conecta el auto con el navegador. Levanta Wi-Fi como estación, punto de acceso o ambos; sirve la Web comprimida desde LittleFS y traduce cada pedido JSON del WebSocket en una transacción ROBO2 con el ATmega, con plazos y validación en los dos sentidos. Corre firmware propio sobre Arduino, sin comandos AT, y no decide sobre el movimiento.',
    specs: [['Núcleo', 'Xtensa L106 · 80 MHz'], ['Flash', '4 MiB · LittleFS 3 MiB'], ['Código', 'C++17 · PlatformIO'], ['Servicios', 'HTTP · WebSocket `/ws`']],
    visual: { src: '/help/esp12s-shield.webp', alt: 'Render CAD del shield Wi-Fi con el módulo ESP-12S', width: 900, height: 647, kind: 'render' },
    pinout: { title: 'Pinout', groups: [
      { label: 'UART de operación (UART0)', rows: [['TX', 'GPIO1'], ['RX', 'GPIO3'], ['Velocidad', '115200 8N1']] },
      { label: 'Hacia el Mega', rows: [['D18 (TX1)', '→ E_RX'], ['E_TX', '→ D19 (RX1)'], ['Masa', 'GND común']] },
      { label: 'KEY y RST', rows: [['KEY', 'GPIO0 · pulsador o AutoKEY D32'], ['RST', 'Pulsador o AutoRESET D33 · cable naranja']] },
      { label: 'Alimentación', rows: [['Lógica del ESP', '3,3 V'], ['Selector', 'E_RX / E_TX']] },
    ] },
    groups: [
      { title: 'Plataforma', chapters: [
        { title: 'Toolchain y memoria',
          text: [
            'El proyecto usa PlatformIO con la plataforma `espressif8266` 4.2.1, la placa `esp12e` y C++17.',
            'Las bibliotecas están fijadas a una versión exacta: ESPAsyncTCP 2.0.0, ESPAsyncWebServer 3.10.3 y ArduinoJson 7.4.2.',
            'Un script previo ajusta los buffers de copia de AsyncTCP antes de compilar.',
            'El linker `eagle.flash.4m3m` supone 4 MiB de flash con 3 MiB reservados para LittleFS.',
            'La flash física y el modo SPI deben identificarse en la unidad real antes de cargar cualquier imagen.',
          ],
          facts: [
            ['Entorno', '`esp12s_candidate` · framework Arduino'],
            ['Plataforma', '`espressif8266` 4.2.1 · placa `esp12e`'],
            ['Bibliotecas', 'ESPAsyncTCP 2.0.0 · ESPAsyncWebServer 3.10.3 · ArduinoJson 7.4.2'],
            ['Flash', '4 MiB supuesta · LittleFS 3 MiB'],
            ['Cola WebSocket', '`WS_MAX_QUEUED_MESSAGES` = 4'],
          ] },
        { title: 'Módulos del firmware',
          text: [
            '`main.cpp` arma el servidor y el bucle principal.',
            '`broker.*` valida el sobre WebSocket y despacha cada comando; `broker_product`, `broker_sensors`, `broker_system`, `broker_wifi` y `broker_network` adaptan una familia de comandos cada uno.',
            '`mega_link.*` es el extremo ROBO2 del lado del ESP.',
            '`storage.*` monta LittleFS y verifica el paquete Web.',
            '`wifi_service.*` maneja los modos STA/AP y la búsqueda de redes.',
          ] },
      ] },
      { title: 'Servidor y WebSocket', chapters: [
        { title: 'HTTP y paquete Web',
          text: [
            'El ESP sirve `/dist` desde LittleFS con los activos ya comprimidos en gzip.',
            '`/` resuelve a `/index.html`, que solo se entrega si el manifiesto y los tamaños del paquete coinciden. El firmware no recalcula los hashes.',
            '`/api/health` informa el servicio y la ocupación del filesystem (`usedBytes`/`totalBytes`).',
            '`/api/web-manifest` devuelve el manifiesto del paquete.',
          ],
          facts: [
            ['Páginas', '`/` · `/index.html` · estáticos en `/dist`'],
            ['API', '`/api/health` · `/api/web-manifest`'],
            ['Tiempo real', '`/ws`'],
          ] },
        { title: 'Protocolo afr2-ws-1',
          text: [
            'Cada mensaje es un sobre JSON con `schema` «afr2-ws-1», `type` (`req`, `res`, `err` o `evt`), `requestId`, `cmd`, `payload` y `ts`.',
            'Las respuestas se correlacionan por `requestId`; los eventos (`system.status`, `boot`, red) se difunden a la sesión.',
            'Hay una sola sesión de control, con hasta cuatro pedidos en cola y una ranura propia para STOP que se adelanta al resto.',
            'Cada frame debe ser texto final de hasta 1024 B; los fragmentos de continuación se rechazan.',
            'Un sobre inválido cierra con `1008`, un frame fuera de orden con `1002` y uno demasiado grande con `1009`.',
            'Mientras el ESP se reinicia solo acepta `system.info`, `system.alive` y `robot.stop`.',
          ],
          facts: [
            ['Frame', 'Texto ≤ 1024 B · JSON con anidamiento ≤ 5'],
            ['Identificadores', '`requestId` ≤ 64 · `cmd` ≤ 32 caracteres'],
            ['Cola', '4 pedidos + STOP prioritario · 1 sesión'],
            ['Cierres', '`1008` sobre inválido · `1002` orden · `1009` tamaño'],
          ] },
      ] },
      { title: 'Puente con el ATmega', chapters: [
        { title: 'Transacciones ROBO2',
          text: [
            'El ESP usa su UART0 (`GPIO1` TX, `GPIO3` RX) a 115200 8N1 y procesa una transacción a la vez.',
            'Cada transacción tiene un plazo de 800 ms: una respuesta tardía se descarta aunque ya esté en la cola de recepción.',
            'Sin respuestas válidas durante 1500 ms, el enlace pasa a «no listo» y las capacidades conocidas se borran.',
            'Toda respuesta se valida contra ruta, comando, sesión, transacción, longitud y rangos del cuerpo.',
            'Una trama válida no significa operación completada: el resultado semántico lo da cada comando.',
            'No se intercalan logs en la UART de aplicación. Los estados Web↔ESP, ESP↔ATmega y el origen de la actividad se informan por separado.',
          ],
          facts: [
            ['UART', '`GPIO1`/`GPIO3` · 115200 8N1'],
            ['Plazo', '800 ms por transacción · enlace caído a 1500 ms'],
            ['Captura OLED', '`robot.screen.get` en páginas de 40 B (26 pedidos por 1024 B)'],
          ] },
        { title: 'Shield y carga',
          text: [
            'En operación con el Mega, `D18` (TX1) va a `E_RX` y `E_TX` va a `D19` (RX1).',
            'El mapa de referencia del UNO que trae el fabricante no aplica al Mega, y el selector `E_RX`/`E_TX` no es la cabecera UART directa de 3,3 V del ESP.',
            'El ESP se programa por el USB del Mega con el puente por software `USART0`↔`USART1` (`D18`/`D19`), con el RESET del Mega libre. El método de cruce `D0`/`D1` con RESET a GND está retirado.',
            '`KEY` y `RST` se accionan con el pulsador del shield o, si está armado el circuito, con AutoKEY (`D32`) y AutoRESET (`D33`): un NPN por línea, en paralelo con el pulsador. Solo los usa el Loader en modo automático; la Web no los maneja.',
          ] },
      ] },
      { title: 'Red, sistema y batería', chapters: [
        { title: 'Wi-Fi',
          text: [
            'Hay tres modos: `STA` (se une a un router), `AP` (crea su red) y `AP_STA` (ambos a la vez).',
            'La búsqueda de redes es asíncrona y la configuración puede ser temporal o guardada.',
            'La seguridad se informa como open, WEP, WPA, WPA2, WPA/WPA2 o protected; si es desconocida no se inventa.',
            'La clave del router nunca se guarda en el navegador ni se expone por HTTP.',
            'La OLED distingue arranque, servidor HTTP listo, IP adquirida, AP creado, clientes conectados, transporte WebSocket y respuesta Web.',
          ] },
        { title: 'Información y reinicios',
          text: [
            '`system.info` y `system.hello` devuelven versiones, fecha y compilador de cada placa, identidad, causa de arranque y `filesystem.usedBytes`/`totalBytes`.',
            'Los datos ausentes se muestran como desconocidos.',
            'Reiniciar el ESP o el ATmega exige confirmación y placas detenidas; el reinicio se difiere para responder primero.',
            '`system.boot` informa el arranque siguiente.',
            'Un ACK de reinicio no prueba que el firmware nuevo haya iniciado.',
          ] },
        { title: 'Medición de batería',
          text: [
            'La medición arranca desde la OLED (Sistema → Batería) o desde la pill Batería de la Web, y se renueva hasta el próximo reinicio.',
            'Los estados son `off`, `measuring`, `valid`, `out-of-range` y `stale`; la Web descarta las lecturas vencidas.',
            'Las barras de la pill cambian por bandas de 6, 7 y 8 V: no son un porcentaje de carga.',
            'Requiere ATmega y ESP con `EX01` v9.',
          ] },
      ] },
    ],
  },
  web: {
    name: 'Web AFR2', role: 'Consola en el navegador', icon: 'screen',
    intro: 'Aplicación de una sola página que el ESP entrega comprimida y el navegador ejecuta por completo: rutas, estado de conexión, simulaciones y escenas 3D corren en el dispositivo del usuario. Envía órdenes por un único WebSocket supervisado y muestra cada lectura con su antigüedad; lo simulado nunca se presenta como medición.',
    specs: [['Versión', WEB_VERSION], ['Stack', 'React 19 · TypeScript 5.9 · Vite 7'], ['3D', 'Three 0.180 · R3F 9 · Draco'], ['Transporte', '`afr2-ws-1` sobre WebSocket']],
    visual: { src: '/help/web-inicio.webp', alt: 'Captura de la pantalla Inicio de la Web AFR2', width: 960, height: 600, kind: 'screen' },
    pinout: { title: 'Puntos de acceso', groups: [
      { label: 'Página', rows: [['Inicio', '`/` → `/index.html`'], ['Estáticos', '`/dist`']] },
      { label: 'API', rows: [['Estado', '`/api/health`'], ['Paquete', '`/api/web-manifest`']] },
      { label: 'Tiempo real', rows: [['WebSocket', '`/ws`'], ['Protocolo', '`afr2-ws-1`']] },
      { label: 'Plazos del cliente', rows: [['hello', '5 s'], ['alive', 'cada 500 ms'], ['Pedido', '2,5 s']] },
    ] },
    groups: [
      { title: 'Arquitectura', chapters: [
        { title: 'Stack y estructura',
          text: [
            'La Web es React 19 con TypeScript 5.9 y Vite 7, rutas hash con react-router 7 y Tailwind 4 sobre tokens propios del manual visual.',
            'Los avisos usan Sonner y las escenas 3D, Three 0.180 con `@react-three/fiber` 9.',
            '`src/app` contiene las rutas y los proveedores (tema, conexión, overlays).',
            '`src/protocol` tiene el cliente WebSocket y los decodificadores.',
            '`src/features` tiene una carpeta por página; `src/components`, las primitivas compartidas; `src/styles`, los tokens y las hojas por área.',
          ],
          facts: [
            ['Versión', 'Web 0.4.0'],
            ['Rutas', '`HashRouter` · las páginas 3D, Infraestructura y Protocol Studio se cargan bajo demanda'],
            ['Escenas', '`frameloop` «demand»: solo dibujan cuando algo cambia'],
          ] },
        { title: 'Cliente WebSocket',
          text: [
            'El cliente abre `/ws` en el mismo host que sirvió la página (`ws` o `wss` según el protocolo). No hay dirección manual ni botón Conectar.',
            'Tras `system.hello` mantiene la sesión con `system.alive` cada 500 ms.',
            'Cada pedido tiene un plazo (2,5 s por defecto). Si la conexión se corta, reconecta con espera exponencial de 500 ms a 15 s.',
            'Las lecturas vencidas se muestran con su antigüedad o como desconocidas, nunca con un valor inventado.',
            'Las lecturas periódicas corren solo mientras hay una vista que las usa: estado de pantalla cada 1 s con el visor abierto y cabezal de sensores cada 100 ms en las vistas que lo animan.',
          ],
          facts: [
            ['`hello`', 'Plazo de 5 s'],
            ['`alive`', 'Cada 500 ms · la sesión vence a 30 s'],
            ['Reconexión', '500 ms × 2ⁿ, tope 15 s'],
            ['Pedido', 'Plazo por defecto de 2,5 s'],
          ] },
      ] },
      { title: 'Operación', chapters: [
        { title: 'Supervisión y parada',
          text: [
            'Perder el foco, ocultar la pestaña, cambiar de tarea o desconectarse detiene de inmediato el envío de consignas, cancela cualquier secuencia física y emite `robot.stop`.',
            'La cabecera tiene la única acción STOP: aparece con movimiento, simulación o transferencia en curso, o si una parada quedó sin confirmar.',
            'Una petición aceptada no se presenta como parada física confirmada.',
          ] },
        { title: 'Movimiento manual y táctil',
          text: [
            'Mecanum y Control táctil comparten disponibilidad PWM y órdenes.',
            'Ambos requieren enlace completo Web–ESP–ATmega, motores habilitados y STOP sin confirmación pendiente. No exigen odometría, CPR calibrados ni lectura HC.',
            'El táctil abre el modelo 3D a pantalla completa: se arrastra hacia una de ocho direcciones y se mantiene. Las diagonales reparten la consigna entre ambos ejes.',
            'Soltar, perder foco o visibilidad, cambiar el tamaño, STOP o perder disponibilidad cancela el movimiento y exige un gesto nuevo.',
            'El modelo es una referencia fija, no una pose medida.',
          ] },
        { title: 'Secuencias, pistas y GOTO',
          text: [
            'Los bloques son desplazamiento, rampa, giro, curva, espera y finalización, en modo Chasis o con cuatro pistas independientes.',
            'El documento v2 importa programas v1 y conserva la biblioteca anterior en otra clave de almacenamiento.',
            'Enviar al robot carga, verifica y pide el inicio al ATmega; los límites son los del ejecutor (32 instrucciones, 120 s).',
            'El simulador estima la duración de cada bloque por distancia a partir de la velocidad.',
            'GOTO arma una lista de hasta 64 comandos y 10 minutos, entre −50 y 50 m por eje, con pasos relativos de 0,1 a 10 m y velocidad simulada de 0,05 a 0,5 m/s.',
            'Esa simulación es Mecanum ideal y no envía la lista al robot.',
          ] },
        { title: 'Calibración y diagnóstico',
          text: [
            'Calibración consulta el parámetro elegido antes de escribir; aplicar, guardar y descartar son órdenes separadas.',
            'STOP, desconexión o pérdida de foco interrumpen los pasos pendientes. Los Hall se calibran por vuelta desde la OLED.',
            'Configuración → Información de hardware muestra montaje y ocupación de LittleFS, versión, fecha, perfil y hash del índice del paquete, tal como los declara el ESP.',
          ] },
      ] },
      { title: 'Distribución', chapters: [
        { title: 'Paquete para el ESP',
          text: [
            '`npm run build:esp` cataloga los modelos, sincroniza la biblioteca OLED, compila TypeScript, construye con Vite en modo `esp`, empaqueta y copia a `ESP/data/dist`. No flashea.',
            'Cada activo se comprime con gzip nivel 9 y se registra en un manifiesto con su tamaño y el SHA-256 del original.',
            'Antes de copiar se estima la ocupación de LittleFS.',
            'Las imágenes son WebP y los modelos GLB usan Draco local, sin CDN.',
          ],
          facts: [
            ['Pipeline', 'catálogo 3D → biblioteca OLED → `tsc` → `vite --mode esp` → paquete → copia'],
            ['Compresión', 'gzip nivel 9 · manifiesto con SHA-256'],
            ['Destino', '`ESP/data/dist`'],
          ] },
      ] },
    ],
  },
  oled: {
    name: 'OLED 0,96 pulgadas + encoder', role: 'Interfaz local del auto', icon: 'encoder',
    intro: 'Pantalla monocroma SSD1306 manejada por el ATmega como único dispositivo I2C, con refresco diferencial por páginas, y un encoder KY-040 como única entrada: giro, corto y largo. La Web usa el mismo renderer para simular el flujo y puede ver y operar la pantalla real en vivo.',
    specs: [['Panel', 'SSD1306 · 128×64 · I2C'], ['Bus', 'TWI 100 kHz'], ['Entrada', 'KY-040 · `INT4`/`INT5` + `D42`'], ['Gesto largo', '≥ 800 ms, al soltar']],
    visual: { src: '/help/oled-encoder.webp', alt: 'Render CAD del panel con OLED de 0,96 pulgadas y encoder KY-040', width: 720, height: 719, kind: 'render' },
    pinout: { title: 'Pinout', groups: [
      { label: 'OLED (I2C)', rows: [['SDA', 'D20'], ['SCL', 'D21'], ['VCC / GND', '+5 V / GND']] },
      { label: 'Encoder KY-040', rows: [['CLK', 'D2 · INT4'], ['DT', 'D3 · INT5'], ['SW', 'D42'], ['Comunes', 'GND']] },
      { label: 'Conectores', rows: [['J1 · 1×5', 'CLK DT SW SDA SCL'], ['J2 · JST 1×2', '+5 V · GND']] },
    ] },
    groups: [
      { title: 'Funcionamiento', chapters: [
        { title: 'Driver y refresco',
          text: [
            'La OLED es el único dispositivo I2C y se maneja a través del gestor I2C, que define propietario, plazo y callback en `main`.',
            'El framebuffer son 1024 B repartidos en 8 páginas de 128 B. Cada página tiene un bit «sucia» y solo se envían los tramos que cambiaron, de hasta 16 columnas, como trabajo cooperativo de baja prioridad.',
            'Dibujar no lee sensores ni dispara transferencias: el snapshot de datos, la navegación y el renderer están separados.',
            'Ante un error aislado se reintenta conservando las páginas ya confirmadas; ante fallos persistentes se reconfigura el controlador.',
            'Para resistir el ruido, cada 1 s se reenvía la configuración que el ruido puede borrar (orientación, contraste, bomba de carga y encendido), sin tocar el oscilador. Además, cada 250 ms se reescribe un tramo, así que la pantalla completa se repara en unos 16 s.',
          ],
          facts: [
            ['Bus', 'TWI 100 kHz (`TWBR` = 72, prescaler 1)'],
            ['Autorreparación', 'configuración cada 1 s · un tramo cada 250 ms'],
            ['Framebuffer', '1024 B · 8 páginas · tramos de hasta 16 columnas'],
            ['Contraste inicial', '`0xCF`'],
          ] },
        { title: 'Encoder y gestos',
          text: [
            'El giro se lee en cuadratura por `INT4`/`INT5`, con cuatro transiciones por detención.',
            'El pulsador `SW` tiene un antirrebote de 20 ms.',
            'La pulsación larga se mide al soltar: desde 800 ms (configurable de 800 a 3000 ms, guardado en EEPROM v8) cuenta como larga y nunca genera además un corto.',
            'No hay botón de menú. En el dashboard, corto abre el Menú principal y largo abre los Controles rápidos.',
            'En el resto de las pantallas, corto confirma o alterna y largo vuelve al contexto anterior.',
            'Con cambios pendientes aparece Sí/No obligatorio; el largo no lo evita.',
          ],
          facts: [
            ['Giro', '`INT4`/`INT5` · 4 transiciones por paso'],
            ['`SW`', 'Antirrebote 20 ms · largo ≥ 800 ms al soltar'],
          ] },
        { title: 'Dashboard y pantallas',
          text: [
            'La cabecera muestra el SSID y la IP de STA/AP, con desplazamiento propio entre el ícono y la batería.',
            'La tarjeta de estados alterna cada 4 s tras 3 s sin usar el encoder. Mantener `SW` la pausa y un fallo queda fijo.',
            'Wi-Fi incluye dos QR, uno para conectarse al AP y otro para abrir la Web. Se generan en el auto con `qrcodegen` y los datos vigentes.',
            'El QR del AP no se transmite por captura remota.',
          ] },
      ] },
      { title: 'Pantalla en vivo y simulación', chapters: [
        { title: 'Captura remota',
          text: [
            'Con el visor abierto, la Web se suscribe con `robot.screen.subscribe` y lee `robot.screen.status` cada 1 s: código de pantalla, revisión de navegación, selección y primera fila visible.',
            '`robot.screen.get` trae el framebuffer, que el ESP pide al ATmega en páginas de 40 B.',
            'En modo Periódica, la Web pide el framebuffer cada 1,2 s.',
            'En modo Al cambiar, lo pide solo cuando cambia el estado informado; cada estado igual confirma la captura ya dibujada.',
            'Los valores que cambian dentro de una misma pantalla solo se actualizan en modo periódico o con «Actualizar pantalla».',
            '`robot.ui.action` envía giro, corto o largo junto con la revisión vista; si la pantalla cambió antes, la acción se rechaza.',
            'Una captura de más de 4 s bloquea el encoder remoto hasta recibir otra.',
          ],
          facts: [
            ['Estado', '`robot.screen.status` cada 1 s'],
            ['Framebuffer', '`robot.screen.get` · 1024 B · vence a 4 s'],
            ['Entrada', '`robot.ui.action` con revisión'],
          ] },
        { title: 'Copia Web del renderer',
          text: [
            'La biblioteca de íconos y pantallas del firmware se sincroniza en cada build con `tools/sync-oled-library.mjs`.',
            'La Web la rasteriza con el mismo orden de páginas que el ATmega.',
            '«Simular flujo» recorre los menús con datos ficticios; «Biblioteca sin datos» muestra el diseño sin simular servicios.',
            'La comparación píxel a píxel con la pantalla física sigue pendiente.',
          ] },
      ] },
    ],
  },
};
