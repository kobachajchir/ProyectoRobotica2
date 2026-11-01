# AFR2-20261028-25 — Loader 1.0.0

Pedido: botón Leer ESP, manual, consulta automática de capacidad y orden de pestañas; promoción de versión a 1.

Adaptaciones por archivo:
- MainForm.Serial.cs: Leer ESP abre la guía, como Leer ATmega.
- MainForm.Preparation.cs: reemplaza declaración manual por consulta asíncrona; requiere repetir la acción dependiente tras terminar.
- MainForm.cs: Detectar ESP usa la capacidad retornada por Probe/esptool para actualizar FlashMB y confirmar; rechaza capacidades fuera del rango admitido. Ordena pestañas y añade ATmega como acceso al flujo existente.
- MainForm.EspModes.cs: textos de consulta y evidencia de capacidad detectada.
- JobState.cs: versión 1.0.0.
- MANUAL.html: lectura ESP, AT frente a firmware propio, límites de baudios, consulta flash_id y orden de navegación.

Autorías existentes conservadas; adaptación mediante Codex por pedido del usuario. Estado: fuente/documentación editadas, sin compilación, tests, validadores, GUI, comandos físicos, puertos ni flasheos. La versión 1.0.0 es una denominación solicitada, no certificación de funcionamiento. ATmega/ESP/Web: sin cambios por esta ficha.
