# Contratos neutrales de plataforma y control

ID: `f4:module/Core/Inc/am_port.h` · Capa: **contrato** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Define resultados, relojes y tablas de operaciones para conectar implementaciones sin incluir sus APIs concretas.

AM_Result conserva 0=OK, 1=ERROR, 2=BUSY y 3=TIMEOUT. Las tablas AM_PidOps, AM_OrientationOps, AM_BalanceOps, AM_DriveOps y AM_LineOps describen servicios de control. No ejecutan hardware por sí mismas.

<a id="modularity"></a>
## Modularidad observada

Sin HAL. control_ports.h reúne varios modelos de datos del dominio: independencia de implementación, pero no independencia del esquema de datos AutoMicro.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

Las tablas son prestadas y los void* apuntan a instancias de la aplicación. El tipo opaco no comprueba en ejecución que el objeto corresponda a la tabla: el integrador debe emparejarlos. Los tipos *_data.h son vocabulario compartido, no dependencias de enlace con los drivers.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Construir tablas con duración suficiente y contextos válidos antes de inicializar consumidores.
- AM_OrientationOps separa ready, calibrating, fresh, sequence y snapshot; no sustituir estas condiciones entre sí.

## Trabajo en tándem

- app_control_ports.h conecta estas tablas con PID, MPU, balance y motores.
- AM_LineOps permanece declarado; el seguidor actual usa LineFollow_Config_t.read/command/hold. No atribuirle un uso activo solo por existir.

## Precauciones y límites

- La ausencia de HAL en un header no prueba ausencia de dependencias transitivas.
- No hay contrato general de exclusión mutua o reentrada; depende del consumidor.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/am_port.h` | `stdint.h`, `stdbool.h`, `stddef.h` | `4c7a59825d57ef7e6cf6251c4706178b023da85e8e6b8346f32d6c0851444aeb` |
| `Core/Inc/i2c_port.h` | `am_port.h` | `1460cd0929d23a042e6a001538c762c5a6670e904fd4fc6c6a24cb59380a8a6e` |
| `Core/Inc/control_ports.h` | `pid_controller_data.h`, `mpu9250_data.h`, `balance_control_data.h`, `motion_control_data.h`, `motion_estimator_data.h`, `tcrt5000_data.h` | `0a8b36d5f5ccb03a2601356c80b367b6a7f0c167743e15820a83df702a83c8d2` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `AM_Clock`

`f4:Core/Inc/am_port.h:9`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `uint32_t (*now_ms)(void *ctx)` | Entrega reloj monotónico modular en milisegundos para deadlines y períodos. |
| `uint32_t (*now_us)(void *ctx)` | Entrega reloj en microsegundos; el adaptador actual deriva ms × 1000. |

### `AM_I2C_Hardware`

`f4:Core/Inc/i2c_port.h:39`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t (*now_ms)(void *ctx)` | Entrega reloj monotónico modular en milisegundos para deadlines y períodos. |
| `uint32_t (*now_us)(void *ctx)` | Entrega reloj en microsegundos; el adaptador actual deriva ms × 1000. |
| `void (*delay_ms)(void *ctx,uint32_t ms)` | Realiza una espera síncrona de inicialización; no vuelve asíncrona la configuración. |
| `bool (*ready)(void *ctx)` | Consulta si el recurso de hardware acepta una transferencia. |
| `AM_Result (*probe)(void *ctx,uint16_t address,uint32_t trials,uint32_t timeout)` | Sondea presencia del dispositivo; el backend actual ejecuta una operación síncrona. |
| `AM_Result (*mem_write)(void *ctx,uint16_t address,uint16_t reg,uint16_t reg_size,uint8_t *data,uint16_t len,uint32_t timeout)` | Escribe un registro de forma síncrona; el llamador entrega dirección, buffer y timeout. |
| `AM_Result (*mem_read)(void *ctx,uint16_t address,uint16_t reg,uint16_t reg_size,uint8_t *data,uint16_t len,uint32_t timeout)` | Lee registros de forma síncrona al buffer provisto. |
| `AM_Result (*mem_write_dma)(void *ctx,uint16_t address,uint16_t reg,uint16_t reg_size,uint8_t *data,uint16_t len)` | Inicia escritura DMA; conservar el buffer hasta el callback final o error. |
| `AM_Result (*tx_dma)(void *ctx,uint16_t address,uint8_t *data,uint16_t len)` | Inicia transmisión DMA al dispositivo; aceptación no equivale a transferencia finalizada. |
| `AM_Result (*rx_dma)(void *ctx,uint16_t address,uint8_t *data,uint16_t len)` | Inicia recepción DMA al buffer persistente del cliente. |

### `AM_I2C_DevicePort`

`f4:Core/Inc/i2c_port.h:51`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `const AM_I2C_Hardware *hardware` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `void *hardware_ctx` | Recurso opaco de plataforma usado por las operaciones de hardware. |
| `AM_Result (*bind)(void *ctx,uint16_t address,uint8_t priority,void *client, I2C_OnGrantedCb,I2C_OnTxDoneCb,I2C_OnRxDoneCb,I2C_OnErrorCb)` | Registra identidad, dirección y callbacks del cliente en el backend I2C. |
| `AM_Result (*request)(void *ctx,I2C_RequestPrio priority)` | Solicita propiedad del bus con prioridad; la concesión llega por on_granted. |
| `AM_Result (*release)(void *ctx)` | Libera la concesión I2C para permitir atender otros clientes. |
| `bool (*is_owner)(void *ctx)` | Consulta la propiedad del bus del cliente conectado a este puerto. |
| `I2C_PolicyMode (*policy)(void *ctx)` | Consulta la política del árbitro sin conocer su handle concreto. |

