# Journal EEPROM v3 — AFR2-20261029-15

El usuario confirma motores y Hall de las cuatro ruedas ya cableados. La
captura AVR ya usa AFR2_HALL_CONNECTED=1 desde20261027-10, pero hall_available
también exige config.hall_wired=1. Un journal antiguo podía conservar0.

La versión local del journal pasa de2a3; mantiene payload58bytes, dos slots128,
CRC16, marcador de commit y selección por secuencia. Acepta v1, v2 y v3 válidos.
En v1 se conserva además la migración de UI descrita en
[migración2](EEPROM_UI_MIGRATION_2.md). En v1/v2 se establece hall_wired=1 por
la confirmación humana actual. Defaults nuevos también usan1. Las demás
preferencias/calibraciones se conservan; un v3 permite deshabilitar Hall
intencionalmente y guardar ese valor. No se rescatan registros corruptos.

La migración actúa en RAM al cargar; no escribe EEPROM automáticamente. La
acción existente Guardar EEPROM persiste v3. No borrar EEPROM para aplicar.
Una versión anterior puede no aceptar v3: no hay downgrade garantizado.

motor_ready representa «Habilitar control», no un detector de cableado. Se
conserva su valor; no se inventan CPR, geometría, polaridades ni calibración.
La prueba individual de motor no exige pose calibrada; movimiento general,
línea y GOTO conservan sus requisitos. La calibración Hall ya podrá comenzar
sin el bloqueo falso de desconexión cuando cargue esta revisión.

ROBO2/EX01, comandos, snapshots y persistencia ESP/Web no cambian. Mensajes
de Hall deshabilitado/odometría ahora describen habilitación y calibración,
sin afirmar ausencia física de los cables. Estado real de build/carga en la
[ficha15](../changes/2026-10-29-15-radar-watchdog-hall-wiring.md).
