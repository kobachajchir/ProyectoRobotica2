# Journal EEPROM v4 — AFR2-20261029-18

El usuario confirma que «encoders de línea» significa la barra IR1..IR5 del
kit. [OSOYOO](https://osoyoo.store/products/5-channel-tracking-sensor-for-osoyoo-model-3-robot-learning-kit-v2-model-2020001700)
documenta cinco salidas digitales con negro LOW/blanco HIGH. No se está
leyendo intensidad analógica del fototransistor a través de esos conectores.

Defaults de producto: line_kind=1 digital, line_invert=31 (cinco canales LOW
activos); line_calibrated sigue0. Habilita muestras crudas, no seguimiento.

Journal v4 conserva dos slots128bytes, payload58bytes, CRC y commit; acepta
v1..v4 válidos. Al leer v1/v2/v3, SOLO si line_kind=0, migra a digital1,
invert31 y calibración0. Configuraciones digital/ADC explícitas se preservan
con su polaridad/calibración. Un v4 deshabilitado intencionalmente se respeta.
Mantiene migraciones UIv2 y Hallv3 previas, calibraciones de ruedas/geometría,
motor_ready y preferencias restantes. No se recuperan registros corruptos.

Migra en RAM, sin escritura automática de EEPROM; Guardar EEPROM persiste v4.
No exige borrar preferencias. Downgrade no garantizado a firmware antiguo.
ROBO2/EX01 y snapshots conservan formato; versiones/validez son datos existentes.
Estado compilación/carga en [ficha18](../changes/2026-10-29-18-ir-input-acquisition.md).
