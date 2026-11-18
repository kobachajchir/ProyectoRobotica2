# Guion hablado · AutoFinalRobo2 (presentación v3)

Generado por `tools/generar_presentacion_v3.js` (mismo texto que las notas del orador). Tiempo de contenido: 23:30 + ~1:30 de transiciones ≈ 25 min. Los anexos no cuentan.

## 01 · F01 · De la teoría a un sistema mecatrónico real  
*20 s · acumulado 0:20 · ramas M+E+C+P*

> No vamos a recorrer la carrera materia por materia. Vamos a recorrer un sistema que necesitó conocimientos de distintas materias para existir. Este auto es el hilo conductor: primero veremos cómo se construye, después cómo mide, cómo controla su comportamiento, cómo se programa y, al final, cómo todo eso se integra.

**Fuentes:** Render F01_A01 desde AutoFinalRobo2-Web/public/models/AutoFinalRobo2.glb (sha256 5bd9d51b…), cámara CAM_F01_HERO_V01 (renders/cameras_all.json).

## 02 · F02 · ¿Qué es la Mecatrónica?  
*35 s · acumulado 0:55 · ramas M+E+C+P*

> Un sistema mecatrónico no es una suma de partes: es estructura que se mueve, electrónica que mide y actúa, control que regula ese comportamiento y software que procesa y comunica. Estas cuatro ramas son el lenguaje que vamos a usar hoy, con su color en cada filmina; no son los bloques formales del plan, que se organiza en Ciencias Básicas, Tecnologías Básicas, Aplicadas y Complementarias.

**Fuentes:** Res. C.S. 012/23 (Plan 2023): cuatro bloques formales; perfil del egresado (electrónica, mecánica, informática, control).

## 03 · F03 · Árbol de conocimientos y correlatividades  
*70 s · acumulado 2:05 · ramas M+E+C+P*

> Acá hay tres recorridos que el auto usa. Arriba, la mecánica: Estática y Resistencia de Materiales y Ecuaciones Diferenciales regularizadas, más Ciencias de los Materiales aprobada, llevan a Mecánica Racional, y esa junto con Sistemas de Control lleva a Robótica I. Abajo, la electrónica digital: Electrónica Digital y Laboratorio de Mediciones regularizadas, Electrónica Básica y Computación II aprobadas, habilitan Microcontroladores, y desde ahí Sistemas Embebidos y luego Sistemas Operativos y Redes. La letra en cada flecha es la condición oficial: R regularizada, A aprobada. La línea punteada no es una correlatividad: es un vínculo que agregamos nosotros porque en el auto el firmware embebido es el que ejecuta el control.

**Fuentes:** Aristas copiadas del informe maestro (C.D. 726/23): Mecánica Racional, Microcontroladores, Sistemas de Control, Sistemas Embebidos, Robótica I, Sist. Operativos y Redes.

**Pendiente / no afirmar:** Revisión humana obligatoria contra el PDF de la C.D. 726/23. El grafo completo del plan (todas las materias) no se generó: solo las aristas citadas.

## 04 · F04 · Arquitectura general del robot  
*45 s · acumulado 2:50 · ramas M+E+C+P*

> Este es el mapa que vamos a recorrer. En el navegador corre una aplicación React. Se comunica por Wi-Fi con el ESP8266, que sirve los archivos de la aplicación y abre un WebSocket. El ESP traduce esos mensajes JSON a una trama binaria por UART hacia el ATmega2560, que es quien tiene la autoridad sobre los motores: ahí corren el control de velocidad, la odometría, la pantalla OLED y la lectura de sensores. Cada flecha de este diagrama tiene un protocolo y un archivo detrás.

**Fuentes:** AutoFinalRobo2-Contracts/contracts/UART_PROFILE_2.md, PROTOCOL.md; AutoFinalRobo2-Atmega/src/services/afr2_product.c (PID y odometría en ATmega); ESP/platformio.ini; Web/package.json.

## 05 · F05 · Geometría de las ruedas Mecanum  
*30 s · acumulado 3:20 · ramas M*

> Las ruedas son Mecanum: los rodillos están inclinados respecto del eje, y por eso combinando el sentido de las cuatro ruedas el auto puede avanzar, ir de costado o girar sobre sí mismo. Esa misma geometría aparece en el firmware: la odometría suma los avances de las cuatro ruedas para obtener el desplazamiento hacia adelante, los combina con signos alternados para el lateral, y divide por la suma de semidistancias, ciento setenta milímetros, para el giro.

