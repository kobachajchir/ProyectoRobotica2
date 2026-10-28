# Plan de prueba: cargar ESP usando el Mega del auto

AFR2-20261028-05 · 28/10/2026 · Hecho por Koba Chajchir.
Actualiza03 para Loader **0.9.0**. **Preparado en fuente, sin ejecutar:** no se
compiló, abrió COM ni flasheó esta revisión. Cada aceptación debe anotarse luego.

Objetivo: probar primero el flasher temporal USART0↔USART1 con D18/D19. Si falla,
guardar evidencia y preparar la alternativa física D0/D1 con RESET a GND.
Ambos usan el USB-B del Mega. No se cambia automáticamente entre montajes.

## Preparación común

1. Cerrar una ventana anterior del Loader y abrir la carpeta completa con
   Iniciar.cmd / Iniciar.ps1. El lanzador recompila solo la app si cambió la fuente.
2. En Operación elegir el COM actual del Mega; cerrar monitores externos.
   El Loader cierra y espera su monitor antes de entregar el puerto al cargador.
3. En Configuración seleccionar carpetas y herramientas AVR, PlatformIO,
   Node/npm, Python y esptool. El Python necesita pyserial y dependencias esptool.
   Las rutas son por perfil; no se fija un COM ni una PC.
4. Para el software del auto elegir **ESP Web / Arduino**, filesystem habilitado
   y un layout coherente. Seleccionar destino ESP + Web / LittleFS y Compilar:
   prepara firmware, build:esp, copia a ESP/data/dist y genera littlefs.bin.
   Revisar los archivos configurados. Compilar el producto ATmega si se desea
   volver a un HEX nuevo; instalar el flasher compila solo el servicio AVR.
5. Desconectar alimentación antes de recablear. Mantener potencia de motores y
   servos desconectada durante el servicio; conservar alimentación lógica correcta
   y GND común. Identificar la ruta adaptada E_RX/E_TX del shield. No conectar
   TX de 5 V al RX directo ESP de 3,3 V ni duplicar divisores sobre un traductor.

## Prueba A: flasher ejecutado en ATmega, RESET libre

En **Operación → Método ESP** elegir **Flasher en ATmega — D18/D19, RESET libre**.
Leer las instrucciones y marcar todas las casillas para continuar. Cancelar
conserva el método previo; elegirlo no instala ni escribe nada.

| Conexión | Estado |
| --- | --- |
| USB-B | PC ↔ Mega |
| RESET Mega | **LIBRE**, sin cable a GND |
| Mega D18/TX1 | → E_RX central, RX del ESP, por ruta adaptada |
| E_TX central, TX del ESP | → Mega D19/RX1 |
| D0/D1 hacia ESP | Retirados |
| Jumpers/emisores paralelos | Retirados; sin Bluetooth en este enlace |

1. En **Flasher ATmega**, elegir **115200**. Esa velocidad corresponde al
   puente, no a MegaBaud del bootloader. Revisar **Backup de ATmega antes de cargar**.
2. Marcado: leerá flash/hfuse del equipo dos veces antes de escribir. Si falla,
   se detiene. Desmarcado: aparece aviso de que no creará backup, con **No mostrar
   de nuevo**. En Preferencias se reactivan aviso y casilla predeterminada al Guardar.
3. Pulsar **Cargar flasher en ATmega...**. Confirmar cableado. Se compila solo
   `esp-bridge-firmware BRIDGE_BAUD=115200`; después se hace el backup si corresponde,
   se muestra su ruta/omisión y se pide confirmar escritura. Esperar firma esperada
   `0x1e9801` y verificación de bytes por avrdude.
4. Tras éxito se selecciona mega_bridge y se iguala EspBaud a BridgeBaud.
   Guardar perfil si se quiere conservar esta selección. El auto ejecuta ahora
   el flasher, no su firmware de producto.
5. Pulsar **Detectar ESP sin escribir**. Cuando lo pida el asistente, mantener
   **KEY/K del ESP**, pulsar y soltar **RST del ESP** y después soltar KEY.
   Marcar las casillas y continuar. **RESET Mega permanece libre.**
6. No se exige escuchar boot a74880 por este puente fijo. El wrapper abre COM
   con DTR/RTS inactivos, espera BridgeStartupSeconds (3s por defecto) en ese
   mismo puerto y deja esptool consultar ROM/flash a115200. Un clic o un LED
   no acreditan comunicación: exigir respuesta y tamaño de flash esperados.
7. Si consulta correctamente, seleccionar ESP Web y **Flashear ESP Web**.
   Repetir la secuencia cuando se solicite y esperar escritura/verificación.
8. Elegir **Web / LittleFS** y flashear el FS del mismo layout, repitiendo
   preparación y verificación. También puede seleccionarse explícitamente
   ESP + Web / LittleFS. El filesystem reemplaza sus datos/preferencias.
9. Al terminar, usar **Flashear Firmware ATmega** (HEX configurado) o su flecha
   **Flashear último backup**, solo si ese respaldo contiene el programa deseado.
   RESET Mega libre y D0/D1 aislados. Esperar verificación. KEY libre y RST ESP
   para arrancar desde flash; revisar firmware/configuración antes de habilitar potencia.

