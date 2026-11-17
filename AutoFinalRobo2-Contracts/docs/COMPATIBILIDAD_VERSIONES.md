# Compatibilidad de versiones AutoFinalRobo2

Registro de qué versiones de Loader y firmware se usan juntas, a partir del momento en que el auto empezó a informar sus versiones (AFR2-20261117-09). Cada fila nueva se agrega arriba en la tabla de línea base cuando cambie la versión de producto o la del Loader.

## Línea base (17/11/2026)

| Componente | Versión | Dónde se define |
|---|---|---|
| **Loader** (Windows) | **2.5.0** | `AutoFinalRobo2-Loader/src/JobState.cs` → `LoaderVersion.Value` |
| **Firmware de producto** (ATmega, ESP y Web, versión única) | **1.36.0** | `AutoFinalRobo2-Contracts/contracts/afr2_build_info.h` y `AutoFinalRobo2-Web/package.json` |
| Extensión de producto UART (EX01) | v13 | `contracts/afr2_product_wire.h` → `AFR2_EXT_VERSION` |
| Catálogo de comandos UART | esquema `afr2-wire-catalog-1` | `contracts/wire_catalog.json` |
| Sobre WebSocket | `afr2-ws-1` | `contracts/commands.json` |

- **Loader 2.5.0** es el primero que lee las versiones del auto en la autodetección de puerto COM.
- **1.36.0** es el primer firmware que las informa.
- Con un firmware anterior, el Loader 2.5.0 lo detecta igual y muestra «sin versiones: firmware anterior a 1.36.0».
- Un Loader anterior ignora la línea `VER` y sigue detectando el puerto como antes.

## Cómo se informan

Autodetección del Loader (Configuración → General → «Autodetectar puerto COM al iniciar»):

1. **Consulta:** el Loader abre cada COM a 115200 sin tocar DTR/RTS (no reinicia la placa) y envía `#AFR2 TAP ?`.
2. **Respuesta del ATmega** (log USB, D1):
   - primero la confirmación de siempre, `TAP mode=<n> <nombre> baud=<baud>`;
   - desde 1.36.0, enseguida:

   ```
   VER mega=<versión ATmega> ext=<EX01> esp=<versión ESP> web=<versión Web>
   ```

   - `mega` es la versión con la que se compiló el ATmega.
   - `esp` y `web` son las que el ESP le informó por UART (`WIFI_UPDATE` subtipo 3). Si todavía no llegaron, `esp` sale de la trama de arranque `BT01` del ESP (mayor.menor.parche). Sin ninguna de las dos, `?`.
   - El ESP no habla por el USB: su versión llega a través del ATmega, así que hace falta el enlace ESP↔ATmega activo.
3. **Lectura en el Loader:** espera la línea `VER` hasta 700 ms después de la confirmación y la marca en dos lugares:
   - **Leyenda amarilla junto a Puerto:** `Autodetectar COM activo · COM16 (Firmware AFR2 · ATmega 1.36.0 · ESP 1.36.0 · Web 1.36.0 (115200) · igual al repo)`.
   - **Registro de operación:**
     - `Versiones detectadas: ATmega …, ESP …, Web …; Loader 2.5.0; repo …`;
     - después, `Compatibilidad de versiones: …` con las diferencias: ESP o Web distintas del ATmega, la versión del ESP todavía desconocida, o el auto distinto de la versión del repositorio del perfil (`afr2_build_info.h`).

Con el flasher de servicio (`esp_uart_bridge`) cargado en el ATmega, la autodetección informa «Flasher AFR2» sin versiones de producto.

## Reglas

- La versión de producto es una sola para ATmega, ESP y Web ([VERSIONADO.md](VERSIONADO.md)). Si el Loader informa versiones distintas, hay que cargar los tres módulos compilados juntos.
- El Loader no sube la versión de producto. Lleva la suya (`MAYOR.MENOR.PARCHE`) y se anota en esta tabla cuando cambia su manera de hablar con el auto.

## Historial

| Fecha | Loader | Producto | EX01 | Cambio |
|---|---|---|---|---|
| 17/11/2026 | 2.5.0 | 1.36.0 | v13 | AFR2-20261117-09: el auto informa versiones en la autodetección; línea base de compatibilidad |
