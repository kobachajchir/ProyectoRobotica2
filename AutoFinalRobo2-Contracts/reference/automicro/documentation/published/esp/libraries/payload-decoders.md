# Decodificadores de payload F4

ID: `esp:module/src/f4_payload_decoders.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Transforma bytes LE de F4 a estructuras de datos del ESP con comprobación de formato/longitud.

MPU, IR, control, telemetría, preferencias y otros formatos definidos; las estructuras no se copian por sizeof al wire.

<a id="modularity"></a>
## Modularidad observada

C sin dependencia de UI; depende del esquema compartido de datos del robot.

<a id="state"></a>
## Handle, estado y atributos externos

El llamador posee salida; los bytes de entrada son prestados durante el decode. Flags/unidades se interpretan según cada formato.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Comprobar resultado F4_DECODE_* antes de usar salida.
- El bridge convierte posteriormente los campos a JSON y sus nombres públicos.

<a id="interactions"></a>
## Interacciones

- reports.inc usa decoders antes de publicar snapshots.
- Web consume números JSON; no vuelve a interpretar padding ni punteros C.

<a id="limitations"></a>
## Precauciones y límites

- Cada formato mantiene su tamaño/versionado; igual longitud no implica igual esquema.
- Mantener escalas g/mg, dps/mdps y m/s frente a cm/s.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/f4_payload_decoders.c` | c-cpp-structural | `7a6f32adb0ba4c839d10d2eff03f946124621eaf43a3eb78f0e9b2207d966051` |
| `src/f4_payload_decoders.h` | c-cpp-structural | `20e379a1fea2e56a2f22479cbead9c11bd1387cffb9455a76be68d9c19b342c6` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `F4_MpuSnapshot`

`esp:src/f4_payload_decoders.h:30`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint8_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint16_t sample_sequence` | Dato de estado/configuración `sample_sequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t sample_dt_us` | Tiempo o intervalo en microsegundos; precisión efectiva dependiente del puerto. |
| `float roll_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float pitch_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float yaw_magnetic_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float accel_g[3]` | Dato de estado/configuración `accel_g`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float gyro_deg_s[3]` | Dato de estado/configuración `gyro_deg_s`; declaración estructural, interpretación específica pendiente de ampliar. |

### `F4_StreamStateResponse`

`esp:src/f4_payload_decoders.h:42`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint8_t active` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t period_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `F4_StatusResponse`

`esp:src/f4_payload_decoders.h:48`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |

### `F4_SetPreferencesResponse`

`esp:src/f4_payload_decoders.h:52`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint32_t changed_mask` | Dato de estado/configuración `changed_mask`; declaración estructural, interpretación específica pendiente de ampliar. |

### `F4_PreferencesSnapshot`

`esp:src/f4_payload_decoders.h:57`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint8_t mode` | Dato de estado/configuración `mode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t descriptor_count` | Dato de estado/configuración `descriptor_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t field_mask` | Dato de estado/configuración `field_mask`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t encoder_inverted` | Dato de estado/configuración `encoder_inverted`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t screen_lock_enabled` | Dato de estado/configuración `screen_lock_enabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t screen_lock_timeout_sec` | Dato de estado/configuración `screen_lock_timeout_sec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t left_min_pwm` | Dato de estado/configuración `left_min_pwm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t right_min_pwm` | Dato de estado/configuración `right_min_pwm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t system_action_timeout_sec` | Dato de estado/configuración `system_action_timeout_sec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t usb_alive_period_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t oled_rotation_degrees` | Dato de estado/configuración `oled_rotation_degrees`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t oled_auto_rotation_enabled` | Dato de estado/configuración `oled_auto_rotation_enabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t uner_router_forwarding_enabled` | Dato de estado/configuración `uner_router_forwarding_enabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t black_line_width_mm` | Dato de estado/configuración `black_line_width_mm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t mpu_calibration_valid` | Dato de estado/configuración `mpu_calibration_valid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t has_pin_lock_enabled` | Dato de estado/configuración `has_pin_lock_enabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t pin_lock_enabled` | Dato de estado/configuración `pin_lock_enabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t has_web_alive_period_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t web_alive_period_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t record_size` | Dato de estado/configuración `record_size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t record_capacity` | Dato de estado/configuración `record_capacity`; declaración estructural, interpretación específica pendiente de ampliar. |

