# Journal EEPROM v2 — AFR2-20261029-10

Solo ATmega. No modifica ROBO2/EX01, comandos ni formato de snapshots Web/ESP.
Estado: implementado en fuente, sin compilar, cargar ni probar esta revisión.

El campo de versión de journal pasa de1a2. Se conservan dos slots128bytes,
payload58bytes, CRC16, commit0xA5 y elección por secuencia. Se aceptan versiones
1y2; los demás campos/restricciones siguen iguales.

Al cargar un slotv1 válido se conservan sus calibraciones y ajustes salvo
estos tres campos de UI, adaptados al montaje confirmado en diagnóstico09:

| Campo | Valor migrado/default |
| --- | --- |
| encoder_invert | 1 |
| rotation | 1, 180grados |
| encoder_steps | 4 |

Se valida antes de migrar; no se rescatan slots corruptos ni se infiere
calibración ausente. La migración no escribe EEPROM automáticamente. Guardar
por la acción existente escribe el journalv2. Los ajustes v2 se respetan,
incluidas modificaciones posteriores de sentido/orientación/pasos. Si no hay
slot válido se usan defaults seguros de producto con esta orientación UI.

No forzar restauración total ni borrar EEPROM para aplicar el montaje.
Un firmware antiguo que solo aceptev1 no leerá slotsv2; no hay compatibilidad
de downgrade garantizada. Esta versión describe persistencia local, no sube
la versión nominal de firmware ni declara probados memoria/movimiento.
