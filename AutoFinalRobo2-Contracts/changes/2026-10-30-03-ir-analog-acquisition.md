# AFR2-20261030-03 — Adquisición analógica de los cinco IR

30/10/2026. Cambio pedido expresamente por Koba Chajchir: leer IR1..IR5 con ADC y rango completo0..1023 en lugar de niveles digitales. Elaboración propia por Codex; autorías y cambios anteriores conservados.

[Contrato de adquisición y persistencia](../contracts/EEPROM_IR_ADC_6.md). A4..A0 sin cambios, defaults en modoADC2, buffers digitales deshabilitados solo para perfilADC/off y habilitados para modo digital explícito. Se conserva ADC único compartido con bateríaA5; conversión de descarte por cambio de canal IR.

Journal6, payload60bytes conservado: registros anteriores con modo distinto deADC pasan a2 en RAM y pierden únicamente la confirmación de calibración de línea del modo anterior. Umbrales, polaridad y resto de preferencias conservados. Sin escritura automática de EEPROM.

ESP/Web/OLED existentes ya admiten kind2 y raw0..1023; revisados en fuente, sin modificación de wire, versiónEX01, pantallas, layouts ni recursos. El servicio diagnóstico separado sigue digital. No se afirma que este cambio resuelva la causa física de los ceros ni que el módulo dé intensidad analógica.

Estado: ATmega solo fuente modificada; ESP/Web sin cambios de código por esta ficha. Sin compilar, tests, validadores, ejecución, generadores, promoción Web, COM, flasheos ni pruebas físicas. Configuración/calibración y funcionamiento real pendientes. Adaptaciones por archivo en `provenance/ir-analog-20261030-03.json`.