### `F4_TelemetrySnapshot`

`esp:src/f4_payload_decoders.h:82`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint8_t backend` | Dato de estado/configuración `backend`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t wifi_mode` | Dato de estado/configuración `wifi_mode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bridge_connected` | Dato de estado/configuración `bridge_connected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t capability_mask` | Dato de estado/configuración `capability_mask`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t i2c_policy` | Dato de estado/configuración `i2c_policy`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t motion_armed` | Dato de estado/configuración `motion_armed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t balance_active` | Dato de estado/configuración `balance_active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t command_active` | Dato de estado/configuración `command_active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t safety_stop` | Dato de estado/configuración `safety_stop`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float pitch_measured_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float pitch_setpoint_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float balance_output_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float left_output_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float right_output_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float kp` | Dato de estado/configuración `kp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float ki` | Dato de estado/configuración `ki`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float kd` | Dato de estado/configuración `kd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float roll_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float pitch_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float yaw_magnetic_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint8_t mpu_stream_active` | Dato de estado/configuración `mpu_stream_active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t mpu_stream_period_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t left_min_pwm` | Dato de estado/configuración `left_min_pwm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t right_min_pwm` | Dato de estado/configuración `right_min_pwm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t raw_adc[8]` | Dato de estado/configuración `raw_adc`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float pid_error_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float pid_integral_deg_s` | Dato de estado/configuración `pid_integral_deg_s`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float pid_derivative_deg_s` | Dato de estado/configuración `pid_derivative_deg_s`; declaración estructural, interpretación específica pendiente de ampliar. |

### `F4_BalancePidResponse`

`esp:src/f4_payload_decoders.h:114`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `float kp` | Dato de estado/configuración `kp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float ki` | Dato de estado/configuración `ki`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float kd` | Dato de estado/configuración `kd`; declaración estructural, interpretación específica pendiente de ampliar. |

### `F4_SetBalancePidResponse`

`esp:src/f4_payload_decoders.h:121`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint8_t changed_mask` | Dato de estado/configuración `changed_mask`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float kp` | Dato de estado/configuración `kp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float ki` | Dato de estado/configuración `ki`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float kd` | Dato de estado/configuración `kd`; declaración estructural, interpretación específica pendiente de ampliar. |

### `F4_ControlSnapshot`

`esp:src/f4_payload_decoders.h:129`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint8_t car_mode` | Dato de estado/configuración `car_mode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t line_follow_fsm` | Dato de estado/configuración `line_follow_fsm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t stable_line_pattern` | Dato de estado/configuración `stable_line_pattern`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t line_confidence_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `uint8_t line_ambiguous` | Dato de estado/configuración `line_ambiguous`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t line_reject_reason` | Dato de estado/configuración `line_reject_reason`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t line_width_mm` | Dato de estado/configuración `line_width_mm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float lateral_error_mm` | Dato de estado/configuración `lateral_error_mm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float normalized_lateral_error` | Dato de estado/configuración `normalized_lateral_error`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float linear_acceleration_m_s2` | Dato de estado/configuración `linear_acceleration_m_s2`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float estimated_velocity_m_s` | Dato de estado/configuración `estimated_velocity_m_s`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float yaw_rate_deg_s` | Dato de estado/configuración `yaw_rate_deg_s`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t estimator_confidence_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `uint8_t estimator_flags` | Dato de estado/configuración `estimator_flags`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float common_motion_output_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float differential_motion_output_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float turn_limit_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `uint8_t saturation_limited` | Dato de estado/configuración `saturation_limited`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t usb_active` | Dato de estado/configuración `usb_active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t usb_alive_period_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t usb_alive_accepted_count` | Dato de estado/configuración `usb_alive_accepted_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t usb_rx_overflow_count` | Dato de estado/configuración `usb_rx_overflow_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t usb_tx_queue_full_count` | Dato de estado/configuración `usb_tx_queue_full_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t usb_queued_tx_count` | Dato de estado/configuración `usb_queued_tx_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t usb_tx_drop_count` | Dato de estado/configuración `usb_tx_drop_count`; declaración estructural, interpretación específica pendiente de ampliar. |

### `F4_FirmwareInfo`

`esp:src/f4_payload_decoders.h:158`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint8_t name_len` | Dato de estado/configuración `name_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t version_len` | Dato de estado/configuración `version_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t target_len` | Dato de estado/configuración `target_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char name[F4_FIRMWARE_TEXT_MAX + 1u]` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char version[F4_FIRMWARE_TEXT_MAX + 1u]` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char target[F4_FIRMWARE_TEXT_MAX + 1u]` | Dato de estado/configuración `target`; declaración estructural, interpretación específica pendiente de ampliar. |

