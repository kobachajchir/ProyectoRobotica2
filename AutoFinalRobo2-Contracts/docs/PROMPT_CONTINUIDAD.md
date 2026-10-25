> **AFR2-20261025-11:** [giro con dos dedos local](../contracts/WEB_TOUCH_SIMULATION.md), rumbo retenido/Q-E15°/0 centra; al entrar cancela target anterior y fija preview, sin yaw físico. Cámara10/chevrones09 conservados. Web compilada/promovida11; firmware0.9.0/EX01v7/build01/gates0 intactos. Gesto no probado en dispositivo; sin CAD o pruebas físicas.

> **Vigente AFR2-20261025-01:** leer primero [DIRECT_CONTROL_7](../contracts/DIRECT_CONTROL_7.md). El usuario elimina expresamente el permiso interno Web: no adquirir/renovar/liberar ni habilitar desde OLED. Control directo,30comandos, EX01v7/0.9.0 y Web0.2.0. Mantener STOP, watchdog, TTL y presencia automática500ms/caducidad1500ms. Formularios siguen exigiendo estado/calibración. Estados inferiores históricos.

> **Vigente AFR2-20261024-15:** [Matriz del maestro](ALINEACION_MAESTRO.md):33comandos y59identidades OLED con fuentes complementarias. Calibración Web adquiere control al escribir; ESP/Web informan FS/paquete. Web/ESP compilados15; AVR sin nueva fuente, build12/EX01v6/firmware0.8.0/gates0. Sin brecha software definida identificada tras lectura; aceptación física, memoria dinámica, flash/SPI y planos CAD pendientes. Solo documentación en CAD, sin diseño nuevo. Estados inferiores históricos.

> **Vigente AFR2-20261024-12:** mapa/comando actual juntos y coordenadas laterales; Hardware/Reinicio anidados, editor de pistas reversible y distancia por rueda EX01v6. Firmware0.8.0 y Web compilados; activos promovidos localmente. Build AVR incluye fuente IRQ11 con gates0; sin navegador, Hall habilitado, hardware, flasheo ni CAD. [Ficha12](../changes/2026-10-24-12-control-hardware-distance.md). Los estados inferiores son históricos.

> **Fuente vigente AFR2-20261024-11:** encoder CLK/DT en D2/D3 por INT4/INT5, SW D42; cola y acciones en main. Hall PCINT2 A8–A15 con preferencia de captura por software, macro0. **Esta adaptación no está compilada ni probada**; build08 corresponde a la fuente anterior. ESP/Web conservan su estado10. [Ficha11](../changes/2026-10-24-11-encoder-hall-interrupts.md). Los estados previos inferiores son históricos.

> **Continuidad AFR2-20261024-10:** GOTO local con puntos del suelo/clic, alternativa por coordenadas y lista de movimientos relativos. Trayectoria analítica Mecanum a rumbo0; comandos ejecutados en orden con activo accent/fade y scroll local interrumpible. Mapa ancho completo, coordenadas en fila, recorrido debajo y controles a la derecha; móvil apila. Usada skill animate; reduced motion/teclado sin transform. Comandos/velocidad conservados al cambiar tarea, reproductor vuelve detenido al origen. No se transmite esta lista a ATmega ni se usa como telemetría. Web compilada/promovida; firmware08/gates0/ScreenCodes59 de09 conservados. [Ficha10](../changes/2026-10-24-10-goto-map-queue.md), [contrato de mapa](../contracts/WEB_CONTROL_MAP.md), [estado](ESTADO_IMPLEMENTACION.md). Sin tests/validadores independientes/navegador/placas/flasheo/CAD/publicación. El navegador previo sigue denegado. Estados inferiores históricos.

> **Continuidad AFR2-20261024-09:** Web completa catálogo59 y copia de biblioteca de dibujo AVR; fuente/XBM/tablas extraídos durante build, algoritmos portados con hashes revisados. Ver [estado](ESTADO_IMPLEMENTACION.md), [ficha09](../changes/2026-10-24-09-linked-oled-library.md) y [build09](../documentation/builds/2026-10-24-09/README.md). La skill afr2-screen-authoring obliga a crear/modificar también la contraparte Web en cada pantalla y registrar ScreenCode/renderer/recursos compartidos en la misma tarea. Mantener permiso expreso para compilaciones/validadores. Visor conectado solo framebuffer; preview local separado. Firmware conserva08 y gates0; no hardware probado. AFR2_OLED_ADDRESS7=0x3c de7bits en Atmega/src/main.c, dirección física no verificada. Sin CAD, tests, render, flasheos ni publicación. Estados anteriores históricos.

