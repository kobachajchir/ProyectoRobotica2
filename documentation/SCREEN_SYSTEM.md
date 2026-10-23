# Sistema de pantallas AutoFinalRobo2

[Maestro](../docs/INFORME_MAESTRO.md), [navegación](NAVIGATION.md), [catálogo](screens.json), [skill](../.agents/skills/afr2-screen-authoring/SKILL.md).

## Capas

1. Datos: snapshot pequeño y coherente; cada sensor tiene validez. Productor es adquisición/control, no pantalla.
2. Wrapper: elige vista/variante, prepara snapshot y aplica acciones semánticas. No conoce registros TWI.
3. Renderer: escribe pixels/texto en framebuffer prestado, sin GPIO/I2C, delay ni consultas a servicios.
4. OLED service: dirty pages y snapshot TX estable; inicialización/páginas asíncronas. No interpreta menú ni mueve motores.
5. I2C Manager: serializa una transferencia, deadline y callback en main. Backend AVR start/poll/abort pendiente.
6. Navegación: giro/corto/largo y opciones Volver; SW largo es menú, no atrás.

Código nuevo en Atmega/include/afr2_*.h y src/{display,services,input,ui}. La biblioteca previa completa está en Atmega/reference/f4/Core, incluida composición/menús/OSK. Los wrappers originales dependen de System/puertos y deben recortarse antes del port.

## Identidad y fidelidad

Los cuatro IDs iniciales son locales a UI; no tienen payload remoto implementado. Para cada pantalla conservar identidad, rol (vista/acción/invocador), renderer, campos/validez, acciones, navegación, variante y recursos. No asignar renderer a una acción solo por tener ID.

Home muestra el estado de base sin control activo; menú ofrece Sensores, Estado OLED y Volver; las dos vistas de diagnóstico solo tienen Volver seleccionable. Sensores muestran '--' sin dato válido. No hay GOTO/motores/Wi-Fi falsos en el prototipo. La pantalla física no se observó y no se generó un preview presentado como resultado de hardware.

Framebuffer: 128x64, bit vertical LSB por página, 1024 bytes. Snapshot TX 129 bytes permite actualizar framebuffer mientras se envía una página; cualquier redibujo marca dirty nuevamente. Puede haber mezcla temporal de páginas, no snapshot atómico de pantalla completa. La API nueva INVERT significa XOR; el F4 histórico tenía semánticas inconsistentes, por lo que no se declara paridad. Font7x10 conserva glifos y usa PROGMEM/pgm_read_byte en AVR, ASCII 32..126.

## Backend y extensión

Falta implementar TWI y conectar main/GPIO/clock. Módulo supuesto SSD1306 128x64 a dirección confirmada, no SH1106. Bloques start/poll/abort no deben bloquear ni llamar callbacks desde ISR. Reconfiguración/reinicialización solo con bus y servicio inactivos.

OSK, iconos, fuentes grandes, menús completos, canvas remoto, animaciones y réplicas Web quedan por portar mediante la skill. Los builders/SVG existentes son referencia y no acreditan igualdad pixel a pixel. Registrar capability/reporte nuevo en AFR2 antes de integrar consumidores.
