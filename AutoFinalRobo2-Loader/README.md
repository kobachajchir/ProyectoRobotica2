> **Loader 2.4.0 / AFR2-20261108-13:** botón **Reinicios (n)** en el Monitor serie: vista propia con razón del reset, sesión nueva (HELLO), enlace perdido antes, excepción, epc1 y línea de tiempo. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-11-08-13-reinicios-ir-radar.md).

> **Loader 2.3.0 / AFR2-20261108-11:** pestaña **Arranques ESP / ATmega** en el intérprete: una fila por arranque de cada chip, siempre activa con el puerto abierto, detalle con el log de antes y después, Copiar detalle y aviso en Eventos. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-11-08-11-loader-arranques.md).

> **Loader 2.2.0 / AFR2-20261108-09:** depurador USB en Monitor serie. Botón **Ver intérprete ↔** (alterna con Enviar), **Intérprete: desactivado** por defecto y habilitado solo con un catálogo válido (`WireCatalog`, por defecto `Contracts/contracts/wire_catalog.json`), y **Fuente USB**: log del ATmega, RX ATmega, RX ESP, enlace completo o bytes crudos. Cambiar de fuente envía `#AFR2 TAP n` por acción del usuario, espera `TAP mode=n`, limpia el log y conserva el catálogo. Pestaña **Resets ESP** con razón, excepción y epc1. Requiere el firmware ATmega con `AFR2_DEBUG_TAP=1`. **2.2.1 / AFR2-20261108-10:** abrir a 115200; enlace completo cambia solo a 250000 (sin reabrir el COM) y vuelve al salir o si el ATmega deja de responder 7 s. Fuente sin compilar ni probar; [manual](MANUAL.html#interprete), [ficha](../AutoFinalRobo2-Contracts/changes/2026-11-08-09-loader-interprete-tap.md).

> **Loader 2.1.0 / AFR2-20261108-08:** el wrapper del puente le pasa al flasher ATmega el título de la operación, el nombre de cada imagen por offset y si esptool terminó con o sin error, para su OLED. Solo si el flasher instalado lo anuncia (bit4 de la respuesta); no cambia esptool ni KEY/RST. Requiere el flasher de la versión de producto 1.3.0. Fuente sin compilar.

> **Loader 2.0.0 / AFR2-20261106-33:** versión mayor por acumulación (MCP, monitor serie y envío manual, Web/AT, flasher ATmega con KEY/RST automático, backups, tarjeta lateral Preparación·FileSystem, reintento de SYNC por el puente). Sin funciones nuevas fuera de lo ya registrado; el flasheo real con hardware sigue sin comprobarse. Compilar con `Compilar-Loader-2.cmd`.

> **Loader 1.2.0 / AFR2-20261106-03:** distribución compartible actualizada, referencias {App}/módulos, rutas relativas a la aplicación, herramientas configurables y datos propios de cada usuario. Retirada la pestaña ATmega redundante; Flasher conserva el acceso al firmware Mega. [Portabilidad y requisitos](PORTABILIDAD.md).

> **AFR2-20261106-02:** regeneración y builds AVR/ESP/Web/Loader comprobados; paridad host OLED 1680/1680. **Carga bloqueada por presupuesto SRAM/pila AVR**; sin COM, flasheo ni prueba física. [Evidencia](../AutoFinalRobo2-Contracts/documentation/builds/2026-11-06-02/README.md).

> **AFR2-20261029-22:** solución a «running scripts is disabled» al preparar FileSystem: [ajustar argumentos PowerShell del perfil](FILESYSTEM_SCRIPTS.md). Predeterminado corregido solo en fuente; sin compilar ni ejecutar. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-29-22-loader-powershell-filesystem.md).

> **Loader 1.1.2 / AFR2-20261029-13:** intervención KEY/RST del ESP obligatoria y al frente, aun minimizado o con omisión de confirmaciones. ATmega sin ese requisito. [Ayuda](MANUAL.html#esp-intervencion) · [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-29-13-loader-esp-physical-required.md).

> **Loader 1.1.1 / AFR2-20261029-12:** omisión opcional de confirmaciones manuales durante la sesión, debajo de Autoaceptar MCP. GUI y MCP requieren aceptar localmente la advertencia antes de activar. No se persiste ni omite comprobaciones automáticas. [Ayuda](MANUAL.html#mcp-session-checks) · [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-29-12-loader-session-checks.md).

> **Distribución para compartir — AFR2-Loader-dist-1.1:** carpeta y ZIP en la raíz del proyecto, con GUI/MCP precompilados, manual y scripts de runtime. Generador: Preparar-Distribucion.ps1. No incluye fuentes ni datos personales. [Ficha y límites](../AutoFinalRobo2-Contracts/changes/2026-10-29-06-loader-distribution.md).

> **Bienvenida 1.1.0 / AFR2-20261029-04:** icono animado durante la preparación de la UI, salida al estar lista y Esc para omitir. Respeta animaciones reducidas de Windows. Compilado, sin ejecutar GUI ni hardware. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-29-04-loader-startup-animation.md).

> **Inicio para usuarios:** abrí **AFR2 Loader.exe**, con el icono de la app, en esta carpeta. Ejecuta Iniciar.ps1 y conserva sus opciones de inicio. Mantené el EXE junto a los scripts, src, assets y manual. Para regenerarlo: Generar-Ejecutable.ps1. Versión 1.1.0; lanzador y aplicación compilados por pedido expreso, sin ejecutar GUI ni hardware. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-29-03-loader-executable-launcher.md).

> **Loader 1.1.0 / AFR2-20261029-01:** el contenido de LittleFS se prepara mediante scripts configurables. Nuestra Web usa scripts/Prepare-Web.ps1; otros proyectos pueden usar su propio script sin npm. [Guía y ejemplos](FILESYSTEM_SCRIPTS.md) · [Estado y archivos](../AutoFinalRobo2-Contracts/changes/2026-10-29-01-loader-filesystem-scripts.md). Sin compilar ni ejecutar esta revisión.

