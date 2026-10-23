# Preferencias y persistencia

ID: `f4:module/Core/Inc/app_preferences.h` · Capa: **servicio** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Mantiene preferencias en RAM y registros persistidos versionados con migración, validación de valores y patches por máscara.

Configuración UI, motor, sesiones, red, calibraciones, perfiles y PIN; la geometría/operaciones Flash están en el adaptador STM32.

<a id="modularity"></a>
## Modularidad observada

Sin llamadas Flash HAL en el núcleo; comparte esquemas de AP, calibración y AutoMotion. Los setters históricos sin _H se adaptan por app_preferences_binding.h.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

AppPreferences_Handle posee copia RAM, dígitos secretos, próxima dirección/secuencia e información de almacenamiento; storage es puerto prestado. Get_H devuelve vista const, GetSnapshot_H copia. Patch.fields determina qué miembros values se aplican: los demás no se deben sobrescribir.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init_H explora registros, decide defaults/migración y prepara la siguiente escritura.
- ApplyPatch_H limita valores y persiste cambios según campos; los setters delegan en el mecanismo común.
- Storage.view/unlock/erase/program/lock son operaciones del backend; una tabla de callbacks no vuelve asíncrona la escritura.

## Trabajo en tándem

- APP aplica preferencias a encoder, display, control y red. Guardar valor no implica que todos los consumidores lo hayan aplicado ya.
- AP conserva bandera applyPending para diferenciar configuración deseada de estado aplicado al ESP.

## Precauciones y límites

- Sector activo y legado, formato y límites deben leerse en adaptador/implementación actuales; el documento de refactor 3.0.0 precede a la expansión posterior.
- No serializar AppPreferences_t completo por sizeof para Web: contiene padding, bool, estructuras y secretos.
- Persistencia síncrona puede pausar atención de tareas; no se midió duración en placa.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/app_preferences.h` | `app_preferences_types.h` | `5aaedf0ef95d0b768c547216e3b9ce07e0a2faf11b93e30f3ceebffe487b9ca3` |
| `Core/Inc/app_preferences_types.h` | `stdbool.h`, `stdint.h`, `am_port.h`, `line_calibration.h`, `auto_motion.h`, `app_ap_config_data.h` | `1b4ac602514944da47c0b57caea99c57e132039d9f43473fc263acc6324c8b6c` |
| `Core/Src/app_preferences.c` | `app_preferences.h`, `balance_control_data.h`, `stddef.h`, `string.h` | `a27322285faffa0b1e1bb64981020de1b19aa6620f8ea147686bb604986f82e2` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `AppPreferencesMpuCalibration_t`

`f4:Core/Inc/app_preferences_types.h:129`

| Atributo / firma | Función y precaución |
| --- | --- |
| `bool valid` | Dato de estado/configuración `valid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float accelRollOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float accelPitchOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float gyroXOffsetDps` | Dato de estado/configuración `gyroXOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float gyroYOffsetDps` | Dato de estado/configuración `gyroYOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float gyroZOffsetDps` | Dato de estado/configuración `gyroZOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t gravityXMg` | Dato de estado/configuración `gravityXMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t gravityYMg` | Dato de estado/configuración `gravityYMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t gravityZMg` | Dato de estado/configuración `gravityZMg`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesBalancePid_t`

`f4:Core/Inc/app_preferences_types.h:141`

| Atributo / firma | Función y precaución |
| --- | --- |
| `float kp` | Dato de estado/configuración `kp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float ki` | Dato de estado/configuración `ki`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float kd` | Dato de estado/configuración `kd`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferences_t`

`f4:Core/Inc/app_preferences_types.h:147`

| Atributo / firma | Función y precaución |
| --- | --- |
| `bool encoderInverted` | Dato de estado/configuración `encoderInverted`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool screenLockEnabled` | Dato de estado/configuración `screenLockEnabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t screenLockTimeoutSec` | Dato de estado/configuración `screenLockTimeoutSec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t motorLeftMinPwm` | Dato de estado/configuración `motorLeftMinPwm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t motorRightMinPwm` | Dato de estado/configuración `motorRightMinPwm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t systemActionTimeoutSec` | Dato de estado/configuración `systemActionTimeoutSec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t usbAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t webAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t oledRotationDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint16_t lineBlackWidthMm` | Dato de estado/configuración `lineBlackWidthMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool oledAutoRotationEnabled` | Dato de estado/configuración `oledAutoRotationEnabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool unerRouterForwardingEnabled` | Dato de estado/configuración `unerRouterForwardingEnabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool pinLockEnabled` | Dato de estado/configuración `pinLockEnabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool espNotifWatchdogEnabled` | Dato de estado/configuración `espNotifWatchdogEnabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t espNotifWatchdogLimit` | Dato de estado/configuración `espNotifWatchdogLimit`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AppPreferencesMpuCalibration_t mpuCalibration` | Dato de estado/configuración `mpuCalibration`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AppPreferencesBalancePid_t balancePid` | Dato de estado/configuración `balancePid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ext_profile_t ext_profile` | Dato de estado/configuración `ext_profile`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char wifiApSsid[APP_PREFS_WIFI_AP_SSID_MAX_LEN]` | Dato de estado/configuración `wifiApSsid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char wifiApPassword[APP_PREFS_WIFI_AP_PASSWORD_MAX_LEN]` | Dato de estado/configuración `wifiApPassword`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool wifiApPasswordProtected` | Dato de estado/configuración `wifiApPasswordProtected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t wifiApIpv4[4]` | Dato de estado/configuración `wifiApIpv4`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool wifiApApplyPending` | Dato de estado/configuración `wifiApApplyPending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t favoriteScreenCodes[3]` | Dato de estado/configuración `favoriteScreenCodes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t wifiAtProtocol` | Dato de estado/configuración `wifiAtProtocol`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool wifiApFallbackEnabled` | Dato de estado/configuración `wifiApFallbackEnabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `LineCalibration_Store lineCalibration` | Dato de estado/configuración `lineCalibration`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AutoMotion_Profile autoMotionProfile` | Dato de estado/configuración `autoMotionProfile`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesPatch_t`

`f4:Core/Inc/app_preferences_types.h:178`

| Atributo / firma | Función y precaución |
| --- | --- |
| `AppPreferencesFieldMask_t fields` | Dato de estado/configuración `fields`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AppPreferences_t values` | Dato de estado/configuración `values`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesDescriptor_t`

`f4:Core/Inc/app_preferences_types.h:183`

| Atributo / firma | Función y precaución |
| --- | --- |
| `AppPreferencesFieldMask_t field` | Dato de estado/configuración `field`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const char *key` | Dato de estado/configuración `key`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AppPreferencesValueType_t type` | Dato de estado/configuración `type`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t minValue` | Dato de estado/configuración `minValue`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t maxValue` | Dato de estado/configuración `maxValue`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t stepValue` | Dato de estado/configuración `stepValue`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t defaultValue` | Dato de estado/configuración `defaultValue`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesStorageInfo_t`

`f4:Core/Inc/app_preferences_types.h:207`

| Atributo / firma | Función y precaución |
| --- | --- |
| `AppPreferencesStorageSource_t source` | Dato de estado/configuración `source`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AppPreferencesStorageStatus_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint16_t formatVersion` | Dato de estado/configuración `formatVersion`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t sequence` | Dato de estado/configuración `sequence`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesStoragePort`

