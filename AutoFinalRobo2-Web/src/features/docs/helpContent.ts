/* AFR2-20261102-23. Ayuda técnica por módulo, en orden ATmega → ESP → Web → OLED.
 * Cada dato sale de la fuente de su repo (board_pinmap.h, afr2_sensors.c, afr2_board.c,
 * afr2_preferences.c, afr2_link.c, broker.cpp, mega_link.cpp, platformio.ini, afr2Client.ts,
 * package-web.mjs, afr2_oled.c, afr2_encoder_ui.c) o de su contrato en Contracts.
 * Fuente revisada ≠ comportamiento físico comprobado: cada capítulo lo aclara donde importa. */

import type { UiIconName } from '../../components/ui/UiIcon';

export interface HelpChapter {
  title: string;
  /** Archivo o contrato de referencia, mostrado junto al título. */
  source?: string;
  text: string[];
  /** Tabla breve de datos técnicos: [dato, valor]. */
  facts?: [string, string][];
}
export interface HelpGroup { title: string; chapters: HelpChapter[] }
export interface HelpModule {
  /** Nombre del componente y su función en el sistema (encabezan la tarjeta del módulo). */
  name: string; role: string; icon: UiIconName;
  intro: string;
  /** Especificaciones clave, en celdas bajo la descripción. */
  specs: [string, string][];
  groups: HelpGroup[];
}
export type HelpModuleId = 'atmega' | 'esp' | 'web' | 'oled';

