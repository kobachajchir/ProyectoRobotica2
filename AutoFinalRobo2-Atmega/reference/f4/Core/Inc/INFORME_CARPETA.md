# Carpeta reference/f4/Core/Inc

[Maestro](../../../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../../../INFORME_ADAPTACION.md).

Fuentes históricas de referencia: no forman parte del producto adaptado.

## Utilidad y reformas específicas

### [Encoder de interfaz y pulsador único](../../../../docs/modules/encoder-ui.md)

encoder.c F4 procesa un mando rotativo y su pulsador. Con la ampliación solicitada vuelve a ser una referencia útil para UI; no mide las cuatro ruedas. user_button queda como antecedente del botón eliminado.

Reformas: CLK D40/PG1 y DT D41/PG0 se leen juntos; SW D42/PL7 activo bajo. Inicializar desde niveles reales, evitar gesto de arranque pulsado, rechazar transición cuadratura inválida y ajustar signo/transiciones por detent. Base: 1 ms poll, 20 ms debounce, 800 ms largo exclusivo; ajustar sin asumir hardware probado. El corto sale al soltar solo si no se emitió largo. No usar polling de UI para Hall motriz.

### [I2C Manager, SSD1306 y recursos](../../../../docs/modules/i2c-oled.md)

Preservar propiedad del bus, callback diferido a main, deadline/recovery, framebuffer por páginas y separación dibujo/transporte. El define SSD1306_USE_DMA existe, pero su rama 0 usa mem_write en bucle; no convierte a TWI asíncrono. AM_I2C_Hardware original conserva métodos DMA.

Reformas: La base nueva reduce Manager a un cliente OLED y cambia la API (no es ABI drop-in). start/poll/abort no bloquean; dirección es 7 bits. Se incluye inicialización SSD1306 128x64, framebuffer 1024, snapshot TX de 129 y dirty pages. Falta backend TWI AVR y confirmación de controlador/voltaje/dirección OLED. Portar fuentes/iconos grandes a PROGMEM y revisar punteros/far. Mantener buffers hasta fin/abort; render y callbacks solo en main. No reinicializar un handle en vuelo. INVERT nuevo usa XOR, diferencia intencional del F4 histórico.

### [Cinco sensores de línea](../../../../docs/modules/line-ir.md)

Reutilizar normalización, estado/calibración, filtrado y separación sensor/control. El origen modela ocho TCRT5000 (línea/objetos), adquisición ADC DMA y emisores por grupo. El módulo OSOYOO de cinco puntos no prueba esas mismas capacidades.

Reformas: Confirmar salida digital/analógica y polaridad. A4..A0 son canales de línea, no dirección I2C. Si digital: bitmask de cinco con validez/edad y calibración de polaridad; no inventar raw ADC. Si analógico: ADC10 bits sin DMA, secuencia iniciar/esperar/publicar y umbrales recalibrados. Retirar tres sensores de objetos y control de emisores si no existe cableado. Adaptar seguimiento a cuatro ruedas/robot estable sin balance.

### [Wrappers, menús y pantallas](../../../../docs/modules/screens.md)

Wrappers preparan datos/contexto; renderer dibuja; navegación decide pantalla/selección; acciones llaman servicios. Reutilizar conceptos y recursos, no System/globales completos. La referencia incluye menús/OSK/avisos y toda la biblioteca propia para port posterior.

Reformas: Nueva UI usa snapshot explícito sin consultas de hardware en renderer. Mantener identidad/rol/estado/acciones por separado. Largo SW abre menú; corto selecciona; atrás solo por ítem Volver. Retirar llamadas al botón menú y no reinterpretar largo como atrás. Main procesa acciones y respeta valid/unknown. Llevar menús/textos a flash; decidir contexto de edición/confirmación sin ejecutar motor al navegar. Skill en Contracts documenta cada pantalla; paridad Web pendiente.

## Archivos vecinos

