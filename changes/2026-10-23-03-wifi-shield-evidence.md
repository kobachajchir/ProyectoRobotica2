# AFR2-20261023-03 - Evidencia del shield y prompt consolidado

Fecha 2026-10-23. Cambio documental de hardware/contexto, sin wire ni código de consumidores.

Antes: regulación/jumpers del shield genéricamente pendientes. Después: OSOYOO UART WiFi Shield V1.3 documentado, regulación/traducción declaradas y puertos/KEY/RST identificados en imágenes. Selector E_RX/E_TX y puerto directo ESP UART 3V3 se distinguen; ruta eléctrica real no probada. Operación Mega USART1 D18/D19, aislada de D4/D5 y D0/D1. D5 pertenece al motor RR.

Mapa aportado es UNO: se mantiene Mega SDA D20/PD1 y SCL D21/PD0. Carga mediante puente USB requiere D0→RX ESP y TX ESP→D1, con Mega en reset y ruta de niveles definida; jumpers normales del selector no sirven como bypass directo. No se publican posiciones físicas finales ni comandos de carga.

Ficha nominal ESP-12S V1.1: 32 Mbit / 4 MiB, variante real/SPI/layout pendientes. Diferenciados AT de fábrica, baud propio candidato y upload. Se registra errata probable `9600 kbps` sin presentarla como medición. AT/RTOS SDK no se incorporan al producto Arduino + LittleFS.

Se copian tres imágenes del usuario y dos fuentes primarias (datasheet/Fritzing) con manifest incremental; originales preservados. Fritzing no acredita esquema interno. Prompt completo en docs/PROMPT_CONTINUIDAD.md conserva pantallas, navegación de un SW y PCB de dos conectores/2 tornillos. KEY/RST no son entradas de menú.

Documentos afectados: hardware/WIFI_SHIELD, FLASHING, PINOUT/CSV, PCB_OLED_ENCODER, RECURSOS; maestro local y raíz; decisiones/roadmap; PROTOCOL/MENU_PLAN solo contexto de transporte; índices/procedencia; hallazgos 007 actualizado y 011-013 nuevos; overlays. No se regeneró referencia AutoMicro. La corrección de nombres UART en CSV explicita dirección, sin renombrar código.

| Consumidor | Estado |
| --- | --- |
| atmega | Código sin cambio; backend/control y adaptación pendientes |
| esp | Código sin cambio; identificación física/configuración/bridge pendientes |
| web | Código sin cambio; port de interfaz pendiente |
| shared | Documentación y evidencia actualizadas; ninguna compatibilidad probada |

No se ejecutaron tests, builds, validadores, CI, flasheos ni ensayos físicos. No se descargaron ejecutables ni firmware AT. Commit local adicional, sin remoto/push. AutoMicro y sus repos asociados sin modificación. CAD diferido hasta planos del usuario.