`f4:Core/Inc/app_preferences_types.h:214`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `uint32_t base, end, legacy_base, legacy_end` | Dato de estado/configuración `legacy_end`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const void *(*view)(void *ctx,uint32_t address,size_t length)` | Expone una vista prestada del almacenamiento en una dirección/rango. |
| `AM_Result (*unlock)(void *ctx)` | Habilita escritura en almacenamiento; puede fallar. |
| `AM_Result (*lock)(void *ctx)` | Cierra escritura en almacenamiento tras la transacción. |
| `AM_Result (*erase)(void *ctx)` | Borra almacenamiento de forma síncrona según el backend. |
| `AM_Result (*program)(void *ctx,uint32_t address,uint32_t word)` | Programa una palabra en la dirección indicada según el backend. |
| `bool (*ipv4_valid)(const uint8_t *)` | Operación ipv4 valid de Preferencias y persistencia; semántica fina pendiente de ampliación. |
| `AppApStatus_t (*apply_ap_patch)(const AppApConfigPatch_t *,uint8_t *)` | Operación apply ap patch de Preferencias y persistencia; semántica fina pendiente de ampliación. |

### `AppPreferences_Handle`

`f4:Core/Inc/app_preferences_types.h:226`

| Atributo / firma | Función y precaución |
| --- | --- |
| `const AppPreferencesStoragePort *storage` | Puerto prestado para vista de memoria, transacción Flash y reglas AP. |
| `AppPreferences_t appPreferences` | Copia RAM de configuración vigente para esta instancia. |
| `uint8_t appAuthPinDigits[APP_PREFS_AUTH_PIN_DIGITS]` | Secreto de autenticación en RAM; documentar el tipo, nunca publicar su contenido. |
| `uint32_t appPreferencesNextWriteAddr, appPreferencesNextSequence` | Dato de estado/configuración `appPreferencesNextSequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool appPreferencesApRepairNeeded` | Indica reparación pendiente de configuración AP cargada. |
| `AppPreferencesStorageInfo_t appPreferencesStorageInfo` | Origen, estado, versión y secuencia del registro cargado. |

### `AppPreferencesRecordHeader_t`

`f4:Core/Src/app_preferences.c:43`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t magic` | Dato de estado/configuración `magic`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t version` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t size` | Dato de estado/configuración `size`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesRecordV1_t`

`f4:Core/Src/app_preferences.c:49`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t magic` | Dato de estado/configuración `magic`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t version` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t size` | Dato de estado/configuración `size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t sequence` | Dato de estado/configuración `sequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint16_t screenLockTimeoutSec` | Dato de estado/configuración `screenLockTimeoutSec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t reserved` | Dato de estado/configuración `reserved`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t checksum` | Dato de estado/configuración `checksum`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t commit` | Dato de estado/configuración `commit`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesRecordV2_t`

`f4:Core/Src/app_preferences.c:61`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t magic` | Dato de estado/configuración `magic`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t version` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t size` | Dato de estado/configuración `size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t sequence` | Dato de estado/configuración `sequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint16_t screenLockTimeoutSec` | Dato de estado/configuración `screenLockTimeoutSec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t reserved0` | Dato de estado/configuración `reserved0`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuAccelRollOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuAccelPitchOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuGyroXOffsetDps` | Dato de estado/configuración `mpuGyroXOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroYOffsetDps` | Dato de estado/configuración `mpuGyroYOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroZOffsetDps` | Dato de estado/configuración `mpuGyroZOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityXMg` | Dato de estado/configuración `mpuGravityXMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityYMg` | Dato de estado/configuración `mpuGravityYMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityZMg` | Dato de estado/configuración `mpuGravityZMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t reserved1` | Dato de estado/configuración `reserved1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t checksum` | Dato de estado/configuración `checksum`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t commit` | Dato de estado/configuración `commit`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesRecordV3_t`

`f4:Core/Src/app_preferences.c:82`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t magic` | Dato de estado/configuración `magic`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t version` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t size` | Dato de estado/configuración `size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t sequence` | Dato de estado/configuración `sequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint16_t screenLockTimeoutSec` | Dato de estado/configuración `screenLockTimeoutSec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t reserved0` | Dato de estado/configuración `reserved0`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuAccelRollOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuAccelPitchOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuGyroXOffsetDps` | Dato de estado/configuración `mpuGyroXOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroYOffsetDps` | Dato de estado/configuración `mpuGyroYOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroZOffsetDps` | Dato de estado/configuración `mpuGyroZOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityXMg` | Dato de estado/configuración `mpuGravityXMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityYMg` | Dato de estado/configuración `mpuGravityYMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityZMg` | Dato de estado/configuración `mpuGravityZMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t oledRotationDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint32_t checksum` | Dato de estado/configuración `checksum`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t commit` | Dato de estado/configuración `commit`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesRecordV4_t`

`f4:Core/Src/app_preferences.c:103`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t magic` | Dato de estado/configuración `magic`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t version` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t size` | Dato de estado/configuración `size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t sequence` | Dato de estado/configuración `sequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint16_t screenLockTimeoutSec` | Dato de estado/configuración `screenLockTimeoutSec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t reserved0` | Dato de estado/configuración `reserved0`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuAccelRollOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuAccelPitchOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuGyroXOffsetDps` | Dato de estado/configuración `mpuGyroXOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroYOffsetDps` | Dato de estado/configuración `mpuGyroYOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroZOffsetDps` | Dato de estado/configuración `mpuGyroZOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityXMg` | Dato de estado/configuración `mpuGravityXMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityYMg` | Dato de estado/configuración `mpuGravityYMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityZMg` | Dato de estado/configuración `mpuGravityZMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t oledRotationDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint16_t lineBlackWidthMm` | Dato de estado/configuración `lineBlackWidthMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t usbAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t checksum` | Dato de estado/configuración `checksum`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t commit` | Dato de estado/configuración `commit`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesRecordV5_t`

`f4:Core/Src/app_preferences.c:126`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t magic` | Dato de estado/configuración `magic`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t version` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t size` | Dato de estado/configuración `size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t sequence` | Dato de estado/configuración `sequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint16_t screenLockTimeoutSec` | Dato de estado/configuración `screenLockTimeoutSec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t reserved0` | Dato de estado/configuración `reserved0`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuAccelRollOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuAccelPitchOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuGyroXOffsetDps` | Dato de estado/configuración `mpuGyroXOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroYOffsetDps` | Dato de estado/configuración `mpuGyroYOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroZOffsetDps` | Dato de estado/configuración `mpuGyroZOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityXMg` | Dato de estado/configuración `mpuGravityXMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityYMg` | Dato de estado/configuración `mpuGravityYMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityZMg` | Dato de estado/configuración `mpuGravityZMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t oledRotationDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint16_t lineBlackWidthMm` | Dato de estado/configuración `lineBlackWidthMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t usbAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS]` | Dato de estado/configuración `authPinDigits`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t checksum` | Dato de estado/configuración `checksum`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t commit` | Dato de estado/configuración `commit`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesRecordV6_t`

