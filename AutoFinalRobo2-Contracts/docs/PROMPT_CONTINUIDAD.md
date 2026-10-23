# Prompt de continuidad: AutoFinalRobo2

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
    Enlace Mega-ESP y dueño del mando
    Alertas / datos caducados
    Volver
  Control manual
    Detener
    Tomar / liberar mando
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