> **Continuidad AFR2-20261024-08:** [estado por módulo](ESTADO_IMPLEMENTACION.md) y [EX01v5](../contracts/EX01_NETWORK_UI_5.md). Firmware0.7.0,48nodos,11avisos, IP/AP local, iconos/scroll y Sistema → Reinicio compilados; Web actualizada/promovida. Clave AP excluida de HTTP/WS/captura protegida; clave STA oculta. SRAM estática4797bytes no acredita pila; memoria/Hall0 se conservan. No pruebas físicas/flash ni nuevo acceso de navegador. CAD/pinout intactos. Estados inferiores históricos.

> **Continuidad vigente AFR2-20261024-07:** leer primero [estado por Web/ESP/ATmega](ESTADO_IMPLEMENTACION.md), [ficha07](../changes/2026-10-24-07-control-system-sequence.md) y [EX01v4](../contracts/EX01_SYSTEM_SEQUENCE_4.md). Control/editor homogeneizados, suelo por vértices precisos, mapa solo simulado, conexión automática al mismo host, Sistema con info/boot/reinicio y Desarrollador por triple toque. Nueva pantalla OLED0x03002c conserva SW/Volver y todo MENU_PLAN. Los tres módulos compilan, sin habilitar memoria/Hall/calibración; no hardware probado. Navegador denegó localhost, sin elusión ni render. No repetir esa vía sin nueva autorización. Layout3MiB candidato, CAD intacto. Estados inferiores históricos.

> Actualizacion **AFR2-20261024-06**: suelo en la base del modelo y GLB final compartido por inspeccion y Control (Superior ortografica, Perspectiva y Seguir auto). Escala1unidad=1m; cuadricula0,5m, sin agrandar la malla. Web compilada/promovida: **1.450.617 bytes gzip**,7 activos. Sin render visual, pruebas fisicas ni flasheo. [Detalle](../changes/2026-10-24-06-grounded-model-control.md).

> Correccion **AFR2-20261024-05**: Reintentar carga 3D elimina el error cacheado por useLoader; publicDir absoluto y copia selectiva de build conservada. Web compilada y promovida: **1.450.338 bytes gzip**. Diagnostico HTTP local: GLB/Draco/WebP200 con contenido correcto; sin renderizado visual ni flasheo. Modelo final y layout candidato3MiB de04 conservados. [Detalle](../changes/2026-10-24-05-model-loading-retry.md).

> Vigente **AFR2-20261024-04**: **modelo final incluido en ESP**, paquete **1.450.201 bytes gzip** (49,96% menos que los 2,90 MB iniciales). WebP, JS/CSS minificados y dependencias/componentes optimizados. Layout candidato: flash 4 MiB / LittleFS nominal 3 MiB, solicitado por el usuario; flash fisica sin identificar. Web full/esp y firmware ESP compilados; activos promovidos a data/dist. Sin navegador, imagen FS ni flasheo. Atmega y comandos conservan el estado de02. [Evidencia](../changes/2026-10-24-04-final-model-littlefs.md). Los estados previos inferiores son historicos.

> Estado vigente **AFR2-20261024-02**, 24/10/2026: integración ROBO2/perfil2 + EX01v3 compilada en Atmega/ESP/Web; GOTO completo, configuración por catálogo compartido y jobs Wi-Fi OLED conectados. Paquete Web actualizado en ESP/data/dist. Sin pruebas de navegador/hardware ni flasheo; secuencias físicas mantienen bloqueo de memoria. Ver [matriz vigente](../contracts/EX01_ALIGNMENT_3.md) y [evidencia](../changes/2026-10-24-02-project-alignment.md). Los estados previos que siguen son históricos.

# Prompt de continuidad: AutoFinalRobo2


Actualización ATmega19,24/10: fuente de producto y documentación completadas,
sin build/tests/validadores/hardware. Leer [matriz](PLAN_ATMEGA.md) y [detalle vigente](ATMEGA_IMPLEMENTACION_19.md).
Hall sin conectar y calibración/aceptación pendientes. EX01 requiere complemento
ESP/Web; perfil3/secuencias de21 es diseño separado. Las menciones previas a
menús/sensores/control pendientes de fuente son históricas.

