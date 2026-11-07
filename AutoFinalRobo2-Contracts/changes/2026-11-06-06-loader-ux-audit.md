# AFR2-20261106-06 — Loader: auditoría UX y primera reorganización

06/11/2026. Pedido humano: auditar y reorganizar AFR2 Loader (frontend-design-audit + ux-audit) priorizando barra inferior, títulos, etapas, seguimiento del registro y siguiente requisito.

## Adaptaciones por archivo (`AutoFinalRobo2-Loader/src`)

- `MainForm.cs`: tabs y barra de actividad en una tabla raíz de dos filas; encabezado con altura medida; pie con fila de requisito; roles de botón explícitos; `AddButton` admite rol.
- `MainForm.Workflow.cs`: barra inferior en dos niveles con texto envolvente; etapas con filas auto-medidas, 8 px entre ellas y sobrante abajo; cálculo de altura requerida.
- `MainForm.Design.cs`: filas de Operación auto-medidas; `FitOperationRows` hace que el registro ceda altura primero; `ButtonStyle` respeta `ButtonRole`; columna de acción sin botón en campos de texto.
- `MainForm.Preparation.cs`: `NextStepText` muestra el próximo requisito (misma condición que habilita Flashear; no la modifica).
- `ActionButton.cs`: `ButtonRole` (Auto/Primary/Secondary/Info/Danger).
- `MainForm.Configuration.cs`, `MainForm.Serial.cs`, `MainForm.MegaBridge.cs`, `MainForm.Usability.cs`: ayuda única por campo, estado vacío en Eventos, filas compatibles con tema oscuro, backup secundario y posición del aviso emergente sobre la nueva barra.

El seguimiento del registro ya cumplía el criterio en `ExecuteCore`; no se modificó.

## Estado real

atmega=sin cambios, esp=sin cambios, web=sin cambios. Loader: solo fuente, revisada por lectura. **Sin compilar, abrir la interfaz, medir DPI/contraste/teclado ni abrir puertos o flashear.** La ventana abierta usa el ejecutable anterior. Informe y pendientes: `AutoFinalRobo2-Loader/ux-audit-2026-11-06-loader/report.md`.

## Segunda tanda (mismo ID)

`MainForm.Serial.cs` y `MainForm.SerialSend.cs`: tarjeta de transferencia plegable y envío avanzado. `MainForm.Converter.cs`: ejemplos y error junto a la entrada. `MainForm.Integration.cs`/`MainForm.McpConnection.cs`: Historial vacío y MCP en tres bloques. `MainForm.EspModes.cs`/`MainForm.Design.cs`: Operación en tres grupos y fechas legibles. `MainForm.Profiles.cs`: estado de guardado. `MainForm.Appearance.cs`/`ActionButton.cs`: colores por función y alto contraste. Fuente de formulario 10 pt. Compilado con `Compilar.ps1 -OutputDirectory` (carpeta temporal, sin tocar el ejecutable instalado): `AFR2Loader.exe` generado sin errores tras corregir `SystemColors.ButtonText`→`ControlText`. No se ejecutó la interfaz ni se abrieron puertos.

Tras abrir el ejecutable (1220×880, tema oscuro) se corrigió el reparto de alturas de Operación (`FitOperationRows`/`MeasureOperationActions`) y el estado vacío del Historial; capturas en `AutoFinalRobo2-Loader/ux-audit-2026-11-06-loader/assets/`. Sin puertos abiertos ni flasheos. Pendiente: ESP con operación en curso, tema claro, alto contraste, DPI y teclado.