`f4:Core/Src/app_preferences.c:150`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t magic` | Dato de estado/configuración `magic`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t version` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t size` | Dato de estado/configuración `size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t sequence` | Dato de estado/configuración `sequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint16_t screenLockTimeoutSec` | Dato de estado/configuración `screenLockTimeoutSec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t reserved0` | Dato de estado/configuración `reserved0`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuAccelRollOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuAccelPitchOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuGyroXOffsetDps` | Dato de estado/configuración `mpuGyroXOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroYOffsetDps` | Dato de estado/configuración `mpuGyroYOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroZOffsetDps` | Dato de estado/configuración `mpuGyroZOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityXMg` | Dato de estado/configuración `mpuGravityXMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityYMg` | Dato de estado/configuración `mpuGravityYMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityZMg` | Dato de estado/configuración `mpuGravityZMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t oledRotationDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint16_t lineBlackWidthMm` | Dato de estado/configuración `lineBlackWidthMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t usbAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS]` | Dato de estado/configuración `authPinDigits`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t webAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t reserved1` | Dato de estado/configuración `reserved1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t checksum` | Dato de estado/configuración `checksum`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t commit` | Dato de estado/configuración `commit`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesRecordV7_t`

`f4:Core/Src/app_preferences.c:176`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t magic` | Dato de estado/configuración `magic`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t version` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t size` | Dato de estado/configuración `size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t sequence` | Dato de estado/configuración `sequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t ext_profile` | Dato de estado/configuración `ext_profile`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint16_t screenLockTimeoutSec` | Dato de estado/configuración `screenLockTimeoutSec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t reserved0` | Dato de estado/configuración `reserved0`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuAccelRollOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuAccelPitchOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuGyroXOffsetDps` | Dato de estado/configuración `mpuGyroXOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroYOffsetDps` | Dato de estado/configuración `mpuGyroYOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroZOffsetDps` | Dato de estado/configuración `mpuGyroZOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityXMg` | Dato de estado/configuración `mpuGravityXMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityYMg` | Dato de estado/configuración `mpuGravityYMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityZMg` | Dato de estado/configuración `mpuGravityZMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t oledRotationDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint16_t lineBlackWidthMm` | Dato de estado/configuración `lineBlackWidthMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t usbAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS]` | Dato de estado/configuración `authPinDigits`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t webAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t reserved1` | Dato de estado/configuración `reserved1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t checksum` | Dato de estado/configuración `checksum`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t commit` | Dato de estado/configuración `commit`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesRecordV8_t`

`f4:Core/Src/app_preferences.c:203`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t magic` | Dato de estado/configuración `magic`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t version` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t size` | Dato de estado/configuración `size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t sequence` | Dato de estado/configuración `sequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t ext_profile` | Dato de estado/configuración `ext_profile`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint16_t screenLockTimeoutSec` | Dato de estado/configuración `screenLockTimeoutSec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t reserved0` | Dato de estado/configuración `reserved0`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuAccelRollOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuAccelPitchOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuGyroXOffsetDps` | Dato de estado/configuración `mpuGyroXOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroYOffsetDps` | Dato de estado/configuración `mpuGyroYOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroZOffsetDps` | Dato de estado/configuración `mpuGyroZOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityXMg` | Dato de estado/configuración `mpuGravityXMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityYMg` | Dato de estado/configuración `mpuGravityYMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityZMg` | Dato de estado/configuración `mpuGravityZMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t oledRotationDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint16_t lineBlackWidthMm` | Dato de estado/configuración `lineBlackWidthMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t usbAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS]` | Dato de estado/configuración `authPinDigits`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t webAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t espNotifWatchdogLimit` | Dato de estado/configuración `espNotifWatchdogLimit`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool espNotifWatchdogEnabled` | Dato de estado/configuración `espNotifWatchdogEnabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t balanceKpX20` | Dato de estado/configuración `balanceKpX20`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t balanceKiX20` | Dato de estado/configuración `balanceKiX20`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t balanceKdX20` | Dato de estado/configuración `balanceKdX20`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t mpuSensorId` | Dato de estado/configuración `mpuSensorId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t checksum` | Dato de estado/configuración `checksum`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t commit` | Dato de estado/configuración `commit`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesRecordV9_t`

`f4:Core/Src/app_preferences.c:235`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t magic` | Dato de estado/configuración `magic`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t version` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t size` | Dato de estado/configuración `size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t sequence` | Dato de estado/configuración `sequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t ext_profile` | Dato de estado/configuración `ext_profile`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint16_t screenLockTimeoutSec` | Dato de estado/configuración `screenLockTimeoutSec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t reserved0` | Dato de estado/configuración `reserved0`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuAccelRollOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuAccelPitchOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuGyroXOffsetDps` | Dato de estado/configuración `mpuGyroXOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroYOffsetDps` | Dato de estado/configuración `mpuGyroYOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroZOffsetDps` | Dato de estado/configuración `mpuGyroZOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityXMg` | Dato de estado/configuración `mpuGravityXMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityYMg` | Dato de estado/configuración `mpuGravityYMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityZMg` | Dato de estado/configuración `mpuGravityZMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t oledRotationDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint16_t lineBlackWidthMm` | Dato de estado/configuración `lineBlackWidthMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t usbAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS]` | Dato de estado/configuración `authPinDigits`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t webAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t espNotifWatchdogLimit` | Dato de estado/configuración `espNotifWatchdogLimit`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool espNotifWatchdogEnabled` | Dato de estado/configuración `espNotifWatchdogEnabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t balanceKpX20` | Dato de estado/configuración `balanceKpX20`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t balanceKiX20` | Dato de estado/configuración `balanceKiX20`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t balanceKdX20` | Dato de estado/configuración `balanceKdX20`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t mpuSensorId` | Dato de estado/configuración `mpuSensorId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char wifiApSsid[32u]` | Dato de estado/configuración `wifiApSsid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char wifiApPassword[64u]` | Dato de estado/configuración `wifiApPassword`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool wifiApPasswordProtected` | Dato de estado/configuración `wifiApPasswordProtected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t checksum` | Dato de estado/configuración `checksum`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t commit` | Dato de estado/configuración `commit`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesRecordV11_t`

`f4:Core/Src/app_preferences.c:270`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t magic` | Dato de estado/configuración `magic`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t version` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t size` | Dato de estado/configuración `size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t sequence` | Dato de estado/configuración `sequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t ext_profile` | Dato de estado/configuración `ext_profile`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint16_t screenLockTimeoutSec` | Dato de estado/configuración `screenLockTimeoutSec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t wifiAtProtocol` | Dato de estado/configuración `wifiAtProtocol`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuAccelRollOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuAccelPitchOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuGyroXOffsetDps` | Dato de estado/configuración `mpuGyroXOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroYOffsetDps` | Dato de estado/configuración `mpuGyroYOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroZOffsetDps` | Dato de estado/configuración `mpuGyroZOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityXMg` | Dato de estado/configuración `mpuGravityXMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityYMg` | Dato de estado/configuración `mpuGravityYMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityZMg` | Dato de estado/configuración `mpuGravityZMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t oledRotationDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint16_t lineBlackWidthMm` | Dato de estado/configuración `lineBlackWidthMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t usbAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS]` | Dato de estado/configuración `authPinDigits`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t webAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t espNotifWatchdogLimit` | Dato de estado/configuración `espNotifWatchdogLimit`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool espNotifWatchdogEnabled` | Dato de estado/configuración `espNotifWatchdogEnabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t balanceKpX20` | Dato de estado/configuración `balanceKpX20`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t balanceKiX20` | Dato de estado/configuración `balanceKiX20`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t balanceKdX20` | Dato de estado/configuración `balanceKdX20`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t mpuSensorId` | Dato de estado/configuración `mpuSensorId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char wifiApSsid[APP_PREFS_WIFI_AP_SSID_MAX_LEN]` | Dato de estado/configuración `wifiApSsid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char wifiApPassword[APP_PREFS_WIFI_AP_PASSWORD_MAX_LEN]` | Dato de estado/configuración `wifiApPassword`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool wifiApPasswordProtected` | Dato de estado/configuración `wifiApPasswordProtected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t wifiApIpv4[4]` | Dato de estado/configuración `wifiApIpv4`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t favoriteScreenCodes[3]` | Dato de estado/configuración `favoriteScreenCodes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool wifiApApplyPending` | Dato de estado/configuración `wifiApApplyPending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t checksum` | Dato de estado/configuración `checksum`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t commit` | Dato de estado/configuración `commit`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesRecordV12_t`