Estado vigente AFR2-20261023-15: ROBO2/perfil2 sustituye UNER/perfil1 en fuente
de ambos extremos; framing único compartido, ruta alto=emisor/bajo=receptor.
AVR integra UART1/Timer0/TWI/I2C Manager/OLED/encoder en main.c. Backend ya
escrito; faltan compilación, recursos reales, tiempos y hardware. Motores,
sensores, STOP autoritativo, lease, Hall/GOTO, UI remota y menús completos siguen
pendientes. No se ejecutaron builds/tests/validadores/flasheos. El texto anterior
se conserva como evolución del proyecto. Leer [ficha15](../changes/2026-10-23-15-robo2-atmega-ui.md),
[perfil2](../contracts/UART_PROFILE_2.md), [criterios comunes](../contracts/ROBO2_VALIDACION.md)
y **[ESP/docs/PROTOCOLO_ROBO2.md](../../AutoFinalRobo2-ESP/docs/PROTOCOLO_ROBO2.md)**
para retomar firmware ESP sin duplicar lo ya escrito.


Contexto consolidado al 23/10/2026, incluida la evidencia del shield OSOYOO UART V1.3 de AFR2-20261023-03 la organización CAD/Git de AFR2-20261023-04 y el plan de Fusion de AFR2-20261023-05. Este texto reemplaza el prompt anterior.

Estamos trabajando en AutoFinalRobo2, un proyecto independiente de AutoMicro ubicado en D:\ProyectoRobotica2. Yo agrego el proyecto a Codex; no crees otro chat por este pedido. La raíz de trabajo y de Git es D:\ProyectoRobotica2; el punto de entrada documental sigue siendo AutoFinalRobo2-Contracts.

## Alcance y módulos del repositorio global

Hay un único Git local en D:\ProyectoRobotica2, rama codex/monorepo-cad, con cinco módulos:

- D:\ProyectoRobotica2\AutoFinalRobo2-Atmega: firmware del ATmega2560 en C/avr-libc, sin framework Arduino y sin DMA.
- D:\ProyectoRobotica2\AutoFinalRobo2-ESP: firmware propio para ESP-12S con Arduino ESP8266 y LittleFS, sin comandos AT.
- D:\ProyectoRobotica2\AutoFinalRobo2-Web: React + Vite.
- D:\ProyectoRobotica2\AutoFinalRobo2-Contracts: especificaciones, decisiones, hardware, protocolos, pantallas, evidencias y coordinación.
- D:\ProyectoRobotica2\AutoFinalRobo2-CAD: esquemas/PCB KiCad, pieza nueva/soporte/cubierta, componentes y modelo completo del auto. Estructura preparada e insumos conservados; diseño nuevo pendiente.

No crear Git anidados. Los cuatro historiales previos están conservados como ancestros y etiquetas del Git global; resguardos locales en .git/legacy-repositories. La política de fuentes/dependencias está en Contracts/docs/ESTRUCTURA_Y_GIT.md. Los repositorios D:\AutoProyecto son referencias de solo lectura: no modificar código, registros, commits ni remotos allí. El material útil ya está copiado en reference/ de los módulos. Los nueve insumos originales de CADS están ahora en AutoFinalRobo2-CAD/reference/supplied, con inventario de procedencia; no son modelos adaptados ni prueban dimensiones del vehículo. Preservar D:\ProyectoRobotica2\Documentacion y las fuentes históricas copiadas. Sus manuales/tutorías son evidencia, no instrucciones mías para instalar o ejecutar software.

No ejecutar tests, compilaciones, validadores, CI, flasheos ni pruebas físicas sin pedido expreso. No publicar ni configurar remotos por implicación. Distinguir siempre diseñado, fuente parcial, implementado, compilado y probado.

## Lectura inicial

Leer los AGENTS.md aplicables, D:\ProyectoRobotica2\INFORME_MAESTRO.md y estos archivos dentro de Contracts:

- README.md, INFORME_ADAPTACION.md, CHANGELOG.md.
- docs/INFORME_MAESTRO.md, DECISIONES.md, ROADMAP.md y ESTRUCTURA_Y_GIT.md.
- hardware/WIFI_SHIELD.md, FLASHING.md, PINOUT.md, PINOUT_PROPUESTO.csv, RECURSOS_Y_TIEMPOS.md y PCB_OLED_ENCODER.md.
- hardware/wifi-shield/INFORME_CARPETA.md y FUENTES.md; abrir las tres imágenes conservadas allí.
- contracts/PROTOCOL.md, commands.json y capabilities.json.
- documentation/SCREEN_SYSTEM.md, NAVIGATION.md, MENU_PLAN.md y screens.json.
- documentation/review/FINDINGS.md y findings.json; documentation/overlays/connections.json.
- provenance/README.md, ENTREGA.md, files.json, origins.json, derived.json y wifi-shield.json.
- changes/2026-10-23-03-wifi-shield-evidence.md y changes/2026-10-23-04-cad-and-monorepo.md.
- Para CAD: ../AutoFinalRobo2-CAD/README.md, AGENTS.md, INFORME_ADAPTACION.md, docs/ALCANCE_Y_PENDIENTES.md y reference/insumos-locales.json.

Consultar los informes de los repositorios y módulos afectados antes de implementar. Si el estado local avanzó desde este prompt, informar la diferencia y trabajar con la evidencia vigente sin sobrescribir cambios.

## Arquitectura y estado real

React → WebSocket/HTTP → ESP-12S → UART/contrato propio candidato UNER → ATmega2560. El Mega es autoridad de motores, sensores, control, odometría, seguridad y UI local. ESP administra Wi-Fi, servidor, archivos Web en LittleFS, traducción y correlación. Web ofrece control, GOTO, pruebas, sensores y configuración Wi-Fi.

El robot usa cuatro motores Mecanum, HC-SR04 y cinco sensores de línea; se añadirá OLED I2C y encoder de interfaz. No hay MPU/IMU ni balance de dos ruedas. GOTO deberá reformularse como movimiento relativo apoyado en odometría de ruedas cuando esté disponible y calibrada. El encoder CLK/DT/SW del usuario no mide desplazamiento del vehículo. No presentar Hall de ruedas como instalados por existir documentación de motores.

ATmega trabaja con máquinas de estados y tiempos acotados, interrupciones breves, buffers/colas y presupuesto de SRAM/flash/pila a 16 MHz. I2C Manager es obligatorio; OLED es el único cliente I2C. Desactivar el define DMA heredado no basta: la rama bloqueante F4 tampoco es el backend deseado. Fuentes/iconos/tablas deben tratarse apropiadamente en PROGMEM.

La base Atmega contiene fuentes parciales de I2C Manager, OLED por páginas, Font7x10, encoder y wrappers de cuatro vistas: Inicio, Menú principal reducido, Sensores y Estado OLED. No hay firmware completo ni backend AVR TWI/UART integrado. ESP y Web son esqueletos independientes con código anterior copiado para migración. No atribuirles todavía servidor, bridge, navegación completa o control del auto funcionales.

## Shield Wi-Fi: evidencia que corrige los supuestos anteriores

El shield de referencia es OSOYOO UART WiFi Shield V1.3 con ESP-12S. Las imágenes aportadas son mapas/fotos de producto, no una comprobación de los jumpers montados en nuestro robot.

OSOYOO declara regulación a 3,3 V y adaptación de niveles integradas para placas de 3,3/5 V. Separar el selector ESP URAT, con E_RX/E_TX, del conector directo ESP UART, rotulado GND/ESP TX/ESP RX/3V3. El puerto directo se trata como lógica de 3,3 V: la compatibilidad del shield completo no permite aplicar 5 V a cualquier pin. Falta identificar el recorrido real del adaptador por conector; el Fritzing disponible representa la pieza y no acredita un esquema interno completo.

Para operación, conservar lo indicado en el manual del robot:

- Mega TX1 D18/PD3 → E_RX del selector.
- E_TX del selector → Mega RX1 D19/PD2.
- Masa común. Retirar/aislar jumpers hacia D4/D5 y D0/D1 cuando se cablee USART1; Bluetooth queda desconectado.
- El ESP utiliza UART0 GPIO1/TX y GPIO3/RX; USART1 corresponde al Mega.
- No adoptar SoftwareSerial D4/D5: D5 ya es dirección del motor RR.

