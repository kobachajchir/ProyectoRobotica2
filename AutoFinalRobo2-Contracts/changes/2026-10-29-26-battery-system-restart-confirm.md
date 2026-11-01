# AFR2-20261029-26 — Batería local/Web y confirmación de reinicios

29/10/2026. Pedido humano: Sistema → Batería → Realizar medición, envío a Web, pill en cabecera como Red, icono variable por voltaje y confirmación antes de reiniciar ESP/ATmega. Adaptación propia por Codex conservando autores y cambios anteriores, sin editar referencias.

[Contrato completo](../contracts/BATTERY_MEASUREMENT_9.md). EX01v9, feature0x40000, comando0x55 Get/Measure; parser explícito en ambos extremos, WS `system.battery.get/measure`. A5 con divisor10k/10k y capacitor104 a masa. Adquisición cooperativa opt-in, promedio8, estados y edad; AVCC5000mV nominal sin calibrar. Sin EEPROM ni porcentaje.

OLED/TypeScript vinculados: Batería0x030039 y Medición0x03003A, acción69, lookup Sistema actualizado con Volver explícito. Renderer genérico conserva coordenadas/recursos; preview sin medición y visor framebuffer. Generación de catálogo pendiente de autorización. Pill comparte estilo de Red, con4barras por bandas de voltaje y contorno discontinuo sin dato válido, sin inventar porcentaje. Modal Web permite iniciar medida y ver estado.

Reinicios Web: modal compartido, foco en Cancelar; solo Confirmar y reiniciar envía orden; bloqueo de duplicados y cierre al perder conexión requerida. Condiciones físicas del firmware y evento real de boot conservados.

Estado atmega: fuente adaptada, no compilada/cargada/probada. ESP: puente/capability/parser adaptados, no compilados/cargados. Web: fuentes/pill/modales/copia OLED adaptados, sin build/render/generación/promoción. Contracts: contrato/catálogos/pinout/documentación/procedencia. CAD/Loader sin intervención de esta ficha. Nada de esta entrega acredita precisión, memoria suficiente, paridad ni hardware probado.

Sin tests, compilaciones, validadores, scripts de producto, generador OLED, navegador, COM, flasheos ni pruebas físicas. Archivos y atribuciones en `provenance/battery-system-20261029-26.json`. Desplegar ATmega/ESP/Web juntos por EX01v9 bajo autorización posterior.
