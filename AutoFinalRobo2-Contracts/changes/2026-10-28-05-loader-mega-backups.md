# AFR2-20261028-05 — Backup opcional y menú de firmware ATmega

28/10/2026. Pedido: casilla antes de instalar el flasher, aviso cuando se omite
con No mostrar de nuevo, preferencias persistentes y botón de firmware con
desplegable Flashear último backup. Guía por ambos métodos ESP para compañeros.

## Implementación en fuente Loader0.9.0

- Flasher ATmega agrega **Backup de ATmega antes de cargar**, marcado por defecto.
  El valor se puede cambiar por carga. Preferencias guarda su valor inicial y
  si debe aparecer el aviso de omisión en preferences.json del usuario.
- Desmarcado: Continuar sin backup / Cancelar y **No mostrar de nuevo**. Solo
  continuar persiste la supresión; se revierte en Preferencias. Los asistentes
  físicos y la confirmación de escritura siguen activos, incluso desde MCP.
- Backup real por avrdude: flash/hfuse leídos dos veces con -n y -D, sin -U de
  escritura. Firma esperada, lecturas coincidentes y aplicación no vacía antes
  de publicar. Si falla, se detiene la instalación; no usa una copia HEX de PC.
- Conserva exportados originales, flash normalizada de256KiB, aplicación
  separada según BOOTSZ de hfuse, hashes y manifest. No respalda EEPROM.
  Requiere BOOTRST compatible y lectura de flash/hfuse soportada por bootloader.
- El índice del último backup se publica al completar la lectura. Los archivos
  están en carpetas únicas bajo LOCALAPPDATA/AutoFinalRobo2Loader/backups.
  Omitir/fallar una lectura no reemplaza el índice anterior. La procedencia
  incluye fecha y COM: no se presume identidad de placa ni programa de producto.
- **Flashear Firmware ATmega** carga el HEX configurado. Su flecha ofrece
  **Flashear último backup**. Disponible directamente en Flasher ATmega y al
  seleccionar destino ATmega en Operación. Se elimina el botón Restaurar.
- Cargar backup exige hashes/tamaños coherentes y hfuse de destino coincidente;
  escribe solo aplicación sin -U de bootloader/fuses/EEPROM, luego lee flash
  para comparar toda la aplicación, incluida cola FF. No ejecuta erase_flash
  ni borrado automático frente a residuos o discrepancias. No se declara éxito
  únicamente por el texto de arranque o un retorno de escritura.
- Las copias HEX de PC de0.8 quedan como archivos históricos, sin convertirse
  en respaldos del chip. Los registros antiguos son legibles; los campos de
  copia legacy permanecen para deserialización, no para nuevas recuperaciones.
- MCP agrega flash_mega_backup. restore_mega_product queda como alias explícito
  de esa acción. loader_status expone casilla/aviso. La solicitud de instalar
  muestra qué opción de backup se aplicará. Cambiar preferencias sigue en GUI.
- Manual, plan A/B, mensajes finales y estados distinguen cargar servicio,
  firmware configurado y programa desconocido del backup. No se restaura al cerrar.

## Decisiones técnicas y límites pendientes

AVRDUDE6.3 recorta FF finales al exportar y cargar imágenes. Se conservan los
exportados y se normaliza su cola implícita para comparar lecturas; después
de cargar backup se verifica la aplicación completa por lectura. Una cola
residual implica fallo, sin atribuir una recuperación correcta. No se usa -A,
que no está disponible en esa versión. Una herramienta o bootloader que no
permita leer hfuse detiene el backup; el usuario debe resolver el error o elegir
explícitamente omitirlo antes de reintentar instalar el servicio.

Referencias consultadas:

- [AVRDUDE: operaciones -U y modo sin escritura](https://avrdudes.github.io/avrdude/7.3/avrdude_4.html).
- [Fuente6.3: opciones](https://github.com/avrdudes/avrdude/blob/v6.3/main.c),
  [lectura y límites de imagen](https://github.com/avrdudes/avrdude/blob/v6.3/avr.c),
  [entrada/salida de archivos](https://github.com/avrdudes/avrdude/blob/v6.3/fileio.c).
- [Datasheet Microchip ATmega2560, tabla29-13 BOOTSZ](https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega640-1280-1281-2560-2561-Datasheet-DS40002211A.pdf).

## Estado real de esta ficha

| Módulo | Estado |
| --- | --- |
| Loader | Fuente0.9.0 y documentación editadas; sin compilación, ejecución GUI/MCP, tests ni validaciones |
| atmega | Servicio fuente02/03 conservado; sin nuevo firmware ni compilación/carga por05 |
| esp | Sin cambios de firmware, compilación, consulta o flasheo por05 |
| web | Sin cambios, build ni promoción de activos por05 |
| CAD | Sin intervención por05 |

No se abrió COM ni se leyó una placa, creó backup físico o cargó firmware en
esta entrega. Pendiente aceptación real: lectura flash/hfuse por wiring,
contraste doble, omisión/cancelación y persistencia de preferencias, menú GUI,
lectura posterior completa, errores/cancelación y ambas rutas de flasheo ESP.
No se ejecutaron esas comprobaciones por instrucción del usuario.

[Plan para seguir en el equipo](../hardware/PLAN_PRUEBA_FLASHER_MEGA.md) ·
[Manual](../../AutoFinalRobo2-Loader/MANUAL.html#preferencias) ·
[Fuente backup](../../AutoFinalRobo2-Loader/src/Operations.MegaBackup.cs).