El mapa ampliado OSOYOO usa numeración/puertos de UNO. No copiar SDA=A4/D18 ni SCL=A5/D19 a Mega. Usar las cabeceras SDA/SCL que ya expone el shield; en Mega equivalen a D20/PD1 y D21/PD0. A4 sigue asignado a IR1. La continuidad de la revisión física todavía no se ha medido.

Para aprovechar el USB del Mega al programar ESP, mantener Mega en reset no conecta USART0 con USART1. El puente USB de Mega Rev3 transmite por el nodo D0/RX0 y recibe por D1/TX0. La ruta prevista es D0 → RX ESP y TX ESP → D1 con niveles compatibles y aislamiento del resto. Los jumpers normales del selector unen E_RX-D1 y E_TX-D0; esa posición no sirve directamente como bypass USB y puede enfrentar salidas. Seguir hardware/FLASHING.md; no inventar posiciones definitivas sin identificar la placa.

KEY y RST son mantenimiento del ESP, no botones de navegación. Se prevé arranque manual manteniendo KEY/GPIO0 bajo durante reset ESP, después de confirmar esa conexión. No asumir auto-reset DTR/RTS. Para volver a cargar Mega, quitar su reset forzado y aislar ESP de D0/D1; operación posterior nuevamente por D18/D19. No ejecutar estas acciones por recibir este contexto.

El AT de fábrica se reemplazará por firmware propio Arduino + LittleFS. Los binarios AT, herramientas antiguas y guías RTOS/FOTA aportados son referencias históricas, no dependencias del producto. No implementar AT ni WiFiEsp.

La página pone “9600 kbps”, registrada como errata probable de unidad: 9600 bit/s 8N1 sería el valor de fábrica esperado, no una medición. La ficha genérica ESP-12S indica otro default. Mantener 115200 8N1 como candidato propio, sujeto al margen de baud AVR; distinguir velocidad de aplicación, upload y mensajes ROM.

La ficha oficial ESP-12S V1.1 indica flash nominal de 32 Mbit, equivalentes a 4 MiB. No es identificación de nuestra unidad: existe también variante ESP-12S/ESP8285 de 2 MB en el catálogo. Confirmar chip/flash, modo SPI, frecuencia y reparto firmware/LittleFS antes de fijarlos. `esp12e` en PlatformIO es candidato. DOUT en el nombre del antiguo firmware AT no acredita el modo necesario para el nuevo firmware. No importar offsets AT ni linker ESP01. Definir una política que no formatee LittleFS silenciosamente al fallar un montaje.

La regulación del shield no fija VCC del OLED/encoder. VIN/VOUT no se consideran salidas reguladas de 5/3,3 V para la PCB nueva; identificar alimentación, módulos y pull-ups.

## OLED, wrappers y navegación

Para nuevas pantallas o adaptación, leer y usar la skill local:
D:\ProyectoRobotica2\AutoFinalRobo2-Contracts\.agents\skills\afr2-screen-authoring\SKILL.md

Conservar la separación snapshot/estado → wrapper/adaptador → renderer → servicio OLED por páginas → I2C Manager → backend TWI AVR. Registrar por pantalla identidad, rol vista/acción/invocador, estado disponible, acciones, renderer y evidencias. Copiar un SVG/builder Web no acredita fidelidad del OLED.

Las únicas entradas de navegación son CLK, DT y SW:

- Giro: seleccionar o ajustar un borrador.
- Clic corto: activar/confirmar.
- Clic largo SW: abrir menú principal desde cualquier vista; al soltar no genera clic corto.
- Atrás: exclusivamente el ítem Volver del submenú/detalle.
- No existe botón menú separado; no usar largo o doble clic para atrás.
- KEY/RST del shield no participan en esta navegación.

Mantener selección visible con desplazamiento de ventana; selectedIndex y firstVisible explícitos. Editar en borrador, aplicar/guardar con acción explícita. Abrir menú o Volver no guarda, arranca ni reanuda movimientos. La política al abandonar una operación activa debe definirse antes de habilitarla. STOP, lease y timeout son independientes de la navegación; las maniobras locales deben estar acotadas.

## Menús objetivo, pendientes de implementar