export const HELP: Record<HelpModuleId, HelpModule> = {
  atmega: {
    name: 'ATmega2560', role: 'Controlador de tiempo real', icon: 'board',
    intro: 'Es la autoridad del auto. Genera el PWM de las cuatro ruedas Mecanum, cuenta los pulsos Hall, mide distancia y línea, ejecuta secuencias, guarda la configuración en EEPROM y dibuja la OLED. Atiende todo en un bucle cooperativo alimentado por interrupciones, sin RTOS ni esperas bloqueantes, y solo responde pedidos del ESP: ninguna orden externa saltea sus guardas ni su STOP.',
    specs: [['Núcleo', 'AVR 8 bits · 16 MHz'], ['Memoria', '256 KiB flash · 8 KiB SRAM · 4 KiB EEPROM'], ['Código', 'C gnu11 + avr-libc'], ['Enlace', 'ROBO2 · USART1 115200']],
    groups: [
      { title: 'Arquitectura y compilación', chapters: [
        { title: 'Estructura del firmware', source: 'AutoFinalRobo2-Atmega/src',
          text: [
            'platform/ accede al hardware (placa, motores, sensores, TWI, USART1, watchdog, preferencias). services/ contiene la lógica sin registros: enlace ROBO2, producto EX01, secuencias, red y gestor I2C. ui/ y display/ dibujan menús, dashboard, QR y radar sobre un framebuffer; input/ decodifica el encoder.',
            'main.c es un bucle cooperativo: las ISR solo capturan (pulsos Hall, flancos del encoder, ticks, bytes UART) y el trabajo se hace en main. Ninguna tarea bloquea esperando hardware; los tiempos se comparan con el tick de 1 ms.',
            'Objetivos de make: firmware de producto, firmware de comunicación, libafr2_ui.a (renderer compartido con el Loader), diagnóstico I2C/encoder por USART0 y servicio puente USB↔ESP. El Loader compila y carga; la Web no flashea.',
          ],
          facts: [
            ['Compilador', 'avr-gcc -mmcu=atmega2560 -std=gnu11 -Os'],
            ['Opciones', '-Wall -Wextra -fstack-usage, secciones por función + --gc-sections'],
            ['Último build registrado', 'AFR2-20261029-20: flash 80 066 B, SRAM estática 7 085 B de 8 192 B'],
            ['Pila', 'Solo .su por función; el máximo dinámico con ISR no está certificado'],
          ] },
        { title: 'Temporización e interrupciones', source: 'afr2_board.c · afr2_sensors.c · afr2_watchdog.c',
          text: [
            'Timer0 en CTC genera el tick de 1 ms. INT4/INT5 capturan el giro del encoder y PCINT2 los ocho canales Hall. USART1 trabaja por interrupción de recepción y de registro vacío; TWI también es por interrupción.',
            'El watchdog arranca con 500 ms y lo alimenta main. Un reinicio pedido por Web u OLED se difiere hasta responder y luego fuerza WDTO_15MS. Tras un reinicio las salidas quedan apagadas y no se reanudan secuencias ni misiones.',
          ],
          facts: [
            ['Tick', 'Timer0 CTC, prescaler 64, OCR0A = 249 → 1 ms'],
            ['Watchdog', '500 ms en marcha · 15 ms para reinicio controlado'],
            ['ISR', 'INT4/INT5 encoder · PCINT2 Hall · TIMER3/TIMER5 sensores · USART1 RX/UDRE · TWI'],
          ] },
      ] },
      { title: 'Hardware y pines', chapters: [
        { title: 'Motores Mecanum', source: 'Contracts/hardware/PINOUT.md',
          text: [
            'Cuatro puentes H del Model Y con IN1/IN2 de dirección y PWM por rueda. El PWM es hardware: Timer2 (D9, D10) y Timer1 (D11, D12) en Fast PWM de 8 bits. Al detener, se desconectan las salidas de comparación y se ponen IN1/IN2 en cero.',
            'Convención del chasis: X positivo adelante, Y positivo izquierda, ángulo positivo antihorario visto desde arriba. La mezcla Mecanum y la cinemática Web usan la misma referencia; polaridad y sentido de cada rueda se corrigen por calibración.',
          ],
          facts: [
            ['DEL DER (FR)', 'IN1 D22 · IN2 D24 · PWM D9'],
            ['DEL IZQ (FL)', 'IN1 D26 · IN2 D28 · PWM D10'],
            ['TRAS DER (RR)', 'IN1 D5 · IN2 D6 · PWM D11'],
            ['TRAS IZQ (RL)', 'IN1 D7 · IN2 D8 · PWM D12'],
            ['Frecuencia PWM', '16 MHz / (64 · 256) ≈ 977 Hz'],
          ] },
        { title: 'Hall, línea, ultrasonido, servo y batería', source: 'afr2_sensors.c · PINOUT.md',
          text: [
            'Hall de ruedas: dos headers de cuatro señales, canal A en A8–A11 y canal B en A12–A15 (orden FR, FL, RR, RL), capturados por PCINT2. La distancia por rueda se calcula como π · diámetro · pulsos / pulsos por vuelta calibrados.',
            'HC-SR04: TRIG D30 y ECHO D31 medidos sin bloquear con Timer3 a 25 µs por tick; si no hay subida o bajada en 32 ms se informa no-rise / no-fall. El rango válido es 2 cm–4 m.',
            'Servo del radar en D13: Timer5 a 0,5 µs alterna el pin por comparación (sin salida OC), 20 ms de período y pulso de 1000–2000 µs. Sin pulso el servo queda libre.',
            'Línea IR1..IR5 en A4..A0 como ADC10 (0..1023) con buffers digitales deshabilitados. Batería en A5 con divisor 10 kΩ/10 kΩ y 100 nF a GND, tomada después de la llave; referencia AVcc 5 V nominal sin calibrar.',
          ],
          facts: [
            ['Hall', 'A: A8–A11 · B: A12–A15 · PCINT2'],
            ['HC-SR04', 'TRIG D30 · ECHO D31 · Timer3 25 µs · tope 32 ms'],
            ['Servo', 'D13 (PB7) · Timer5 · 1000–2000 µs cada 20 ms'],
            ['ADC', 'AVcc, prescaler 128 (125 kHz) · IR A4..A0 · batería A5'],
          ] },
        { title: 'Panel OLED y encoder', source: 'PINOUT.md',
          text: [
            'La OLED usa el TWI del Mega en D20/D21; A4/A5 no se usan como I2C porque están ocupados por IR1 y batería. El encoder KY-040 sin breakout cierra contra GND: CLK D2, DT D3 y SW D42.',
            'Conectores del panel: J1 hembra 1×5 a 2,54 mm (CLK, DT, SW, SDA, SCL) y J2 JST 1×2 a 2,00 mm (+5 V, GND). PCB y pieza nueva siguen pendientes de planos.',
          ],
          facts: [
            ['OLED', 'SDA D20 · SCL D21 · +5 V'],
            ['Encoder', 'CLK D2 (INT4) · DT D3 (INT5) · SW D42'],
            ['J1 / J2', '1×5 2,54 mm señales · 1×2 2,00 mm alimentación'],
          ] },
      ] },
      { title: 'Comunicación con el ESP', chapters: [
        { title: 'Enlace ROBO2 y sesión', source: 'Contracts/contracts/UART_PROFILE_2.md · afr2_link.c',
          text: [
            'USART1 (TX1 D18, RX1 D19) hacia el ESP por E_RX/E_TX del shield. Trama ROBO2: cabecera ASCII «ROBO2», longitud N ≤ 64, tokens 3A A5, versión 01, ruta, comando, payload y XOR de todos los bytes anteriores. Máximo 76 bytes. El XOR verifica estructura; no es CRC ni autenticación.',
            'Solo hay dos rutas: ESP→Mega (31) y Mega→ESP (13). El Mega nunca envía tramas espontáneas: responde. Cada payload empieza con sesión u32 y transacción u32 en little-endian; el Mega rechaza transacciones repetidas o anteriores.',
            'Supervisión: si la sesión negociada pasa 1500 ms sin pedidos, el Mega detiene el movimiento, revoca la sesión y exige un nuevo hello. Un error de recepción hace lo mismo.',
          ],
          facts: [
            ['UART', 'USART1 115200 8N1 · U2X, UBRR = 16 (+2,12 % nominal)'],
            ['Trama', '12 + N bytes, N ≤ 64 · XOR final'],
            ['Resultados', '0 OK · 1 BAD_REQUEST · 2 CAPABILITY_UNAVAILABLE · 3 SESSION_REQUIRED · 4 INTERNAL_ERROR'],
            ['Supervisión', '1500 ms sin pedidos → STOP y revocación'],
            ['Extensión', 'EX01 v9 sobre perfil 2'],
          ] },
      ] },
      { title: 'Movimiento, secuencias y memoria', chapters: [
        { title: 'Guardas de movimiento y STOP', source: 'DIRECT_CONTROL_7.md',
          text: [
            'El control es directo desde la Web, sin pedir permiso previo, pero el ATmega rechaza la orden si hay fallo, medición Hall, escritura EEPROM, radar o secuencia en curso, control local desde la OLED o edición de servo. Los motores deben estar habilitados en Calibración → Motores.',
            'Las consignas caducan si no se renuevan. STOP tiene prioridad en todas las colas; la Web solo lo muestra como confirmado cuando el ATmega lo informa como productor autoritativo.',
          ] },
        { title: 'Ejecutor de secuencias', source: 'afr2_sequence.c · EX01_SEQUENCE_DISTANCE_6.md',
          text: [
            'Hasta 32 instrucciones de rueda y 120 s por programa, en memoria estática. Los bloques terminan por tiempo o por distancia de rueda (en Chasis, la rueda de mayor recorrido). El watchdog de ejecución de 120 s produce fallo, nunca una llegada inventada.',
            'Las etiquetas viajan como IDs compactos y nombres constantes en flash hasta la pantalla de ejecución. La ejecución física sigue sujeta a la aprobación de SRAM/pila, Hall y calibración.',
          ],
          facts: [['Límites', '32 instrucciones · 120 s · distancia por pulsos Hall']] },
        { title: 'EEPROM y preferencias', source: 'afr2_preferences.c',
          text: [
            'Journal propio con dos registros de 128 bytes: firma «AFR2», versión, longitud de payload, CRC16-CCITT y marcador de commit 0xA5 escrito al final. Se lee el último registro válido; si ninguno valida se usan valores por defecto, sin escritura automática.',
            'Guardar y Descartar son operaciones distintas con resultado autoritativo. Las migraciones de versiones anteriores ocurren en RAM y preservan calibraciones (CPR por rueda, polaridad, Hall conectado, perfil IR, pulsación larga y servo).',
          ],
          facts: [
            ['Formato', '2 × 128 B · little-endian · CRC16-CCITT · commit 0xA5'],
            ['Versión actual', 'v8 (pulsación larga y preferencia de servo)'],
          ] },
      ] },
    ],
  },
  esp: {
    name: 'ESP8266 · ESP-12S', role: 'Pasarela Wi-Fi y servidor', icon: 'wifi',
    intro: 'Conecta el auto con el navegador. Levanta Wi-Fi como estación, punto de acceso o ambos; sirve la Web comprimida desde LittleFS y traduce cada pedido JSON del WebSocket a una transacción ROBO2 con el ATmega, con plazos y validación en los dos sentidos. Corre firmware propio sobre Arduino, sin comandos AT, y no decide sobre el movimiento.',
    specs: [['Núcleo', 'Xtensa L106 · 80 MHz'], ['Flash', '4 MiB · LittleFS 3 MiB'], ['Código', 'C++17 · PlatformIO'], ['Servicios', 'HTTP · WebSocket /ws']],
    groups: [
      { title: 'Plataforma', chapters: [
        { title: 'Toolchain y memoria', source: 'AutoFinalRobo2-ESP/platformio.ini',
          text: [
            'PlatformIO con espressif8266 4.2.1, placa esp12e y C++17. Bibliotecas fijadas: ESPAsyncTCP 2.0.0, ESPAsyncWebServer 3.10.3 y ArduinoJson 7.4.2. Un script previo ajusta los buffers de copia de AsyncTCP.',
            'El linker eagle.flash.4m3m supone 4 MiB de flash con 3 MiB para LittleFS. La flash física y el modo SPI deben identificarse en la unidad real antes de cargar una imagen.',
          ],
          facts: [
            ['Entorno', 'esp12s_candidate · framework arduino'],
            ['Flash', '4 MiB supuesta · LittleFS 3 MiB'],
            ['Cola WS', 'WS_MAX_QUEUED_MESSAGES = 4'],
          ] },
        { title: 'Módulos del firmware', source: 'AutoFinalRobo2-ESP/src',
          text: [
            'main.cpp arma el servidor y el bucle. broker.* valida el sobre WebSocket y despacha; broker_product, broker_sensors, broker_system, broker_wifi y broker_network adaptan cada familia de comandos. mega_link.* es el extremo ROBO2. storage.* monta LittleFS y verifica el paquete Web. wifi_service.* maneja STA/AP y búsqueda.',
          ] },
      ] },
      { title: 'Servidor y WebSocket', chapters: [
        { title: 'HTTP y paquete Web', source: 'main.cpp · storage.cpp',
          text: [
            'Sirve /dist de LittleFS con los activos ya comprimidos en gzip. «/» resuelve a /index.html, que solo se entrega si el manifiesto y los tamaños del paquete coinciden. El firmware no recalcula los hashes.',
            '/api/health informa servicio y ocupación del filesystem (usedBytes/totalBytes). /api/web-manifest devuelve el manifiesto del paquete.',
          ],
          facts: [['Rutas', '/ · /index.html · estáticos /dist · /api/health · /api/web-manifest · /ws']] },
        { title: 'Protocolo afr2-ws-1', source: 'broker.cpp · Contracts/contracts/PROTOCOL.md',
          text: [
            'Sobre JSON con schema «afr2-ws-1», type req/res/err/evt, requestId, cmd, payload y ts. Las respuestas se correlacionan por requestId; los eventos (system.status, boot, red) se difunden a la sesión.',
            'Una sola sesión de control. Hasta cuatro pedidos en cola, más una ranura propia para STOP que se adelanta. Cada frame debe ser texto final de hasta 1024 B; los fragmentos de continuación se rechazan.',
            'Un sobre inválido cierra con 1008, un frame fuera de orden con 1002 y uno demasiado grande con 1009. Mientras el ESP se reinicia solo acepta system.info, system.alive y robot.stop.',
          ],
          facts: [
            ['Frame', 'Texto ≤ 1024 B · JSON con anidamiento ≤ 5'],
            ['Identificadores', 'requestId ≤ 64 · cmd ≤ 32 caracteres'],
            ['Cola', '4 pedidos + STOP prioritario · 1 sesión'],
          ] },
      ] },
      { title: 'Puente con el ATmega', chapters: [
        { title: 'Transacciones ROBO2', source: 'mega_link.cpp',
          text: [
            'UART0 del ESP (GPIO1 TX, GPIO3 RX) a 115200 8N1. Una transacción a la vez con plazo de 800 ms: una respuesta tardía se descarta aunque ya esté en la cola de recepción. Sin respuestas válidas por 1500 ms, el enlace pasa a no listo y las capacidades se borran.',
            'Toda respuesta se valida contra ruta, comando, sesión, transacción, longitud y rangos del cuerpo. Wire válido no significa operación completada: el resultado semántico lo da cada comando.',
            'No se intercalan logs en la UART de aplicación. Los estados Web↔ESP, ESP↔ATmega y origen de actividad se informan por separado.',
          ],
          facts: [
            ['UART', 'GPIO1/GPIO3 · 115200 8N1'],
            ['Plazo', '800 ms por transacción · enlace caído a 1500 ms'],
            ['Captura OLED', 'robot.screen.get en páginas de 40 B (26 pedidos por 1024 B)'],
          ] },
        { title: 'Shield y carga', source: 'Contracts/hardware/WIFI_SHIELD.md',
          text: [
            'Operación con Mega: TX1 D18 → E_RX y E_TX → RX1 D19. El mapa de referencia del UNO no aplica, y el selector E_RX/E_TX no es la cabecera UART de 3,3 V del ESP.',
            'Programar el ESP por el puente USB del Mega exige el servicio puente y un cableado temporal distinto (cruce D0/D1 y niveles adecuados). No se cambia desde la Web. KEY/RST son mantenimiento físico.',
          ] },
      ] },
      { title: 'Red, sistema y batería', chapters: [
        { title: 'Wi-Fi', source: 'wifi_service.cpp · WIFI_UI_11.md',
          text: [
            'Modos STA, AP y AP_STA; la búsqueda es asíncrona y la configuración puede ser temporal o guardada. Seguridad informada como open, WEP, WPA, WPA2, WPA/WPA2 o protected; si es desconocida no se inventa.',
            'La clave del router nunca se guarda en el navegador ni se expone por HTTP. La OLED distingue arranque, servidor HTTP listo, IP adquirida, AP creado, clientes, transporte WebSocket y respuesta Web.',
          ] },
        { title: 'Información y reinicios', source: 'broker_system.cpp · SYSTEM_INFO_FILESYSTEM_15.md',
          text: [
            'system.info y system.hello devuelven versiones, fecha y compilador de cada placa, identidad y causa de arranque, y filesystem.usedBytes/totalBytes. Los datos ausentes se muestran como desconocidos.',
            'Reiniciar ESP o ATmega exige confirmación y placas detenidas; el reinicio se difiere para responder primero. system.boot informa el arranque siguiente. Un ACK de reinicio no prueba que el nuevo firmware haya iniciado.',
          ] },
        { title: 'Medición de batería', source: 'BATTERY_MEASUREMENT_9.md',
          text: [
            'La medición arranca desde OLED (Sistema → Batería) o desde la pill Batería de la Web y se renueva hasta reiniciar. Estados: off, measuring, valid, out-of-range y stale. La Web descarta lecturas vencidas.',
            'Las barras de la pill cambian por bandas de 6, 7 y 8 V; no son porcentaje de carga. Requiere ATmega y ESP con EX01 v9.',
          ] },
      ] },
    ],
  },
  web: {
    name: 'Web AFR2', role: 'Consola en el navegador', icon: 'screen',
    intro: 'Aplicación de una sola página que el ESP entrega comprimida y el navegador ejecuta por completo: rutas, estado de conexión, simulaciones y escenas 3D corren en el dispositivo del usuario. Envía órdenes por un único WebSocket supervisado y muestra cada lectura con su antigüedad; lo simulado nunca se presenta como medición.',
    specs: [['Versión', '0.4.0'], ['Stack', 'React 19 · TypeScript 5.9 · Vite 7'], ['3D', 'Three 0.180 · R3F 9 · Draco'], ['Transporte', 'afr2-ws-1 sobre WebSocket']],
    groups: [
      { title: 'Arquitectura', chapters: [
        { title: 'Stack y estructura', source: 'AutoFinalRobo2-Web/package.json · src/',
          text: [
            'Versión 0.4.0. React 19 con TypeScript 5.9 y Vite 7, rutas hash (react-router 7), Tailwind 4 y tokens propios del manual visual, Sonner para avisos, Three 0.180 con @react-three/fiber 9 para las escenas.',
            'src/app tiene rutas y proveedores (tema, conexión, overlays); src/protocol el cliente WebSocket y los decodificadores; src/features una carpeta por página; src/components las primitivas compartidas; src/styles tokens y hojas por área.',
          ],
          facts: [
            ['Versión', 'Web 0.4.0'],
            ['Rutas', 'HashRouter · páginas 3D, Infraestructura y Protocol Studio cargadas bajo demanda'],
            ['Escenas', 'frameloop «demand»: solo dibujan cuando algo cambia'],
          ] },
        { title: 'Cliente WebSocket', source: 'src/protocol/afr2Client.ts · ConnectionContext.tsx',
          text: [
            'Abre /ws en el mismo host que sirvió la página (ws o wss según el protocolo); no hay dirección manual ni botón Conectar. Tras system.hello mantiene la sesión con system.alive cada 500 ms.',
            'Cada pedido tiene un plazo (2,5 s por defecto). Al cortarse, reconecta con espera exponencial de 500 ms a 15 s. Las lecturas vencidas se muestran con su antigüedad o como desconocidas, nunca con un valor inventado.',
            'Lecturas periódicas solo mientras hay una vista que las usa: estado de pantalla cada 1 s con el visor abierto, cabezal de sensores cada 100 ms en las vistas que lo animan.',
          ],
          facts: [
            ['hello', 'Plazo 5 s'],
            ['alive', 'Cada 500 ms · sesión vence a 30 s'],
            ['Reconexión', '500 ms × 2ⁿ, tope 15 s'],
          ] },
      ] },
      { title: 'Operación', chapters: [
        { title: 'Supervisión y parada', text: [
          'La pérdida de foco, ocultar la pestaña, cambiar de tarea o desconectarse detiene de inmediato el envío de consignas, cancela cualquier secuencia física y emite robot.stop.',
          'La cabecera tiene la única acción STOP: aparece con movimiento, simulación o transferencia en curso, o si una parada quedó sin confirmar. Una petición aceptada no se presenta como parada física confirmada.',
        ] },
        { title: 'Movimiento manual y táctil', source: 'MANUAL_TOUCH_PERMISSIONS_18.md · TOUCH_MODEL_READY_19.md', text: [
          'Mecanum y Control táctil comparten disponibilidad PWM y órdenes. Requieren enlace completo Web–ESP–ATmega, motores habilitados y STOP sin confirmación pendiente; no exigen odometría, CPR calibrados ni lectura HC.',
          'El táctil abre el modelo 3D a pantalla completa: se arrastra hacia una de ocho direcciones y se mantiene; las diagonales reparten la consigna entre ambos ejes. Soltar, perder foco o visibilidad, cambiar el tamaño, STOP o perder disponibilidad cancela y exige un gesto nuevo. El modelo es referencia fija, no pose medida.',
        ] },
        { title: 'Secuencias, pistas y GOTO', source: 'EX01_SEQUENCE_DISTANCE_6.md', text: [
          'Bloques de desplazamiento, rampa, giro, curva, espera y finalización, en modo Chasis o con cuatro pistas independientes. El documento v2 importa programas v1 y conserva la biblioteca anterior en otra clave de almacenamiento.',
          'Enviar al robot carga, verifica y pide el inicio al ATmega; los límites son los del ejecutor (32 instrucciones, 120 s). El simulador estima la duración de los bloques por distancia a partir de la velocidad.',
          'GOTO arma una lista de hasta 64 comandos y 10 minutos, entre −50 y 50 m por eje, con pasos relativos de 0,1 a 10 m y velocidad simulada de 0,05 a 0,5 m/s. La simulación es Mecanum ideal y no envía la lista al robot.',
        ] },
        { title: 'Calibración y diagnóstico', text: [
          'Calibración consulta el parámetro elegido antes de escribir, y aplicar, guardar y descartar son órdenes separadas. STOP, desconexión o pérdida de foco interrumpen los pasos pendientes. Los Hall se calibran por vuelta desde la OLED.',
          'Configuración → Información de hardware muestra montaje y ocupación de LittleFS, versión, fecha, perfil y hash del índice del paquete declarados por el ESP.',
        ] },
      ] },
      { title: 'Distribución', chapters: [
        { title: 'Paquete para el ESP', source: 'tools/package-web.mjs · WEB_ASSETS.md',
          text: [
            'npm run build:esp cataloga los modelos, sincroniza la biblioteca OLED, compila TypeScript, construye con Vite en modo esp, empaqueta y copia a ESP/data/dist. No flashea.',
            'Cada activo se comprime con gzip nivel 9 y se registra en un manifiesto con tamaño y SHA-256 del original. Se estima la ocupación de LittleFS antes de copiar. Imágenes WebP; modelos GLB con Draco locales, sin CDN.',
          ],
          facts: [
            ['Pipeline', 'catálogo 3D → biblioteca OLED → tsc → vite --mode esp → paquete → copia'],
            ['Compresión', 'gzip nivel 9 · manifiesto con SHA-256'],
          ] },
      ] },
    ],
  },
  oled: {
    name: 'OLED 0,96 pulgadas + encoder', role: 'Interfaz local del auto', icon: 'encoder',
    intro: 'Pantalla monocroma SSD1306 manejada por el ATmega como único dispositivo I2C, con refresco diferencial por páginas, y un encoder KY-040 como única entrada: giro, corto y largo. La Web usa el mismo renderer para simular el flujo y puede ver y operar la pantalla real en vivo.',
    specs: [['Panel', 'SSD1306 · 128×64 · I2C'], ['Bus', 'TWI 100 kHz'], ['Entrada', 'KY-040 · INT4/INT5 + D42'], ['Gesto largo', '≥ 800 ms, al soltar']],
    groups: [
      { title: 'Funcionamiento', chapters: [
        { title: 'Driver y refresco', source: 'display/afr2_oled.c · services/afr2_i2c_manager.c',
          text: [
            'La OLED es el único dispositivo I2C y usa el gestor I2C (propiedad, plazo y callback en main). Framebuffer de 1024 B en 8 páginas de 128 B; cada página tiene un bit «sucia» y solo se envían los tramos que cambiaron, de hasta 16 columnas, como trabajo cooperativo de baja prioridad.',
            'Dibujar no lee sensores ni dispara transferencias: snapshot, navegación y renderer están separados. Ante un error aislado se reintenta conservando las páginas confirmadas; ante fallos persistentes se reconfigura el controlador.',
          ],
          facts: [
            ['Bus', 'TWI 100 kHz (TWBR = 72, prescaler 1)'],
            ['Framebuffer', '1024 B · 8 páginas · tramos de hasta 16 columnas'],
            ['Contraste inicial', '0xCF'],
          ] },
        { title: 'Encoder y gestos', source: 'input/afr2_encoder_ui.c',
          text: [
            'Giro en cuadratura por INT4/INT5, cuatro transiciones por detención. SW con antirrebote de 20 ms. La pulsación larga se mide al soltar: desde 800 ms (configurable de 800 a 3000 ms, guardado en EEPROM v8) cuenta como larga y nunca genera además un corto.',
            'No hay botón de menú. En el dashboard, corto abre el Menú principal y largo abre los Controles rápidos. En el resto, corto confirma o alterna y largo vuelve al contexto anterior. Con cambios pendientes aparece Sí/No obligatorio; el largo no lo evita.',
          ],
          facts: [
            ['Giro', 'INT4/INT5 · 4 transiciones por paso'],
            ['SW', 'Antirrebote 20 ms · largo ≥ 800 ms al soltar'],
          ] },
        { title: 'Dashboard y pantallas', source: 'ui/afr2_dashboard.c · OLED_DASHBOARD_STATUS_20261102.md',
          text: [
            'La cabecera muestra SSID e IP de STA/AP con desplazamiento propio entre el ícono y la batería. La tarjeta de estados alterna cada 4 s tras 3 s sin usar el encoder; mantener SW la pausa y un fallo queda fijo.',
            'Wi-Fi incluye QR para conectarse al AP y para abrir la Web, generados en el auto con qrcodegen y los datos vigentes. El QR del AP no se transmite por captura remota.',
          ] },
      ] },
      { title: 'Pantalla en vivo y simulación', chapters: [
        { title: 'Captura remota', source: 'LiveOledViewer.tsx · WEB_OLED_CONSUMER.md',
          text: [
            'Con el visor abierto la Web se suscribe (robot.screen.subscribe) y lee robot.screen.status cada 1 s: código de pantalla, revisión de navegación, selección y primera fila visible. robot.screen.get trae el framebuffer, que el ESP pide al ATmega en páginas de 40 B.',
            'Modo Periódica: pide el framebuffer cada 1,2 s. Modo Al cambiar: solo cuando cambia el estado informado; cada estado igual confirma la captura dibujada. Los valores que cambian dentro de una misma pantalla solo se actualizan en modo periódico o con «Actualizar pantalla».',
            'robot.ui.action envía giro, corto o largo junto con la revisión vista; si la pantalla cambió antes, la acción se rechaza. Una captura de más de 4 s bloquea el encoder remoto hasta recibir otra.',
          ],
          facts: [
            ['Estado', 'robot.screen.status cada 1 s'],
            ['Framebuffer', 'robot.screen.get · 1024 B · vence a 4 s'],
            ['Entrada', 'robot.ui.action con revisión'],
          ] },
        { title: 'Copia Web del renderer', source: 'tools/sync-oled-library.mjs · features/oled/',
          text: [
            'La biblioteca de íconos y pantallas del firmware se sincroniza en cada build y la Web la rasteriza con el mismo orden de páginas que el ATmega. «Simular flujo» recorre los menús con datos ficticios; «Biblioteca sin datos» muestra el diseño sin simular servicios.',
            'La comparación píxel a píxel con la pantalla física sigue pendiente.',
          ] },
      ] },
    ],
  },
};
