# Loader 1.1.2 — autorización de confirmaciones manuales por sesión

AFR2-20261029-12. Autoría Koba Chajchir / AFR2; adaptación mediante Codex.

## Estado y alcance

`loader_status.skipManualChecks` refleja una autorización local, inicialmente false en cada ventana nueva. No pertenece al perfil ni se escribe en preferencias. Es independiente de `autoAcceptRequests`. Su checkbox está debajo del de autoaceptación en Servidor MCP.

Al estar activo, el asistente Confirm da por aceptados sus textos/casillas de preparación/diagnóstico/carga y registra lo omitido. El aviso de continuar sin backup nuevo también se omite, conservando la elección de crear backup. Desde AFR2-20261029-13 / 1.1.2, la intervención KEY/RST del ESP está expresamente excluida de la omisión. La ventana se restaura y el aviso aparece al frente. Con escucha activa se pide Continuar antes de abrir el puerto y luego evidencia real del arranque; sin escucha o con puente software se exige una casilla local y Confirmar antes de consultar ROM. listenBoot:false no omite este requisito. No afecta al flasheo ATmega. Los mensajes informativos posteriores se conservan.

Siguen vigentes la exclusión de trabajos/propiedad del COM, disponibilidad de puerto, firma, capacidad física, layout, recibos de preparación, integridad de archivos, códigos de salida, escritura/verificación y fallos bloqueantes. No se alteran productores/consumidores de firmware ni el protocolo del robot.

## Activación

Una transición false→true, desde checkbox o MCP, exige una advertencia modal local específica con Rechazar y Aceptar y continuar. No usa Confirm ni las preferencias de checklists. La variable y el checkbox permanecen desactivados hasta aceptar. Escape/cierre/rechazo no activan. Desactivar no requiere advertencia. Reactivar requiere repetirla; cerrar la app revoca la autorización.

El texto explica responsabilidad del montaje físico (cables, alimentación, aislación, potencia de actuadores), alcance de las confirmaciones omitidas, continuidad de comprobaciones automáticas y necesidad de desactivar al cambiar el montaje.

## Herramienta MCP

`loader_request_session_options` recibe:

```json
{"skipManualChecks": true, "requestId": "sesion-checks-01"}
```

Devuelve jobId y autoAccepted=false. La GUI presenta la advertencia aun sin autoaceptación general. `loader_job` pasa por awaiting_approval/awaiting_local_confirmation y termina completed o rejected; los errores quedan failed. Autoaceptar nunca aprueba esta solicitud. La tool no ofrece un parámetro para aceptar la advertencia por servidor. Para desactivar usar false con un requestId nuevo. Se mantienen deduplicación y una solicitud/trabajo por vez. Una cancelación mientras la advertencia está abierta impide activar al aceptar después. Consultar el job y el estado real antes de pedir cargas.

## Evidencia

Implementado y compilado GUI/MCP; distribución AFR2-Loader-dist-1.1.1 y ZIP generados. Sin ejecución GUI/MCP, tests, validadores, COM, firmware ni pruebas físicas en esta revisión. ATmega/ESP/Web sin cambios por este contrato; no se acredita montaje ni hardware probado.