**Fuentes:** afr2_product.c odometry() (fórmulas dx, dy, angle); afr2_geometry.h (AFR2_FIXED_WHEEL_MM 80, AFR2_FIXED_RADIUS_SUM_MM 170); render: bbox del objeto AFR2_Rueda80_ManoA:1 = 47 × 109,2 × 109,2 mm.

**Pendiente / no afirmar:** El diámetro del modelo GLB (≈109 mm) no coincide con la constante del firmware (80 mm). Medir la rueda física y corregir el que esté mal antes de presentar un diámetro. No afirmar ángulo de rodillos (no medido).

## 06 · F06 · De un perfil 2D a placas tridimensionales  
*40 s · acumulado 4:00 · ramas M*

> Las placas del chasis vienen del kit. Para modelarlas partimos del contorno en dos dimensiones, lo llevamos a un sketch en Fusion, lo extruimos con el espesor de la placa y verificamos que agujeros y ranuras coincidan con los de la pieza real. Es exactamente lo que se practica en Sistemas de Representación II: pasar de un dibujo plano a un sólido y después a un plano de conjunto.

**Fuentes:** AutoFinalRobo2-CAD/mecanica/auto-completo/fusion_ensamble.py, AutoFinalRobo2.f3d; objetos AFR2_CapaSuperior/AFR2_CapaInferior del GLB.

**Pendiente / no afirmar:** F06_A01/A02 (captura del SVG fuente y del timeline de Fusion) no se tomaron. Confirmar con qué archivo se generó el perfil antes de nombrarlo SVG. No afirmar material (acrílico/chapa) sin confirmarlo.

## 07 · F07 · Componentes reutilizados y ensamblaje CAD  
*40 s · acumulado 4:40 · ramas M*

> El auto parte de un kit comercial. Lo que ya existía, como el Arduino Mega, los shields de motores y Wi-Fi, los motores, el ultrasónico o las ruedas, lo integramos desde modelos de biblioteca. Lo que no existía lo diseñamos: el soporte del panel de pantalla y perilla, su placa en KiCad y el divisor para medir la batería. La vista explotada es solo para mostrar las capas; no representa un orden de montaje.

**Fuentes:** Nombres de componente del GLB (AFR2_Mega2560_CAD, AFR2_OSOYOOMotorDriver_ModelY_V2, AFR2_OSOYOOWifiShield, AFR2_GM25_370 ×4, AFR2_HC_SR04, AFR2_SG90, AFR2_SensorLinea_5IR, AFR2_Rueda80_ManoA/B, AFR2_Celda_18650 ×2); CAD/docs/PANEL_UI_20261025.md.

**Pendiente / no afirmar:** Nombrar GrabCAD solo si se demuestra la procedencia de cada archivo (ver inventario CAD/reference/supplied). Explosión vertical: desplazamientos ilustrativos.

## 08 · F08 · Integrar OLED y encoder modifica la mecánica  
*35 s · acumulado 5:15 · ramas M+E*

> Agregar una pantalla y una perilla parece un cambio electrónico, pero obliga a resolver mecánica: dónde se fija, si se puede girar la perilla sin tocar las ruedas, cómo entra el conector. Diseñamos un soporte con bisel sobre la placa superior, con torretas para tornillos M3, y una placa de veinte por treinta y cinco milímetros.

**Fuentes:** CAD/docs/PANEL_UI_20261025.md (20×35 mm, B.Cu, torretas M3×15); AGENTS.md (conectores 1×5 y 1×2, 2,54 mm); render F08_A01 (objeto AFR2_AFR2_UI_B2_Fondo_OLED15_Auto5).

**Pendiente / no afirmar:** Montaje físico e interferencias del soporte sin verificar (según la ficha).

## 09 · F09 · Fuerzas y momentos en el brazo  
*50 s · acumulado 6:05 · ramas M*

> El brazo es donde la estática se vuelve concreta. Cada peso y la carga en la pinza generan, respecto de cada articulación, un momento igual a la fuerza por el brazo de palanca. Por eso la base y el primer eslabón son los más exigidos: ahí se acumulan todos los momentos. En cada sección hay que separar la fuerza axial, el cortante, el momento flector y el torsor; no son lo mismo y no se dimensionan igual.

