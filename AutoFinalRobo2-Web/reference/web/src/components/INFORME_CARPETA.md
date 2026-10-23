# Carpeta reference/web/src/components

[Maestro](../../../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../../../INFORME_ADAPTACION.md).

Fuentes históricas de referencia: no forman parte del producto adaptado.

## Utilidad y reformas específicas

### [Visor OLED, pantallas y acciones](../../../../docs/modules/oled-ui.md)

Identidades, builders, rasterizador/editor y controles permiten estudiar la biblioteca completa; SVG no es captura de pantalla física.

Reformas: Portar motor/recursos/estado o transportar framebuffer si se requiere fidelidad. Distinguir acción/invocador/vista. Mismo esquema de navegación que encoder: menú explícito, atrás por ítem; no simular botón físico inexistente. Implementar estados nuevos solo si Mega los reporta. No mantener sensores falsos para rellenar una pantalla.

## Archivos vecinos

- [AboutProjectPanel.tsx](<AboutProjectPanel.tsx>)
- [ApFallbackControls.tsx](<ApFallbackControls.tsx>)
- [AppFooter.tsx](<AppFooter.tsx>)
- [AppLoadingScreen.tsx](<AppLoadingScreen.tsx>)
- [ApProvisioningGate.tsx](<ApProvisioningGate.tsx>)
- [BrandMark.tsx](<BrandMark.tsx>)
- [CameraPresets.tsx](<CameraPresets.tsx>)
- [CameraPresetsPanel.tsx](<CameraPresetsPanel.tsx>)
- [CameraRig.tsx](<CameraRig.tsx>)
- [CarModeDropdown.tsx](<CarModeDropdown.tsx>)
- [CarModeSelectorModal.tsx](<CarModeSelectorModal.tsx>)
- [ConditionStatus.css](<ConditionStatus.css>)
- [ConditionStatus.tsx](<ConditionStatus.tsx>)
- [ConnectingScreen.tsx](<ConnectingScreen.tsx>)
- [IrEmittersControls.tsx](<IrEmittersControls.tsx>)
- [IrObjectDetectionMarkers.tsx](<IrObjectDetectionMarkers.tsx>)
- [MockEulerGenerator.tsx](<MockEulerGenerator.tsx>)
- [MockInjectButton.tsx](<MockInjectButton.tsx>)
- [modal.tsx](<modal.tsx>)
- [ModelLoadBoundary.tsx](<ModelLoadBoundary.tsx>)
- [ModelLoadingScreen.tsx](<ModelLoadingScreen.tsx>)
- [MotorBlock.tsx](<MotorBlock.tsx>)
- [MpuIrScene3D.tsx](<MpuIrScene3D.tsx>)
- [NavigateVT.tsx](<NavigateVT.tsx>)
- [OledBitmapText.tsx](<OledBitmapText.tsx>)
- [OledCommandPreview.tsx](<OledCommandPreview.tsx>)
- [OrientationControls.tsx](<OrientationControls.tsx>)
- [PageHeader.tsx](<PageHeader.tsx>)
- [PinScreen.tsx](<PinScreen.tsx>)
- [PinScreenModal.tsx](<PinScreenModal.tsx>)
- [ProtectedRoute.tsx](<ProtectedRoute.tsx>)
- [PublicRoute.tsx](<PublicRoute.tsx>)
- [RealTimeEulerPanel.tsx](<RealTimeEulerPanel.tsx>)
- [RedirectRoot.tsx](<RedirectRoot.tsx>)
- [RootLayout.tsx](<RootLayout.tsx>)
- [ScreenDashboardPanel.tsx](<ScreenDashboardPanel.tsx>)
- [ScreenStreamModal.tsx](<ScreenStreamModal.tsx>)
- [ScreenStreamWorkspace.tsx](<ScreenStreamWorkspace.tsx>)
- [SectionStatusStrip.tsx](<SectionStatusStrip.tsx>)
- [SegmentedControl.css](<SegmentedControl.css>)
- [SegmentedControl.tsx](<SegmentedControl.tsx>)
- [SensorBeamControls.tsx](<SensorBeamControls.tsx>)
- [SensorCameraViewIcon.tsx](<SensorCameraViewIcon.tsx>)
- [SensorDisclosure.tsx](<SensorDisclosure.tsx>)
- [SimulatedBadge.tsx](<SimulatedBadge.tsx>)
- [StartupWelcome.css](<StartupWelcome.css>)
- [StartupWelcome.tsx](<StartupWelcome.tsx>)
- [StopDiscontinuityGlyph.tsx](<StopDiscontinuityGlyph.tsx>)
- [SystemResetActions.tsx](<SystemResetActions.tsx>)
- [ThemeModeToggleCard.tsx](<ThemeModeToggleCard.tsx>)
- [ThreeModelViewer.tsx](<ThreeModelViewer.tsx>)
- [TimelineRow.tsx](<TimelineRow.tsx>)
- [toggleButton.tsx](<toggleButton.tsx>)
- [TransportActionButton.tsx](<TransportActionButton.tsx>)
- [UnderbodySensorBeams.tsx](<UnderbodySensorBeams.tsx>)
- [WebGLCanvas.tsx](<WebGLCanvas.tsx>)
- [WifiCredentialsModal.tsx](<WifiCredentialsModal.tsx>)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../../../AutoFinalRobo2-Contracts/provenance/files.json).