> **Loader 1.0.0 / AFR2-20261028-25:** lectura ESP documentada, consulta automática de capacidad por esptool y navegación reorganizada. Fuente sin compilar ni probar. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-28-25-loader-version-1.md).

> **Loader AFR2-20261028-23 / 0.9.5:** preparación previa por sección, FileSystem/capacidad confirmada, verificación ESP integrada sin reconexión adicional, aviso KEY al primer porcentaje, prompts de Servidor MCP y manual/cableado/UNER renovados. Interfaz compilada y revisada visualmente; sin COM, flash ni compilación de firmware por el agente. [Ficha y límites](../AutoFinalRobo2-Contracts/changes/2026-10-28-23-loader-flujo-manual-mcp.md).

## Flujo actualizado

Al iniciar una operación se abre Operación y se reactiva Seguir registro. Instalar el flasher exige D18/D19 desconectados del ESP; se reconectan después, con alimentación cortada, para usar el puente. Detectar ESP tiene su etapa ESP Detectado.

Confirmá la capacidad total ESP antes de ver particiones. Firmware propio ofrece Firmware ESP / FileSystem / ambos; AT oculta la sección y requiere revisar sus BIN precompilados. Flashear solo se habilita tras compilar/preparar esa sección en la sesión actual. Cambiar perfil, fuentes, linker o imágenes requiere prepararla otra vez. Código usa el límite real del linker ESP8266, sin inventar que toda flash libre sea ejecutable.

Esptool escribe y verifica hashes en la misma sesión. Se eliminan la reconexión read_mac, el bucle KEY/RST posterior y verify_flash adicional. Se exige confirmación de hash por imagen y salida correcta; si falta, no se marca verificado. El primer porcentaje de escritura dispara una ventana no modal para soltar KEY; Connecting solo no la dispara.

Servidor MCP genera prompt genérico de configuración stdio para cualquier IA compatible y prompt de prueba de conexión específico de esta sesión para Codex. Copiar del monitor habilita Mostrar en conversor HEX. Leer ATmega abre el manual exacto; no abre COM. El manual y Acerca de incluyen propósito académico, logo AFR2 y recursos UNER/FCAL.

Las secciones históricas inferiores quedan sustituidas donde contradigan este flujo.

> **Revisión visual AFR2-20261028-17 / 0.9.5:** botón físico KEY; botones y desplegables con fondo visible, iconos separados y alineados, etapas por operación sin scroll, Flasher junto a Operación y monitor/conversor reorganizados. Compilado y revisado en GUI con capturas; sin abrir COM ni ejecutar operaciones de firmware. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-28-17-loader-095-revision-visual.md).

> **AFR2-20261028-16 / 0.9.5:** interfaz organizada por destino ATmega/ESP y Configuración por placa/subsección. Flasher junto a Operación, Preferencias en Opciones, ayuda por campo, colores/iconos y márgenes comunes. Monitor con envío explícito ASCII/HEX/números, CR/LF, cantidad y pausa de repetición, Detener, y archivos TX independientes. Conserva feedback, conversor y BIN AT reconocido. **Fuentes sin compilar, ejecutar ni probar.** [Ficha completa](../AutoFinalRobo2-Contracts/changes/2026-10-28-16-loader-095-interfaz-envio-serie.md).

## Flujo vigente 0.9.5

1. En Operación elegí Puerto y Destino: ATmega o ESP. Actualizar puertos y Detectar tienen funciones separadas.
2. ATmega ofrece firmware completo del auto o solo comunicación. ESP ofrece Web propio o Paquete AT y sección Firmware / Web / Firmware + Web. La sección incluye el filesystem cuando corresponde; el tamaño se elige por layouts existentes. La imagen AT monolítica solo admite Firmware.
3. Elegí Método de flasheo ESP si corresponde. Configurar flasher abre la pestaña contigua. Instalar el flasher solo está allí, abajo a la derecha; Detectar ESP lleva a Operación antes de abrir el asistente.
4. Compilar y Flashear tienen botones divididos del mismo tamaño. Flashear queda abajo a la derecha. La barra inferior muestra operación, progreso, COM/baud, monitor y MCP. Las etapas de operación se mantienen.
5. Configuración separa General, ATmega y ESP con subsecciones; la búsqueda filtra dentro de lo elegido. Cada campo tiene ayuda y acceso al manual. Guardar/Guardar como comparten botón.
6. Opciones → Preferencias permite ajustar backups, casillas de asistentes y registros. Las casillas pueden ser requeridas, estar marcadas por defecto u ocultarse; la confirmación final y las comprobaciones técnicas siguen activas. El COM se comprueba después de cerrar el monitor y antes de la operación; es una comprobación puntual, no una reserva. La referencia al botón del shield es KEY por indicación del usuario.
7. Codex/MCP distingue conector activado de conexión comprobada. El comando de registro comienza oculto; las solicitudes aparecen en tabla. No cambia el esquema de aprobación/autoaceptación.

## Enviar desde Monitor serie (0.9.5)

Abrí el puerto desde el monitor y escribí los datos en su editor inferior. Elegí:

| Formato | Ejemplo | Bytes enviados sin terminadores |
| --- | --- | --- |
| ASCII literal | `AT` | `41 54` |
| HEX | `41 54` o `0x41 0x54` | `41 54` |
| Números decimales | `65 84` | `41 54` |

Las casillas CR y LF agregan `0D` y `0A`, en ese orden, a **cada envío**. Los escapes escritos son literales en ASCII. También se pueden enviar solo terminadores con entrada vacía. ASCII rechaza caracteres fuera de0..127; números acepta bytes0..255. Hasta64KiB por envío, incluidos terminadores; de1 a1000 repeticiones y pausa0..60000ms. La carga útil se toma al pulsar Enviar y los controles quedan bloqueados durante la repetición.

Detener cancela lo pendiente. Cerrar monitor, cambiar baudios o iniciar una operación de carga también detiene TX antes de ceder el COM. El mismo worker administra RX/TX, sin abrir un segundo handle. No se transmite al abrir/reconectar ni desde MCP. Se escriben bloques cortos con cancelación entre bloques y timeout de2s por Write; los bytes ya aceptados por el puerto no se pueden retirar. La confirmación del driver no es un ACK del dispositivo.