**Fuentes:** CAD/mecanica/brazo-antropomorfo/analisis_cargas_200g/INFORME_ITERACION_DISENO.md; revision_v17/README.md; fotos_existentes/FOTO-01, FOTO-30.

**Pendiente / no afirmar:** El brazo no está aprobado para fabricación ni resistencia (J1 abierto). No mostrar valores numéricos de momento sin la hoja de equilibrio. F09_A01 (lateral ortográfica del brazo sobre el auto) no se renderizó: el GLB del auto no incluye el brazo montado.

## 10 · F10 · Centro de masa, inercia y vuelco  
*50 s · acumulado 6:55 · ramas M+C*

> Cuando el brazo se estira, el centro de masa del conjunto se desplaza. Mientras su proyección quede dentro del polígono que forman los contactos de las ruedas, el auto no vuelca. Si además frenamos o aceleramos, sumamos una fuerza inercial equivalente, menos masa por aceleración: es la construcción de D'Alembert, no una fuerza física nueva. El criterio no es mirar el dibujo: es comparar los momentos respecto del borde crítico.

**Fuentes:** CAD/mecanica/brazo-antropomorfo/busqueda_actuadores_v13/v14 README (estabilidad condicionada a contactos hipotéticos); fotos_existentes/FOTO-29.

**Pendiente / no afirmar:** Contactos reales y masa medida del auto pendientes: no declarar vuelco ni margen numérico.

## 11 · F11 · Del datasheet al circuito  
*40 s · acumulado 7:35 · ramas E*

> Antes de conectar algo leemos su hoja de datos: qué tensión tolera cada pin, qué corriente entrega, qué nivel lógico espera. Por ejemplo, el ADC del ATmega no acepta más tensión que su referencia de cinco volts, y eso define el circuito que vamos a ver para medir la batería. A la izquierda, el pinout del shield; a la derecha, el esquemático real tal como está en KiCad.

**Fuentes:** fotos_existentes/FOTO-12; AutoFinalRobo2-CAD/kicad/oled-encoder/sensorbateria.kicad_sch exportado con kicad-cli 10.99 (capturas/sensorbateria.png).

**Pendiente / no afirmar:** F11_A01: captura del datasheet oficial (pinout + límites) no tomada; guardar URL y revisión del documento.

## 12 · F12 · De KiCad al ensamblaje  
*40 s · acumulado 8:15 · ramas E+M*

> La placa del panel nace en KiCad: primero el esquemático, después el ruteo, y como cada huella tiene su modelo tridimensional, KiCad nos da la placa armada en 3D. Ese modelo se exporta en STEP y entra a Fusion para verificar que encaje en el soporte. La placa se fabricó por transferencia y ácido, con componentes de agujero pasante soldados a mano.

**Fuentes:** AutoFinalRobo2-CAD/kicad/oled-encoder/PlacaOLED-Encoder-AutoRobo2.kicad_sch/.kicad_pcb (kicad-cli 10.99: sch export svg, pcb render); exports/oled-encoder/PCB_UI_Final_20261025.step, Panel_UI_Fusion_A.png.

**Pendiente / no afirmar:** Fabricación por ácido y soldadura TH: DECLARADO por el equipo (ideas de la charla), sin foto. Sin ERC/DRC registrados.

## 13 · F13 · Batería y ADC  
*40 s · acumulado 8:55 · ramas E*

> Las dos celdas 18650 en serie dan más de cinco volts, y el ADC del ATmega no puede medir más que su referencia. La solución es un divisor con dos resistencias iguales de diez kilo: en el punto medio tenemos exactamente la mitad, con un capacitor de cien nanofaradios para filtrar. El ADC de diez bits convierte eso en un número de cero a mil veintitrés; como dividimos por dos, el firmware multiplica por diez mil en vez de cinco mil y divide por mil veinticuatro. Dos detalles: es una tensión, no un porcentaje de carga, y todavía no está calibrada.

**Fuentes:** contracts/BATTERY_MEASUREMENT_9.md (10 kΩ+10 kΩ, 100 nF, A5/PF5/ADC5, fórmula, prescaler 128, 8 muestras); afr2_sensors.c:30; kicad sensorbateria.kicad_sch.

**Pendiente / no afirmar:** Montaje y calibración no comprobados (según contrato). Rango 2×4,2 V es DERIVADO de la química nominal, no medido.