### `F4_BuildInfo`

`esp:src/f4_payload_decoders.h:168`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint8_t date_len` | Dato de estado/configuración `date_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t time_len` | Dato de estado/configuración `time_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t profile_len` | Dato de estado/configuración `profile_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t compiler_len` | Dato de estado/configuración `compiler_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char date[F4_BUILD_SHORT_TEXT_MAX + 1u]` | Dato de estado/configuración `date`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char time[F4_BUILD_SHORT_TEXT_MAX + 1u]` | Dato de estado/configuración `time`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char profile[F4_BUILD_SHORT_TEXT_MAX + 1u]` | Dato de estado/configuración `profile`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char compiler[F4_COMPILER_TEXT_MAX + 1u]` | Dato de estado/configuración `compiler`; declaración estructural, interpretación específica pendiente de ampliar. |

### `F4_BootReport`

`esp:src/f4_payload_decoders.h:180`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t schema` | Dato de estado/configuración `schema`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint8_t handoff` | Dato de estado/configuración `handoff`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t reset_flags` | Dato de estado/configuración `reset_flags`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t app_validation_flags` | Dato de estado/configuración `app_validation_flags`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t extension_profile` | Dato de estado/configuración `extension_profile`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t bootloader_version` | Dato de estado/configuración `bootloader_version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t app_version` | Dato de estado/configuración `app_version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t app_size` | Dato de estado/configuración `app_size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t app_crc32` | Dato de estado/configuración `app_crc32`; declaración estructural, interpretación específica pendiente de ampliar. |

### `F4_IrSnapshot`