Los registros `.rx.bin/.rx.txt` conservan RX y `.tx.bin/.tx.txt` registran los bloques TX aceptados y las solicitudes. Si una escritura falla, el bloque que falló puede haber salido parcialmente; queda anotado sin reintento automático. El registro TX binario no presenta ese bloque incierto como confirmado.

Seleccionar texto en vista ASCII o decodificada muestra el acceso al conversor: copia la selección y la pega como ASCII literal. Convertir sigue siendo una acción explícita. HEX normalizado y resultado tienen su copia con toast. Eventos usa Ver detalles y resalta en amarillo el detalle abierto.

Las descripciones de versiones inferiores se conservan como historia y quedan reemplazadas por este flujo donde difieran.

> **AFR2-20261028-15 / 0.9.4:** en modo AT, **Paquete AT / BIN...** permite seleccionar directamente Ai-Thinker_ESP8266_DIO_32Mbit_V1.5.4.1_20161202.bin. Se reconoce el contenido exacto por SHA-256; no se convierte el BIN ni hace falta un JSON adicional. Mapa de imagen completa: 4 MiB desde 0x0, DIO/40 MHz declarados en archivo. Reemplaza toda la flash; no incluye filesystem separable. Otros BIN requieren manifiesto del proveedor. Seleccionar no flashea ni abre COM. Fuente sin compilar/probar. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-28-15-loader-at-bin-directo.md).

> **AFR2-20261028-13 / 0.9.3:** feedback de carga en Operación y resumen desde cualquier pestaña: tiempo del proceso, último mensaje y segundos sin salida; porcentaje y barra determinados solo cuando esptool informa Writing at …%. Cada porcentaje corresponde a una imagen; 100% no sustituye la verificación final. Sin porcentaje se muestra actividad; tras 15 s sin salida cambia el color del texto, sin inferir bloqueo ni modificar timeouts. Captura también mensajes parciales sin salto de línea. Fuente sin compilar/ejecutar. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-28-13-loader-feedback-carga.md).

# AFR2 Loader - Windows

> **AFR2-20261028-12 / 0.9.2:** pestaña **Conversor HEX / ASCII**, universal e independiente de comandos y del puerto serie. Pegar HEX, convertir a ASCII legible/escapado o ASCII literal a HEX y copiar resultado/HEX normalizado. Acceso también desde Monitor serie. Fuente sin compilar ni ejecutar. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-28-12-loader-conversor-universal.md).

> **AFR2-20261028-11 / 0.9.1:** integración de Loader remoto0.9.0 con mejoras locales0.7.4. Conserva flasher ATmega, backups y Preferencias; incorpora USB–TTL externo, monitor ASCII/HEX/texto, Limpiar ventana, Python sin buffer y comprobación nueva antes de verificar. Solo fuente, sin compilar, ejecutar ni usar COM. [Ficha de integración](../AutoFinalRobo2-Contracts/changes/2026-10-28-11-integracion-local-remoto.md).

> **AFR2-20261028-05 / 0.9.0:** backup opcional leído del ATmega, aviso configurable y botón **Flashear Firmware ATmega** con menú **Flashear último backup**. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-28-05-loader-mega-backups.md) · [Plan A/B](../AutoFinalRobo2-Contracts/hardware/PLAN_PRUEBA_FLASHER_MEGA.md). Fuente entregada; sin compilar, ejecutar, abrir COM ni flashear.

Herramienta gráfica en español para compilar y cargar el software de
AutoFinalRobo2 y cargar paquetes ESP8266 AT precompilados para otros usos.
Hecho por Koba Chajchir. Versión **0.9.5**,
AFR2-20261028-12, 28/10/2026. Desde **AFR2-20261027-14** es un módulo propio
en la raíz, `AutoFinalRobo2-Loader`, al mismo nivel que Atmega, ESP, Web,
Contracts y CAD. Comparte su único Git. [Registro de integración](../AutoFinalRobo2-Contracts/changes/2026-10-27-14-loader-root-module.md).

## Secuencia ESP corregida

Antes de enviar `verify_flash`/`verify-flash`, el Loader comprueba nuevamente
la respuesta del cargador ROM/stub con `read_mac`/`read-mac`, sin escribir,
subir otro stub ni solicitar reset automático. La consulta tiene dos intentos
de conexión y un máximo de15segundos (30 con el wrapper del flasher ATmega); exige salida0 y una respuesta MAC válida.
No se reutiliza como prueba el modo observado antes de escribir.

Si no puede confirmarlo, **detiene la verificación** y muestra un aviso para
repetir KEY/RST. Continuar abre la recuperación del transporte elegido: USB–TTL o Mega físico
esperan PUERTO LISTO a74880; flasher ATmega conserva baud fijo y RESET libre,
sin escucha74880. Mantener KEY, pulsar y soltar solo RST, conservar KEY. Vuelve a
consultar el cargador y continúa únicamente con la verificación de los mismos
BIN. Cancelar, un fallo de recuperación o de verificación mantienen el resultado
sin verificación completada; no se repite la escritura. También se aplica a
LittleFS y paquetes AT, desde GUI o MCP, aun con Guiar K/RST desactivado.

La consulta confirma respuesta en ese momento; la sincronización del propio
comando de verificación sigue siendo necesaria. [Registro AFR2-20261027-29](../AutoFinalRobo2-Contracts/changes/2026-10-27-29-loader-verification-bootloader-gate.md).
Fuente0.7.3 sin compilar ni ejecutar esta revisión.

El usuario aportó una carga manual por USB–TTL en COM5 a 115200, con
esptool4.5.1 y hash verificado. Mantuvo K (KEY) apretado durante la carga,
soltando solo RST después de pulsarlo. [Evidencia y adaptación del Loader](../AutoFinalRobo2-Contracts/changes/2026-10-27-27-loader-esp-command-key.md).