## 14 · F14 · Encoders Hall y cuadratura  
*45 s · acumulado 9:40 · ramas E+C*

> Cada motor GM25-370 tiene un encoder Hall con dos canales, A y B, desfasados un cuarto de ciclo. El firmware cuenta solo los flancos de subida de A, con una interrupción por cambio de pin, y mira en ese instante el nivel de B: si está alto resta, si está bajo suma. Así sabemos cuánto giró y hacia dónde. Contando pulsos en un intervalo de tiempo obtenemos la velocidad angular. La resolución de trescientos setenta pulsos por vuelta la medimos girando la rueda a mano.

**Fuentes:** afr2_board.c:40-47 (hall_capture, ISR PCINT2, A8–A15); include/afr2_product.h:17 (AFR2_DEFAULT_CPR 370, medido por el usuario); contracts/hardware/PINOUT.md.

**Pendiente / no afirmar:** 370 es DECLARADO (medición manual del usuario): mostrarlo como tal. El código real usa suma en u32 con UINT32_MAX para restar; el snippet lo simplifica a ±1 (indicarlo si se pregunta).

## 15 · F15 · Ultrasonido: tiempo de vuelo  
*45 s · acumulado 10:25 · ramas E*

> El HC-SR04 emite un pulso de ultrasonido y devuelve por el pin de eco un pulso cuyo ancho es el tiempo que tardó el sonido en ir y volver. La distancia es la velocidad del sonido por ese tiempo, dividido dos. En el firmware no esperamos con un bucle: un temporizador interrumpe cada veinticinco microsegundos, detecta el flanco de subida y el de bajada del eco, y si en treinta y dos milisegundos no llegó, informa el error sin trabar el resto del programa. El sensor va sobre un servo, y con eso armamos el radar.

**Fuentes:** afr2_sensors.c:119-148 (Timer3 CTC, OCR3A=49, ÷8 → 25 µs; timeout 1280 ticks; TRIG PC7/D30, ECHO PC6/D31); contracts/SENSORS_RADAR_8.md.

**Pendiente / no afirmar:** Temperatura/velocidad del sonido asumida en la conversión: verificar en el código de conversión antes de mostrar un valor de c.

## 16 · F16 · Sensores de línea  
*45 s · acumulado 11:10 · ramas E+C*

> Cinco emisores y receptores infrarrojos miran el piso. La cinta oscura refleja menos que el piso claro, y cada sensor entrega un bit. El firmware le asigna a cada sensor un peso, de más dos a menos dos, promedia los que ven cinta y obtiene un error: cuánto está la línea corrida respecto del centro. Ese error multiplicado por una ganancia es el giro que se le pide al auto, mientras avanza a velocidad constante. Si deja de ver la línea, se detiene.

**Fuentes:** afr2_product.c:370-388 (pesos 2−i, line_kp, parada sin línea y por cruce ≥150 ms); line_speed 80 y line_kp 100 por defecto (afr2_product.c:27).

**Pendiente / no afirmar:** Polaridad analógica (blanco = ADC alto) no verificada: el modo mostrado es digital. F16_A03 sin lecturas reales: no mostrar números.

## 17 · F17 · I²C y pantalla OLED  
*35 s · acumulado 11:45 · ramas E+P*

> La pantalla habla I²C: dos cables, uno de datos y uno de reloj, que el maestro, el ATmega, genera a cien kilohertz. Como es el único dispositivo del bus, un administrador de I²C es el dueño de las transferencias y nunca se escribe desde una interrupción. La imagen vive en un buffer de mil veinticuatro bytes, uno por cada ocho píxeles, y se manda por bloques. La perilla es un encoder rotativo leído por interrupciones: corto confirma, largo abre los controles rápidos.

**Fuentes:** afr2_twi.c:42 (TWBR=72 → 100 kHz); afr2_oled.c (SSD1306, 128×64 páginas); builds/2026-11-06-04 (buffer 1024 + sombra ACK 1024); AGENTS.md (navegación); afr2_board.c (INT4/INT5).

**Pendiente / no afirmar:** Dirección I²C y tasa real de actualización (F17_A03) no medidas: no mostrar FPS ni dirección. El esquemático alimenta el panel a 3V3 y anota «adaptación de niveles I²C con Mega 5 V: pendiente externa»; la ficha de pinout reporta el panel a 5 V. Resolver antes de presentar la tensión del bus.

