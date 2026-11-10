# AFR2-20261110-10 · Fallos y sensores en el auto

Versión: 1.18.0 (subversión).

Pedido: resolver desde el auto los fallos visibles en dashboard, mantener el cono HC-SR04 con servo apagado y mostrar batería de 8,5–9 V.

Implementación: Estado → Fallos 030007 reutilizado con código, explicación, indicación física y pie de acciones Detener/Limpiar/Volver. Limpieza local condicionada a STOP. Cono nominal siempre que existe ángulo conocido. Adquisición ADC5 automática desde arranque, sin modificar escala, validez ni circuito. Contrapartes raster y simulación local Web implementadas junto al AVR; ayuda Web de alertas y batería actualizada.

[Contrato, fuentes y límites](../contracts/OLED_FAULTS_RADAR_BATTERY_20261110.md).

Estado: atmega=fuente modificada; esp=solo versión compartida; web=fuentes vinculadas modificadas, recursos generados pendientes. No se ejecutaron tests, compilaciones, validaciones, generadores, navegador ni cargas. No confirma la causa física de cualquier fallo o lectura inválida restante. Cambios previos y ajenos preservados; sin commit/push.
