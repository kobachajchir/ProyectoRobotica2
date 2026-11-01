# AFR2-20261028-18 — Revisión de arranque del OLED ATmega

Autoría: Koba Chajchir / AFR2; revisión mediante Codex, 28/10/2026.

## Evidencia recibida y alcance

El usuario informa que cargó el firmware del auto y el OLED sigue apagado.
El módulo es nuevo y nunca se probó en este montaje. Cree que su dirección es
`0x7C`; ese valor no está confirmado por identificación ni lectura del bus.
No se atribuye el síntoma al flasher temporal después de esta aclaración.

Revisión de fuentes y del mapa de enlace ya existente, sin ejecutar herramientas
de compilación, tests, validadores, COM, escáner I2C, flasheos ni pruebas físicas.
No se modifican parámetros del controlador a partir de una dirección supuesta.

## Recorrido revisado

- `AutoFinalRobo2-Atmega/src/main.c`: inicializa TWI, Manager y OLED;
  habilita interrupciones y atiende sus tareas en el bucle principal.
  La dirección candidata por defecto es `AFR2_OLED_ADDRESS7=0x3C`.
- `src/display/afr2_oled.c`: espera 100 ms del reloj de aplicación y envía
  la secuencia candidata SSD1306 128x64, incluida bomba de carga y `0xAF`
  (display ON). Después transmite ajustes y páginas del framebuffer.
  Ante fallo invalida la inicialización, incrementa errores y reintenta
  después de 1000 ms. No controla VCC mediante GPIO.
- `src/platform/afr2_twi.c`: maestro por IRQ a 100 kHz, SDA PD1/D20,
  SCL PD0/D21, sin pull-ups internos; requiere pull-ups externos.
  El backend transforma `address7` en el byte de escritura con `addr << 1`.
- `src/services/afr2_i2c_manager.c`: callback en main y timeout de la
  transferencia; el OLED solicita 40 ms. La recuperación es no bloqueante.
- El mapa existente sitúa `init_commands` en flash `0x03BA`; no hay evidencia
  de que esta tabla esté fuera del alcance del acceso PROGMEM cercano.
- `src/services/afr2_app.c`: expone listo/contador OLED en el snapshot y en
  pantallas de diagnóstico. El contador no distingue SLA NACK, DATA NACK,
  arbitraje o error de bus; no se observó su valor real durante esta revisión.

No se identificó una llamada de encendido ausente en este recorrido. Eso no
confirma ejecución del binario cargado, ACK del OLED ni compatibilidad del módulo.

## Dirección pendiente de identificar

| Dirección de 7 bits que recibe este driver | Byte de dirección al escribir |
| --- | --- |
| `0x3C` (fuente actual) | `0x78` |
| `0x3D` | `0x7A` |
| `0x3E` | `0x7C` |

La [hoja SSD1306 de Solomon Systech](https://www.mouser.com/datasheet/2/813/SSD1306-3401599.pdf),
sección 8.1.5, documenta direcciones de 7 bits `0111100` y `0111101`
(`0x3C`/`0x3D`). Si `0x7C` es realmente el byte de escritura declarado para
esta unidad, equivale a `0x3E` y no coincide con esas opciones SSD1306.
Si fuera una dirección declarada de 7 bits, tampoco es la dirección actual.
No se presupone que sea un error tipográfico ni se confirma otro controlador.

Primero identificar modelo/controlador y origen del dato `0x7C` mediante
serigrafía, ficha o publicación del módulo. Si no basta, queda pendiente una
consulta física de ACK/dirección y niveles SDA/SCL, solo por pedido expreso.
Una respuesta I2C por sí sola no identifica el controlador ni valida su renderer.

## Estado y procedencia por archivo

Único archivo añadido: esta ficha de revisión original AFR2; no copia código
ni altera autorías existentes. Sin cambio de contrato, ScreenCodes, navegación,
renderer o recursos gráficos.

ATmega: fuente y binarios sin cambios; síntoma reportado, causa aún no aislada.
ESP/Web/Loader/CAD: sin cambios por esta revisión. Los cambios previos del
Loader en el árbol de trabajo se conservan y no forman parte de esta ficha.