## 18 · F18 · Motor DC y servo: dos usos de señales temporizadas  
*40 s · acumulado 12:25 · ramas E+C*

> El motor y el servo reciben pulsos, pero leen cosas distintas. Al motor le llega un PWM de casi mil hertz: lo que importa es qué fracción del período está encendido, porque eso fija la tensión media y por lo tanto la velocidad. Al servo le llega un pulso cada veinte milisegundos, y lo que importa es el ancho: entre uno y dos milisegundos, que el servo traduce en un ángulo de cero a ciento ochenta grados.

**Fuentes:** afr2_motor.c:21-23 (Fast PWM 8 bits, ÷64 → 976 Hz, Timer1/Timer2; OCR por rueda); afr2_sensors.c:149-170 (Timer5, 0,5 µs, 40000 ticks = 20 ms, 1000–2000 µs, PB7/D13).

**Pendiente / no afirmar:** Frecuencia PWM 976 Hz es la configurada en el timer (comentario: «candidate»), no medida con osciloscopio.

## 19 · F19 · Lazo abierto y lazo cerrado  
*45 s · acumulado 13:10 · ramas C*

> En lazo abierto mandamos un PWM y confiamos en que el motor gire a la velocidad esperada. Pero cada motor es distinto, la batería baja y el piso cambia. En lazo cerrado medimos la velocidad con el encoder, la comparamos con la pedida y corregimos con esa diferencia, el error. El auto tiene los dos: el modo PWM manual es lazo abierto, y los modos de conducción, línea y GOTO cierran el lazo.

**Fuentes:** afr2_product.c:420-450 (PID solo si modo ≠ WHEEL_TEST y ≠ PWM_MANUAL); afr2_product.c:428-431.

## 20 · F20 · El controlador se convierte en firmware  
*55 s · acumulado 14:05 · ramas C+P*

> Este es el controlador que dibujamos en bloques, escrito en C. Cada diez milisegundos, para cada rueda, se calcula el error entre la velocidad pedida y la medida, se acumula la integral con un límite para que no crezca sin fin, se combinan proporcional, integral y derivativo, se aplica una corrección por motor, se satura al PWM máximo y se pasa por una rampa para no dar tirones. Con los valores por defecto la derivativa es cero, así que en la práctica es un PI. Todo con enteros, porque el ATmega no tiene unidad de punto flotante.

**Fuentes:** afr2_product.c:12-27 (defaults kp=128, ki=8, kd=0, ramp_step=4, pwm_limit=255), :243 ramp(), :428-445 lazo; main.c:119 (control_ms ≥ 10).

**Pendiente / no afirmar:** Sin log experimental: el gráfico de respuesta queda BLOQUEADO. Los valores de ganancias pueden haberse modificado por EEPROM en el auto real.

## 21 · F21 · Motores diferentes, sistema real  
*30 s · acumulado 14:35 · ramas C*

> Cuatro motores iguales en el catálogo no son iguales en la realidad. Por eso cada rueda tiene un factor de corrección y la odometría tiene factores por dirección. Pero el control tiene límites: si el PWM llega al máximo, no hay más corrección posible. Y el firmware se protege: si una rueda recibe potencia y su encoder no se mueve, o si el signo del encoder quedó invertido, lo que generaría una realimentación positiva, se detiene todo.

**Fuentes:** afr2_product.c:51 (motor_trim 500–1500), :297 (odom_scale por dirección, AFR2-20261111-06), :446 (fault 3 sin cambio Hall 500 ms), :424 (AFR2_FAULT_SIGN_MISMATCH).

**Pendiente / no afirmar:** No concluir «motor defectuoso» sin ensayo.

## 22 · F22 · ATmega2560 y ESP8266  
*35 s · acumulado 15:10 · ramas P+E*

> Usamos dos procesadores porque cada uno es bueno en algo. El ATmega2560 es un microcontrolador de ocho bits con muchos temporizadores y pines: tiene la autoridad sobre motores, sensores y pantalla, con tiempos predecibles. El ESP8266 tiene Wi-Fi y más memoria: sirve la aplicación web y hace de puente. Si se cae la red, el ATmega sigue controlando el auto de forma segura.

