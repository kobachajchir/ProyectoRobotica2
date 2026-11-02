# AFR2-20261102-14 — Dashboard con estados en reposo y pantallas vinculadas

Pedido humano: implementar en el ATmega el diseño de modo y panel lateral,
la rotación por inactividad y alinear las pantallas con lo acordado.

ATmega: módulo `afr2_dashboard.c`, snapshot real del producto, actividad del
encoder incluida la pulsación mantenida, caché y redibujo por regiones.
Inactividad3s, permanencia4s, fallo prioritario fijo y retiro de estados caducados.
Sin buffers de pantalla adicionales ni cambios de acciones/wire/EEPROM.

Web: renderer equivalente, reductor de simulación y aviso de SW mantenido en
EncoderControl. Se unifican cabeceras/datos/contadores/selectores/editores y
confirmación Sí/No en las dos implementaciones. Se conservan las composiciones
compactas de radar/Hall/prueba de encoder, los avisos y la excepción QR.

Submenús mantienen flecha grande + Volver; demás páginas mantienen gestos. OLED
blanca, SSID/IP con margen2px y scroll, iconos comunes y progreso124×8.

Estado ATmega/Web: fuentes integradas y recursos copiados sin checks. ESP:
transporte reutilizado, sin cambio de esta ficha ni promoción de paquete.
Sin tests, builds, validadores, navegador, hardware, flasheo, commit ni push.
Memoria/Flash/stack/latencia y comportamiento físico pendientes.

[Contrato y límites](../contracts/OLED_DASHBOARD_STATUS_20261102.md).
