# Referencias de trabajo

Rutas relativas a la raíz de AutoFinalRobo2-Contracts:

- `../AutoFinalRobo2-Atmega/include/afr2_oled.h`: dibujo/presentación por páginas.
- `../AutoFinalRobo2-Atmega/include/afr2_i2c_manager.h`: puerto start/poll/abort.
- `../AutoFinalRobo2-Atmega/include/afr2_screens.h`: snapshot y wrappers.
- `../AutoFinalRobo2-Atmega/src/ui/afr2_screens.c`: ejemplos Inicio, menú, sensores y OLED.
- `../AutoFinalRobo2-Atmega/src/input/afr2_encoder_ui.c`: corto/largo exclusivos.
- `../AutoFinalRobo2-Atmega/reference/f4/Core/Src/{oled_utils,screenWrappers,menusystem,osk}.c`: fuentes de referencia.
- `reference/automicro/documentation/published/shared/SCREEN_SYSTEM.md`: limitaciones históricas de identidad/raster/estado.

El handle OLED posee 1024 bytes de framebuffer y 129 de snapshot TX; el Manager y hardware se prestan. Render y servicio se llaman en main. Una página en vuelo usa snapshot estable; render puede ensuciar nuevamente esa página. No prometer pantalla completa atómica. Fuente 7x10 tiene accesores PROGMEM; al sumar recursos revisar límite de direccionamiento AVR.

Uso: preparar snapshot coherente → Apply de evento → Render si hay cambio → Task OLED periódica. Manager Tick despacha finalización en main. Renderer no llama Task ni Manager. Poll encoder recibe SW ya normalizado activo bajo y CLK/DT simultáneos; no es adquisición de Hall de ruedas.

Para copiar una pantalla F4 localizar toda su cadena de helpers, recursos y estado. Registrar campos que el nuevo robot no tiene; rediseñar la vista en lugar de simular sensores. Si requiere datos remotos aún no reportados, mantener pendiente ESP/Web.

El árbol funcional pendiente está en `documentation/MENU_PLAN.md`; leerlo al crear submenús. Distinguir ese diseño de las cuatro vistas actuales de `screens.json`. La PCB nueva usa J1 señales 1x5 y J2 alimentación común 1x2 de 2,54 mm, según `hardware/PCB_OLED_ENCODER.md`. SDA/SCL se conectan a cabeceras I2C ya expuestas por el shield.
