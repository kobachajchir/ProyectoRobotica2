# AFR2-20261029-27 — Controles rápidos, radar y actuadores

Implementación autorizada del plan del usuario. ATmega/ESP/Web: trabajo en fuente; sin compilación, validadores, flasheos ni ensayos. Loader fuera de alcance. Se conservan cambios previos, incluida batería AFR2-20261029-26.

## Implementación

- ATmega/ESP0.11.0 y Web0.4.0. EX01v9 conserva batería y agrega PWM0x1e/feature0x80000, modo7 y estado de habilitación de motores. EEPROMv5 agrega alcance/rastro sin perder registros válidos v1..4.
- Carrusel010003 con seis tarjetas y configuración existente010002. Largo al soltar, retorno contextual, confirmaciones obligatorias y Hall con borradores conservados. Opciones03003b, alcance03003c y editor030033 compartido.
- Radar4/2/1/0.5m con adquisición nominal, rastro opcional, marcadores rellenos, cono por PWM aplicado y aviso fuera de escala. IR RAW centrados sin fuente adicional y listado en dos columnas más sensor5.
- PWM manual centralizado sin odometría/HC; límites, rampas, bloqueo, TTL y STOP. Se conservan comandos anteriores de velocidad. Deshabilitar motores detiene y bloquea también pruebas individuales.
- Servo Timer5/D13: propiedad explícita de pin, compare futuro y pausa/reanudación distinta de apagado. La causa del servo quieto no queda físicamente confirmada ni se declara resuelta por compilar.
- ESP: ranura hello/alive independiente de UART, diagnóstico WS/Wi-Fi/boot y un único reintento STA. Web: cierre idempotente por generación, un latido pendiente, porcentajes PWM y diagnóstico visible en Sistema.

## Entrega y procedencia

Catálogo, fuente e iconos copiados mediante `--copy-only`, sin validadores ni compilación. C/TS adaptados antes de actualizar sus identidades de fuente. Las diferencias se documentan como fuente sin prueba de paridad. Sin promoción a ESP/data/dist ni apertura de COM8 u otro puerto. Se preservan autores y cambios ajenos de batería/Loader.

[Contrato e interfaces](../contracts/QUICK_RADAR_ACTUATORS_9.md) · [Archivos y procedencia](../provenance/quick-radar-actuators-20261029-27.json).

## Pendiente

Compilar y medir SRAM/pila, revisar paridad raster y ensayar navegación/Hall/EEPROM/IR/WS. Contrastar el firmware de prueba de servo con Timer5 del producto; probar motores por rueda/dirección, bloqueo y STOP. Comprobar pantalla negra y latencia. Los7085bytes estáticos del build19 no representan estas fuentes ni batería26.

Identificar firmware efectivo del Mega: el último registro disponible dejó instalado el flasher temporal. El despliegue coordinado ESP+LittleFS+producto ATmega requiere autorización posterior. No se afirma que la placa esté ejecutando estas fuentes.