`f4:Core/Src/app_preferences.c:307`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t magic` | Dato de estado/configuración `magic`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t version` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t size` | Dato de estado/configuración `size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t sequence` | Dato de estado/configuración `sequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t ext_profile` | Dato de estado/configuración `ext_profile`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint16_t screenLockTimeoutSec` | Dato de estado/configuración `screenLockTimeoutSec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t wifiAtProtocol` | Dato de estado/configuración `wifiAtProtocol`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuAccelRollOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuAccelPitchOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuGyroXOffsetDps` | Dato de estado/configuración `mpuGyroXOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroYOffsetDps` | Dato de estado/configuración `mpuGyroYOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroZOffsetDps` | Dato de estado/configuración `mpuGyroZOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityXMg` | Dato de estado/configuración `mpuGravityXMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityYMg` | Dato de estado/configuración `mpuGravityYMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityZMg` | Dato de estado/configuración `mpuGravityZMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t oledRotationDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint16_t lineBlackWidthMm` | Dato de estado/configuración `lineBlackWidthMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t usbAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS]` | Dato de estado/configuración `authPinDigits`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t webAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t espNotifWatchdogLimit` | Dato de estado/configuración `espNotifWatchdogLimit`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool espNotifWatchdogEnabled` | Dato de estado/configuración `espNotifWatchdogEnabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t balanceKpX20` | Dato de estado/configuración `balanceKpX20`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t balanceKiX20` | Dato de estado/configuración `balanceKiX20`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t balanceKdX20` | Dato de estado/configuración `balanceKdX20`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t mpuSensorId` | Dato de estado/configuración `mpuSensorId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char wifiApSsid[APP_PREFS_WIFI_AP_SSID_MAX_LEN]` | Dato de estado/configuración `wifiApSsid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char wifiApPassword[APP_PREFS_WIFI_AP_PASSWORD_MAX_LEN]` | Dato de estado/configuración `wifiApPassword`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool wifiApPasswordProtected` | Dato de estado/configuración `wifiApPasswordProtected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t wifiApIpv4[4]` | Dato de estado/configuración `wifiApIpv4`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t favoriteScreenCodes[3]` | Dato de estado/configuración `favoriteScreenCodes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool wifiApApplyPending` | Dato de estado/configuración `wifiApApplyPending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `LineCalibration_Store lineCalibration` | Dato de estado/configuración `lineCalibration`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t checksum` | Dato de estado/configuración `checksum`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t commit` | Dato de estado/configuración `commit`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesRecord_t`

