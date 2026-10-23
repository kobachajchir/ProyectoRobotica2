# Preparación y navegación de pantallas

ID: `f4:module/Core/Inc/screenWrappers.h` · Capa: **integración** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Prepara modo de vista, renderer, event manager, flags de redibujado y screen code de cada pantalla.

Coordina entrada/salida de pantallas de sensores, motores, preferencias, router, red y dashboard. Los detalles de raster se delegan al renderer.

<a id="modularity"></a>
## Modularidad observada

Integración con ScreensServices y dependencias dentro del conjunto de pantallas. No es núcleo independiente de menús o sensores.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

Opera sobre Screens_Context->state y el MenuSystem prestado; las sesiones IR guardan/restauran iluminación. Los flags first draw/refresh no son respuestas remotas.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Un evento o acción de menú llama al wrapper.
- El wrapper configura renderer y handler, publica identidad y solicita dibujo.
- Al volver se restaura navegación y, cuando corresponde, iluminación o vista previa.

## Trabajo en tándem

- oled_utils.c dibuja el contenido seleccionado; MenuSystem mantiene reportes de pantalla/selección.
- Las vistas de router alternan RX/TX y transporte; no modifican el parser.

## Precauciones y límites

- La existencia de un wrapper no demuestra que sea alcanzable desde el menú actual.
- No equiparar un reporte screen code con réplica exacta Web de la pantalla.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/screenWrappers.h` | `stdbool.h`, `types/menu_types.h` | `95a93774f9723451eb07696316ad55940b8fb8e853eba33e23f5c993f46a1f16` |
| `Core/Src/screenWrappers.c` | `screens_internal.h`, `screenWrappers.h`, `types/menu_types.h`, `oled_utils.h`, `oled_canvas.h`, `fonts.h`, `encoder_types.h`, `uner_v2.h`, `string.h` | `617db2ed83da25f4696dc95cb5ee9963b2257f7116c531142c7cc914440c38de` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-dashboard_invalidateconnectivity"></a>`void Dashboard_InvalidateConnectivity(void)` | Marca datos de conectividad del dashboard para redibujado. | `f4:Core/Inc/screenWrappers.h:16`; editorial |
| <a id="fn-dashboard_returnafterconnectivitynotification"></a>`void Dashboard_ReturnAfterConnectivityNotification(void)` | Restaura dashboard tras una notificación de conectividad. | `f4:Core/Inc/screenWrappers.h:17`; editorial |
| <a id="fn-menusys_goback_wrapper"></a>`void MenuSys_GoBack_Wrapper(void)` | Adapta la llamada a `MenuSys_NavigateBack` con el contexto de esta integración. | `f4:Core/Inc/screenWrappers.h:27`; structural |
| <a id="fn-menusys_rendermenu_wrapper"></a>`void MenuSys_RenderMenu_Wrapper(void)` | Prepara el contexto de UI y selecciona la vista Menu Sys Render Menu. | `f4:Core/Inc/screenWrappers.h:26`; inferred-name |
| <a id="fn-oledutils_aboutbootloader_wrapper"></a>`void OledUtils_AboutBootloader_Wrapper(void)` | Prepara el contexto de UI y selecciona la vista About Bootloader. | `f4:Core/Inc/screenWrappers.h:24`; inferred-name |
| <a id="fn-oledutils_aboutbuild_wrapper"></a>`void OledUtils_AboutBuild_Wrapper(void)` | Prepara el contexto de UI y selecciona la vista About Build. | `f4:Core/Inc/screenWrappers.h:23`; inferred-name |
| <a id="fn-oledutils_aboutinfo_wrapper"></a>`void OledUtils_AboutInfo_Wrapper(void)` | Prepara el contexto de UI y selecciona la vista About Info. | `f4:Core/Inc/screenWrappers.h:21`; inferred-name |
| <a id="fn-oledutils_aboutrepo_wrapper"></a>`void OledUtils_AboutRepo_Wrapper(void)` | Prepara el contexto de UI y selecciona la vista About Repo. | `f4:Core/Inc/screenWrappers.h:22`; inferred-name |
| <a id="fn-oledutils_about_wrapper"></a>`void OledUtils_About_Wrapper(void)` | Prepara el contexto de UI y selecciona la vista About. | `f4:Core/Inc/screenWrappers.h:20`; inferred-name |
| <a id="fn-oledutils_beginunerrouternrf"></a>`void OledUtils_BeginUnerRouterNrf(void)` | Adapta la llamada a `OledUtils_BeginUnerRouterTransport` con el contexto de esta integración. | `f4:Core/Inc/screenWrappers.h:39`; structural |
| <a id="fn-oledutils_beginunerrouteruart"></a>`void OledUtils_BeginUnerRouterUart(void)` | Adapta la llamada a `OledUtils_BeginUnerRouterTransport` con el contexto de esta integración. | `f4:Core/Inc/screenWrappers.h:38`; structural |
| <a id="fn-oledutils_beginunerrouterusb"></a>`void OledUtils_BeginUnerRouterUsb(void)` | Adapta la llamada a `OledUtils_BeginUnerRouterTransport` con el contexto de esta integración. | `f4:Core/Inc/screenWrappers.h:37`; structural |
| <a id="fn-oledutils_drawitem_wrapper"></a>`void OledUtils_DrawItem_Wrapper(const MenuItem *item, int y, bool selected)` | Adapta la llamada a `OledUtils_DrawItem` con el contexto de esta integración. | `f4:Core/Inc/screenWrappers.h:14`; structural |
| <a id="fn-oledutils_endirvaluessession"></a>`void OledUtils_EndIRValuesSession(void)` | Coordina `APP_LineCalibrationBusy`, `OledUtils_DisableContinuousRender`, `TCRT5000_EmittersGroup3Set` para Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:31`; structural |
| <a id="fn-oledutils_getunerroutertransportid"></a>`uint8_t OledUtils_GetUnerRouterTransportId(void)` | Consulta Uner Router Transport Id dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:43`; inferred-name |
| <a id="fn-oledutils_isunerroutertxview"></a>`bool OledUtils_IsUnerRouterTxView(void)` | Indica si se cumple la condición Uner Router Tx View dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:42`; inferred-name |
| <a id="fn-oledutils_nextunerrouterpacketview"></a>`void OledUtils_NextUnerRouterPacketView(void)` | Operación Next Uner Router Packet View de Preparación y navegación de pantallas; semántica fina pendiente de ampliación. | `f4:Core/Inc/screenWrappers.h:40`; inferred-name |
| <a id="fn-oledutils_previousunerrouterpacketview"></a>`void OledUtils_PreviousUnerRouterPacketView(void)` | Adapta la llamada a `OledUtils_NextUnerRouterPacketView` con el contexto de esta integración. | `f4:Core/Inc/screenWrappers.h:41`; structural |
| <a id="fn-oledutils_renderbalancepidgain_wrapper"></a>`void OledUtils_RenderBalancePidGain_Wrapper(void)` | Renderiza Balance Pid Gain Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:56`; inferred-name |
| <a id="fn-oledutils_renderbalancepidlog_wrapper"></a>`void OledUtils_RenderBalancePidLog_Wrapper(void)` | Renderiza Balance Pid Log Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:55`; inferred-name |
| <a id="fn-oledutils_renderbalancepidstatus_wrapper"></a>`void OledUtils_RenderBalancePidStatus_Wrapper(void)` | Renderiza Balance Pid Status Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:54`; inferred-name |
| <a id="fn-oledutils_renderdashboard_wrapper"></a>`void OledUtils_RenderDashboard_Wrapper(void)` | Renderiza Dashboard Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:15`; inferred-name |
| <a id="fn-oledutils_renderespfirmwarescreen_wrapper"></a>`void OledUtils_RenderESPFirmwareScreen_Wrapper(void)` | Renderiza ESPFirmware Screen Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:64`; inferred-name |
| <a id="fn-oledutils_rendergenericconfirm_wrapper"></a>`void OledUtils_RenderGenericConfirm_Wrapper(void)` | Renderiza Generic Confirm Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:25`; inferred-name |
| <a id="fn-oledutils_renderirambiente_wrapper"></a>`void OledUtils_RenderIRAmbiente_Wrapper(void)` | Renderiza IRAmbiente Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:30`; inferred-name |
| <a id="fn-oledutils_renderlineblackwidth_wrapper"></a>`void OledUtils_RenderLineBlackWidth_Wrapper(void)` | Renderiza Line Black Width Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:48`; inferred-name |
| <a id="fn-oledutils_rendermpucalibration_wrapper"></a>`void OledUtils_RenderMPUCalibration_Wrapper(void)` | Renderiza MPUCalibration Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:44`; inferred-name |
| <a id="fn-oledutils_rendermodeprompt_wrapper"></a>`void OledUtils_RenderModePrompt_Wrapper(void)` | Renderiza Mode Prompt Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:19`; inferred-name |
| <a id="fn-oledutils_rendermotorminpwm_wrapper"></a>`void OledUtils_RenderMotorMinPwm_Wrapper(void)` | Renderiza Motor Min Pwm Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:52`; inferred-name |
| <a id="fn-oledutils_rendermotortest_wrapper"></a>`void OledUtils_RenderMotorTest_Wrapper(void)` | Renderiza Motor Test Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:28`; inferred-name |
| <a id="fn-oledutils_renderoledcanvas_wrapper"></a>`void OledUtils_RenderOledCanvas_Wrapper(void)` | Renderiza Oled Canvas Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:51`; inferred-name |
| <a id="fn-oledutils_renderoledrotationmputest_wrapper"></a>`void OledUtils_RenderOledRotationMpuTest_Wrapper(void)` | Renderiza Oled Rotation Mpu Test Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:50`; inferred-name |
| <a id="fn-oledutils_renderoledrotationtest_wrapper"></a>`void OledUtils_RenderOledRotationTest_Wrapper(void)` | Renderiza Oled Rotation Test Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:49`; inferred-name |
| <a id="fn-oledutils_renderscreenlocktimeout_wrapper"></a>`void OledUtils_RenderScreenLockTimeout_Wrapper(void)` | Renderiza Screen Lock Timeout Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:45`; inferred-name |
| <a id="fn-oledutils_renderscreenlocked_wrapper"></a>`void OledUtils_RenderScreenLocked_Wrapper(void)` | Renderiza Screen Locked Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:58`; inferred-name |
| <a id="fn-oledutils_rendersinglemotortest_wrapper"></a>`void OledUtils_RenderSingleMotorTest_Wrapper(void)` | Renderiza Single Motor Test Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:53`; inferred-name |
| <a id="fn-oledutils_renderstabilizationprompt_wrapper"></a>`void OledUtils_RenderStabilizationPrompt_Wrapper(void)` | Renderiza Stabilization Prompt Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:18`; inferred-name |
| <a id="fn-oledutils_rendersystemactiontimeout_wrapper"></a>`void OledUtils_RenderSystemActionTimeout_Wrapper(void)` | Renderiza System Action Timeout Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:46`; inferred-name |
| <a id="fn-oledutils_rendertestcamino_wrapper"></a>`void OledUtils_RenderTestCamino_Wrapper(void)` | Renderiza Test Camino Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:33`; inferred-name |
| <a id="fn-oledutils_renderunerroutertest_wrapper"></a>`void OledUtils_RenderUnerRouterTest_Wrapper(void)` | Renderiza Uner Router Test Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:57`; inferred-name |
| <a id="fn-oledutils_renderusbaliveperiod_wrapper"></a>`void OledUtils_RenderUsbAlivePeriod_Wrapper(void)` | Renderiza Usb Alive Period Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:47`; inferred-name |
| <a id="fn-oledutils_rendervaloresir_wrapper"></a>`void OledUtils_RenderValoresIR_Wrapper(void)` | Renderiza Valores IR Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:29`; inferred-name |
| <a id="fn-oledutils_rendervaloresmpu_wrapper"></a>`void OledUtils_RenderValoresMPU_Wrapper(void)` | Renderiza Valores MPU Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:35`; inferred-name |
| <a id="fn-oledutils_renderwifiatpassword_wrapper"></a>`void OledUtils_RenderWiFiAtPassword_Wrapper(void)` | Renderiza Wi Fi At Password Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:62`; inferred-name |
| <a id="fn-oledutils_renderwificonnectionstatus_wrapper"></a>`void OledUtils_RenderWiFiConnectionStatus_Wrapper(void)` | Renderiza Wi Fi Connection Status Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:63`; inferred-name |
| <a id="fn-oledutils_renderwifidetails_wrapper"></a>`void OledUtils_RenderWiFiDetails_Wrapper(void)` | Renderiza Wi Fi Details Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:61`; inferred-name |
| <a id="fn-oledutils_renderwifisearchresults_wrapper"></a>`void OledUtils_RenderWiFiSearchResults_Wrapper(void)` | Renderiza Wi Fi Search Results Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:60`; inferred-name |
| <a id="fn-oledutils_renderwifisearching_wrapper"></a>`void OledUtils_RenderWiFiSearching_Wrapper(void)` | Renderiza Wi Fi Searching Wrapper dentro de Preparación y navegación de pantallas. | `f4:Core/Inc/screenWrappers.h:59`; inferred-name |
| <a id="fn-oledutils_toggletestcaminoview"></a>`void OledUtils_ToggleTestCaminoView(void)` | Operación Toggle Test Camino View de Preparación y navegación de pantallas; semántica fina pendiente de ampliación. | `f4:Core/Inc/screenWrappers.h:34`; inferred-name |
| <a id="fn-oledutils_togglevaloresirview"></a>`void OledUtils_ToggleValoresIRView(void)` | Operación Toggle Valores IRView de Preparación y navegación de pantallas; semántica fina pendiente de ampliación. | `f4:Core/Inc/screenWrappers.h:32`; inferred-name |
| <a id="fn-oledutils_togglevaloresmpuview"></a>`void OledUtils_ToggleValoresMPUView(void)` | Operación Toggle Valores MPUView de Preparación y navegación de pantallas; semántica fina pendiente de ampliación. | `f4:Core/Inc/screenWrappers.h:36`; inferred-name |
| <a id="fn-screens_init_screenwrappers"></a>`void Screens_Init_screenWrappers(void)` | Inicializa estado de navegación y vistas preparadas por wrappers. | `f4:Core/Src/screenWrappers.c:1110`; editorial |
| <a id="fn-wifiscan_clearresults"></a>`void WiFiScan_ClearResults(void)` | Limpia resultados y estado de la sesión de exploración Wi-Fi. | `f4:Core/Inc/screenWrappers.h:65`; editorial |
| <a id="fn-onrendercomplete"></a>`void onRenderComplete(void)` | Atiende finalización de render para actualizar el estado de UI. | `f4:Core/Inc/screenWrappers.h:66`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-dashboard_requestwebstatusifneeded"></a>`static void Dashboard_RequestWebStatusIfNeeded(void)` | Solicita Web Status If Needed dentro de Preparación y navegación de pantallas. | `f4:Core/Src/screenWrappers.c:34`; inferred-name |
| <a id="fn-wifiresults_resetmenustate"></a>`static void WiFiResults_ResetMenuState(void)` | Coordina `memset` para Preparación y navegación de pantallas. | `f4:Core/Src/screenWrappers.c:55`; structural |
| <a id="fn-wifiresults_setdefaultselection"></a>`static void WiFiResults_SetDefaultSelection(SubMenu *menu)` | Operación Wi Fi Results Set Default Selection de Preparación y navegación de pantallas; semántica fina pendiente de ampliación. | `f4:Core/Src/screenWrappers.c:79`; inferred-name |
| <a id="fn-wifiresults_buildmenu"></a>`static void WiFiResults_BuildMenu(void)` | Coordina `WiFiResults_SetDefaultSelection`, `memset` para Preparación y navegación de pantallas. | `f4:Core/Src/screenWrappers.c:105`; structural |
| <a id="fn-menusys_visibleoffset"></a>`static uint8_t MenuSys_VisibleOffset(const SubMenu *menu, int8_t firstVisible, int8_t itemIndex)` | Adapta la llamada a `MenuSys_IsItemVisible` con el contexto de esta integración. | `f4:Core/Src/screenWrappers.c:140`; structural |
| <a id="fn-oledutils_beginunerroutertransport"></a>`static void OledUtils_BeginUnerRouterTransport(uint8_t transport_id)` | Operación Begin Uner Router Transport de Preparación y navegación de pantallas; semántica fina pendiente de ampliación. | `f4:Core/Src/screenWrappers.c:381`; inferred-name |
| <a id="fn-oledutils_getunerrouterscreencode"></a>`static ScreenCode_t OledUtils_GetUnerRouterScreenCode(void)` | Consulta Uner Router Screen Code dentro de Preparación y navegación de pantallas. | `f4:Core/Src/screenWrappers.c:425`; inferred-name |
| <a id="fn-oledutils_beginirvaluessession"></a>`static void OledUtils_BeginIRValuesSession(bool ambientMode)` | Coordina `APP_IsIrLineEnabled`, `APP_IsIrObjectsEnabled`, `APP_LineCalibrationBusy` para Preparación y navegación de pantallas. | `f4:Core/Src/screenWrappers.c:533`; structural |
| <a id="fn-oledutils_renderirvaluesmode"></a>`static void OledUtils_RenderIRValuesMode(bool ambientMode)` | Renderiza IRValues Mode dentro de Preparación y navegación de pantallas. | `f4:Core/Src/screenWrappers.c:571`; inferred-name |