```text
Inicio / dashboard
Menú principal (SW largo)
  Estado
    Resumen del robot
    Enlace Mega-ESP y responsable del control
    Alertas / datos caducados
    Volver
  Control manual
    Detener
    Prioridad local / liberar control
    Límite de velocidad
    Movimiento Mecanum
      Adelante / atrás
      Lateral izquierda / derecha
      Diagonales
      Giro izquierda / derecha
      Detener
      Volver
    Volver
  Modos y misiones
    Reposo / detenido
    Seguimiento de línea
      Estado y requisitos
      Velocidad / parámetros permitidos
      Iniciar / detener
      Volver
    GOTO relativo
      Estado de odometría / requisitos
      Objetivo X / Y
      Velocidad / límites
      Revisar objetivo
      Iniciar / cancelar / resultado
      Volver
    Volver
  Pruebas
    Motor individual FL / FR / RL / RR
      Selección / sentido / PWM acotado / duración
      Ejecutar / detener / resultado
      Volver
    Hall de ruedas: cuentas y sentido
    Cinco sensores de línea
    HC-SR04
    OLED e I2C
    Encoder UI: giro, corto y largo
    UART Mega-ESP
    Volver
  Sensores
    Línea IR1..IR5
    Distancia HC-SR04 y validez
    Hall / velocidades de ruedas (si están conectados)
    Volver
  Calibración
    Línea: polaridad y parámetros disponibles
    Motores: canal, signo y perfil
    Hall: cuentas por vuelta y sentido
    Geometría Mecanum / odometría
    Encoder UI: sentido y pasos
    Guardar / descartar cambios
    Volver
  Wi-Fi
    Estado STA / AP / IP
    Modo STA / AP / combinado, según soporte
    Buscar redes / seleccionar
    Configurar conexión
    Reintentar conexión
    Punto de acceso / fallback
    Aplicar / resultado
    Volver
  Pantalla e interfaz
    Contraste / orientación
    Estado OLED / I2C
    Parámetros del encoder
    Volver
  Sistema
    Versiones Mega / ESP / Web
    Capacidades disponibles
    Preferencias y guardado
    Diagnóstico / errores
    Volver
  Volver a Inicio
```

Todo detalle/submenú tendrá Volver al padre. El menú principal tendrá Volver a Inicio. No asignar por este prompt IDs wire a todas las entradas ni declarar funciones listas. Conservar los cuatro IDs locales existentes; ampliar registros al implementar. Sin hardware/servicio requerido, indicar no disponible; lecturas ausentes/caducadas son unknown/stale, no ceros ficticios. GOTO requiere odometría válida. Wi-Fi se gestiona en ESP sin AT; ningún ítem de menú conmuta jumpers o reemplaza el procedimiento de programación.

## PCB OLED/encoder y montaje posterior

La placa nueva tendrá OLED y encoder, con dos conectores externos Dupont de paso 2,54 mm:

- J1 1x5: CLK→D40/PG1, DT→D41/PG0, SW→D42/PL7, SDA→cabecera SDA del shield (Mega D20/PD1), SCL→cabecera SCL (Mega D21/PD0).
- J2 1x2: VCC y GND comunes a OLED y encoder, masa común con el robot.

El orden descrito es propuesta; confirmar pin1/orientación al diseñar. VCC común está acordado, su tensión numérica sigue pendiente de los módulos y pull-ups. SW se plantea activo bajo. No convertir el puerto UART 3V3 del ESP en entrada del bus I2C.

Más adelante entregaré planos de la cubierta superior, informe/idea y la skill de ruteo. Entonces prepararemos esquema y PCB KiCad editables, plano acotado y soporte 3D paramétrico, fijados con DOS tornillos y tuercas que sujeten placa, soporte y cubierta. Todavía no se eligieron dimensiones, posiciones, tornillería, huellas ni altura. La ubicación ya está creada: AutoFinalRobo2-CAD, dentro del Git global, con kicad/oled-encoder, mecanica/pieza-nueva y mecanica/auto-completo. El plan de Fusion en ../AutoFinalRobo2-CAD/docs/PLAN_MODELO_3D_FUSION.md define el ensamble futuro del auto base sin cables, con CAD locales/descargados e inferencias explícitas. Pieza nueva/soporte/PCB siguen pendientes de planos; reservar espacio y preparar referencias OLED 0,96 pulgadas y encoder mecánico KY-040 sin breakout/perilla. La entrega actual solo guarda/publica el plan; no inicia CAD. Mantener manifests/lockfiles, bibliotecas propias y fuentes editables en Git; dependencias, descargas recuperables, caches y exportaciones regenerables quedan excluidas.