El asistente, la consulta ROM, las confirmaciones y los estados de carga ahora
piden **mantener KEY hasta terminar la operación, incluidas escritura y
verificación**. Cerrar la pantalla de botones o ver modo1 no indica soltarlo.
Al finalizar la operación se avisa que ya puede soltarse, también ante un fallo
o cancelación; esto no acredita una carga válida si no se verificó el contenido.

En Configuración, **Conexión serie ESP** (`EspTransport`) acepta `usb-ttl`
para el adaptador externo de lógica3,3V o `mega_bridge` para el flasher de software D18/D19.
El asistente muestra el cableado correspondiente. El método D0/D1 con RESET-GND
(`mega_reset`, `mega-usb`) fue **retirado** (AFR2-20261106-11): un perfil antiguo
que lo tenga pasa a `mega_bridge` al abrirse; revisá el cableado antes de operar.
Elegir `usb-ttl` para reproducir la conexión externa; nunca se exige RESET-GND
del Mega en ese modo.
El COM se elige en la ventana: no se fija COM5 ni se modifica un perfil guardado
por editar las fuentes.

El paso de escritura invoca Python con `-u` antes de esptool, para salida sin
buffer de Python. Con perfil Web, firmware solo, puerto COM5 y los valores
predeterminados, sus argumentos son equivalentes al comando manual aportado:

    python -u esptool.py --chip esp8266 --port COM5 --baud 115200 --before no_reset --after no_reset write_flash --flash_mode keep --flash_freq keep --flash_size 4MB 0x0 firmware.bin

Las rutas de Python/esptool/BIN se resuelven desde el perfil. `0x0` y `0x00000`
representan el mismo offset. La escritura usa el stub normal; `--no-stub`
se reserva para la consulta ROM previa. El Loader conserva esa consulta y la
verificación explícita posterior, sin reinicios ni reescrituras automáticas.
Los parámetros SPI de paquetes AT siguen viniendo de su manifiesto.

Esta revisión solo modifica fuentes/documentación: **Loader0.7.1 sin compilar,
ejecutar ni probar**. La carga manual aportada no valida el programa corregido.

## Abrir

En **Operación → Método ESP** se elige entre **Flasher en ATmega — D18/D19,
RESET libre** y **Adaptador USB–TTL externo — lógica3,3V**. Al elegir se
cierra la lectura serie, se muestra el cableado de ese método y se habilita
Continuar solo al marcar todas las casillas. Cancelar conserva el método
anterior y deja el monitor cerrado. Elegir no instala ni carga firmware. La
misma selección está sincronizada con la pestaña Flasher ATmega; las cargas
mantienen además su asistente y confirmación final. Cambios de perfil/importación
no se presentan como confirmación física; el asistente de cada carga sigue activo.

La pestaña **Flasher ATmega** contiene conexión ESP, baudios, instalación,
consulta sin escritura y el botón dividido **Flashear Firmware ATmega**.
**Cargar flasher en ATmega...** también aparece en Operación; compila solo el
servicio AVR y pide confirmar antes de escribirlo y verificarlo.

**Backup de ATmega antes de cargar** está marcado por defecto. Lee flash y
hfuse dos veces con avrdude en modo sin escritura; compara ambas lecturas y
conserva los archivos originales, imagen completa, aplicación y hashes. Un
fallo detiene la instalación antes de reemplazar el programa. El respaldo es
lo que tiene el equipo en ese momento: puede contener otro flasher. No requiere
un HEX de producto para crearlo y no incluye EEPROM.

Desmarcarlo muestra **No se va a crear backup del ATmega**, con Continuar sin
backup / Cancelar y **No mostrar de nuevo**. Esta última opción se guarda solo
al continuar. En **Preferencias** se puede reactivar el aviso y cambiar la
casilla predeterminada. Guardar persiste ambos ajustes para este usuario en
`%LOCALAPPDATA%/AutoFinalRobo2Loader/preferences.json`, fuera de los perfiles.
Omitir un backup no elimina ni reemplaza los anteriores. El aviso suprimible
no desactiva los asistentes de cableado ni la confirmación de escritura.

Tras instalar el flasher se selecciona `mega_bridge`, se alinea EspBaud con
BridgeBaud y se conserva RESET Mega **libre**, D18/TX1→E_RX y E_TX→D19/RX1 por
la ruta de niveles identificada. Sin D0/D1 al ESP ni jumpers paralelos; potencia
de actuadores desconectada. No aplicar TX de 5 V al RX directo ESP de 3,3 V.
Primero **Detectar ESP sin escribir**, luego firmware y LittleFS del mismo
layout. En software se guía KEY/RST manualmente: no se escucha el boot a 74880.
El wrapper abre COM con DTR/RTS inactivos, espera BridgeStartupSeconds y entrega
ese mismo puerto a esptool. Baudios coincidentes: 115200, 57600, 38400 o 9600;
cambiar variante requiere volver a cargar el flasher. MegaBaud es independiente.

Al terminar hay dos acciones en el mismo botón:

- **Flashear Firmware ATmega** carga/verifica el HEX elegido en Configuración.
- La flecha abre **Flashear último backup**: revisa fecha, COM de origen/destino,
  tamaños, hashes y hfuse; escribe solo aplicación y la contrasta mediante una
  lectura posterior completa, incluida su cola FF. No programa bootloader,
  fuses ni EEPROM. No supone que el backup sea el firmware del auto.

El último backup completado se indexa para este usuario en `backups/last-mega-backup.json`;
los respaldos históricos tienen carpetas únicas. Un COM puede cambiar entre
sesiones: se muestra origen y destino, no se adivina la identidad física. Las
copias de HEX de PC de0.8 en recovery/ se conservan pero no se convierten en
backups del equipo. Si falta el respaldo, se detiene la acción: seleccionar un
HEX conocido para Flashear Firmware ATmega. Un bootloader que no permite leer
flash/hfuse no habilita backup; no se omite silenciosamente el fallo. AVRDUDE6.3
recorta colas FF: se normaliza el exportado y se contrasta después de escribir;
si quedan bytes de otro programa se informa fallo, sin borrado automático.