- [am_port.h](<am_port.h>)
- [ap_context.h](<ap_context.h>)
- [app_ap_config.h](<app_ap_config.h>)
- [app_ap_config_data.h](<app_ap_config_data.h>)
- [app_auxiliary_ports.h](<app_auxiliary_ports.h>)
- [app_control_ports.h](<app_control_ports.h>)
- [app_display.h](<app_display.h>)
- [app_i2c_ports.h](<app_i2c_ports.h>)
- [app_menu_binding.h](<app_menu_binding.h>)
- [app_menu_services.h](<app_menu_services.h>)
- [app_preferences.h](<app_preferences.h>)
- [app_preferences_binding.h](<app_preferences_binding.h>)
- [app_preferences_stm32.h](<app_preferences_stm32.h>)
- [app_preferences_types.h](<app_preferences_types.h>)
- [app_screens_ports.h](<app_screens_ports.h>)
- [app_stm32_ports.h](<app_stm32_ports.h>)
- [app_tasks.h](<app_tasks.h>)
- [app_version.h](<app_version.h>)
- [app_wifi_binding.h](<app_wifi_binding.h>)
- [app_wifi_ports.h](<app_wifi_ports.h>)
- [auto_motion.h](<auto_motion.h>)
- [automicro_boot_contract.h](<automicro_boot_contract.h>)
- [balance_control.h](<balance_control.h>)
- [balance_control_data.h](<balance_control_data.h>)
- [balance_control_types.h](<balance_control_types.h>)
- [binary_asset_types.h](<binary_asset_types.h>)
- [control_ports.h](<control_ports.h>)
- [encoder.h](<encoder.h>)
- [encoder_types.h](<encoder_types.h>)
- [eventManagers.h](<eventManagers.h>)
- [favorites_context.h](<favorites_context.h>)
- [fonts.h](<fonts.h>)
- [globals.h](<globals.h>)
- [i2c_manager.h](<i2c_manager.h>)
- [i2c_port.h](<i2c_port.h>)
- [line_calibration.h](<line_calibration.h>)
- [line_follow_control.h](<line_follow_control.h>)
- [line_follow_control_types.h](<line_follow_control_types.h>)
- [main.h](<main.h>)
- [menu_definitions.h](<menu_definitions.h>)
- [menu_favorites.h](<menu_favorites.h>)
- [menusystem.h](<menusystem.h>)
- [motion_control.h](<motion_control.h>)
- [motion_control_data.h](<motion_control_data.h>)
- [motion_control_types.h](<motion_control_types.h>)
- [motion_estimator.h](<motion_estimator.h>)
- [motion_estimator_data.h](<motion_estimator_data.h>)
- [motion_estimator_types.h](<motion_estimator_types.h>)
- [motor_control.h](<motor_control.h>)
- [motor_control_types.h](<motor_control_types.h>)
- [mpu9250.h](<mpu9250.h>)
- [mpu9250_data.h](<mpu9250_data.h>)
- [mpu9250_types.h](<mpu9250_types.h>)
- [notificationWrappers.h](<notificationWrappers.h>)
- [oled_canvas.h](<oled_canvas.h>)
- [oled_handle.h](<oled_handle.h>)
- [oled_utils.h](<oled_utils.h>)
- [osk.h](<osk.h>)
- [osk_integration.h](<osk_integration.h>)
- [osk_types.h](<osk_types.h>)
- [permission_context.h](<permission_context.h>)
- [permissions.h](<permissions.h>)
- [permissions_types.h](<permissions_types.h>)
- [pid_controller.h](<pid_controller.h>)
- [pid_controller_data.h](<pid_controller_data.h>)
- [pid_controller_types.h](<pid_controller_types.h>)
- [screen_codes.h](<screen_codes.h>)
- [screen_context.h](<screen_context.h>)
- [screen_lock.h](<screen_lock.h>)
- [screens_internal.h](<screens_internal.h>)
- [screenWrappers.h](<screenWrappers.h>)
- [ssd1306.h](<ssd1306.h>)
- [ssd1306_defines.h](<ssd1306_defines.h>)
- [ssd1306_types.h](<ssd1306_types.h>)
- [stm32f4xx_hal_conf.h](<stm32f4xx_hal_conf.h>)
- [stm32f4xx_it.h](<stm32f4xx_it.h>)
- [system.h](<system.h>)
- [system_flags.h](<system_flags.h>)
- [tcrt5000.h](<tcrt5000.h>)
- [tcrt5000_data.h](<tcrt5000_data.h>)
- [tcrt5000_types.h](<tcrt5000_types.h>)
- [ui_event_router.h](<ui_event_router.h>)
- [uirouter_context.h](<uirouter_context.h>)
- [uner_app.h](<uner_app.h>)
- [uner_core.h](<uner_core.h>)
- [uner_core_types.h](<uner_core_types.h>)
- [uner_handle.h](<uner_handle.h>)
- [uner_handle_types.h](<uner_handle_types.h>)
- [uner_status.h](<uner_status.h>)
- [uner_transport.h](<uner_transport.h>)
- [uner_transport_uart1_dma.h](<uner_transport_uart1_dma.h>)
- [uner_transport_usbcdc.h](<uner_transport_usbcdc.h>)
- [uner_v2.h](<uner_v2.h>)
- [user_button.h](<user_button.h>)
- [wifi_bridge.h](<wifi_bridge.h>)
- [wifi_bridge_at.h](<wifi_bridge_at.h>)
- [wifi_bridge_config.h](<wifi_bridge_config.h>)
- [wifi_bridge_port.h](<wifi_bridge_port.h>)
- [wifi_bridge_types.h](<wifi_bridge_types.h>)
- [wifi_bridge_uner.h](<wifi_bridge_uner.h>)

## Subcarpetas

- [types](types/INFORME_CARPETA.md)
- [utils](utils/INFORME_CARPETA.md)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../../../AutoFinalRobo2-Contracts/provenance/files.json).
