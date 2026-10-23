# Carpeta reference/web/src/contexts

[Maestro](../../../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../../../INFORME_ADAPTACION.md).

Fuentes históricas de referencia: no forman parte del producto adaptado.

## Utilidad y reformas específicas

### [Cliente WebSocket y estado](../../../../docs/modules/protocol.md)

Reutilizar promesas por requestId, respuesta/error/evento, cancelación al desconectar y estados de conexión. Contextos capturan APIs históricas y simulación.

Reformas: Crear cliente de producto/capabilities nuevo; no mandar stm.* antes del port ESP/Mega. Separar aceptación y final. Unknown/stale no se rellena con datos de demo. Adaptar dueño de mando/lease, reconexión y permisos. No inferir compatibilidad del catálogo TypeScript.

## Archivos vecinos

- [CarModeContext.tsx](<CarModeContext.tsx>)
- [EspWifiStatusContext.tsx](<EspWifiStatusContext.tsx>)
- [IrEmittersContext.tsx](<IrEmittersContext.tsx>)
- [SavedOledScreensContext.tsx](<SavedOledScreensContext.tsx>)
- [ScreenContext.tsx](<ScreenContext.tsx>)
- [ScreenStreamModalContext.tsx](<ScreenStreamModalContext.tsx>)
- [UNERProtocolContext.tsx](<UNERProtocolContext.tsx>)
- [UserContext.tsx](<UserContext.tsx>)
- [WebSocketContext.tsx](<WebSocketContext.tsx>)
- [WifiCredentialsContext.tsx](<WifiCredentialsContext.tsx>)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../../../AutoFinalRobo2-Contracts/provenance/files.json).