El método físico D0/D1 y RESET-GND fue retirado. No se cambia de montaje
ni se carga firmware al cerrar la app. El [plan A/B](../AutoFinalRobo2-Contracts/hardware/PLAN_PRUEBA_FLASHER_MEGA.md)
detalla preparación y recuperación. Código nuevo en `src/Operations.MegaBackup.cs`,
`src/MainForm.Preferences.cs` y `src/LoaderPreferences.cs`; transporte en
`src/Operations.MegaBridge.cs`, `src/MainForm.MegaBridge.cs` y el wrapper Python.

Hacer doble clic en **Iniciar.cmd**. En el primer inicio, el lanzador genera
el ejecutable WinForms con el compilador de .NET Framework de Windows y lo
guarda en `%LOCALAPPDATA%\AutoFinalRobo2Loader\bin\AFR2Loader.exe`. Si cambian
los fuentes, vuelve a generarlo al iniciar. No requiere Visual Studio ni un
proyecto C# instalado. Requiere Windows 10/11, Windows PowerShell 5.1 y .NET
Framework 4.8 con `csc.exe`; no usa PowerShell 7 para la compilación del programa.

Este arranque solo construye la aplicación. **No compila firmware, no abre COM,
no descarga dependencias ni carga placas hasta elegir una acción.**
No pide elevación ni cambia la política global: ExecutionPolicy Bypass se usa
solo en el proceso del lanzador. No hace falta ejecutar como administrador.

Para generar el EXE sin abrir la ventana, cuando se autorice esa compilación:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\Iniciar.ps1 -SoloCompilar
```

El lanzador también permite abrir una familia y un perfil ya guardado:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\Iniciar.ps1 -FirmwareEsp at -Perfil "robot-at.json"
```

`-FirmwareEsp` admite `web` o `at`; `-Perfil` es un nombre JSON de la biblioteca
local. Solo abre esa selección: no inicia firmware, puertos ni cargas. Sin
opciones conserva el último perfil. Si la ventana ya está abierta usa su sesión
actual; el lanzador no cambia una operación en curso.

## Firmware ESP y memoria

En Operación, arriba del registro, elegir **Web propio / Arduino** o
**Firmware AT / paquete**, y marcar **Incluir filesystem** cuando corresponda.
Ambos se cargan por USB con la ROM del ESP8266. No hay OTA/HTTP en esta revisión.
AT designa el firmware que interpreta comandos AT después de instalarlo.

- **Web:** dropdown de los linkers instalados para la capacidad del perfil,
  con/sin LittleFS. Muestra bytes reales de código, FS y reservas/espacio.
  Elegir un preset actualiza el perfil y usa una configuración de build local,
  sin reescribir platformio.ini del producto. El cambio de layout exige construir
  los BIN correspondientes: sus recibos locales verifican layout y hash antes
  de cargar. Para imágenes externas se mantiene **Personalizado** con su linker.
- **AT:** **Paquete AT...**, visible únicamente con esta familia seleccionada,
  abre un manifiesto con variantes de memoria y BIN
  del proveedor. El dropdown muestra sus layouts y reservas FS; marcar la
  casilla incluye las imágenes filesystem en la escritura, sin redimensionar
  una tabla ya compilada. Permite firmware/sistema, FS o ambos y verifica cada
  BIN. No usa el linker Arduino ni construye LittleFS para un paquete AT.
- **ATmega:** conserva HEX, avrdude y sus reglas propias. La familia ESP no
  modifica el código del microcontrolador ni lo convierte a AT.

En AT, el botón se llama **Revisar paquete AT**: comprueba archivos, regiones,
capacidades y hashes; no compila un firmware del proveedor. **Compilar todo**
compila ATmega y prepara ESP según familia; construye Web/LittleFS solo con
familia Web y FS activado. Las acciones MCP usan la misma selección del perfil.