`f4:Core/Src/app_preferences.c:345`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t magic` | Dato de estado/configuración `magic`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t version` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t size` | Dato de estado/configuración `size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t sequence` | Dato de estado/configuración `sequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t ext_profile` | Dato de estado/configuración `ext_profile`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint16_t screenLockTimeoutSec` | Dato de estado/configuración `screenLockTimeoutSec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t wifiAtProtocol` | Dato de estado/configuración `wifiAtProtocol`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuAccelRollOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuAccelPitchOffsetDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float mpuGyroXOffsetDps` | Dato de estado/configuración `mpuGyroXOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroYOffsetDps` | Dato de estado/configuración `mpuGyroYOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float mpuGyroZOffsetDps` | Dato de estado/configuración `mpuGyroZOffsetDps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityXMg` | Dato de estado/configuración `mpuGravityXMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityYMg` | Dato de estado/configuración `mpuGravityYMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t mpuGravityZMg` | Dato de estado/configuración `mpuGravityZMg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t oledRotationDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint16_t lineBlackWidthMm` | Dato de estado/configuración `lineBlackWidthMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t usbAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS]` | Dato de estado/configuración `authPinDigits`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t webAlivePeriodMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t espNotifWatchdogLimit` | Dato de estado/configuración `espNotifWatchdogLimit`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool espNotifWatchdogEnabled` | Dato de estado/configuración `espNotifWatchdogEnabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t balanceKpX20` | Dato de estado/configuración `balanceKpX20`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t balanceKiX20` | Dato de estado/configuración `balanceKiX20`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t balanceKdX20` | Dato de estado/configuración `balanceKdX20`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t mpuSensorId` | Dato de estado/configuración `mpuSensorId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char wifiApSsid[APP_PREFS_WIFI_AP_SSID_MAX_LEN]` | Dato de estado/configuración `wifiApSsid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char wifiApPassword[APP_PREFS_WIFI_AP_PASSWORD_MAX_LEN]` | Dato de estado/configuración `wifiApPassword`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool wifiApPasswordProtected` | Dato de estado/configuración `wifiApPasswordProtected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t wifiApIpv4[4]` | Dato de estado/configuración `wifiApIpv4`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t favoriteScreenCodes[3]` | Dato de estado/configuración `favoriteScreenCodes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool wifiApApplyPending` | Dato de estado/configuración `wifiApApplyPending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `LineCalibration_Store lineCalibration` | Dato de estado/configuración `lineCalibration`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AutoMotion_Profile autoMotionProfile` | Dato de estado/configuración `autoMotionProfile`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t checksum` | Dato de estado/configuración `checksum`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t commit` | Dato de estado/configuración `commit`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppPreferencesScanResult_t`

`f4:Core/Src/app_preferences.c:387`

| Atributo / firma | Función y precaución |
| --- | --- |
| `bool found` | Dato de estado/configuración `found`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t formatVersion` | Dato de estado/configuración `formatVersion`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t sequence` | Dato de estado/configuración `sequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t firstFreeAddr` | Dato de estado/configuración `firstFreeAddr`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AppPreferences_t preferences` | Dato de estado/configuración `preferences`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS]` | Dato de estado/configuración `authPinDigits`; declaración estructural, interpretación específica pendiente de ampliar. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-apppreferences_applypatch_h"></a>`AM_Result AppPreferences_ApplyPatch_H(AppPreferences_Handle *h, const AppPreferencesPatch_t *patch, AppPreferencesFieldMask_t *changedFields)` | Aplica campos seleccionados y solicita persistencia transaccional. | `f4:Core/Inc/app_preferences.h:79`; editorial |
| <a id="fn-apppreferences_changeauthpin_h"></a>`AM_Result AppPreferences_ChangeAuthPin_H(AppPreferences_Handle *h, const uint8_t currentDigits[APP_PREFS_AUTH_PIN_DIGITS], const uint8_t newDigits[APP_PREFS_AUTH_PIN_DIGITS])` | Solicita cambio del PIN siguiendo las reglas de persistencia. | `f4:Core/Inc/app_preferences.h:77`; editorial |
| <a id="fn-apppreferences_clamplineblackwidthmm"></a>`uint16_t AppPreferences_ClampLineBlackWidthMm(uint16_t widthMm)` | Limita anchura de línea al rango persistible del proyecto, en milímetros. | `f4:Core/Inc/app_preferences.h:46`; editorial |
| <a id="fn-apppreferences_clampmotorminpwm"></a>`uint16_t AppPreferences_ClampMotorMinPwm(uint16_t minPwm)` | Limita PWM mínimo al intervalo admitido por preferencias. | `f4:Core/Inc/app_preferences.h:22`; editorial |
| <a id="fn-apppreferences_clampoledrotationdeg"></a>`uint16_t AppPreferences_ClampOledRotationDeg(uint16_t rotationDeg)` | Normaliza orientación persistible a las rotaciones de panel admitidas. | `f4:Core/Inc/app_preferences.h:44`; editorial |
| <a id="fn-apppreferences_clampscreenlocktimeoutsec"></a>`uint16_t AppPreferences_ClampScreenLockTimeoutSec(uint16_t timeoutSec)` | Limita timeout de bloqueo al rango de preferencias, en segundos. | `f4:Core/Inc/app_preferences.h:19`; editorial |
| <a id="fn-apppreferences_clampsystemactiontimeoutsec"></a>`uint16_t AppPreferences_ClampSystemActionTimeoutSec(uint16_t timeoutSec)` | Limita timeout de confirmación de acciones, en segundos. | `f4:Core/Inc/app_preferences.h:24`; editorial |
| <a id="fn-apppreferences_clampusbaliveperiodms"></a>`uint16_t AppPreferences_ClampUsbAlivePeriodMs(uint16_t periodMs)` | Limita período de alive USB, en milisegundos. | `f4:Core/Inc/app_preferences.h:26`; editorial |
| <a id="fn-apppreferences_clampwebaliveperiodms"></a>`uint16_t AppPreferences_ClampWebAlivePeriodMs(uint16_t periodMs)` | Limita período de alive Web, en milisegundos. | `f4:Core/Inc/app_preferences.h:28`; editorial |
| <a id="fn-apppreferences_clearmpucalibration_h"></a>`AM_Result AppPreferences_ClearMpuCalibration_H(AppPreferences_Handle *h)` | Borra calibración MPU persistida de la instancia y solicita guardar el cambio. | `f4:Core/Inc/app_preferences.h:76`; editorial |
| <a id="fn-apppreferences_getbalancepid_h"></a>`void AppPreferences_GetBalancePid_H(AppPreferences_Handle *h, AppPreferencesBalancePid_t *out)` | Consulta Balance Pid H dentro de Preferencias y persistencia. | `f4:Core/Inc/app_preferences.h:52`; inferred-name |
| <a id="fn-apppreferences_getdefaults"></a>`void AppPreferences_GetDefaults(AppPreferences_t *out)` | Carga configuración de fábrica en la salida. | `f4:Core/Inc/app_preferences.h:12`; editorial |
| <a id="fn-apppreferences_getdescriptors"></a>`const AppPreferencesDescriptor_t *AppPreferences_GetDescriptors(uint8_t *count)` | Expone descriptores de campos y restricciones. | `f4:Core/Inc/app_preferences.h:13`; editorial |
| <a id="fn-apppreferences_getespnotifwatchdoglimit_h"></a>`uint8_t AppPreferences_GetEspNotifWatchdogLimit_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.espNotifWatchdogLimit` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:42`; structural |
| <a id="fn-apppreferences_getextprofile_h"></a>`ext_profile_t AppPreferences_GetExtProfile_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.ext_profile` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:15`; structural |
| <a id="fn-apppreferences_getlineblackwidthmm_h"></a>`uint16_t AppPreferences_GetLineBlackWidthMm_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.lineBlackWidthMm` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:45`; structural |
| <a id="fn-apppreferences_getmotorleftminpwm_h"></a>`uint16_t AppPreferences_GetMotorLeftMinPwm_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.motorLeftMinPwm` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:20`; structural |
| <a id="fn-apppreferences_getmotorrightminpwm_h"></a>`uint16_t AppPreferences_GetMotorRightMinPwm_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.motorRightMinPwm` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:21`; structural |
| <a id="fn-apppreferences_getmpucalibration_h"></a>`bool AppPreferences_GetMpuCalibration_H(AppPreferences_Handle *h, AppPreferencesMpuCalibration_t *out)` | Devuelve el campo `h->appPreferences.mpuCalibration.valid` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:51`; structural |
| <a id="fn-apppreferences_getoledrotationdeg_h"></a>`uint16_t AppPreferences_GetOledRotationDeg_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.oledRotationDeg` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:43`; structural |
| <a id="fn-apppreferences_getrecordcapacity"></a>`uint32_t AppPreferences_GetRecordCapacity(void)` | Consulta Record Capacity dentro de Preferencias y persistencia. | `f4:Core/Inc/app_preferences.h:55`; inferred-name |
| <a id="fn-apppreferences_getrecordsizebytes"></a>`uint32_t AppPreferences_GetRecordSizeBytes(void)` | Consulta Record Size Bytes dentro de Preferencias y persistencia. | `f4:Core/Inc/app_preferences.h:54`; inferred-name |
| <a id="fn-apppreferences_getscreenlocktimeoutsec_h"></a>`uint16_t AppPreferences_GetScreenLockTimeoutSec_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.screenLockTimeoutSec` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:18`; structural |
| <a id="fn-apppreferences_getsnapshot_h"></a>`void AppPreferences_GetSnapshot_H(AppPreferences_Handle *h, AppPreferences_t *out)` | Copia preferencias actuales a una estructura del llamador. | `f4:Core/Inc/app_preferences.h:11`; editorial |
| <a id="fn-apppreferences_getstorageinfo_h"></a>`void AppPreferences_GetStorageInfo_H(AppPreferences_Handle *h, AppPreferencesStorageInfo_t *out)` | Copia origen, estado, versión y secuencia del almacenamiento. | `f4:Core/Inc/app_preferences.h:56`; editorial |
| <a id="fn-apppreferences_getsystemactiontimeoutsec_h"></a>`uint16_t AppPreferences_GetSystemActionTimeoutSec_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.systemActionTimeoutSec` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:23`; structural |
| <a id="fn-apppreferences_getusbaliveperiodms_h"></a>`uint16_t AppPreferences_GetUsbAlivePeriodMs_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.usbAlivePeriodMs` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:25`; structural |
| <a id="fn-apppreferences_getwebaliveperiodms_h"></a>`uint16_t AppPreferences_GetWebAlivePeriodMs_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.webAlivePeriodMs` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:27`; structural |
| <a id="fn-apppreferences_getwifiapconfig_h"></a>`void AppPreferences_GetWifiApConfig_H(AppPreferences_Handle *h, AppApConfig_t *out)` | Consulta Wifi Ap Config H dentro de Preferencias y persistencia. | `f4:Core/Inc/app_preferences.h:38`; inferred-name |
| <a id="fn-apppreferences_getwifiatprotocol_h"></a>`uint16_t AppPreferences_GetWifiAtProtocol_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.wifiAtProtocol` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:29`; structural |
| <a id="fn-apppreferences_get_h"></a>`const AppPreferences_t *AppPreferences_Get_H(AppPreferences_Handle *h)` | Devuelve vista const prestada de preferencias actuales. | `f4:Core/Inc/app_preferences.h:10`; editorial |
| <a id="fn-apppreferences_init_h"></a>`void AppPreferences_Init_H(AppPreferences_Handle *h)` | Carga registros/defaults y prepara estado de almacenamiento de la instancia. | `f4:Core/Inc/app_preferences.h:9`; editorial |
| <a id="fn-apppreferences_isencoderinverted_h"></a>`bool AppPreferences_IsEncoderInverted_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.encoderInverted` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:16`; structural |
| <a id="fn-apppreferences_isespnotifwatchdogenabled_h"></a>`bool AppPreferences_IsEspNotifWatchdogEnabled_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.espNotifWatchdogEnabled` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:41`; structural |
| <a id="fn-apppreferences_isoledautorotationenabled_h"></a>`bool AppPreferences_IsOledAutoRotationEnabled_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.oledAutoRotationEnabled` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:47`; structural |
| <a id="fn-apppreferences_ispinlockenabled_h"></a>`bool AppPreferences_IsPinLockEnabled_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.pinLockEnabled` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:49`; structural |
| <a id="fn-apppreferences_isscreenlockenabled_h"></a>`bool AppPreferences_IsScreenLockEnabled_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.screenLockEnabled` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:17`; structural |
| <a id="fn-apppreferences_isunerrouterforwardingenabled_h"></a>`bool AppPreferences_IsUnerRouterForwardingEnabled_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.unerRouterForwardingEnabled` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:48`; structural |
| <a id="fn-apppreferences_iswifiapfallbackenabled_h"></a>`bool AppPreferences_IsWifiApFallbackEnabled_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.wifiApFallbackEnabled` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:50`; structural |
| <a id="fn-apppreferences_iswifiappasswordprotected_h"></a>`bool AppPreferences_IsWifiApPasswordProtected_H(AppPreferences_Handle *h)` | Devuelve el campo `h->appPreferences.wifiApPasswordProtected` del estado actual de la instancia. | `f4:Core/Inc/app_preferences.h:34`; structural |
| <a id="fn-apppreferences_resetfields_h"></a>`AM_Result AppPreferences_ResetFields_H(AppPreferences_Handle *h, AppPreferencesFieldMask_t fields, AppPreferencesFieldMask_t *changedFields)` | Restablece los campos indicados a valores predeterminados. | `f4:Core/Inc/app_preferences.h:81`; editorial |
| <a id="fn-apppreferences_setautomotionprofile_h"></a>`AM_Result AppPreferences_SetAutoMotionProfile_H(AppPreferences_Handle *h, const AutoMotion_Profile *profile)` | Persiste el perfil de motores/IR recibido. | `f4:Core/Inc/app_preferences.h:7`; editorial |
| <a id="fn-apppreferences_setbalancepid_h"></a>`AM_Result AppPreferences_SetBalancePid_H(AppPreferences_Handle *h, float kp, float ki, float kd)` | Configura Balance Pid H dentro de Preferencias y persistencia. | `f4:Core/Inc/app_preferences.h:75`; inferred-name |
| <a id="fn-apppreferences_setencoderinverted_h"></a>`AM_Result AppPreferences_SetEncoderInverted_H(AppPreferences_Handle *h, bool inverted)` | Adapta la llamada a `AppPreferences_SaveTransaction` con el contexto de esta integración. | `f4:Core/Inc/app_preferences.h:59`; structural |
| <a id="fn-apppreferences_setextprofile_h"></a>`AM_Result AppPreferences_SetExtProfile_H(AppPreferences_Handle *h, ext_profile_t ext_profile)` | Adapta la llamada a `AppPreferences_SaveTransaction` con el contexto de esta integración. | `f4:Core/Inc/app_preferences.h:58`; structural |
| <a id="fn-apppreferences_setlineblackwidthmm_h"></a>`AM_Result AppPreferences_SetLineBlackWidthMm_H(AppPreferences_Handle *h, uint16_t widthMm)` | Configura Line Black Width Mm H dentro de Preferencias y persistencia. | `f4:Core/Inc/app_preferences.h:69`; inferred-name |
| <a id="fn-apppreferences_setlinecalibration_h"></a>`AM_Result AppPreferences_SetLineCalibration_H(AppPreferences_Handle *h, const LineCalibration_Store *store)` | Persiste el store de calibración de línea recibido. | `f4:Core/Inc/app_preferences.h:8`; editorial |
| <a id="fn-apppreferences_setmotorleftminpwm_h"></a>`AM_Result AppPreferences_SetMotorLeftMinPwm_H(AppPreferences_Handle *h, uint16_t minPwm)` | Configura Motor Left Min Pwm H dentro de Preferencias y persistencia. | `f4:Core/Inc/app_preferences.h:62`; inferred-name |
| <a id="fn-apppreferences_setmotorrightminpwm_h"></a>`AM_Result AppPreferences_SetMotorRightMinPwm_H(AppPreferences_Handle *h, uint16_t minPwm)` | Configura Motor Right Min Pwm H dentro de Preferencias y persistencia. | `f4:Core/Inc/app_preferences.h:63`; inferred-name |
| <a id="fn-apppreferences_setmpucalibration_h"></a>`AM_Result AppPreferences_SetMpuCalibration_H(AppPreferences_Handle *h, const AppPreferencesMpuCalibration_t *calibration)` | Configura Mpu Calibration H dentro de Preferencias y persistencia. | `f4:Core/Inc/app_preferences.h:74`; inferred-name |
| <a id="fn-apppreferences_setoledautorotationenabled_h"></a>`AM_Result AppPreferences_SetOledAutoRotationEnabled_H(AppPreferences_Handle *h, bool enabled)` | Adapta la llamada a `AppPreferences_SaveTransaction` con el contexto de esta integración. | `f4:Core/Inc/app_preferences.h:70`; structural |
| <a id="fn-apppreferences_setoledrotationdeg_h"></a>`AM_Result AppPreferences_SetOledRotationDeg_H(AppPreferences_Handle *h, uint16_t rotationDeg)` | Configura Oled Rotation Deg H dentro de Preferencias y persistencia. | `f4:Core/Inc/app_preferences.h:68`; inferred-name |
| <a id="fn-apppreferences_setpinlockenabled_h"></a>`AM_Result AppPreferences_SetPinLockEnabled_H(AppPreferences_Handle *h, bool enabled)` | Adapta la llamada a `AppPreferences_SaveTransaction` con el contexto de esta integración. | `f4:Core/Inc/app_preferences.h:72`; structural |
| <a id="fn-apppreferences_setscreenlockenabled_h"></a>`AM_Result AppPreferences_SetScreenLockEnabled_H(AppPreferences_Handle *h, bool enabled)` | Adapta la llamada a `AppPreferences_SaveTransaction` con el contexto de esta integración. | `f4:Core/Inc/app_preferences.h:60`; structural |
| <a id="fn-apppreferences_setscreenlocktimeoutsec_h"></a>`AM_Result AppPreferences_SetScreenLockTimeoutSec_H(AppPreferences_Handle *h, uint16_t timeoutSec)` | Configura Screen Lock Timeout Sec H dentro de Preferencias y persistencia. | `f4:Core/Inc/app_preferences.h:61`; inferred-name |
| <a id="fn-apppreferences_setsystemactiontimeoutsec_h"></a>`AM_Result AppPreferences_SetSystemActionTimeoutSec_H(AppPreferences_Handle *h, uint16_t timeoutSec)` | Configura System Action Timeout Sec H dentro de Preferencias y persistencia. | `f4:Core/Inc/app_preferences.h:64`; inferred-name |
| <a id="fn-apppreferences_setunerrouterforwardingenabled_h"></a>`AM_Result AppPreferences_SetUnerRouterForwardingEnabled_H(AppPreferences_Handle *h, bool enabled)` | Adapta la llamada a `AppPreferences_SaveTransaction` con el contexto de esta integración. | `f4:Core/Inc/app_preferences.h:71`; structural |
| <a id="fn-apppreferences_setusbaliveperiodms_h"></a>`AM_Result AppPreferences_SetUsbAlivePeriodMs_H(AppPreferences_Handle *h, uint16_t periodMs)` | Configura Usb Alive Period Ms H dentro de Preferencias y persistencia. | `f4:Core/Inc/app_preferences.h:65`; inferred-name |
| <a id="fn-apppreferences_setwebaliveperiodms_h"></a>`AM_Result AppPreferences_SetWebAlivePeriodMs_H(AppPreferences_Handle *h, uint16_t periodMs)` | Configura Web Alive Period Ms H dentro de Preferencias y persistencia. | `f4:Core/Inc/app_preferences.h:66`; inferred-name |
| <a id="fn-apppreferences_setwifiapapplypending_h"></a>`AM_Result AppPreferences_SetWifiApApplyPending_H(AppPreferences_Handle *h, bool pending)` | Adapta la llamada a `AppPreferences_SaveTransaction` con el contexto de esta integración. | `f4:Core/Inc/app_preferences.h:40`; structural |
| <a id="fn-apppreferences_setwifiapconfig_h"></a>`AM_Result AppPreferences_SetWifiApConfig_H(AppPreferences_Handle *h, const AppApConfig_t *config, bool applyPending)` | Configura Wifi Ap Config H dentro de Preferencias y persistencia. | `f4:Core/Inc/app_preferences.h:39`; inferred-name |
| <a id="fn-apppreferences_setwifiapfallbackenabled_h"></a>`AM_Result AppPreferences_SetWifiApFallbackEnabled_H(AppPreferences_Handle *h, bool enabled)` | Adapta la llamada a `AppPreferences_ApplyPatch_H` con el contexto de esta integración. | `f4:Core/Inc/app_preferences.h:73`; structural |
| <a id="fn-apppreferences_setwifiappasswordprotected_h"></a>`AM_Result AppPreferences_SetWifiApPasswordProtected_H(AppPreferences_Handle *h, bool protected)` | Adapta la llamada a `apply_ap_patch` con el contexto de esta integración. | `f4:Core/Inc/app_preferences.h:37`; structural |
| <a id="fn-apppreferences_setwifiappassword_h"></a>`AM_Result AppPreferences_SetWifiApPassword_H(AppPreferences_Handle *h, const char *password)` | Configura Wifi Ap Password H dentro de Preferencias y persistencia. | `f4:Core/Inc/app_preferences.h:36`; inferred-name |
| <a id="fn-apppreferences_setwifiapssid_h"></a>`AM_Result AppPreferences_SetWifiApSsid_H(AppPreferences_Handle *h, const char *ssid)` | Configura Wifi Ap Ssid H dentro de Preferencias y persistencia. | `f4:Core/Inc/app_preferences.h:35`; inferred-name |
| <a id="fn-apppreferences_setwifiatprotocol_h"></a>`AM_Result AppPreferences_SetWifiAtProtocol_H(AppPreferences_Handle *h, uint16_t protocol)` | Adapta la llamada a `AppPreferences_SaveTransaction` con el contexto de esta integración. | `f4:Core/Inc/app_preferences.h:67`; structural |
| <a id="fn-apppreferences_verifyauthpin_h"></a>`bool AppPreferences_VerifyAuthPin_H(AppPreferences_Handle *h, const uint8_t pinDigits[APP_PREFS_AUTH_PIN_DIGITS])` | Compara los dígitos recibidos con el PIN privado de la instancia. | `f4:Core/Inc/app_preferences.h:53`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-apppreferences_defaults"></a>`static AppPreferences_t AppPreferences_Defaults(void)` | Carga los valores predeterminados de s dentro de Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:608`; inferred-name |
| <a id="fn-apppreferences_boundedstringlength"></a>`static size_t AppPreferences_BoundedStringLength(const char *text, size_t capacity)` | Operación Bounded String Length de Preferencias y persistencia; semántica fina pendiente de ampliación. | `f4:Core/Src/app_preferences.c:647`; inferred-name |
| <a id="fn-apppreferences_wifiapconfigurationisvalid"></a>`static bool AppPreferences_WifiApConfigurationIsValid(const AppPreferences_t *prefs)` | Coordina `AppPreferences_BoundedStringLength` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:662`; structural |
| <a id="fn-apppreferences_loaddefaultauthpin"></a>`static void AppPreferences_LoadDefaultAuthPin(uint8_t pinDigits[APP_PREFS_AUTH_PIN_DIGITS])` | Coordina `memcpy` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:685`; structural |
| <a id="fn-apppreferences_authpinformatisvalid"></a>`static bool AppPreferences_AuthPinFormatIsValid( const uint8_t pinDigits[APP_PREFS_AUTH_PIN_DIGITS])` | Operación Auth Pin Format Is Valid de Preferencias y persistencia; semántica fina pendiente de ampliación. | `f4:Core/Src/app_preferences.c:692`; inferred-name |
| <a id="fn-apppreferences_decodemotorminpwm"></a>`static uint16_t AppPreferences_DecodeMotorMinPwm(uint32_t flags, uint8_t shift)` | Adapta la llamada a `AppPreferences_ClampMotorMinPwm` con el contexto de esta integración. | `f4:Core/Src/app_preferences.c:801`; structural |
| <a id="fn-apppreferences_encodemotorminpwm"></a>`static uint32_t AppPreferences_EncodeMotorMinPwm(uint16_t minPwm, uint8_t shift)` | Adapta la llamada a `AppPreferences_ClampMotorMinPwm` con el contexto de esta integración. | `f4:Core/Src/app_preferences.c:812`; structural |
| <a id="fn-apppreferences_decodesystemactiontimeoutsec"></a>`static uint16_t AppPreferences_DecodeSystemActionTimeoutSec(uint32_t flags)` | Adapta la llamada a `AppPreferences_ClampSystemActionTimeoutSec` con el contexto de esta integración. | `f4:Core/Src/app_preferences.c:819`; structural |
| <a id="fn-apppreferences_encodesystemactiontimeoutsec"></a>`static uint32_t AppPreferences_EncodeSystemActionTimeoutSec(uint16_t timeoutSec)` | Adapta la llamada a `AppPreferences_ClampSystemActionTimeoutSec` con el contexto de esta integración. | `f4:Core/Src/app_preferences.c:831`; structural |
| <a id="fn-apppreferences_clampbalancepid"></a>`static float AppPreferences_ClampBalancePid(float value)` | Operación Clamp Balance Pid de Preferencias y persistencia; semántica fina pendiente de ampliación. | `f4:Core/Src/app_preferences.c:839`; inferred-name |
| <a id="fn-apppreferences_encodebalancepid"></a>`static uint16_t AppPreferences_EncodeBalancePid(float value)` | Adapta la llamada a `AppPreferences_ClampBalancePid` con el contexto de esta integración. | `f4:Core/Src/app_preferences.c:850`; structural |
| <a id="fn-apppreferences_decodebalancepid"></a>`static float AppPreferences_DecodeBalancePid(uint16_t encoded)` | Adapta la llamada a `AppPreferences_ClampBalancePid` con el contexto de esta integración. | `f4:Core/Src/app_preferences.c:856`; structural |
| <a id="fn-apppreferences_checksumbytes"></a>`static uint32_t AppPreferences_ChecksumBytes(const void *record, uint32_t bytes)` | Consulta las condiciones de sum Bytes dentro de Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:862`; inferred-name |
| <a id="fn-apppreferences_iserasedrange"></a>`static bool AppPreferences_IsErasedRange(const void *base, uint32_t bytes)` | Indica si se cumple la condición Erased Range dentro de Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:876`; inferred-name |
| <a id="fn-apppreferences_recordv1isvalid"></a>`static bool AppPreferences_RecordV1IsValid(const AppPreferencesRecordV1_t *record)` | Coordina `AppPreferences_ChecksumBytes`, `offsetof` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:890`; structural |
| <a id="fn-apppreferences_recordv2isvalid"></a>`static bool AppPreferences_RecordV2IsValid(const AppPreferencesRecordV2_t *record)` | Coordina `AppPreferences_ChecksumBytes`, `offsetof` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:903`; structural |
| <a id="fn-apppreferences_recordv3isvalid"></a>`static bool AppPreferences_RecordV3IsValid(const AppPreferencesRecordV3_t *record)` | Coordina `AppPreferences_ChecksumBytes`, `offsetof` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:916`; structural |
| <a id="fn-apppreferences_recordv4isvalid"></a>`static bool AppPreferences_RecordV4IsValid(const AppPreferencesRecordV4_t *record)` | Coordina `AppPreferences_ChecksumBytes`, `offsetof` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:929`; structural |
| <a id="fn-apppreferences_recordv5isvalid"></a>`static bool AppPreferences_RecordV5IsValid(const AppPreferencesRecordV5_t *record)` | Coordina `AppPreferences_AuthPinFormatIsValid`, `AppPreferences_ChecksumBytes`, `offsetof` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:942`; structural |
| <a id="fn-apppreferences_recordv6isvalid"></a>`static bool AppPreferences_RecordV6IsValid(const AppPreferencesRecordV6_t *record)` | Coordina `AppPreferences_AuthPinFormatIsValid`, `AppPreferences_ChecksumBytes`, `offsetof` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:956`; structural |
| <a id="fn-apppreferences_recordv7isvalid"></a>`static bool AppPreferences_RecordV7IsValid(const AppPreferencesRecordV7_t *record)` | Coordina `AppPreferences_AuthPinFormatIsValid`, `AppPreferences_ChecksumBytes`, `offsetof` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:970`; structural |
| <a id="fn-apppreferences_recordv8isvalid"></a>`static bool AppPreferences_RecordV8IsValid(const AppPreferencesRecordV8_t *record)` | Coordina `AppPreferences_AuthPinFormatIsValid`, `AppPreferences_ChecksumBytes`, `offsetof` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:984`; structural |
| <a id="fn-apppreferences_recordv9isvalid"></a>`static bool AppPreferences_RecordV9IsValid(const AppPreferencesRecordV9_t *record)` | Coordina `AppPreferences_AuthPinFormatIsValid`, `AppPreferences_ChecksumBytes`, `offsetof` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:998`; structural |
| <a id="fn-apppreferences_recordv10isvalid"></a>`static bool AppPreferences_RecordV10IsValid(const AppPreferencesRecordV10_t *record)` | Coordina `AppPreferences_AuthPinFormatIsValid`, `AppPreferences_ChecksumBytes`, `offsetof` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1011`; structural |
| <a id="fn-apppreferences_recordv11isvalid"></a>`static bool AppPreferences_RecordV11IsValid(const AppPreferencesRecordV11_t *record)` | Coordina `AppPreferences_AuthPinFormatIsValid`, `AppPreferences_ChecksumBytes`, `offsetof` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1026`; structural |
| <a id="fn-apppreferences_recordv12isvalid"></a>`static bool AppPreferences_RecordV12IsValid(const AppPreferencesRecordV12_t *record)` | Coordina `AppPreferences_AuthPinFormatIsValid`, `AppPreferences_ChecksumBytes`, `offsetof` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1041`; structural |
| <a id="fn-apppreferences_recordisvalid"></a>`static bool AppPreferences_RecordIsValid(const AppPreferencesRecord_t *record)` | Coordina `AppPreferences_AuthPinFormatIsValid`, `AppPreferences_ChecksumBytes`, `offsetof` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1056`; structural |
| <a id="fn-apppreferences_fromrecordv1"></a>`static AppPreferences_t AppPreferences_FromRecordV1(const AppPreferencesRecordV1_t *record)` | Coordina `AppPreferences_ClampScreenLockTimeoutSec`, `AppPreferences_DecodeMotorMinPwm`, `AppPreferences_DecodeSystemActionTimeoutSec` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1071`; structural |
| <a id="fn-apppreferences_fromrecordv2"></a>`static AppPreferences_t AppPreferences_FromRecordV2(const AppPreferencesRecordV2_t *record)` | Coordina `AppPreferences_FromRecordV1` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1092`; structural |
| <a id="fn-apppreferences_fromrecordv3"></a>`static AppPreferences_t AppPreferences_FromRecordV3(const AppPreferencesRecordV3_t *record)` | Coordina `AppPreferences_ClampOledRotationDeg`, `AppPreferences_FromRecordV2` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1109`; structural |
| <a id="fn-apppreferences_fromrecordv4"></a>`static AppPreferences_t AppPreferences_FromRecordV4(const AppPreferencesRecordV4_t *record)` | Coordina `AppPreferences_ClampLineBlackWidthMm`, `AppPreferences_ClampUsbAlivePeriodMs`, `AppPreferences_FromRecordV3` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1121`; structural |
| <a id="fn-apppreferences_fromrecordv5"></a>`static AppPreferences_t AppPreferences_FromRecordV5(const AppPreferencesRecordV5_t *record)` | Adapta la llamada a `AppPreferences_FromRecordV4` con el contexto de esta integración. | `f4:Core/Src/app_preferences.c:1134`; structural |
| <a id="fn-apppreferences_fromrecordv6"></a>`static AppPreferences_t AppPreferences_FromRecordV6(const AppPreferencesRecordV6_t *record)` | Coordina `AppPreferences_ClampWebAlivePeriodMs`, `AppPreferences_FromRecordV5` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1145`; structural |
| <a id="fn-apppreferences_fromrecordv7"></a>`static AppPreferences_t AppPreferences_FromRecordV7(const AppPreferencesRecordV7_t *record)` | Coordina `AppPreferences_ClampLineBlackWidthMm`, `AppPreferences_ClampOledRotationDeg`, `AppPreferences_ClampScreenLockTimeoutSec` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1156`; structural |
| <a id="fn-apppreferences_fromrecordv8"></a>`static AppPreferences_t AppPreferences_FromRecordV8(const AppPreferencesRecordV8_t *record)` | Coordina `AppPreferences_DecodeBalancePid`, `AppPreferences_FromRecordV7` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1196`; structural |
| <a id="fn-apppreferences_repairloadedap"></a>`static bool AppPreferences_RepairLoadedAp(AppPreferences_Handle *h, AppPreferences_t *prefs)` | Coordina `AppPreferences_BoundedStringLength`, `AppPreferences_Defaults`, `ipv4_valid` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1213`; structural |
| <a id="fn-apppreferences_fromrecordv9"></a>`static AppPreferences_t AppPreferences_FromRecordV9(AppPreferences_Handle *h, const AppPreferencesRecordV9_t *record)` | Coordina `AppPreferences_FromRecordV8`, `AppPreferences_RepairLoadedAp`, `memcpy` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1253`; structural |
| <a id="fn-apppreferences_fromrecordv11"></a>`static AppPreferences_t AppPreferences_FromRecordV11(AppPreferences_Handle *h, const AppPreferencesRecordV11_t *record)` | Coordina `AppPreferences_FromRecordV8`, `AppPreferences_RepairLoadedAp`, `memcpy` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1271`; structural |
| <a id="fn-apppreferences_fromrecordv12"></a>`static AppPreferences_t AppPreferences_FromRecordV12(AppPreferences_Handle *h, const AppPreferencesRecordV12_t *record)` | Coordina `AppPreferences_FromRecordV8`, `AppPreferences_RepairLoadedAp`, `memcpy` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1293`; structural |
| <a id="fn-apppreferences_fromrecord"></a>`static AppPreferences_t AppPreferences_FromRecord(AppPreferences_Handle *h, const AppPreferencesRecord_t *record)` | Coordina `AppPreferences_FromRecordV8`, `AppPreferences_RepairLoadedAp`, `memcpy` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1315`; structural |
| <a id="fn-apppreferences_fromrecordv10"></a>`static AppPreferences_t AppPreferences_FromRecordV10(AppPreferences_Handle *h, const AppPreferencesRecordV10_t *record)` | Adapta la llamada a `AppPreferences_FromRecordV11` con el contexto de esta integración. | `f4:Core/Src/app_preferences.c:1338`; structural |
| <a id="fn-apppreferences_torecord"></a>`static AppPreferencesRecord_t AppPreferences_ToRecord(AppPreferences_Handle *h, const AppPreferences_t *prefs, uint32_t sequence)` | Coordina `AppPreferences_ChecksumBytes`, `AppPreferences_ClampLineBlackWidthMm`, `AppPreferences_ClampOledRotationDeg` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1347`; structural |
| <a id="fn-apppreferences_erasesector"></a>`static AM_Result AppPreferences_EraseSector(AppPreferences_Handle *h)` | Adapta la llamada a `erase` con el contexto de esta integración. | `f4:Core/Src/app_preferences.c:1430`; structural |
| <a id="fn-apppreferences_writerecord"></a>`static AM_Result AppPreferences_WriteRecord(AppPreferences_Handle *h, uint32_t address, const AppPreferencesRecord_t *record)` | Escribe Record dentro de Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1435`; inferred-name |
| <a id="fn-apppreferences_scanflash"></a>`static void AppPreferences_ScanFlash(AppPreferences_Handle *h, uint32_t baseAddr, uint32_t endAddr, AppPreferencesScanResult_t *result)` | Coordina `AppPreferences_Defaults`, `AppPreferences_FromRecord`, `AppPreferences_FromRecordV1` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1451`; structural |
| <a id="fn-apppreferences_usescanresult"></a>`static void AppPreferences_UseScanResult(AppPreferences_Handle *h, const AppPreferencesScanResult_t *result, AppPreferencesStorageSource_t source)` | Coordina `memcpy` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1683`; structural |
| <a id="fn-apppreferences_save"></a>`static AM_Result AppPreferences_Save(AppPreferences_Handle *h)` | Coordina `AppPreferences_EraseSector`, `AppPreferences_RecordIsValid`, `AppPreferences_ToRecord` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1698`; structural |
| <a id="fn-apppreferences_savetransaction"></a>`static AM_Result AppPreferences_SaveTransaction(AppPreferences_Handle *h, const AppPreferences_t *nextPreferences, const uint8_t nextAuthPinDigits[APP_PREFS_AUTH_PIN_DIGITS])` | Coordina `AppPreferences_Save`, `memcpy`, `memset` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:1761`; structural |
| <a id="fn-apppreferences_normalize"></a>`static void AppPreferences_Normalize(AppPreferences_t *prefs, AppPreferencesFieldMask_t fields)` | Coordina `AppPreferences_ClampLineBlackWidthMm`, `AppPreferences_ClampMotorMinPwm`, `AppPreferences_ClampOledRotationDeg` para Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:2063`; structural |
| <a id="fn-apppreferences_getchangedfields"></a>`static AppPreferencesFieldMask_t AppPreferences_GetChangedFields( const AppPreferences_t *current, const AppPreferences_t *next, AppPreferencesFieldMask_t fields)` | Consulta Changed Fields dentro de Preferencias y persistencia. | `f4:Core/Src/app_preferences.c:2138`; inferred-name |
| <a id="fn-apppreferences_mpucalibrationequals"></a>`static bool AppPreferences_MpuCalibrationEquals(const AppPreferencesMpuCalibration_t *a, const AppPreferencesMpuCalibration_t *b)` | Operación Mpu Calibration Equals de Preferencias y persistencia; semántica fina pendiente de ampliación. | `f4:Core/Src/app_preferences.c:2711`; inferred-name |