`esp:src/f4_payload_decoders.h:193`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint8_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint16_t sample_sequence` | Dato de estado/configuración `sample_sequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t stream_period_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t hal_tick_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t raw_adc[8]` | Dato de estado/configuración `raw_adc`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t normalized[8]` | Dato de estado/configuración `normalized`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t line_pattern` | Dato de estado/configuración `line_pattern`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t route_alignment` | Dato de estado/configuración `route_alignment`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t confidence_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `uint8_t ambiguous` | Dato de estado/configuración `ambiguous`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t line_width_mm` | Dato de estado/configuración `line_width_mm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float lateral_error_mm` | Dato de estado/configuración `lateral_error_mm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float normalized_lateral_error` | Dato de estado/configuración `normalized_lateral_error`; declaración estructural, interpretación específica pendiente de ampliar. |

### `F4_GetApConfigResponse`

`esp:src/f4_payload_decoders.h:239`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint8_t apply_state` | Dato de estado/configuración `apply_state`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t ap_protected` | Dato de estado/configuración `ap_protected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t password_set` | Dato de estado/configuración `password_set`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t ssid_len` | Dato de estado/configuración `ssid_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char ssid[33]` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t ip[4]` | Dato de estado/configuración `ip`; declaración estructural, interpretación específica pendiente de ampliar. |

### `F4_SetApConfigResponse`

`esp:src/f4_payload_decoders.h:251`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint8_t changed_mask` | Dato de estado/configuración `changed_mask`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t apply_state` | Dato de estado/configuración `apply_state`; declaración estructural, interpretación específica pendiente de ampliar. |

### `F4_IrEmitters`

`esp:src/f4_payload_decoders.h:263`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t objects` | Dato de estado/configuración `objects`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t line` | Dato de estado/configuración `line`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t lights` | Dato de estado/configuración `lights`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-f4_decodebalancepidresponse"></a>`F4_DecodeStatus F4_DecodeBalancePidResponse(const uint8_t *payload, size_t len, F4_BalancePidResponse *out)` | Coordina `read_f32_le`, `require_fixed` para Decodificadores de payload F4. | `esp:src/f4_payload_decoders.c:169`; structural |
| <a id="fn-f4_decodebootreport"></a>`F4_DecodeStatus F4_DecodeBootReport(const uint8_t *payload, size_t len, F4_BootReport *out)` | Coordina `memset`, `read_u32_le`, `require_fixed` para Decodificadores de payload F4. | `esp:src/f4_payload_decoders.c:283`; structural |
| <a id="fn-f4_decodebuildinfo"></a>`F4_DecodeStatus F4_DecodeBuildInfo(const uint8_t *payload, size_t len, F4_BuildInfo *out)` | Construye Info dentro de Decodificadores de payload F4. | `esp:src/f4_payload_decoders.c:263`; inferred-name |
| <a id="fn-f4_decodecontrolsnapshot"></a>`F4_DecodeStatus F4_DecodeControlSnapshot(const uint8_t *payload, size_t len, F4_ControlSnapshot *out)` | Coordina `memset`, `read_f32_le`, `read_u16_le` para Decodificadores de payload F4. | `esp:src/f4_payload_decoders.c:192`; structural |
| <a id="fn-f4_decodefirmwareinfo"></a>`F4_DecodeStatus F4_DecodeFirmwareInfo(const uint8_t *payload, size_t len, F4_FirmwareInfo *out)` | Coordina `decode_text_field`, `memset` para Decodificadores de payload F4. | `esp:src/f4_payload_decoders.c:245`; structural |
| <a id="fn-f4_decodegetapconfigresponse"></a>`F4_DecodeStatus F4_DecodeGetApConfigResponse(const uint8_t *payload, size_t len, F4_GetApConfigResponse *out)` | Consulta Ap Config Response dentro de Decodificadores de payload F4. | `esp:src/f4_payload_decoders.c:328`; inferred-name |
| <a id="fn-f4_decodeiremitters"></a>`F4_DecodeStatus F4_DecodeIrEmitters(const uint8_t *payload, size_t len, F4_IrEmitters *out)` | Adapta la llamada a `require_fixed` con el contexto de esta integración. | `esp:src/f4_payload_decoders.c:379`; structural |
| <a id="fn-f4_decodeiremittersresult"></a>`F4_DecodeStatus F4_DecodeIrEmittersResult(const uint8_t *payload, size_t len, uint8_t *status, F4_IrEmitters *out)` | Coordina `F4_DecodeIrEmitters`, `require_fixed` para Decodificadores de payload F4. | `esp:src/f4_payload_decoders.c:388`; structural |
| <a id="fn-f4_decodeirsnapshot"></a>`F4_DecodeStatus F4_DecodeIrSnapshot(const uint8_t *payload, size_t len, F4_IrSnapshot *out)` | Coordina `memset`, `read_f32_le`, `read_u16_le` para Decodificadores de payload F4. | `esp:src/f4_payload_decoders.c:304`; structural |
| <a id="fn-f4_decodempusnapshot"></a>`F4_DecodeStatus F4_DecodeMpuSnapshot(const uint8_t *payload, size_t len, F4_MpuSnapshot *out)` | Coordina `memset`, `read_f32_le`, `read_u16_le` para Decodificadores de payload F4. | `esp:src/f4_payload_decoders.c:34`; structural |
| <a id="fn-f4_decodepreferencessnapshot"></a>`F4_DecodeStatus F4_DecodePreferencesSnapshot(const uint8_t *payload, size_t len, F4_PreferencesSnapshot *out)` | Coordina `memset`, `read_u16_le`, `read_u32_le` para Decodificadores de payload F4. | `esp:src/f4_payload_decoders.c:83`; structural |
| <a id="fn-f4_decodesetapconfigresponse"></a>`F4_DecodeStatus F4_DecodeSetApConfigResponse(const uint8_t *payload, size_t len, F4_SetApConfigResponse *out)` | Configura Ap Config Response dentro de Decodificadores de payload F4. | `esp:src/f4_payload_decoders.c:364`; inferred-name |
| <a id="fn-f4_decodesetbalancepidresponse"></a>`F4_DecodeStatus F4_DecodeSetBalancePidResponse(const uint8_t *payload, size_t len, F4_SetBalancePidResponse *out)` | Configura Balance Pid Response dentro de Decodificadores de payload F4. | `esp:src/f4_payload_decoders.c:180`; inferred-name |
| <a id="fn-f4_decodesetpreferencesresponse"></a>`F4_DecodeStatus F4_DecodeSetPreferencesResponse(const uint8_t *payload, size_t len, F4_SetPreferencesResponse *out)` | Configura Preferences Response dentro de Decodificadores de payload F4. | `esp:src/f4_payload_decoders.c:74`; inferred-name |
| <a id="fn-f4_decodestatusresponse"></a>`F4_DecodeStatus F4_DecodeStatusResponse(const uint8_t *payload, size_t len, F4_StatusResponse *out)` | Adapta la llamada a `require_fixed` con el contexto de esta integración. | `esp:src/f4_payload_decoders.c:66`; structural |
| <a id="fn-f4_decodestreamstateresponse"></a>`F4_DecodeStatus F4_DecodeStreamStateResponse(const uint8_t *payload, size_t len, F4_StreamStateResponse *out)` | Coordina `read_u16_le`, `require_fixed` para Decodificadores de payload F4. | `esp:src/f4_payload_decoders.c:56`; structural |
| <a id="fn-f4_decodetelemetrysnapshot"></a>`F4_DecodeStatus F4_DecodeTelemetrySnapshot(const uint8_t *payload, size_t len, F4_TelemetrySnapshot *out)` | Coordina `memset`, `read_f32_le`, `read_u16_le` para Decodificadores de payload F4. | `esp:src/f4_payload_decoders.c:130`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-decode_text_field"></a>`static F4_DecodeStatus decode_text_field(const uint8_t *payload, size_t len, size_t *offset, char *dst, size_t dst_capacity, uint8_t *decoded_len)` | Coordina `memcpy` para Decodificadores de payload F4. | `esp:src/f4_payload_decoders.c:227`; structural |
| <a id="fn-read_f32_le"></a>`static float read_f32_le(const uint8_t *p)` | Adapta la llamada a `read_u32_le` con el contexto de esta integración. | `esp:src/f4_payload_decoders.c:20`; structural |
| <a id="fn-read_u16_le"></a>`static uint16_t read_u16_le(const uint8_t *p)` | Operación read u16 le de Decodificadores de payload F4; semántica fina pendiente de ampliación. | `esp:src/f4_payload_decoders.c:7`; inferred-name |
| <a id="fn-read_u32_le"></a>`static uint32_t read_u32_le(const uint8_t *p)` | Operación read u32 le de Decodificadores de payload F4; semántica fina pendiente de ampliación. | `esp:src/f4_payload_decoders.c:12`; inferred-name |
| <a id="fn-require_fixed"></a>`static F4_DecodeStatus require_fixed(const uint8_t *payload, size_t len, const void *out, size_t expected)` | Operación require fixed de Decodificadores de payload F4; semántica fina pendiente de ampliación. | `esp:src/f4_payload_decoders.c:28`; inferred-name |
