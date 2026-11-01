# AFR2-20261028-13 — Feedback de carga en Loader 0.9.3

Autoría: Koba Chajchir; adaptación de fuentes propias AFR2 mediante Codex.

## Pedido y evidencia aportada

El usuario comunica que la detección del ESP usando el flasher del Mega funciona,
pero no recibe feedback al cargar firmware Web por ese método. Es evidencia
humana de detección en su montaje; no aportó aquí COM, baud, versión ejecutada,
salida, hash ni resultado de escritura/verificación. No se infieren esos datos.

## Cambio

- Runner consume stdout/stderr por bloques y separa CR, LF y backspace. Conserva
  mensajes en el registro, captura mensajes parciales sin salto de línea y limita
  fragmentos y memoria como antes. Drena ambos canales antes de continuar.
- Publica una instantánea de actividad cada medio segundo, con duración del
  proceso actual, último mensaje y tiempo sin nueva salida. No abre otro COM.
- El panel Operación presenta esos datos y un porcentaje cuando aparece una
  línea `Writing at 0x… …%`. Admite decimal y formatos CLI con guion o subrayado.
  La barra usa porcentaje real informado o actividad indeterminada si no existe.
  El porcentaje corresponde a la imagen actual y puede reiniciarse entre imágenes.
- Tras 15 segundos sin salida, el texto cambia de color. No implica que el
  dispositivo esté detenido ni que siga escribiendo. No cambia los timeouts.
- La barra de estado mantiene un resumen visible desde cualquier pestaña.
  Las esperas de confirmación mantienen sus mensajes propios. No hay estimación
  inventada de bytes, velocidad o porcentaje.
- Un 100% informado no marca éxito. Se conservan los eventos de operación,
  códigos de salida, consulta previa y verificación explícita existentes.
- Loader pasa a 0.9.3. Fuentes AVR del flasher, firmware ESP y Web intactos.

## Estado real

Implementado y revisado mediante lectura de fuentes/diff. Sin tests, validadores,
compilación, GUI, COM, flasheos ni pruebas físicas por el agente. Pendiente comprobar
el feedback con la versión de esptool del usuario y durante una escritura real.

Adaptaciones por archivo: `../provenance/loader-feedback-carga-20261028.json`.