Preparar un paquete requiere el mapa oficial de esa versión, para **ESP8266**.
Copiar [paquete-at.plantilla.json](paquete-at.plantilla.json) junto a los BIN
y completar capacidad, SPI, layouts y regiones de firmware/system/filesystem.
La plantilla contiene datos incompletos a propósito: no representa un paquete
compatible para cargar. Ver [manual portable](MANUAL.html#modos) y
[contrato completo](../AutoFinalRobo2-Contracts/contracts/LOADER_FLASH_MODES.md).
No se distribuyen BIN AT ni se asume AT+FS en un firmware ESP8266 genérico.

El firmware de producto AutoFinalRobo2 sigue usando su protocolo propio:
cargar AT en el ESP no acredita compatibilidad con ese firmware del Mega.

## Incluye

- **Estado del servidor MCP** arriba de Codex / MCP y **Ejecutar prueba**:
  despliega una instrucción para pegar en el chat de Codex, con identificadores
  de esta ventana y de la prueba. `loader_connection_test` confirma la conexión
  y muestra **Servidor MCP conectado**, hora y cliente. El transporte es stdio
  y se muestra su canal local Windows; no hay IP ni puerto TCP. Detecta cierre
  del cliente o ausencia de su señal durante 30 segundos. La prueba vence a
  los cinco minutos y no abre COM ni ejecuta operaciones del robot.
- **Comando para añadir servidor MCP**, con copia y confirmación de portapapeles,
  separado de la instrucción de prueba que se ejecuta en el chat de Codex.
- Casilla **Autoaceptar todas las solicitudes de Codex (esta sesión)** en
  Codex / MCP, visible desde la apertura en su propia fila adaptable al texto.
  Si se deshabilitan solicitudes queda visible pero inactiva y desmarcada.
  Activa la ejecución de solicitudes nuevas y la pendiente sin
  revisión MCP adicional; también aplica cambios de perfil y acepta solicitudes
  de cancelación. Empieza desmarcada al abrir. Los asistentes de cableado,
  K/RST y confirmación final de escritura siguen requiriendo interacción.
- Historial con selección de filas por Ctrl + clic, rangos por Shift + clic
  o teclado y copia por Ctrl + C, Ctrl + Insert, botón o menú contextual.
  Solo se copian las filas elegidas, con encabezados, en orden visible. La
  actualización conserva selección y posición; copiar muestra abajo
  **Copiado al portapapeles**, también en los botones de copia de otras vistas.
- Selector de perfiles guardados arriba de Configuración. Enumera los JSON
  de LOCALAPPDATA/profiles por nombre y archivo; Guardar conserva el perfil
  activo y **Guardar como...** crea otro. Importar permite revisar antes de
  guardarlo en la biblioteca; Exportar conserva la copia portable. Se recuerda
  el último perfil seleccionado y se consultan cambios sin guardar al cambiar.
- Monitor con aviso separado en título/mensaje, icono alineado, primera letra
  mayúscula y mensaje completo con desplazamiento; datos originales conservados
  en Eventos y errores. Los asistentes abren con foco en la acción o primera
  casilla, sin seleccionar automáticamente las instrucciones.
- Barra inferior visible en todas las pestañas: etapa actual, monitor serie y
  revisión MCP. Casillas laterales de avance alimentadas por eventos de la
  operación, con compilaciones, reinicio ESP, modo UART, conexión y carga.
- Botón dividido Compilar: la flecha permite elegir ATmega, ESP, Web/LittleFS
  o ESP + Web/LittleFS y compilar; la selección queda también como destino de
  flasheo. Compilar todo conserva ese destino. El botón Flashear nombra qué
  cargará, y Detectar junto a Destino cambia entre ATmega y ESP / ROM.
- Detección ATmega por firma con avrdude sin escritura; ESP mantiene la consulta
  ROM. Asistente K/RST separado del cableado, con aviso de puerto listo después
  de Continuar y antes de pulsar los botones del shield.
- Puerto COM seleccionable y actualización de puertos presentes.
- Configuración de carpetas, herramientas, HEX/BIN, entorno y layout; perfiles
  JSON importables/exportables, sin nombres de usuario ni COM fijos.
- Build de ATmega producto/comunicación/biblioteca, ESP y Web perfil esp.
- Web `build:esp` con promoción a `ESP/data/dist`, seguido de `buildfs`.
- Detección de `boot mode:(1,...)` a74880 con puerto liberado al terminar;
  sincronización ROM y lectura de flash con esptool antes de cada carga ESP.
- Asistentes de cableado ATmega/ESP, confirmación explícita previa a escritura,
  y advertencia de reemplazo de preferencias al cargar LittleFS.
- Aviso al terminar una carga verificada (ATmega, ESP o LittleFS): restaurar
  D18/TX1 → E_RX y E_TX → D19/RX1 del selector, retirar servicio D0/D1 y quitar
  RESET Mega–GND con alimentación desconectada. No aparece por solo compilar.
- Carga con avrdude y verificación AVR incorporada; escritura ESP y comando
  `verify_flash`/`verify-flash` posterior contra los BIN seleccionados.
- Registro vivo de stdout/stderr, errores, códigos de salida, SHA-256,
  capacidad detectada, resumen JSON y cancelación de procesos hijos.
- Opciones / ayuda: manual local, carpeta de logs y Acerca de con datos del
  desarrollador tomados de DeveloperPage.tsx, sin agregar correos inventados.
- MCP stdio para Codex, conectado a la misma ventana mediante un canal local
  del usuario. Consultas de estado/perfil/puertos y solicitudes revisables de
  build, diagnóstico, carga y cambios de perfil.
- Pestaña Codex / MCP con solicitud pendiente, comparación de perfil y botones
  Revisar/Rechazar; historial de sesión, filtro de configuración y seguimiento
  de registro opcional. Evidencias UART y ROM diferenciadas por puerto/hora.
- Una operación por sesión; deduplicación de solicitudes MCP, registros
  incrementales y límites de memoria; interrupción de procesos ante fallos.

## Conectar Codex

Abrir **Iniciar.cmd**. Ejecutar **RegistrarMcp.ps1** desde PowerShell para
registrar `afr2-loader`, o copiar el **Comando para añadir servidor MCP**
de la pestaña Codex / MCP.
Reiniciar/reconectar el cliente MCP. Guía completa, alternativas sin CLI y
herramientas en [MCP.md](MCP.md). La app permanece abierta: el servidor comparte
su trabajo/registro y no flashea fuera de esa sesión.

```powershell
cd "<carpeta donde está AutoFinalRobo2-Loader>"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File ".\Iniciar.ps1"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File ".\RegistrarMcp.ps1"
```

El registro modifica la configuración de Codex de quien lo ejecuta. No se
ejecutó en esta entrega. Registrar o listar herramientas no demuestra conexión:
la primera consulta `loader_status` debe devolver la sesión de la ventana.
Para el indicador de conexión, pulsar **Ejecutar prueba** y pegar en el chat
de Codex la instrucción desplegada. La herramienta `loader_connection_test`
debe devolver `ok:true`, `status:connected` y los mismos `sessionId`/`testId`.
La prueba también funciona con las solicitudes deshabilitadas. Si no aparece
la herramienta nueva, cerrar el cliente MCP anterior y reconectarlo a los
fuentes actualizados; iniciar los lanzadores puede reconstruir el ejecutable.
`awaiting_approval` espera revisión manual; `accepted` indica aceptación
automática y todavía no acredita ejecución. Consultar `loader_job` para el
estado real; `loader_status.autoAcceptRequests` muestra el modo de la ventana.

## Monitor serie y errores

En **Operación**, el botón **Limpiar ventana**, junto a **Copiar registro**,
vacía el texto mostrado y su cola pendiente de visualización. Está disponible
también durante una operación; la salida nueva continúa apareciendo.
Conserva archivos de log, historial MCP, estados y la operación en curso.
[Registro AFR2-20261027-28](../AutoFinalRobo2-Contracts/changes/2026-10-27-28-loader-clear-operation-view.md).
Fuente0.7.2 sin compilar ni ejecutar esta revisión.

En **Monitor serie** elegir COM, baudios (lista o valor propio) y codificación.
El selector **Vista** permite alternar en vivo entre **ASCII** (inicial),
**HEX con espacios** y **Texto decodificado**. HEX usa los bytes originales,
dos dígitos mayúsculos por byte, un espacio entre bytes y16bytes por fila,
sin depender de cómo se fragmentó la lectura. ASCII también parte de los
bytes; Texto decodificado conserva la codificación UTF-8/ASCII/Latin-1 elegida
al abrir la captura.

Cambiar Vista reconvierte la ventana retenida sin cerrar/reabrir COM, cambiar
baudios ni reiniciar la captura. El historial para reconversión está acotado
a65536unidades (bytes + caracteres decodificados); no reconstruye lo que ya
salió de esa ventana. Limpiar vista descarta también ese historial y lo ya
pendiente de mostrar, para no recuperar texto viejo al cambiar de formato.
Copiar texto copia la representación visible. Los archivos `.rx.bin`/`.rx.txt`,
el parser de arranque y los cursores MCP siguen siendo independientes de Vista.
[Registro AFR2-20261027-30](../AutoFinalRobo2-Contracts/changes/2026-10-27-30-loader-serial-hex-ascii.md).
Fuente0.7.4 sin compilar ni ejecutar esta revisión.

**Abrir puerto** inicia lectura continua; **Cerrar puerto** termina y libera
el COM. **Aplicar baudios** cierra la captura y abre otra al nuevo valor, sin
enviar comandos al dispositivo. Lectura 8N1, DTR/RTS inactivos. La apertura
del adaptador puede afectar reset; no hay apertura ni reconexión automática.

Usar **74880** para recibir el arranque ROM del ESP; para la aplicación elegir
sus baudios. El parser reconoce mensajes fragmentados de `rst cause`,
programación UART modo 1 y arranque desde flash modo 3. Son observaciones del
texto recibido, con fecha; no se declara modo actual permanente ni éxito por
un mensaje genérico `OK`/`done`. La consulta esptool sigue comprobando ida/vuelta.

La vista conserva texto entre fragmentos, permite pausar desplazamiento,
pausar la vista, limpiar y copiar. La captura guarda `.rx.txt` UTF-8, `.rx.bin`
con bytes originales, `.events.txt` y resumen `.json` en LOCALAPPDATA/logs.
La memoria de pantalla es limitada; si se omiten caracteres por atraso avisa
y conserva los archivos mientras el disco permita escribir. Limpiar/pausar
la vista no borra archivos ni detiene la lectura. Cambiar de captura no borra
los registros anteriores; no se implementa rotación automática por tamaño.

En Operación y Monitor serie, mover rueda/barra, navegar con teclas o seleccionar
texto desactiva Seguir. Los nuevos datos conservan selección y posición vertical
y horizontal; no se desplaza la vista en ticks sin texto. Volver a marcar Seguir
lleva al final y retoma el seguimiento. La pantalla conserva un tramo limitado:
si expira texto antiguo se ajusta la posición al contenido todavía disponible;
los archivos contienen el registro completo mientras se puedan escribir.

Antes de diagnóstico/carga se cierra el monitor, se espera su liberación y
recién después se ejecuta esptool/avrdude. Durante la carga, la fuente del
progreso es la herramienta, cuyo texto está en Operación y su log; no se abre
un segundo lector sobre el mismo COM. Después no se reabre automáticamente:
restaurar el cableado/modo y abrir manualmente para observar la aplicación.

**Eventos y errores** distingue reinicio observado, modo UART, sincronización,
escritura, verificación y carga verificada. Un fallo abre esta pestaña, conserva
el mensaje original y genera `failure-*.txt`. **Ver/Copiar último error** reúne
origen, sugerencia, código de salida y última salida de la herramienta cuando
existen, más rutas de registros. Las sugerencias son diagnósticos posibles,
no una medición del cableado. Un fallo no queda oculto por el posterior aviso
de cierre del puerto. El resumen de carga registra `contentVerified`.

Codex dispone de `loader_serial` para consultar la última captura y su texto
incremental sin abrir/cerrar puertos. `loader_status` incluye captura y ruta
del último informe de fallo. Guía en [MCP.md](MCP.md).

## Para otro compañero

Copiar **toda la carpeta AutoFinalRobo2-Loader**, incluyendo `src/`, `MANUAL.html`, lanzadores y
plantilla JSON. Para compilar firmware, copiar/clonar también los módulos
Atmega, ESP, Web y Contracts, conservando su relación de carpetas. Para cargar
archivos ya compilados se pueden seleccionar HEX/BIN externos sin compilar;
ESP requiere además el linker correspondiente y esptool/Python.

En Configuración, seleccionar la raíz de su copia, Detectar rutas vacías y
completar lo que falte con `...`. Guardar. Las dependencias de build/carga
son externas: GNU Make y avr-gcc/avr-ar/avr-objcopy, avrdude/conf, PlatformIO,
Node/npm y Python/esptool. No se redistribuyen ni instalan silenciosamente.
Abrir **MANUAL.html** para el recorrido completo y las conexiones explicadas.

El perfil usa `{Repo}`, `{Env}` y variables de Windows como `%USERPROFILE%`.
Las herramientas se detectan usando PATH, Arduino15 y la instalación PlatformIO
del usuario que abre el programa. Todos esos resultados se pueden reemplazar.

## Arquitectura

| Archivo | Responsabilidad |
| --- | --- |
| src/Program.cs | Entrada STA y manejo de errores de interfaz |
| src/MainForm.McpConnection.cs | Alta, instrucción de prueba, evidencia por sesión e indicador de conexión MCP |
| src/McpClientConnection.cs | Identidad del proceso stdio, señal local de vida y aviso de cierre |
| src/Profile.cs | Esquema, valores, resolución de rutas y persistencia de perfiles |
| src/Runner.cs | Procesos externos asíncronos, salida, timeout y cancelación |
| src/Operations.cs | Builds, escucha UART, consulta ROM, layout, escritura y verificación |
| src/Operations.EspModes.cs | Selección de preparación y carga USB de mapas AT con verificación por BIN |
| src/Operations.WebLayouts.cs | Configuración de build por layout y recibos locales de imágenes Web |
| src/EspFlashRules.cs | Rangos del linker Web y esquema/reglas independientes de paquetes AT |
| src/MainForm.cs | Ventana, configuración, asistentes, estados y registro |
| src/MainForm.Integration.cs | Revisión MCP, historial, indicadores y filtro |
| src/MainForm.Serial.cs | Monitor, baudios, consola RX, eventos e informes de error |
| src/MainForm.Workflow.cs | Barra de estado, destino visible, etapas y asistente K/RST |
| src/MainForm.EspModes.cs | Selector Web/AT, filesystem y resumen de memoria |
| src/MainForm.Profiles.cs | Biblioteca local, selector y persistencia del perfil activo |
| src/MainForm.Usability.cs | Copia con toast y foco inicial de diálogos de lectura |
| src/LogTextBox.cs | Consolas con posición/selección conservadas y seguimiento manual |
| src/SerialCapture.cs | Lectura en segundo plano, cierre y archivos de captura |
| src/BootObservation.cs | Parser incremental de reinicio y modo de arranque ESP |
| src/FailureDetails.cs | Error de herramienta con salida/código y orientación diagnóstica |
| src/JobState.cs | Trabajo, estado y registro paginado en memoria |
| src/LocalBridge.cs | Named pipe limitada al usuario, comparte la sesión GUI |
| src/McpServer.cs | Protocolo MCP stdio, herramientas y manual como recurso |
| Compilar.ps1 | Generación local de EXE WinForms o consola MCP; diagnósticos por stderr |
| Iniciar.ps1 / Iniciar.cmd | Apertura de la interfaz |
| Mcp.ps1 / RegistrarMcp.ps1 | Entrada stdio y registro optativo en Codex |
| MCP.md / codex-mcp.example.toml | Configuración portable y contrato operativo MCP |
| paquete-at.plantilla.json | Plantilla sin offsets candidatos para describir el mapa oficial de un paquete AT |
| MANUAL.html | Manual portable que se abre desde Opciones / ayuda |

No hay Git anidado: Loader es el sexto módulo del repositorio global.
Contracts mantiene los registros compartidos; el código y manual del cargador
pertenecen a esta carpeta propia.
El EXE, los perfiles y los logs de usuario quedan fuera del repositorio.

## Límites de la entrega

Implementada en fuente con lanzador y manual. **No se compiló ni ejecutó la
aplicación en esta entrega; no se hicieron tests, validación de interfaz,
pruebas de puerto, flasheos ni comprobación física.** La instrucción del
proyecto exige un pedido explícito para esas actividades. No se acredita una
versión probada por haber entregado estos archivos. El primer lanzamiento
realizará la compilación local del programa; cualquier error se verá en la
consola del lanzador y no iniciará una carga.

La recepción del mensaje ROM prueba ESP→PC y su modo en ese reset; no prueba
PC→ESP. La aplicación exige una consulta ROM exitosa y flash identificada antes
de escribir. La verificación de flash no acredita sensores, Wi-Fi ni movimiento.
Los asistentes no pueden medir tensiones, mover cables ni garantizar que las
confirmaciones manuales coincidan con el montaje.

## Conversor universal HEX / ASCII

Abrir la pestaña **Conversor HEX / ASCII**, o el botón del mismo nombre en
Monitor serie. No hace falta abrir COM, elegir dispositivo ni iniciar un trabajo.

1. Elegir **HEX → ASCII** y pegar los bytes (HH,0xHH o pares continuos).
   Se permiten las cabeceras completas --- hora | COM | baudios | codec ---:
   se omiten como metadatos, sin convertir los números del título.
2. Pulsar **Convertir** o Ctrl+Enter. La vista Legible conserva saltos/tab;
   Escapada muestra controles como \r,\n,\t,\xHH. Los bytes80..FF y controles
   como NUL se conservan como \xHH en ambas, sin inventar texto ASCII.
3. **Copiar resultado** copia lo visible. **Copiar HEX normalizado** obtiene los
   bytes originales en filas de16. Pegar y Limpiar afectan solo al conversor.

**ASCII literal → HEX** convierte caracteres0..127 y saltos reales. Una secuencia
escrita como \n se codifica literalmente; no interpreta escapes. Los caracteres
fuera de ASCII se rechazan con posición, sin sustituirlos por ? ni cambiar a
UTF-8. Hasta64KiB de bytes/262144caracteres; una entrada inválida/excesiva no
produce resultado parcial. Editar invalida la conversión anterior.

No se identifican comandos, protocolos ni arranques; 41 es A y02 es \x02.
No transmite, no abre COM y no añade jobs/logs del texto pegado. Los borradores
son solo de la ventana. Disponible durante operaciones porque es independiente
de sus recursos. Fuente0.9.2 sin compilación, tests, validadores ni ejecución GUI.









## Loader1.1.3 — Verificar flasher (AFR2-20261029-21)

En **Flasher ATmega → Verificar flasher**, el Loader compara la flash instalada con la referencia compilada para los baudios seleccionados, por bootloader y avrdude `-n -D -U flash:v:...:i`. Requiere firma y confirmación de bytes coincidentes. Reinicia el Mega por auto-reset; no escribe firmware, EEPROM ni fuses y no consulta al ESP. Un firmware distinto/otra variante no se acredita. El flasher transparente existente no necesita actualización. GUI/MCP compilados; sin prueba COM de esta revisión.

Desde Codex: `loader_request_operation` con `action: verify_mega_flasher`, `port` y `requestId` nuevo; consultar `loader_job` hasta completar. Se aplican aprobación/exclusión del COM y confirmaciones vigentes. No confundir el último registro local con una comprobación nueva del dispositivo.


## Loader1.1.4 — Configuración y carga explícitas (AFR2-20261029-22)

Elegir paquete/sección solo cambia configuración. La selección de paquete invalida la revisión anterior; pulsá Consultar tamaño de flash/Detectar ESP y luego Revisar paquete AT o Compilar según corresponda. La carga se inicia desde Flashear; selección/preparación no abren automáticamente detección ni escritura. Compilar/Revisar con capacidad pendiente informa el requisito sin iniciar una consulta.