## Forma de continuar

Los cambios compartidos se registran solo en Contracts con ID AFR2-YYYYMMDD-NN y estado real de atmega/esp/web. Mantener informes, Markdown/JSON, hallazgos persistentes e historial/overlays relacionados; no cerrar una brecha por haber escrito documentación. Sin secretos en código, logs o prompts.

En tu primera respuesta, después de leer, resumí el estado real, las decisiones firmes y los pendientes de la siguiente etapa. Identificá qué evidencia pertenece al shield y cuál al módulo/placa Mega. No implementes todos los subsistemas ni inicies CAD, compilaciones o flasheos como consecuencia de este contexto. Esperá mi siguiente tarea concreta para avanzar.

## Fuentes primarias de esta actualización

- [Shield OSOYOO UART V1.3](https://osoyoo.com/2020/07/31/introduction-of-osoyoo-uart-wifi-shield/).
- [Índice Ai-Thinker ESP8266](https://docs.ai-thinker.com/en/esp8266/) y [ficha ESP-12S V1.1](https://aithinker-static.oss-cn-shenzhen.aliyuncs.com/docs/Public%20Document%20Center/WiFi/esp8266/Specification/ESP-12S%20Specification-20220807.pdf).
- [Pinout oficial Mega](https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf) y [esquema Mega Rev3](https://www.arduino.cc/en/uploads/Main/arduino-mega2560_R3-schematic.pdf).
- [Arranque ESP8266](https://docs.espressif.com/projects/esptool/en/latest/esp8266/advanced-topics/boot-mode-selection.html).
- [LittleFS/Core Arduino ESP8266](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html).

Las fuentes completas y sus límites están en hardware/wifi-shield/FUENTES.md y provenance/wifi-shield.json; no hace falta volver a descargarlas para iniciar la lectura local.

## Retoma del modelo 3D / AFR2-20261023-05

Leer ../AutoFinalRobo2-CAD/docs/PLAN_MODELO_3D_FUSION.md antes de continuar. Esa especificación incorpora las decisiones del usuario para ensamble base, búsqueda Web, correspondencia de CAD, inferencias y ausencia de cables. La pieza nueva/PCB quedan pendientes; OLED 0,96 pulgadas y encoder KY-040 sin breakout/perilla son referencias de la etapa futura. Usar el MCP de Fusion existente en la PC de trabajo. La entrega actual documenta/publica el plan; no implementa el modelo.

Repositorio de continuidad: https://github.com/kobachajchir/ProyectoRobotica2, privado, origin HTTPS y rama remota main. La publicación fue pedida explícitamente por el usuario para esta entrega; no supone permiso general para nuevos remotos/publicaciones. Un clone nuevo usa main; la rama local original codex/monorepo-cad sigue origin/main. El plan se conserva en el repo y el modelado comienza como tarea posterior en la otra PC.

## Continuación ESP/AVR — AFR2-20261023-11

Leer docs/PLAN_ESP.md, changes/2026-10-23-11-esp-server-and-mega-link.md y contracts/UART_PROFILE_1.md antes de continuar firmware. La ESP tiene HTTP :80, /ws, LittleFS sin autoformat, preferencias y Wi-Fi STA/AP/AP_STA escritos; UART usa sesión/transacción y catálogo mínimo. AVR tiene endpoint C, USART1 IRQ y composición de comunicaciones con Timer0 1 ms/potencia inactiva. No se implementaron productores de movimiento/sensores, lease, GOTO ni UI remota; STOP sin callback devuelve unavailable y nunca confirmación física.

No hubo build, tests, validadores, imagen FS, puertos serie ni flasheo en esta ficha. Identificar flash/SPI y definir layout común antes de preparar imágenes: data/dist conserva el paquete AFR2-20261023-09 (2.925.405 bytes gzip, más overhead), y la Web fuente tiene modificaciones posteriores AFR2-20261023-10/11 pendientes de reconstruir/promover. Preservar preferencias antes de una carga FS completa. La referencia ESP01 y AutoMicro permanecen de solo lectura; las adaptaciones están en provenance/esp-firmware.json. El número 10 corresponde al refactor Web y el 11 a esta implementación de firmware.

## Diagnóstico preparado — AFR2-20261023-12

Antes de elegir el layout ESP, leer hardware/ESP_DIAGNOSTICO.md. tools/esp-probe.ps1 ejecuta tools/esp_probe.py: conexión primero, ID/tamaño flash y ocho bytes de cabecera después; -CheckOnly solo conecta. Ejecutar cuando el usuario tenga la ESP cableada en modo de servicio/cargador según FLASHING.md. No se ejecutó al crear la herramienta ni se obtuvo todavía evidencia del dispositivo. Cada intento guarda hardware/diagnostics/<fecha-hora-id>/report.md, report.json y logs, incluidos fallos. Al retomar aquí, leer el informe del intento correspondiente. No inferir partición desde flash_id/cabecera: offsets LittleFS siguen pendientes de linker/mapa de un firmware conocido. No hay escrituras de flash, borrados ni formateos; la lectura final usa un stub RAM temporal.

AFR2-20261023-13: el entorno ya se comprobó con el lanzador -CheckTools: Python PlatformIO 3.11.7, pyserial 3.5 y esptool 4.5.1 disponibles; CLI --help exit 0. El script resuelve el paquete esptool instalado por PlatformIO sin instalar ni cambiar PATH persistente. Evidencia en hardware/diagnostics/20261023-155540-554529-6488/report.md. Solo aparecen puertos Bluetooth: todavía no se abrió puerto ni consultó ESP/flash. No confundir tools-ok-no-device-opened con conexión a la placa.

## Compilaciones corregidas — AFR2-20261023-15

ESP y Web compiladas por pedido expreso: ESP SUCCESS con cero warnings finales, 39.980 bytes RAM estática / 385.481 bytes de programa del perfil candidato; Web TypeScript/Vite/postbuild OK, seis assets gzip cotejados (2.925.630 bytes). Corregidos memset de objetos C++ y EPERM Windows en publicación del paquete. Ver changes/2026-10-23-15-esp-web-build-fixes.md y documentation/builds/2026-10-23-15-esp-web/results.json. Paquete local Web nuevo, ESP/data/dist anterior; no buildfs ni upload ni puertos. ATmega no tocado/compilado en esta ficha; esperar documentación del nuevo protocolo que entregará el usuario. No asumir interoperabilidad ni elegir partición por un build candidato.

AFR2-20261023-17: el usuario entregó ESP/docs/PROTOCOLO_ROBO2.md. Se leyó con UART_PROFILE_2.md/ROBO2_VALIDACION.md; el enlace/header ROBO2 ya estaban implementados por el trabajo paralelo. Revisión sin cambios de código y recompilación ESP SUCCESS, sin warnings/errors: RAM 40.096 bytes, programa 386.189 bytes del perfil candidato. Evidencia documentation/builds/2026-10-23-17-robo2-esp/. No ejecutar pruebas de matriz ni atribuir interoperabilidad por ese build. Sin compilación AVR/Web ni hardware en esta ficha. Las referencias previas a esperar la guía o usar perfil1 quedan históricas; el contrato vigente es perfil2 ROBO2.


## Continuación independiente del enlace — AFR2-20261023-20

Por pedido del usuario no editar enlace Mega/ROBO2, broker ni fuentes AVR mientras se desarrollan en paralelo. Se completó Wi-Fi ESP/Web: scan terminal con timeout/error/red vacía/truncado; status automático visible/WS listo sin Mega; persistencia RAM/flash seleccionable; validación UTF-8/SSID exacto; ESP reporta aplicación/fallback/estado STA. Builds ESP/Web exit 0 (RAM estática ESP40.272 B; programa386.797 B del perfil candidato); paquete Web local6assets/2.855.479 B gzip, sin promover a ESP/data/dist ni flashear. No prueba radio/renderizado. Procedencia por archivo provenance/wifi-esp-web.json y ficha changes/2026-10-23-20-wifi-esp-web.md. Pendientes no Mega en docs/PLAN_ESP.md: diagnóstico flash/layout, radio/HTTP en dispositivo, diagnóstico FS en Web, robustecer promoción y luego imagen FS con layout conocido.



