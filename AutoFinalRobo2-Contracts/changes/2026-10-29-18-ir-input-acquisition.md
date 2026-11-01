> **AFR2-20261029-18 compilado SIN cargar:** diagnóstico BuildOnly exit0 y producto/comunicación/biblioteca Loader exit0, sin errores/advertencias. Diagnóstico flash13800/SRAM3180bytes; producto flash75600/SRAM6934bytes, margen1258bytes sin certificación dinámica de pila. Producto final12:34:40 del29/10/2026. Placa conserva15; no se abrió COM ni se ejecutó ensayo físico. Web solo fuente. Evidencia en Contracts/documentation/builds/2026-10-29-18/README.md; pendientes inferiores históricos. La prohibición de flashear sigue vigente.

# AFR2-20261029-18 — Adquisición de sensores de línea IR1..IR5

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex, 29/10/2026.

Usuario reporta ausencia de lecturas y aclara que se refiere a la barra IR1..IR5,
no a los Hall de las ruedas. Revisión por lectura de inicialización, puertos,
digital/ADC, filtro, EEPROM y publicación. No se interrogó la placa ni se midió
continuidad/tensión. No se atribuye a su EEPROM un valor que no fue leído.

## Hallazgo concreto y perfil de hardware

Defaults anteriores memset dejaban line_kind=0. Sensors_Task solo toma muestras
con1(digital) o2(ADC), por lo que0 no publica lecturas. Cargar firmware conserva
EEPROM: un0 antiguo puede mantenerlas deshabilitadas. line_calibrated solo
afecta validez de control; no bloquea raw con tipo habilitado. Un raw0 puede
ser una lectura correcta y no significa ausencia de datos.

[Fabricante OSOYOO](https://osoyoo.store/products/5-channel-tracking-sensor-for-osoyoo-model-3-robot-learning-kit-v2-model-2020001700):
barra de cinco salidas digitales, negro LOW/blanco HIGH. Perfil de este kit:
IR1=A4/PF4, IR2=A3/PF3, IR3=A2/PF2, IR4=A1/PF1, IR5=A0/PF0, como mapa del
proyecto. Son entradas distintas de Hall A8..A15. Se mantiene ADC opcional para
otro módulo explícitamente configurado; la barra digital no proporciona gris
analógico por sus cinco salidas. Polaridad nominal no prueba contraste físico.

## Adaptaciones por archivo

- Atmega/src/services/afr2_product.c: defaults digital1/invert31, sin confirmar
  calibración ni habilitar seguimiento/movimiento.
- Atmega/include/afr2_product.h y src/platform/afr2_preferences.c: journalv4 y
  migración SOLO de tipos deshabilitados en journals anteriores. Conserva
  perfiles ADC/digital explícitos. [Contrato](../contracts/EEPROM_IR_MIGRATION_4.md).
- Atmega/src/platform/afr2_sensors.c: garantiza buffers de entrada digital
  PF0..4 habilitados y reloj ADC disponible; conserva JTAG doble escritura ya
  existente, lectura digital de5ms, filtro2de3 y separación raw/control.
- Atmega/service/i2c_uart0_diagnostic.c: lectura PINF directa cada5ms, orden
  IR1..5, con JTAG/buffers habilitados y sin leer EEPROM. OLED agrega IR:xxxxx
  (niveles eléctricos) manteniendo servo, contador y botón. UART0 muestra
  baseline y cambios, hasta10reportes/s, RAW y máscara BLACK_LOW complementaria.
  No usa calibración ni filtro del producto; permite distinguir entrada física
  de procesamiento/configuración. Sin mover ruedas ni llamar sensores producto.
- Atmega/tools/flash-i2c-encoder-diagnostic.ps1: identifica banner18 y entradas
  IR, conserva -BuildOnly para compilar sin puerto/flash.
- Atmega/src/ui/afr2_sensor_screens.c y Web/src/features/oled/firmwareSensorRenderer.ts:
  ambas vistas IR muestran IR deshabilitado cuando tipo0, Esperando IR si aún
  no hay muestra vigente. Cero válido conserva valor/gráfico. Misma copia Web,
  coordenadas, glifos y ScreenCodes0x030016/0x030032; sin cambios de navegación.
- Web/src/features/oled/firmwareRenderer.sources.json: fingerprint revisado
  después de vincular el guard de validez. Catálogo documenta presentación.
- README/informes/guía de prueba: estado y límites, autorías preservadas.

Se revisó Hall por ambigüedad inicial: puertos/mapa agrupado y x1/risingA+B
coinciden con contrato vigente; no se diagnosticó su funcionamiento físico ni
se agregaron cambios Hall a18. Se conserva corrección de radar/latencia16 y
prueba servo17. I2C Manager único, sin Arduino ni DMA en AVR.

Estado inicial: fuentes completas, compilaciones pendientes siguiendo la
autorización previa de compilar al terminar el trabajo/prueba. No se flashea:
«No lo subas aun» sigue vigente. Sin tests, validadores o ensayo físico por el
agente. Web solo fuente, sin build/promoción a ESP; ESP/Loader sin cambios.