### `I2C_OnGrantedCb`

`f4:Core/Inc/i2c_port.h:15`

Firma: `void (*I2C_OnGrantedCb)(void *ctx)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `I2C_OnTxDoneCb`

`f4:Core/Inc/i2c_port.h:18`

Firma: `void (*I2C_OnTxDoneCb)(void *ctx)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `I2C_OnRxDoneCb`

`f4:Core/Inc/i2c_port.h:19`

Firma: `void (*I2C_OnRxDoneCb)(void *ctx)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `I2C_OnErrorCb`

`f4:Core/Inc/i2c_port.h:20`

Firma: `void (*I2C_OnErrorCb)(void *ctx, I2C_ManagerError err)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `AM_PidOps`

`f4:Core/Inc/control_ports.h:9`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void (*defaults)(PidController_Config_t *)` | Carga configuración PID por defecto en la estructura del llamador. |
| `AM_Result (*init)(void *,const PidController_Config_t *)` | Inicializa la instancia opaca con la configuración provista. |
| `void (*reset)(void *)` | Reinicia memoria dinámica del controlador sin reemplazar su almacenamiento. |
| `void (*setpoint)(void *,float)` | Configura la referencia escalar del PID. |
| `void (*gains)(void *,float,float,float)` | Configura Kp, Ki y Kd del PID. |
| `AM_Result (*update)(void *,float,float,float)` | Calcula el controlador con sus argumentos de medida/tiempo. |
| `float (*output)(const void *)` | Consulta la salida calculada; no ejecuta el control. |
| `const PidController_Status_t *(*status)(const void *)` | Devuelve una vista prestada del estado del PID. |

### `AM_OrientationOps`

`f4:Core/Inc/control_ports.h:19`

| Atributo / firma | Función y precaución |
| --- | --- |
| `bool (*ready)(const void *)` | Consulta si el recurso de hardware acepta una transferencia. |
| `uint8_t (*calibrating)(const void *)` | Operación calibrating de Contratos neutrales de plataforma y control; semántica fina pendiente de ampliación. |
| `uint8_t (*fresh)(const void *,uint32_t)` | Comprueba frescura de la orientación en el tiempo indicado. |
| `float (*front_tilt)(const void *)` | Consulta inclinación frontal del robot en grados. |
| `uint16_t (*sequence)(const void *)` | Consulta número de muestra para evitar reprocesar la misma adquisición. |
| `uint32_t (*attitude_ms)(const void *)` | Consulta tiempo de última actitud calculada. |
| `AM_Result (*snapshot)(void *,MPU9250_Snapshot_t *)` | Copia el snapshot de orientación a la estructura de salida. |
| `uint32_t (*now_ms)(const void *)` | Entrega reloj monotónico modular en milisegundos para deadlines y períodos. |

### `AM_BalanceOps`

`f4:Core/Inc/control_ports.h:29`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void (*target)(void *,float)` | Operación target de Contratos neutrales de plataforma y control; semántica fina pendiente de ampliación. |
| `void (*enable)(void *,uint8_t)` | Operación enable de Contratos neutrales de plataforma y control; semántica fina pendiente de ampliación. |
| `void (*reset)(void *)` | Reinicia memoria dinámica del controlador sin reemplazar su almacenamiento. |
| `AM_Result (*update)(void *,float,uint32_t)` | Calcula el controlador con sus argumentos de medida/tiempo. |
| `float (*output)(const void *)` | Consulta la salida calculada; no ejecuta el control. |

### `AM_DriveOps`

`f4:Core/Inc/control_ports.h:36`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void (*stop)(void *)` | Ordena parada de la tracción sin delegar el cálculo de seguridad al PWM. |
| `void (*brake)(void *)` | Ordena frenado de la tracción. |
| `void (*apply)(void *,float,float)` | Aplica porcentajes firmados izquierdo y derecho. |

### `AM_LineOps`

`f4:Core/Inc/control_ports.h:41`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void (*command_defaults)(MotionControl_Command_t *)` | Operación command defaults de Contratos neutrales de plataforma y control; semántica fina pendiente de ampliación. |
| `AM_Result (*command)(void *,const MotionControl_Command_t *,uint32_t)` | Operación command de Contratos neutrales de plataforma y control; semántica fina pendiente de ampliación. |
| `uint8_t (*armed)(const void *)` | Operación armed de Contratos neutrales de plataforma y control; semántica fina pendiente de ampliación. |
| `const MotionEstimator_Status_t *(*estimate)(const void *)` | Operación estimate de Contratos neutrales de plataforma y control; semántica fina pendiente de ampliación. |
| `void (*line_defaults)(TCRT5000_LineConfig_t *)` | Operación line defaults de Contratos neutrales de plataforma y control; semántica fina pendiente de ampliación. |
| `void (*line_width)(TCRT5000_LineConfig_t *,uint16_t)` | Operación line width de Contratos neutrales de plataforma y control; semántica fina pendiente de ampliación. |
| `bool (*emitters_on)(void *)` | Operación emitters on de Contratos neutrales de plataforma y control; semántica fina pendiente de ampliación. |
| `void (*read_line)(const volatile uint16_t *,bool,const TCRT5000_LineConfig_t *,TCRT5000_RouteSample_t *)` | Operación read line de Contratos neutrales de plataforma y control; semántica fina pendiente de ampliación. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
