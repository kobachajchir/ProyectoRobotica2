# PIN y autoridad F4 — contrato actual

ID: `shared:message/auth-pin/v1`. Fuente por lectura del 20-10-2026; sin ejecución
ni validación. Las etiquetas v1 identifican esta ficha documental y API JSON 1;
no agregan un byte de versión al payload UNER existente.

| Operación JSON | Args | Comando F4 | Payload request |
| --- | --- | --- | --- |
| `esp.auth.pin.login` | pin: string de exactamente cuatro dígitos | 0x51 | 0 authRequestId:u8; 1 scope=0; 2..5 cuatro dígitos numéricos 0..9. Total 6. |
| `esp.auth.pin.validateScreen` | pin + screenCode:u32 | 0x51 | Base con scope=1; 6..9 screenCode:u32 LE. Total 10. |
| `esp.auth.pin.change` | currentPin + newPin, ambos strings de cuatro dígitos | 0x51 | 0 authRequestId; 1 scope=2; 2..5 dígitos actuales; 6..9 dígitos nuevos. Total 10. |

El PIN JSON conserva ceros iniciales porque es string. ESP transforma cada ASCII
restando `'0'`: el wire contiene valores de dígito, no ASCII, BCD ni un entero
con los cuatro dígitos. No incluir ejemplos con credenciales reales. Los args
son vistas de ArduinoJson; el broker copia el payload que deba sobrevivir.

La correlación JSON requestId/command la conserva el transaction manager. El
authRequestId wire es otro contador u8 que evita cero al avanzar; no sustituye
requestId ni la generación de socket. El frame UNER sigue sin llevar cadenas
de requestId.

Respuesta 0x51: exactamente nueve bytes.

| Offset | Tipo | JSON / significado |
| --- | --- | --- |
| 0 | u8 | status |
| 1 | u8 | authRequestId |
| 2 | u8 | scope |
| 3 | u8 | granted, convertido a boolean |
| 4..7 | u32 LE | ttlMs |
| 8 | u8 | attemptsLeft |

El consumidor combina `status==0` y granted distinto de cero. ESP solo concede
stm_authenticated desde una respuesta de scope login; validar una pantalla no
concede por implicación una sesión general nueva. Cambio de PIN concedido
invalida las sesiones remotas y streams asociados. Mantener tiempo/TTL y
generación antes de aplicar un resultado.

El comando 0x5D `AUTH_REMOTE_RESULT` recibe action:u8 y devuelve seis bytes:
status en 0, authenticated en 1 y ttlMs:u32 LE en 2..5. Acción y concesión no
son el mismo dato; el cliente debe procesar resultado efectivo. La operación
histórica 0x59 figura deprecada en catálogo y no reemplaza el flujo actual.

La edición del PIN físico (digits/pinIndex), el diálogo de login Web, las
concesiones de pantalla y el estado de sesión son modelos distintos. `0x51`
verifica una entrada remitida; no captura los dígitos/cursor que está mostrando
el formulario F4. Ver [PIN/OSK](../SCREEN_SYSTEM.md#pin-osk) y
[AMDOC-WEB-002](../../../review/FINDINGS.md#amdoc-web-002).

Productores/consumidores inspeccionados:

- `web:src/contexts/UserContext.tsx` y componentes PinScreen.
- `esp:src/ws_bridge/esp_commands.inc:dispatch_pin_auth_command/dispatch_pin_change_command`.
- `esp:src/ws_bridge/reports.inc`, decoder y aplicación del resultado por sesión.
- `f4:Core/Src/uner_app.c`, handlers de autenticación y permisos.

Las guardas de intentos, timeout local y permisos F4 siguen activas; un ACK o
queued no concede autorización. Esta ficha no publica PIN, contraseñas ni tokens.