**Fuentes:** Datasheets Microchip ATmega2560 y Espressif ESP8266EX (capacidades del chip); Atmega/Makefile (-mmcu=atmega2560, F_CPU 16 MHz); ESP/platformio.ini (esp12e, eagle.flash.4m3m.ld, «candidate only»).

**Pendiente / no afirmar:** Modelo exacto del módulo ESP y su flash no identificados físicamente (platformio.ini lo dice).

## 23 · F23 · C, Arduino Core y abstracción  
*30 s · acumulado 15:40 · ramas P*

> En el ATmega programamos en C sobre los registros: para abrir la UART escribimos directamente los bits que fijan la velocidad, el formato y las interrupciones. En el ESP usamos el núcleo Arduino: una sola llamada configura el Wi-Fi como punto de acceso o como estación. Abstraer nos hace más rápidos, pero el hardware sigue ahí: alguien tiene que saber qué hace esa llamada por debajo.

**Fuentes:** afr2_uart1.c:13-19; wifi_service.cpp:98-99; AGENTS.md (ATmega sin Arduino ni DMA).

## 24 · F24 · Wi-Fi, HTTP y carga de la aplicación  
*35 s · acumulado 16:15 · ramas P*

> Primero el teléfono o la computadora se asocia a la red: o al punto de acceso del propio auto, o a la red de casa donde también está el auto. Recién después el navegador pide la página por HTTP. El ESP responde con los archivos ya comprimidos en gzip desde su memoria, y cuando la aplicación arranca abre un WebSocket para el tiempo real. Esos archivos los generamos con Vite: con npm run dev trabajamos en la computadora con recarga instantánea, y con npm run build:esp se compila, se comprime y se copia a la carpeta que se graba en el ESP.

**Fuentes:** Web/package.json (scripts y versiones declaradas en el manifest); ESP/platformio.ini; contracts/PROTOCOL.md (HTTP/LittleFS: gzip obligatorio, 406, no-store, ETag); wifi_service.cpp:98-109; afr2_build_info.h (1.39.6).

**Pendiente / no afirmar:** Las versiones Web son rangos del package.json; las instaladas exactas están en package-lock.json. F24_A02 captura de DevTools no tomada.

## 25 · F25 · El modelo OSI como mapa conceptual  
*45 s · acumulado 17:00 · ramas P*

> El modelo OSI nos sirve como mapa. Arriba, en aplicación, están HTTP y WebSocket y nuestros mensajes. Los datos viajan como JSON, texto. Debajo está TCP, que garantiza que lleguen en orden, después IP y el Wi-Fi. Ojo con dos cosas: es una asociación didáctica, no una implementación capa por capa, y el identificador de pedido que usamos para correlacionar respuestas no es la capa de sesión. A la derecha, el enlace entre el ESP y el ATmega ni siquiera es TCP/IP: es una UART con nuestra propia trama.

**Fuentes:** ESP/platformio.ini (ESPAsyncTCP, ESPAsyncWebServer); contracts/PROTOCOL.md (afr2-ws-1, requestId); UART_PROFILE_2.md.

## 26 · F26 · React ↔ ESP ↔ ATmega  
*40 s · acumulado 17:40 · ramas P+E*

> Sigamos una operación concreta: leer la batería. El navegador manda por WebSocket un mensaje JSON con un identificador de pedido y el nombre del comando. El ESP lo valida y lo traduce a una trama binaria por UART: la cabecera ROBO2, la ruta de ESP a ATmega, el código cero cincuenta y cinco, los números de sesión y transacción, el byte que pide leer y el XOR. El ATmega responde por la ruta inversa con veintitrés bytes, y el ESP los convierte otra vez en JSON para la página. Tres formatos distintos para la misma pregunta.

**Fuentes:** contracts/BATTERY_MEASUREMENT_9.md (0x55, cuerpo 1 B pedido, 23 B respuesta, nombres de campos WS); wire_catalog.json envelope (rutas 0x31/0x13, prefijo session/transaction u32le); Web/src/protocol/afr2Client.ts:567-575 (schema, type 'req', requestId, cmd, payload, ts).

**Pendiente / no afirmar:** Sobre JSON verificado en afr2Client.ts; falta capturar un mensaje y sus bytes reales (F26_A02) antes de presentarlo como captura.

## 27 · F27 · Trama serial, parser y tabla de comandos  
*55 s · acumulado 18:35 · ramas P*