**Ante fallo:** conservar texto, JSON, código/salida y etapa. No escribir ESP
si falla la consulta. No pasar automáticamente al montaje B ni borrar flash.
Para cambiar a57600/38400 hay que cargar esa variante del flasher y alinear
EspBaud; cambiar solo el monitor no sirve. 9600 queda para diagnóstico y exige
revisar timeouts de operaciones largas; no se afirma que un timeout fijo de
ROM prohíba universalmente esa velocidad. No subir dinámicamente el baud.

## Prueba B: conversor USB del Mega, ATmega detenido

1. Guardar el fallo de A y apagar antes de recablear. En **Método ESP** elegir
   **Puente serial del Mega — D0/D1, RESET a GND** y confirmar sus casillas.
2. Retirar D18/D19 hacia ESP. Unir **RESET del Mega a GND**, no RST del ESP.
3. **D0/RX0 → RX del ESP por ruta adaptada (E_RX central identificada)**.
   **TX del ESP (E_TX central) → D1/TX0**. Estos nombres pertenecen al micro
   detenido: D0 recibe la salida del conversor USB y D1 vuelve a su entrada.
   Por eso no se copia el cruce funcional del montaje A.
4. Mantener GND común y adaptación de niveles. Si se usa RX directo ESP,
   identificar un divisor/convertidor apropiado: ejemplo TX5V→1kΩ→nodo RX,
   2kΩ nodo→GND, 3,33V nominales. No alimenta el ESP y no se duplica sobre
   la adaptación del shield. No mover el cableado con alimentación.
5. Alimentar lógica y pulsar **Detectar ESP / ROM**, con escucha de arranque
   habilitada. El asistente primero abre COM a74880. **Esperar Puerto listo**;
   luego mantener KEY, pulsar/soltar RST ESP y soltar KEY. RESET Mega sigue aGND.
6. **boot mode:(1,...)** acredita el arranque del cargador; después la consulta
   esptool debe responder en ambos sentidos. Modo3 indica arranque desde flash.
   No escribir si falla la consulta aunque se haya visto modo1.
7. Si responde, flashear firmware y LittleFS con el layout preparado. Cada
   operación repite su preparación/consulta y verifica sus regiones. Puede
   usarse inicialmente115200; este montaje no ejecuta el puente de baud fijo.
8. Para volver al auto: apagar, retirar RESET-GND y cables D0/D1, conectar
   **D18→E_RX y E_TX→D19**. Si A dejó el flasher instalado, cargar producto
   con **Flashear Firmware ATmega** o el último backup apropiado. KEY libre,
   reset ESP y revisión de configuración antes de habilitar actuadores.

## Qué respalda y qué verifica el nuevo backup

- Guarda las exportaciones originales y normaliza FF finales omitidos por
  avrdude6.3; compara dos lecturas de flash y hfuse. Imagen completa de256KiB
  y aplicación separada por BOOTSZ. Requiere lectura BOOTRST/hfuse compatible.
- No incluye EEPROM ni identidad funcional del programa. Puede respaldar otro
  flasher. No sustituye una lectura fallida por una copia HEX de PC.
- Publica el último respaldo solo al completarlo en
  `%LOCALAPPDATA%/AutoFinalRobo2Loader/backups/last-mega-backup.json`. Conserva
  históricos y el índice anterior si falla o se omite el backup.
- **Flashear último backup** muestra origen/destino, comprueba hashes/hfuse,
  escribe solo aplicación y la lee después completa, incluyendo FF finales.
  Si quedan bytes residuales se informa fallo; no ejecuta borrado automático.
  No programa bootloader/fuses/EEPROM. Su aceptación física sigue pendiente.
- Las antiguas copias de HEX de PC en recovery/ se conservan como archivos;
  no son respaldos del chip para este menú. El botón principal carga el HEX
  configurado cuando se quiere un firmware conocido.

## Evidencia a registrar durante la prueba

| Paso | Evidencia | Criterio |
| --- | --- | --- |
| Compilar | Rutas, layout, salida, imágenes/hash | Herramientas finalizaron correctamente |
| Backup opcional | Lecturas, hfuse, hashes, manifest | Dos lecturas coincidentes publicadas |
| Instalar flasher | COM, baud, HEX/hash, avrdude | Firma y bytes verificados |
| Consultar ESP | Comando, baud, tamaño, salida | Respuesta ROM y capacidad esperada |
| Firmware/FS | BIN/hash/offset/layout, esptool | Escritura y verificación completas |
| Firmware Mega | HEX/hash o manifest backup, lectura posterior cuando aplica | Programa deseado verificado |
| Cambio a B | Ruta de niveles/cableado, log boot y ROM | Consulta correcta antes de escribir |

Logs: LOCALAPPDATA/AutoFinalRobo2Loader/logs; backups: backups/; servicio: service/;
preferencias: preferences.json. Ningún paso se marca aprobado por entregar fuente.

[Servicio AVR](../../AutoFinalRobo2-Atmega/service/esp_uart_bridge.c) ·
[Investigación eléctrica](MEGA_ESP_PUENTE_SOFTWARE.md) ·
[Loader](../../AutoFinalRobo2-Loader/README.md) ·
[Ficha05](../changes/2026-10-28-05-loader-mega-backups.md).
