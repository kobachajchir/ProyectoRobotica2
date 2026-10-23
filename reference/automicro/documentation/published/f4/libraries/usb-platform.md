# Plataforma USB CDC/MSC

ID: `f4:module/USB_DEVICE/App/usb_mode_manager.h` · Capa: **plataforma** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Configura clases USB, descriptores, callbacks CDC y almacenamiento MSC sobre el stack ST.

Modo USB, conexión/DTR y recursos del controlador OTG_FS. El middleware ST queda como dependencia vendor externa al manual propio.

<a id="modularity"></a>
## Modularidad observada

Es plataforma STM32 y depende del stack ST/HAL por diseño. La interfaz UNER_Transport aísla su consumo por los núcleos.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

Conserva objetos y tablas de callbacks del stack USB, buffers CDC y estado de modo/disco. Los buffers entregados al stack deben vivir hasta fin de transferencia.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- MX_USB_DEVICE_Init configura la clase y el dispositivo.
- El stack invoca callbacks RX/TX/control; CDC notifica al transporte y UNER_App.
- El modo MSC presta bloques del disco declarado; no interpretar esos datos como preferencias Flash.

## Trabajo en tándem

- UNER USB utiliza la clase CDC; boot/mode manager gobierna selección según lógica de la aplicación.

## Precauciones y límites

- No se enumeró el dispositivo en un host ni se comprobó DTR físicamente.
- Las funciones internas del stack se registran como callbacks y no equivalen a llamadas directas desde main.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `USB_DEVICE/App/usb_mode_manager.h` | `stdint.h` | `b0074d9d5d85133a9c84315bce88834ba13e3512392b10ba96da7cca2637c4f0` |
| `USB_DEVICE/App/usb_mode_manager.c` | `usb_mode_manager.h`, `main.h`, `usb_device.h`, `usbd_cdc.h`, `usbd_cdc_if.h`, `usbd_core.h`, `usbd_desc.h`, `usbd_msc.h`, `usbd_msc_storage.h` | `7f0b94175b8b32ec972b9cd6c1fc49b5867943921e370a8b6c2e2ab997a1b5d8` |
| `USB_DEVICE/App/usb_device.h` | `stm32f4xx.h`, `stm32f4xx_hal.h`, `usbd_def.h` | `5da12da5450c1e4344d68beb710e530da7d728c6e831a93442614446f84757b1` |
| `USB_DEVICE/App/usb_device.c` | `usb_device.h`, `usbd_core.h`, `usbd_desc.h`, `usbd_cdc.h`, `usbd_cdc_if.h`, `usb_mode_manager.h` | `16b517f340022860b39052e83d69eec26e5635cdea2017f0e669176e1eac8eb8` |
| `USB_DEVICE/App/usbd_cdc_if.h` | `usbd_cdc.h` | `95ff2081039db4beb9874dc7739342421df4c016928602ef0a05d569599b276b` |
| `USB_DEVICE/App/usbd_cdc_if.c` | `usbd_cdc_if.h`, `usb_mode_manager.h`, `uner_transport_usbcdc.h` | `b87a0eeb942778cc91b5ddf2c232c9cc8d0afc1fd0b038225bbe69a68045ad07` |
| `USB_DEVICE/App/usbd_desc.h` | `usbd_def.h` | `a4fbe168f224d3e2d8091b90df1115301247e1a6e16177bab101ca81aa70172d` |
| `USB_DEVICE/App/usbd_desc.c` | `usbd_core.h`, `usbd_desc.h`, `usbd_conf.h`, `usb_mode_manager.h` | `12b923dded83ac973ad79e8aa417c6e9ea71522e73bd0b0ea4fd7fa557b1561b` |
| `USB_DEVICE/App/usbd_msc_storage.h` | `usbd_msc.h` | `73d3e66ea4f0318c405c91a2394407b7c1c7f7c331e161d88f51b6fb457a1103` |
| `USB_DEVICE/App/usbd_msc_storage.c` | `usbd_msc_storage.h`, `usbd_msc_disk_data.h` | `c41ae0707e2bf2af34e9dff93454ae07bda7701231b93ebceedfae68a0e75d66` |
| `USB_DEVICE/App/usbd_msc_disk_data.h` | `stdint.h` | `02712c1241c579e3aecfe6abc832691426e0d2d2edfb1b8383637ced4d60c636` |
| `USB_DEVICE/App/usbd_msc_disk_data.c` | `usbd_msc_disk_data.h`, `stddef.h`, `string.h` | `dcd6fce5732cd91ba46ea8184d01df3c47d72c326b13815a24ab9d3b7b93c08a` |
| `USB_DEVICE/Target/usbd_conf.h` | `stdio.h`, `stdlib.h`, `string.h`, `main.h`, `stm32f4xx.h`, `stm32f4xx_hal.h` | `99dcff1bcbf4fbda59073bbc32ec34c61fbf7cbc0456c4f42b568078dee56fdf` |
| `USB_DEVICE/Target/usbd_conf.c` | `stm32f4xx.h`, `stm32f4xx_hal.h`, `usbd_def.h`, `usbd_core.h`, `usbd_cdc.h`, `usbd_msc.h` | `08bd4d4e976f669281ab7942042e73156e6a9765db74314e00678b68e76e748f` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `classMemory`