> Una UART solo entrega bytes sueltos; el significado lo pone el protocolo. Nuestra trama empieza con las letras ROBO2, sigue el largo, dos bytes fijos de control, la versión, la ruta, el comando, los datos, y termina con un XOR de todos los bytes anteriores. El receptor es una máquina de estados: avanza un estado por cada byte esperado y, si algo no coincide, se resincroniza buscando otra R. Solo cuando el XOR coincide la trama se entrega al despachador. Y la lista de comandos no está escrita a mano en tres lugares: sale de un único catálogo que leen el firmware, la Web y el Loader.

**Fuentes:** contracts/UART_PROFILE_2.md (tabla de offsets); afr2_wire.h:100-137 (parser y XOR); wire_catalog.json (59 comandos, ws, bytes).

**Pendiente / no afirmar:** Aprobación humana obligatoria del protocolo (F27).

## 28 · F28 · Eventos, estado y notificaciones  
*40 s · acumulado 19:15 · ramas P*

> Cuando llega un byte, la interrupción de la UART solo lo guarda en un buffer circular de ciento veintiocho bytes; no interpreta nada. En el lazo principal el parser arma la trama, el despachador llama al manejador del comando, y ese manejador cambia el estado del auto: el modo, una consigna, una falla. Ese cambio de estado es el que después se ve: por ejemplo, como un aviso en la pantalla, que se guarda en un arreglo de cuatro notificaciones y dura tres segundos.

**Fuentes:** afr2_uart1.c:1-40 (ISR RX/UDRE, RING_MASK 127); include/afr2_screens.h:65 (notification[4]); include/afr2_product.h:15 (AFR2_NOTICE_MS 3000).

**Pendiente / no afirmar:** El informe suponía capacidad 8: el código tiene 4. Política de reemplazo/overflow de las notificaciones: verificar en afr2_screens.c:324 antes de describirla.

## 29 · F29 · Dashboard y modelo 3D  
*40 s · acumulado 19:55 · ramas P+M*

> El mismo modelo del auto que diseñamos en Fusion es el que ve el usuario en la página. Lo pasamos a Blender para ajustar materiales y aligerarlo, lo exportamos como GLB comprimido con Draco, de unos ochocientos ochenta kilobytes, y el ESP lo sirve comprimido en gzip. En el navegador lo dibuja Three.js dentro de React. La aplicación tiene control táctil, el visor del seguidor de línea, el simulador de GOTO, las secuencias, los sensores y la pantalla del auto en vivo.

**Fuentes:** Web/public/models/catalog.json (AutoFinalRobo2.glb 880.232 B); CAD/mecanica/auto-completo/blender_ligero.py, fusion_exportar_blender.py; contracts/WEB_INFRASTRUCTURE.md.

**Pendiente / no afirmar:** Capturas tomadas el 18/11/2026 de la fuente 1.39.6 con npm run dev y Chrome headless 1920×1080, sin auto conectado: no muestran telemetría real. Para F29_A03 (indicadores ↔ sensores) falta una captura con el auto en un estado conocido. Uso de Draco: confirmar en el pipeline antes de afirmarlo.

## 30 · F30 · Flash, LittleFS, RAM y recursos 3D  
*50 s · acumulado 20:45 · ramas P*

> La memoria también es un recurso de ingeniería. En el ATmega tenemos ocho kilobytes de RAM: entre variables inicializadas y no inicializadas usamos unos cinco mil seiscientos bytes, y lo que queda es espacio para la pila, no memoria libre garantizada. En el ESP reducimos más de cuatro kilobytes de RAM estática moviendo textos a flash. Y no hay que confundir: la flash del ESP guarda la aplicación web, la RAM del ESP atiende las conexiones, y el modelo 3D se descomprime y se dibuja en la memoria del navegador, no del auto.

**Fuentes:** documentation/builds/2026-11-06-04/README.md (SRAM 5614, flash 124240, 2578 antes de pila); changes/2026-11-09-09 (ESP 51172→46644 B); build:esp (17 activos 2.487.493 B gzip; presupuesto 2.867.200). Capacidades del chip: datasheets.

**Pendiente / no afirmar:** Builds de 06/11 y 09/11: la versión actual 1.39.6 puede diferir. Antes de presentar, regenerar size/map de la versión que se cargue (el repo y el auto no están alineados).

## 31 · F31 · Sesiones, concurrencia y latencia  
*45 s · acumulado 21:30 · ramas P*

