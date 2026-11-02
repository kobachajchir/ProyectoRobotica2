# AFR2-20261102-05 — Diseños OLED activos en fuente y QR dentro del auto

02/11/2026. El usuario detectó vistas antiguas en Ayuda y pidió QR de conexión
al AP y de acceso a la app Web generados dentro del auto.

- Dashboard con paneles, métrica de voltaje 5×7 ampliada y estado; menús con
  selección invertida; siete cards rápidas con contador superior; canales IR
  arriba, barras uniformes y lectura debajo; progreso común de 124×8.
- Componentes C/TS compartidos y gestos alineados, padding4; Volver usa encoder
  largo con L y flecha. Se mantienen corto/largo al soltar y guardas Sí/No.
- Ayuda → OLED incorpora Simular flujo usando el reductor local existente;
  Biblioteca sin datos continúa disponible y el visor remoto sigue separado.
- QR 030045/030046 en Wi-Fi, con SSID/clave/IP vigentes del snapshot del ESP;
  componente reutilizable para bytes propios, sin dependencias de red.
- QR AP privado y bloqueado para captura/entrada remota. Datos caducados, Web
  sin IP o AP apagado muestran indisponibilidad; generar exige detener auto/radar.
- Encoder Nayuki C/TS MIT, tablas C en PROGMEM, framebuffer como scratch, sin
  dos buffers globales adicionales. Recursos Web copiados sin comprobaciones.

Estado ATmega: fuente, no compilada ni flasheada. Estado Web: fuente y biblioteca
generada, sin build/navegador/promoción. Estado ESP: transporte existente, sin
cambios de esta ficha ni nuevo filesystem. Sin tests, validaciones, decoder,
medición de memoria/tiempo, ensayo físico, commit ni push.

[Contrato y límites](../contracts/OLED_RUNTIME_QR_20261102.md).
