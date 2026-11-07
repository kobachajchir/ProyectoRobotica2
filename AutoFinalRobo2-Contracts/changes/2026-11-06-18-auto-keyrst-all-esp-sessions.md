# AFR2-20261106-18 · KEY/RST automático en sesiones ESP

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex.
Pedido: aplicar automático a consulta de tamaño, pruebas, cargas y demás donde
corresponda. Compilar sin abrir puerto no debe reiniciar hardware.

## Diagnóstico y alcance

El registro 20261106-202622-560-93b6f9.txt ya contiene --auto-key-rst,
AUTO_ON y confirmación de ENTER_BOOT. La causa de ese fallo no se puede describir
como ausencia de orden automática: después no hubo respuesta ROM. La prueba
independiente de boot de AFR2-20261106-17 sí recibió mode:(1,7), sin errores.

Se comparte ahora esa captura comprobada con todas las sesiones esptool del
puente: detectar ESP, consultar capacidad, firmware, FileSystem, combinado,
paquete AT y solicitudes MCP que usan las mismas operaciones. En cada apertura
del COM se configura automático, se ejecuta KEY/RST y se verifica boot antes de
la sincronización ROM, sin cerrar/reabrir el COM entre ambos pasos.

Flasher con capacidad bit3: usa captura a 74880, exige status0 y mode1; UART1
restaurada al baud del perfil y pines sueltos, ROM aún en programación hasta
otro RST. Un error de captura detiene la sesión antes de consultar/escribir.
Flasher anterior: conserva ENTER_BOOT, valida estado y anuncia explícitamente
que no existe captura boot. El modo manual no envía órdenes automáticas.
Se solicita RUN al cerrar y AUTO_OFF, con lectura de vuelta; los mensajes
distinguen limpieza confirmada de no confirmada. No se deduce arranque de
aplicación por solo recibir el ACK del flasher.

## Adaptaciones por archivo

- AutoFinalRobo2-Loader/tools/esp_bridge_esptool.py: extracción de
  capture_programming_boot, compartida por diagnóstico y transporte esptool;
  limpieza y respuestas de control comprobadas.
- AutoFinalRobo2-Loader/src/Operations.cs: evento bootloader_observed solo por
  marcador de captura exitosa de la sesión; diagnóstico separado de falla ROM;
  estado automático central en EspArgs, reutilizado por todas las cargas.
- AutoFinalRobo2-Loader/src/MainForm.cs: fin de sesión no afirma limpieza sin
  revisar el log. No se confunde boot con comunicación de ida/vuelta.
- AutoFinalRobo2-Loader/src/Operations.MegaBridge.cs: texto de restauración
  preciso sobre evidencias de reinicio y liberación de pines.
- AutoFinalRobo2-Loader/src/Operations.EspModes.cs: textos de cargas AT sin
  instrucciones de pulsación manual cuando está seleccionado automático.
- AutoFinalRobo2-Loader/src/MainForm.Workflow.cs: mensajes finales automáticos
  remiten a la confirmación del puente; no afirman un reinicio no observado.
- AutoFinalRobo2-Loader/src/FailureDetails.cs: boot UART confirmado sin respuesta
  ROM dirige a revisar PC→ESP, sin pedir KEY/RST manual en ese caso.

BuildMega, BuildEsp, BuildWebFs, BuildAll y revisión de paquetes siguen sin COM.
No se reinicia ESP al compilar, abrir la app ni cambiar preferencias. La carga
del flasher Mega mantiene sus checklists y no aplica KEY/RST al ESP.

## Verificación y estado

Loader C# compilado. Wrapper: nueve escenarios del diagnóstico y siete de
integración simulada PASS, sin COM: automático con captura, manual, flasher
anterior, boot distinto, ruido, error ROM tras boot válido y nueva apertura de
COM. Se verificaron orden de entrada antes de consulta, un solo dueño del COM,
entrada en cada apertura y limpieza única aun ante errores. No se escribió ESP.

No se modificó ni recompiló el firmware ATmega por esta ficha: se usa el
flasher de 1886 bytes cargado y verificado en AFR2-20261106-17. ESP/Web sin cambios.
La consulta física de tamaño con la nueva integración se completó; evidencia al final.

También adaptados MainForm.Preparation.cs y MainForm.Integration.cs: ApplyDetectedFlashCapacity compartido por GUI/MCP; consulta MCP invalida primero la confirmación anterior y actualiza perfil/layout solo tras capacidad física válida. Compilación de esa revisión completada en .tmp/auto-boot18. La ventana abierta ya usa el wrapper nuevo; esta última unificación de capacidad de la GUI requiere el siguiente inicio del ejecutable recompilado. Solicitud física de consulta c564892dd62c436fad1d1801970103ea en COM8, inicialmente awaiting_approval con autoaceptación desactivada; no se acredita tamaño hasta observar resultado.

## Resultado físico observado
Consulta MCP c564892dd62c436fad1d1801970103ea en COM8: completed a las 20:36:35 -03:00.
Registro 20261106-203625-958-b0d747.txt; WritingStarted=false.
Captura status0, 51 bytes, boot mode:(1,7); AFR2-AUTO: boot_mode=1 antes de Connecting.
En la misma sesión COM esptool 4.5.1 identifica ESP8266EX, Manufacturer 5e, Device 4016 y Detected flash size: 4MB (4194304 bytes).
Cierre: pines sueltos y pulso RST normal confirmados, AUTO_OFF con auto=no.
Esto acredita ida/vuelta ROM y tamaño de flash en esta prueba, no firmware de aplicación ni carga de imágenes.
Segunda consulta desde GUI: 20261106-203644-217-094574.json, Detectar ESP y consultar ROM, completed a las 20:36:52, sin escritura.
Luego el usuario inició compilación de firmware ESP (job 4f64c28fd57a47ca9d4c3fc067fdd297). No se reinició la GUI ni se interrumpió esa compilación para cargar la última unificación C# de capacidad GUI/MCP, compilada aparte; Iniciar.ps1 la reconstruirá en el próximo arranque.
Las rutas de escritura comparten EspArgs/wrapper, pero no se hizo una escritura ESP de prueba por esta ficha.
Revisión final: captura compartida fija temporalmente timeout=0.1 durante la lectura y restaura el timeout original de esptool incluso al fallar. Pruebas de integración con timeout inicial None PASS, verificando límite en todas las lecturas de control/captura y restauración antes de consultar ROM. Sin reinicios o consultas físicas adicionales para este ajuste.