> Un auto que se mueve no puede quedar obedeciendo a un navegador que se colgó. Por eso la página avisa que sigue presente cada medio segundo, y si pasa un segundo y medio sin noticias, el auto se detiene. Cada orden de movimiento vence a los cuatrocientos milisegundos si no se renueva, y la orden de parar tiene prioridad sobre todo. Los identificadores de pedido sirven para saber qué respuesta corresponde a qué pregunta; no son una contraseña. Lo que todavía no medimos es la latencia de punta a punta.

**Fuentes:** contracts/PROTOCOL.md (alive 500 ms, 1500 ms, TTL 400 ms, STOP prioritario); BATTERY_MEASUREMENT_9.md (UART 800 ms, WS 2500 ms); afr2_product.c:322 (web_ms ≥ 1500 → Stop).

**Pendiente / no afirmar:** Política multicliente: el contrato dice «una sesión»; verificar en broker.cpp cómo se rechaza un segundo cliente antes de dibujar dos clientes.

## 32 · F32 · Workflow organizado e IA agéntica  
*75 s · acumulado 22:45 · ramas M+E+C+P*

> Organizar el proyecto no hace el trabajo solo. Lo que hace es partir un sistema complejo en módulos con responsabilidades y contratos claros: firmware del ATmega, firmware del ESP, la Web, el CAD, el Loader para cargar los firmwares, y en el centro Contracts, donde se documenta cada cambio compartido con un identificador y el estado real de cada módulo. Todo vive en un solo repositorio con una única versión de producto. Así, el caso de la batería atraviesa hardware, firmware, protocolo y React, y cada eslabón sabe con qué archivo se conecta. Una herramienta de IA puede ayudarnos a buscar una función, preparar una prueba o implementar un componente; nosotros seguimos definiendo requisitos, arquitectura, seguridad, límites físicos y criterios de aceptación. Esa reducción del trabajo repetitivo nos permite mirar más la vista macro.

**Fuentes:** AGENTS.md raíz (seis módulos, un solo Git, versión única en afr2_build_info.h y package.json); Contracts/docs/ESTRUCTURA_Y_GIT.md, VERSIONADO.md; Loader 2.5.0 (Loader/src/JobState.cs:62); fotos_existentes/FOTO-54.

**Pendiente / no afirmar:** No atribuir al Loader capacidades MCP/IA no demostradas en la versión instalada (2.5.0 en fuente; distribución en raíz 2.0.0).

## 33 · F33 · Cuatro ramas, un sistema  
*45 s · acumulado 23:30 · ramas M+E+C+P*

> Empezamos diciendo que no íbamos a recorrer materias sino un sistema. Vimos que la mecánica define cómo se mueve y cuánto aguanta, la electrónica cómo mide y actúa, el control cómo corrige lo que pasa en la realidad, y la programación cómo todo eso se coordina y se comunica. Ninguna de las cuatro alcanza sola: la mecatrónica no consiste en sumar componentes, sino en integrar conocimientos para resolver problemas reales. Muchas gracias.

**Fuentes:** Render F33_A01 (CAM_F33_HERO_FINAL_V01) desde el mismo GLB que F01.

**Pendiente / no afirmar:** Agregar QR y link al repositorio si se decide publicarlo (idea del equipo).

## 34 · A1 · Anexo · KEY y RST automáticos con NPN  
*0 s · acumulado 23:30 · ramas E+P*

> Respaldo para preguntas. El ESP entra en modo de carga bajando GPIO0 mientras se reinicia. En lugar de apretar dos botones, dos transistores NPN en paralelo con los pulsadores lo hacen comandados por el Mega.

**Fuentes:** presentacion/Filmina_12_Agregado_NPN_KEY_RST.md; Contracts/hardware/PLAN_KEY_RST_AUTOMATICO.md; FOTO-61.

**Pendiente / no afirmar:** Circuito en armado, sin mediciones.

## 35 · A2 · Anexo · AFR2 Loader  
*0 s · acumulado 23:30 · ramas P*

> Respaldo para preguntas: el Loader elige puerto y destino, compila, instala el puente en el Mega para cargar el ESP, verifica y trae un monitor serie que entiende nuestros comandos.

**Fuentes:** presentacion/Filmina_11_Agregado_Loader.md; AutoFinalRobo2-Loader/README.md, MANUAL.html; FOTO-55, FOTO-56.

