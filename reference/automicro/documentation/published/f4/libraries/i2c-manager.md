# I2C Manager

ID: `f4:module/Core/Inc/i2c_manager.h` · Capa: **servicio** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Arbitra una concesión exclusiva de bus entre clientes y enruta finalizaciones y errores al propietario actual.

Dos entradas de dispositivo, solicitudes pendientes, política, desempate y watchdog de lease; no interpreta registros OLED/MPU.

<a id="modularity"></a>
## Modularidad observada

No depende de HAL. Sí contiene IDs DEVICE_ID_OLED/MPU y políticas que favorecen esos IDs: desacoplado de implementaciones, con conocimiento del dominio. Prioridad efectiva se calcula desde pending_prio y policy; el campo priority registrado no basta para explicar la selección.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

I2C_ManagerHandle conserva hardware prestado, recurso opaco hi2c, tabla devices, owner/last_owner, lease y contadores de callbacks espurios. Cada I2C_DeviceEntry contiene callbacks y ctx del cliente. El registro de dirección usa addr_7bit; las operaciones de hardware siguen la convención del adaptador, por lo que no se debe desplazar dos veces la dirección.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init prepara tabla y lease; RegisterDevice instala callbacks de cada cliente.
- RequestBusPrio deja una solicitud pendiente o concede el bus; on_granted puede ocurrir durante la solicitud.
- OnTxCplt/OnRxCplt/OnError se llaman desde el adaptador HAL y notifican al propietario.
- ReleaseBus cede a otro pendiente; Tick aplica el watchdog de concesión.

## Trabajo en tándem

- OLED y MPU se conectan a través de AppI2cBinding; no se llaman directamente.
- app_tasks.c reenvía callbacks de memoria/master I2C al mismo gestor.

## Precauciones y límites

- Lease por defecto 120 ms; la operación del cliente debe finalizar o liberar el bus.
- Callbacks de concesión pueden ejecutarse en el llamador y finalizaciones en contexto de IRQ; no se garantiza reentrada o exclusión RTOS.
- Probe/read/write síncronos de arranque no se convierten en transferencias asíncronas por existir el gestor.

## Hallazgos vinculados

