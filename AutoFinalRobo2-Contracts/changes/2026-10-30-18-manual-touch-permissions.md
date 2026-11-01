> **Sustituido parcialmente por19:** por aclaración del usuario el táctil espera el modelo3D y su vista; se conserva el reintento tras error y el permiso de motores común. [Política vigente](2026-10-30-19-touch-model-ready.md).

# AFR2-20261030-18 — mismas condiciones para Mecanum y táctil

Usuario informa No disponible en control táctil y solicita poder mover como en Mecanum. Ruta común confirmada por lectura: robot.drive.pwm/Afr2Product_DrivePwm. No requiere odometría/CPR/HC ni validación formal del hardware; mantiene motores habilitados, ausencia de fallo/servicios incompatibles, enlace y STOP.

Diferencia real encontrada: el táctil añadía !error al permiso compartido; un error anterior persistente impedía la solicitud nueva que lo limpiaba. Corregido: error cancela el gesto y conserva STOP, con reintento por nuevo gesto solo cuando el permiso común se recupera. Los callbacks visuales de carga/fallo del modelo dejan de cancelar movimiento. Se muestra la razón común de enlace/STOP/PWM en ambos mandos, y último error separado del bloqueo actual. No se eliminan guardas del firmware ni se reanuda una consigna automáticamente.

Archivos: ControlPage.tsx, ManualDriveOverlay.tsx y DocsPage.tsx; contratos/flujo/informe/procedencia18. Autores y cambios previos conservados. ATmega/ESP auditados, sin edición; Loader/CAD sin cambios. Sin wire/ScreenCodes/EEPROM nuevos. No se ejecutaron compilación, tests, validadores, generadores, navegador, puertos o flasheos. No se puede identificar el motivo físico de hoy sin captura/telemetría; el usuario solo recuerda el texto genérico.

[Condiciones completas y pendientes](../contracts/MANUAL_TOUCH_PERMISSIONS_18.md). [Procedencia por archivo](../provenance/manual-touch-permissions-20261030-18.json). El paquete compilado14 es anterior: esta corrección sigue pendiente de build/despliegue/prueba autorizados.
