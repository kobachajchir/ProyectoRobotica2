# AMI-20261005-06 — error de codigo local y acceso QT visible

- Fecha: 2026-10-05 (America/Buenos_Aires).
- Repositorio de origen: Web ESP01.
- Referencia de codigo: cambios locales de esta tarea en UserContext.tsx, PinScreen.tsx, Login.tsx, Home.tsx e index.css; checkout con trabajo previo preservado.
- Estado del origen: implementado en fuentes, sin compilacion ni validacion.
- Area: flujo de login local y representacion Web.

## Comportamiento anterior y nuevo

En desarrollo, el PIN fijo de acceso local crea una dev-session sin placa. Un codigo diferente intentaba esp.auth.pin.login incluso cuando el WebSocket no estaba listo, mostrando Sin comunicacion con la placa y el detalle tecnico del socket.

Ahora se conserva el acceso local existente. Si se ingresa otro codigo de cuatro digitos en desarrollo y el WebSocket no esta conectado, login devuelve localmente {ok:false, reason:"invalid-pin", message:"Código incorrecto"}; no envia solicitudes ni crea una sesion. Con WebSocket listo sigue disponible el login real con F4. Produccion mantiene el flujo remoto.

La pantalla de login muestra solamente Código incorrecto ante invalid-pin sin bloqueo, ademas del boton Volver a intentar. Omite el mensaje adicional, diagnosticos de transporte e intentos restantes en ese caso. Los bloqueos conservan su espera y los errores reales de transporte conservan su diagnostico. El modal PIN de otras pantallas mantiene su presentacion, porque la opcion compactInvalidPinError se activa solo en Login.

El indicador QT / USB · PF conserva su accion de abrir el modal de diagnostico y agrega un chevron hacia abajo. El grid reserva una columna para el icono; los indicadores informativos mantienen sus tres columnas. No cambia navegacion OLED, screen codes ni reportes remotos.

## Flujo completo y contrato remoto

1. Login recibe cuatro digitos. La entrada local de desarrollo se resuelve antes del transporte; un codigo distinto sin WebSocket devuelve invalid-pin.
2. Cuando corresponde login remoto se conserva esp.auth.pin.login con {pin:string}, AUTH_VALIDATE_PIN 0x51 y reconciliacion esp.auth.session.get / AUTH_REMOTE_RESULT 0x5D. No cambian payloads, longitudes, endian, ACK, correlacion, TTL, timeouts ni eventos.
3. Una denegacion invalid-pin se presenta con el texto compacto del login; un bloqueo sigue impidiendo reintentar durante su plazo. La opcion compacta solo modifica presentacion, sin alterar permisos ni estado F4.
4. Volver a intentar limpia la entrada y retorna al formulario. QT abre su modal existente, con las mismas consultas y limitaciones remotas.

## Impacto en repositorios

| Repositorio | Estado | Accion o motivo concreto |
| --- | --- | --- |
| FirmwareF4 | no_aplica | Ajuste local de Web; no se modifican solicitudes ni respuestas F4. |
| Firmware ESP01 | no_aplica | No recibe una solicitud en el rechazo local; contrato remoto preservado. |
| Web ESP01 | origen | Rechazo local explicito, presentacion compacta del error y chevron QT. |
| AutoMicroQtDesktop | no_aplica | Sin cambios de su login, API ni transporte; QT aqui es el indicador del dashboard Web. |

## Integracion y limites

El cambio local requiere usar las fuentes Web actualizadas. Para la Web servida desde ESP queda pendiente reconstruir y cargar el bundle, por instruccion del usuario. No requiere modificar firmware para estos ajustes.

Sin tests, compilaciones, validaciones, revision visual ni prueba fisica. No se afirma interoperabilidad nueva. Los cambios del checkout Web permanecen locales sin commit ni push de esta tarea; solo se publica este registro central.