`f4:USB_DEVICE/Target/usbd_conf.c:620`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t alignment` | Dato de estado/configuración `alignment`; declaración estructural, interpretación específica pendiente de ampliar. |
| `USBD_CDC_HandleTypeDef cdc` | Dato de estado/configuración `cdc`; declaración estructural, interpretación específica pendiente de ampliar. |
| `USBD_MSC_BOT_HandleTypeDef msc` | Dato de estado/configuración `msc`; declaración estructural, interpretación específica pendiente de ampliar. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-automicromscdisk_readblocks"></a>`int8_t AutoMicroMscDisk_ReadBlocks(uint8_t *destination, uint32_t firstBlock, uint16_t blockCount)` | Coordina `memcpy`, `memset` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usbd_msc_disk_data.h:15`; structural |
| <a id="fn-cdc_isdtractive_fs"></a>`uint8_t CDC_IsDtrActive_FS(void)` | Adapta la llamada a `USB_ModeManager_GetActiveMode` con el contexto de esta integración. | `f4:USB_DEVICE/App/usbd_cdc_if.h:112`; structural |
| <a id="fn-cdc_isrtsactive_fs"></a>`uint8_t CDC_IsRtsActive_FS(void)` | Adapta la llamada a `USB_ModeManager_GetActiveMode` con el contexto de esta integración. | `f4:USB_DEVICE/App/usbd_cdc_if.h:113`; structural |
| <a id="fn-cdc_transmit_fs"></a>`uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)` | Ofrece buffer de transmisión a la clase CDC del stack ST. | `f4:USB_DEVICE/App/usbd_cdc_if.h:109`; editorial |
| <a id="fn-error_handler"></a>`void Error_Handler(void)` | Entra al manejo de fallo no recuperable de plataforma. Implementación no localizada en los archivos de esta ficha. | `f4:USB_DEVICE/Target/usbd_conf.c:44`; editorial |
| <a id="fn-hal_pcd_mspdeinit"></a>`void HAL_PCD_MspDeInit(PCD_HandleTypeDef* pcdHandle)` | Coordina `HAL_GPIO_DeInit`, `HAL_NVIC_DisableIRQ`, `__HAL_RCC_USB_OTG_FS_CLK_DISABLE` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/Target/usbd_conf.c:103`; structural |
| <a id="fn-hal_pcd_mspinit"></a>`void HAL_PCD_MspInit(PCD_HandleTypeDef* pcdHandle)` | Coordina `HAL_GPIO_Init`, `HAL_NVIC_EnableIRQ`, `HAL_NVIC_SetPriority` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/Target/usbd_conf.c:70`; structural |
| <a id="fn-mx_usb_device_init"></a>`void MX_USB_DEVICE_Init(void)` | Inicializa el dispositivo USB y la clase seleccionada. | `f4:USB_DEVICE/App/usb_device.h:78`; editorial |
| <a id="fn-systemclock_config"></a>`void SystemClock_Config(void)` | Operación System Clock Config de Plataforma USB CDC/MSC; semántica fina pendiente de ampliación. Implementación no localizada en los archivos de esta ficha. | `f4:USB_DEVICE/Target/usbd_conf.c:47`; inferred-name |
| <a id="fn-usbd_fs_configstrdescriptor"></a>`uint8_t * USBD_FS_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)` | Coordina `UNUSED`, `USBD_GetString`, `USB_ModeManager_GetDescriptorMode` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usbd_desc.c:132`; structural |
| <a id="fn-usbd_fs_devicedescriptor"></a>`uint8_t * USBD_FS_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)` | Coordina `HIBYTE`, `LOBYTE`, `UNUSED` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usbd_desc.c:127`; structural |
| <a id="fn-usbd_fs_interfacestrdescriptor"></a>`uint8_t * USBD_FS_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)` | Coordina `UNUSED`, `USBD_GetString`, `USB_ModeManager_GetDescriptorMode` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usbd_desc.c:133`; structural |
| <a id="fn-usbd_fs_langidstrdescriptor"></a>`uint8_t * USBD_FS_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)` | Adapta la llamada a `UNUSED` con el contexto de esta integración. | `f4:USB_DEVICE/App/usbd_desc.c:128`; structural |
| <a id="fn-usbd_fs_manufacturerstrdescriptor"></a>`uint8_t * USBD_FS_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)` | Coordina `UNUSED`, `USBD_GetString` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usbd_desc.c:129`; structural |
| <a id="fn-usbd_fs_productstrdescriptor"></a>`uint8_t * USBD_FS_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)` | Coordina `UNUSED`, `USBD_GetString`, `USB_ModeManager_GetDescriptorMode` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usbd_desc.c:130`; structural |
| <a id="fn-usbd_fs_serialstrdescriptor"></a>`uint8_t * USBD_FS_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)` | Coordina `Get_SerialNum`, `UNUSED` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usbd_desc.c:131`; structural |
| <a id="fn-usbd_fs_usr_bosdescriptor"></a>`uint8_t * USBD_FS_USR_BOSDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)` | Adapta la llamada a `UNUSED` con el contexto de esta integración. | `f4:USB_DEVICE/App/usbd_desc.c:135`; structural |
| <a id="fn-usbd_get_usb_status"></a>`USBD_StatusTypeDef USBD_Get_USB_Status(HAL_StatusTypeDef hal_status)` | Operación USBD Get USB Status de Plataforma USB CDC/MSC; semántica fina pendiente de ampliación. | `f4:USB_DEVICE/Target/usbd_conf.c:55`; inferred-name |
| <a id="fn-usbd_ll_clearstallep"></a>`USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)` | Coordina `HAL_PCD_EP_ClrStall`, `USBD_Get_USB_Status` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/Target/usbd_conf.c:496`; structural |
| <a id="fn-usbd_ll_closeep"></a>`USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)` | Coordina `HAL_PCD_EP_Close`, `USBD_Get_USB_Status` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/Target/usbd_conf.c:442`; structural |
| <a id="fn-usbd_ll_deinit"></a>`USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev)` | Coordina `HAL_PCD_DeInit`, `USBD_Get_USB_Status` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/Target/usbd_conf.c:370`; structural |
| <a id="fn-usbd_ll_delay"></a>`void USBD_LL_Delay(uint32_t Delay)` | Adapta la llamada a `HAL_Delay` con el contexto de esta integración. | `f4:USB_DEVICE/Target/usbd_conf.c:645`; structural |
| <a id="fn-usbd_ll_flushep"></a>`USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)` | Coordina `HAL_PCD_EP_Flush`, `USBD_Get_USB_Status` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/Target/usbd_conf.c:460`; structural |
| <a id="fn-usbd_ll_getrxdatasize"></a>`uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr)` | Adapta la llamada a `HAL_PCD_EP_GetRxCount` con el contexto de esta integración. | `f4:USB_DEVICE/Target/usbd_conf.c:592`; structural |
| <a id="fn-usbd_ll_init"></a>`USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev)` | Coordina `Error_Handler`, `HAL_PCDEx_SetRxFiFo`, `HAL_PCDEx_SetTxFiFo` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/Target/usbd_conf.c:320`; structural |
| <a id="fn-usbd_ll_isstallep"></a>`uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)` | Operación USBD LL Is Stall EP de Plataforma USB CDC/MSC; semántica fina pendiente de ampliación. | `f4:USB_DEVICE/Target/usbd_conf.c:514`; inferred-name |
| <a id="fn-usbd_ll_openep"></a>`USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps)` | Coordina `HAL_PCD_EP_Open`, `USBD_Get_USB_Status` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/Target/usbd_conf.c:424`; structural |
| <a id="fn-usbd_ll_preparereceive"></a>`USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint32_t size)` | Coordina `HAL_PCD_EP_Receive`, `USBD_Get_USB_Status` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/Target/usbd_conf.c:574`; structural |
| <a id="fn-usbd_ll_settestmode"></a>`USBD_StatusTypeDef USBD_LL_SetTestMode(USBD_HandleTypeDef *pdev, uint8_t testmode)` | Adapta la llamada a `UNUSED` con el contexto de esta integración. | `f4:USB_DEVICE/Target/usbd_conf.c:604`; structural |
| <a id="fn-usbd_ll_setusbaddress"></a>`USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr)` | Coordina `HAL_PCD_SetAddress`, `USBD_Get_USB_Status` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/Target/usbd_conf.c:534`; structural |
| <a id="fn-usbd_ll_stallep"></a>`USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)` | Coordina `HAL_PCD_EP_SetStall`, `USBD_Get_USB_Status` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/Target/usbd_conf.c:478`; structural |
| <a id="fn-usbd_ll_start"></a>`USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev)` | Coordina `HAL_PCD_Start`, `USBD_Get_USB_Status` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/Target/usbd_conf.c:387`; structural |
| <a id="fn-usbd_ll_stop"></a>`USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev)` | Coordina `HAL_PCD_Stop`, `USBD_Get_USB_Status` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/Target/usbd_conf.c:404`; structural |
| <a id="fn-usbd_ll_transmit"></a>`USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint32_t size)` | Coordina `HAL_PCD_EP_Transmit`, `USBD_Get_USB_Status` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/Target/usbd_conf.c:554`; structural |
| <a id="fn-usbd_static_free"></a>`void USBD_static_free(void *p)` | Operación USBD static free de Plataforma USB CDC/MSC; semántica fina pendiente de ampliación. | `f4:USB_DEVICE/Target/usbd_conf.h:154`; inferred-name |
| <a id="fn-usbd_static_malloc"></a>`void *USBD_static_malloc(uint32_t size)` | Adapta la llamada a `UNUSED` con el contexto de esta integración. | `f4:USB_DEVICE/Target/usbd_conf.h:153`; structural |
| <a id="fn-usb_modemanager_getactivemode"></a>`USB_Mode_t USB_ModeManager_GetActiveMode(void)` | Consulta el modo de clase USB actualmente activo. | `f4:USB_DEVICE/App/usb_mode_manager.h:29`; editorial |
| <a id="fn-usb_modemanager_getdescriptormode"></a>`USB_Mode_t USB_ModeManager_GetDescriptorMode(void)` | Consulta el modo usado para construir descriptores USB. | `f4:USB_DEVICE/App/usb_mode_manager.h:30`; editorial |
| <a id="fn-usb_modemanager_getstate"></a>`USB_ModeState_t USB_ModeManager_GetState(void)` | Consulta la fase de transición del gestor USB. | `f4:USB_DEVICE/App/usb_mode_manager.h:31`; editorial |
| <a id="fn-usb_modemanager_init"></a>`void USB_ModeManager_Init(void)` | Inicializa el estado del gestor de modo USB. | `f4:USB_DEVICE/App/usb_mode_manager.h:24`; editorial |
| <a id="fn-usb_modemanager_requestcdc"></a>`void USB_ModeManager_RequestCdc(void)` | Adapta la llamada a `USB_ModeManager_RequestMode` con el contexto de esta integración. | `f4:USB_DEVICE/App/usb_mode_manager.h:27`; structural |
| <a id="fn-usb_modemanager_requestinstaller"></a>`void USB_ModeManager_RequestInstaller(void)` | Adapta la llamada a `USB_ModeManager_RequestMode` con el contexto de esta integración. | `f4:USB_DEVICE/App/usb_mode_manager.h:28`; structural |
| <a id="fn-usb_modemanager_requestmode"></a>`uint8_t USB_ModeManager_RequestMode(USB_Mode_t mode)` | Solicita cambio de modo USB para su procesamiento por el gestor. | `f4:USB_DEVICE/App/usb_mode_manager.h:26`; editorial |
| <a id="fn-usb_modemanager_task"></a>`void USB_ModeManager_Task(void)` | Coordina `HAL_GetTick`, `USBD_DeInit`, `USB_ModeManager_Start` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usb_mode_manager.h:25`; structural |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-usb_modemanager_start"></a>`static uint8_t USB_ModeManager_Start(USB_Mode_t mode)` | Inicia Plataforma USB CDC/MSC dentro de Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usb_mode_manager.c:22`; inferred-name |
| <a id="fn-cdc_init_fs"></a>`static int8_t CDC_Init_FS(void)` | Inicializa FS dentro de Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usbd_cdc_if.c:156`; inferred-name |
| <a id="fn-cdc_deinit_fs"></a>`static int8_t CDC_DeInit_FS(void)` | Adapta la llamada a `UNER_TransportUsbCdc_OnDisconnect` con el contexto de esta integración. | `f4:USB_DEVICE/App/usbd_cdc_if.c:173`; structural |
| <a id="fn-cdc_control_fs"></a>`static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)` | Coordina `UNUSED` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usbd_cdc_if.c:190`; structural |
| <a id="fn-cdc_receive_fs"></a>`static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)` | Coordina `UNER_TransportUsbCdc_OnRx`, `USBD_CDC_ReceivePacket`, `USBD_CDC_SetRxBuffer` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usbd_cdc_if.c:281`; structural |
| <a id="fn-cdc_transmitcplt_fs"></a>`static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)` | Coordina `UNER_TransportUsbCdc_OnTxComplete`, `UNUSED` para Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usbd_cdc_if.c:336`; structural |
| <a id="fn-get_serialnum"></a>`static void Get_SerialNum(void)` | Consulta Serial Num dentro de Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usbd_desc.c:407`; inferred-name |
| <a id="fn-inttounicode"></a>`static void IntToUnicode(uint32_t value, uint8_t * pbuf, uint8_t len)` | Operación Int To Unicode de Plataforma USB CDC/MSC; semántica fina pendiente de ampliación. | `f4:USB_DEVICE/App/usbd_desc.c:433`; inferred-name |
| <a id="fn-storage_init_fs"></a>`static int8_t STORAGE_Init_FS(uint8_t lun)` | Inicializa FS dentro de Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usbd_msc_storage.c:49`; inferred-name |
| <a id="fn-storage_getcapacity_fs"></a>`static int8_t STORAGE_GetCapacity_FS(uint8_t lun, uint32_t *block_num, uint16_t *block_size)` | Consulta Capacity FS dentro de Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usbd_msc_storage.c:54`; inferred-name |
| <a id="fn-storage_isready_fs"></a>`static int8_t STORAGE_IsReady_FS(uint8_t lun)` | Indica si se cumple la condición Ready FS dentro de Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usbd_msc_storage.c:67`; inferred-name |
| <a id="fn-storage_iswriteprotected_fs"></a>`static int8_t STORAGE_IsWriteProtected_FS(uint8_t lun)` | Indica si se cumple la condición Write Protected FS dentro de Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usbd_msc_storage.c:72`; inferred-name |
| <a id="fn-storage_read_fs"></a>`static int8_t STORAGE_Read_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)` | Adapta la llamada a `AutoMicroMscDisk_ReadBlocks` con el contexto de esta integración. | `f4:USB_DEVICE/App/usbd_msc_storage.c:77`; structural |
| <a id="fn-storage_write_fs"></a>`static int8_t STORAGE_Write_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)` | Adapta la llamada a `UNUSED` con el contexto de esta integración. | `f4:USB_DEVICE/App/usbd_msc_storage.c:88`; structural |
| <a id="fn-storage_getmaxlun_fs"></a>`static int8_t STORAGE_GetMaxLun_FS(void)` | Consulta Max Lun FS dentro de Plataforma USB CDC/MSC. | `f4:USB_DEVICE/App/usbd_msc_storage.c:100`; inferred-name |
