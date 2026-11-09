---
name: afr2-wire-commands
description: Alta, baja o modificación de comandos UART ROBO2/EX01 (CMD) entre ESP y ATmega en AutoFinalRobo2, manteniendo sincronizados headers, despachadores y el catálogo wire_catalog.json que usan el Loader y la Web.
---

# Comandos UART AutoFinalRobo2 (CMD)

Usar siempre que se agregue, quite o cambie un comando del enlace ESP ↔ ATmega: su ID, nombre,
largo de pedido o respuesta, campos, unidades, dirección, familia o si lleva datos privados. También
al cambiar la envoltura (prefijo de sesión/transacción, resultado, rutas) o una tabla de nombres
(resultados, causas de reset, eventos de red).

## Regla obligatoria (AFR2-20261108-09)

**Toda alta, baja o modificación (ABM) de un CMD actualiza en el mismo cambio el catálogo
`AutoFinalRobo2-Contracts/contracts/wire_catalog.json`.** No hay excepciones por tamaño del cambio
ni por tratarse de un comando "interno". Sin esa actualización el cambio está incompleto.

En el mismo cambio, además:

1. Header de Contracts que define el ID (`afr2_wire.h`, `afr2_product_wire.h`, `afr2_sensor_wire.h`,
   `afr2_network_wire.h`, `afr2_battery_wire.h` u otro nuevo).
2. `contracts/commands.json` si el comando tiene nombre WebSocket (`ws`) o cambia su estado.
3. `AutoFinalRobo2-Web/src/features/protocolStudio/catalog.ts` (y `codec.ts` si decodifica campos).
4. Subir `updated_by` del catálogo al ID AFR2 del cambio.
5. Ficha en `changes/` y entrada en `CHANGELOG.md` con estados reales atmega/esp/web.

## Cómo registrar el comando en el catálogo

- `id` en hex de dos dígitos en minúscula (`"0x1e"`), único. `name` en MAYÚSCULAS como en el header
  sin el prefijo `AFR2_EXT_`/`AFR2_CMD_`.
- `does`: qué hace, en una frase, en castellano.
- `request` / `reply`: `bytes` cuenta solo el cuerpo de aplicación, **sin** sesión, transacción ni
  resultado. Puede ser número, lista (`[12, 16]`), `"variable"` (con `min`/`max`) o `null` (retirado).
- `fields` (opcional pero recomendado): en orden, tipos `u8`, `i8`, `u16le`, `i16le`, `u32le`,
  `i32le`, `ascii:N`, `ascii:rest`, `bytes:N`, `bytes:rest`; `unit`, `format` (`hex`, `bin`),
  `enum` (tabla de `enums`) y `optional` (campos finales de un largo alternativo).
- `sensitive: true` si el payload, en cualquier dirección, puede llevar SSID, claves, la clave del AP
  u otro dato privado. El ATmega redacta esos comandos en la salida de depuración USB (TAP) y el
  Loader nunca los muestra. **Marcarlo es obligatorio**: olvidarlo filtra la clave por USB.
- `no_prefix: true` si no lleva sesión/transacción (hoy solo `0x52 BOOT`).
- Los largos se cotejan con el despachador real del ATmega (`afr2_link.c`, `afr2_app_wire.c`,
  `afr2_sensor_commands.c`, `afr2_network.c`, `afr2_sequence.c`) y con el emisor del ESP
  (`mega_link.cpp`, `broker*.cpp`), no con documentos históricos.

## Bajas

- Una baja **no borra** la entrada: queda con `"retired": true`, `bytes: null` y en `does` la versión
  y el ID AFR2 que la retiró. El ID **nunca se reutiliza** para otro comando.

## Lugares a revisar en un ABM

| Módulo | Archivos |
|---|---|
| Contracts | headers `afr2_*_wire.h`, `commands.json`, `wire_catalog.json`, `PROTOCOL.md` |
| ATmega | `src/services/afr2_link.c`, `afr2_app_wire.c`, `afr2_sensor_commands.c`, `afr2_network.c`, `afr2_sequence.c`; lista `sensitive_command()` de `afr2_debug.c` |
| ESP | `src/mega_link.cpp`, `src/broker*.cpp` |
| Web | `features/protocolStudio/catalog.ts`, `codec.ts`, `guidedBodies.ts` |
| Loader | nada que editar: lee el catálogo en tiempo de ejecución. Solo cambiar `WireCatalog.cs` si cambia el esquema `afr2-wire-catalog-1` |

## Límites

- Esta skill no autoriza compilar, flashear ni probar. Registrar el estado real.
- No poner credenciales ni valores de ejemplo con claves en el catálogo.