- [AMDOC-F4-005: La prioridad registrada del dispositivo no participa del arbitraje](../../../review/FINDINGS.md#amdoc-f4-005)

## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/i2c_manager.h` | `i2c_port.h`, `stdint.h` | `fc14fbea88360501d241076df6e5b22525395732dee8781e75045849cfefa92d` |
| `Core/Src/i2c_manager.c` | `i2c_manager.h` | `1c72a357bb2ad860d4888673a8fc670a91e41071c76dc31d411bba54dab75895` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `I2C_DeviceEntry`

`f4:Core/Inc/i2c_manager.h:32`

| Atributo / firma | Función y precaución |
| --- | --- |
| `I2C_DeviceID id` | Identificador lógico que usa el árbitro, independiente de la dirección I2C. |
| `uint16_t addr_7bit` | Dirección registrada de siete bits; el adaptador resuelve la convención exigida por hardware. |
| `uint8_t priority` | Prioridad base registrada; no confundir con pending_prio ni asumir que gobierna la selección. |
| `uint8_t enabled` | Marca de entrada registrada/habilitada. |
| `uint8_t pending` | Marca que el cliente solicita una próxima concesión. |
| `I2C_RequestPrio pending_prio` | Prioridad de solicitud acumulada para el próximo arbitraje. |
| `void *ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `I2C_OnGrantedCb on_granted` | Callback del cliente invocado al conceder propiedad; recibe ctx y puede iniciar DMA. |
| `I2C_OnTxDoneCb on_tx_done` | Callback del propietario al completar TX del backend. |
| `I2C_OnRxDoneCb on_rx_done` | Callback del propietario al completar RX del backend. |
| `I2C_OnErrorCb on_error` | Callback del propietario con motivo de error del gestor. |

### `I2C_ManagerHandle`

`f4:Core/Inc/i2c_manager.h:48`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *hi2c` | Recurso opaco del bus; la aplicación STM32 entrega el handle HAL. |
| `const AM_I2C_Hardware *hardware` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `I2C_DeviceEntry devices[I2C_MANAGER_MAX_DEVICES]` | Entradas de los dos clientes con dirección, pendientes, contexto y callbacks. |
| `I2C_DeviceID owner_id` | ID lógico del propietario actual del bus; NONE significa libre. |
| `I2C_DeviceID last_owner_id` | Propietario previo usado en el desempate de solicitudes. |
| `uint8_t owner_index` | Índice cacheado de propietario en devices o índice inválido. |
| `uint32_t lease_start_tick` | Dato de estado/configuración `lease_start_tick`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t lease_timeout_ms` | Duración máxima de concesión; cero deshabilita el watchdog. |
| `I2C_PolicyMode policy` | Política de arbitraje con preferencias explícitas OLED/MPU. |
| `void (*recover_cb)(void *hi2c)` | Hook opcional para recuperar el bus tras fallo; recibe el recurso opaco de hardware. |
| `uint32_t spurious_tx` | Número de callbacks TX sin propietario/recurso esperado. |
| `uint32_t spurious_rx` | Número de callbacks RX sin propietario/recurso esperado. |
| `uint32_t spurious_err` | Número de callbacks de error sin propietario/recurso esperado. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-i2c_manager_getaddress7"></a>`uint16_t I2C_Manager_GetAddress7(I2C_ManagerHandle *hmgr, I2C_DeviceID id)` | Consulta la dirección registrada de siete bits. | `f4:Core/Inc/i2c_manager.h:94`; editorial |
| <a id="fn-i2c_manager_getowner"></a>`I2C_DeviceID I2C_Manager_GetOwner(I2C_ManagerHandle *hmgr)` | Consulta el ID propietario o NONE. | `f4:Core/Inc/i2c_manager.h:93`; editorial |
| <a id="fn-i2c_manager_getpolicy"></a>`I2C_PolicyMode I2C_Manager_GetPolicy(const I2C_ManagerHandle *hmgr)` | Consulta la política actual de arbitraje. | `f4:Core/Inc/i2c_manager.h:91`; editorial |
| <a id="fn-i2c_manager_init"></a>`AM_Result I2C_Manager_Init(I2C_ManagerHandle *hmgr, const AM_I2C_Hardware *hardware, void *hi2c, uint32_t lease_timeout_ms)` | Inicializa árbitro, backend y timeout de concesión. | `f4:Core/Inc/i2c_manager.h:70`; editorial |
| <a id="fn-i2c_manager_isaddressready"></a>`AM_Result I2C_Manager_IsAddressReady(I2C_ManagerHandle *hmgr, uint16_t addr_7bit)` | Sondea disponibilidad de dirección mediante el backend síncrono. | `f4:Core/Inc/i2c_manager.h:96`; editorial |
| <a id="fn-i2c_manager_onerror"></a>`void I2C_Manager_OnError(I2C_ManagerHandle *hmgr, void *hi2c)` | Enruta error de bus al cliente propietario. | `f4:Core/Inc/i2c_manager.h:101`; editorial |
| <a id="fn-i2c_manager_onrxcplt"></a>`void I2C_Manager_OnRxCplt(I2C_ManagerHandle *hmgr, void *hi2c)` | Enruta finalización RX al propietario del recurso de bus. | `f4:Core/Inc/i2c_manager.h:100`; editorial |
| <a id="fn-i2c_manager_ontxcplt"></a>`void I2C_Manager_OnTxCplt(I2C_ManagerHandle *hmgr, void *hi2c)` | Enruta finalización TX al propietario del recurso de bus. | `f4:Core/Inc/i2c_manager.h:99`; editorial |
| <a id="fn-i2c_manager_registerdevice"></a>`AM_Result I2C_Manager_RegisterDevice(I2C_ManagerHandle *hmgr, I2C_DeviceID id, uint16_t addr_7bit, uint8_t priority, void *ctx, I2C_OnGrantedCb on_granted, I2C_OnTxDoneCb on_tx_done, I2C_OnRxDoneCb on_rx_done, I2C_OnErrorCb on_error)` | Registra ID, dirección, prioridad y callbacks de un cliente. | `f4:Core/Inc/i2c_manager.h:75`; editorial |
| <a id="fn-i2c_manager_releasebus"></a>`AM_Result I2C_Manager_ReleaseBus(I2C_ManagerHandle *hmgr, I2C_DeviceID id)` | Libera la propiedad del cliente y busca la siguiente solicitud pendiente. | `f4:Core/Inc/i2c_manager.h:89`; editorial |
| <a id="fn-i2c_manager_requestbus"></a>`AM_Result I2C_Manager_RequestBus(I2C_ManagerHandle *hmgr, I2C_DeviceID id)` | Solicita el bus usando la prioridad regular. | `f4:Core/Inc/i2c_manager.h:88`; editorial |
| <a id="fn-i2c_manager_requestbusprio"></a>`AM_Result I2C_Manager_RequestBusPrio(I2C_ManagerHandle *hmgr, I2C_DeviceID id, I2C_RequestPrio prio)` | Solicita el bus con prioridad acumulable y concede cuando corresponde. | `f4:Core/Inc/i2c_manager.h:85`; editorial |
| <a id="fn-i2c_manager_setpolicy"></a>`AM_Result I2C_Manager_SetPolicy(I2C_ManagerHandle *hmgr, I2C_PolicyMode policy)` | Selecciona política balanced, priorizar MPU o priorizar OLED. | `f4:Core/Inc/i2c_manager.h:90`; editorial |
| <a id="fn-i2c_manager_tick"></a>`void I2C_Manager_Tick(I2C_ManagerHandle *hmgr)` | Detecta vencimiento de lease y aplica la recuperación/notificación prevista. | `f4:Core/Inc/i2c_manager.h:104`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-find_index"></a>`static uint8_t find_index(I2C_ManagerHandle *hmgr, I2C_DeviceID id)` | Operación find index de I2C Manager; semántica fina pendiente de ampliación. | `f4:Core/Src/i2c_manager.c:19`; inferred-name |
| <a id="fn-effective_prio"></a>`static uint8_t effective_prio(I2C_ManagerHandle *hmgr, const I2C_DeviceEntry *dev)` | Operación effective prio de I2C Manager; semántica fina pendiente de ampliación. | `f4:Core/Src/i2c_manager.c:30`; inferred-name |
| <a id="fn-clear_device_entry"></a>`static void clear_device_entry(I2C_DeviceEntry *e)` | Operación clear device entry de I2C Manager; semántica fina pendiente de ampliación. | `f4:Core/Src/i2c_manager.c:48`; inferred-name |
| <a id="fn-pick_next_pending_index"></a>`static uint8_t pick_next_pending_index(I2C_ManagerHandle *hmgr)` | Coordina `effective_prio` para I2C Manager. | `f4:Core/Src/i2c_manager.c:64`; structural |
| <a id="fn-grant_to_index"></a>`static void grant_to_index(I2C_ManagerHandle *hmgr, uint8_t idx)` | Coordina `now_ms`, `on_granted`, `ready` para I2C Manager. | `f4:Core/Src/i2c_manager.c:93`; structural |
