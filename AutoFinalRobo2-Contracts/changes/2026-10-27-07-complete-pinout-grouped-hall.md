# AFR2-20261027-07 - Pinout completo y Hall agrupados

27/10/2026. Pedido directo de Koba Chajchir: preparar un pinout completo para
dejar junto al auto, con OLED, encoder de menú y cuatro Hall; firmado
"Hecho por Koba Chajchir" e incorporado a la documentación. Durante el pedido
confirma panel OLED/encoder alimentado a 5 V y solicita dos grupos consecutivos
para Hall, en orden DEL DER, DEL IZQ, TRAS DER, TRAS IZQ.

## Antes y después

Antes los Hall estaban asignados por pares intercalados: FL A8/A9, FR A10/A11,
RL A12/A13, RR A14/A15, con máscara de interrupción 0x55.

| Rueda | A / S1 naranja | B / S2 verde |
| --- | --- | --- |
| DEL DER / FR | A8 / PK0 | A12 / PK4 |
| DEL IZQ / FL | A9 / PK1 | A13 / PK5 |
| TRAS DER / RR | A10 / PK2 | A14 / PK6 |
| TRAS IZQ / RL | A11 / PK3 | A15 / PK7 |

La máscara nueva es 0x0F. Las entradas A8-A15 pertenecen al puerto K y al grupo
PCINT2. No hay restricción de emparejar pines contiguos: el driver captura PINK
en una sola lectura y utiliza símbolos por rueda para A y B. Se conserva x1
(subida de A, signo desde B) y el orden lógico FL/FR/RL/RR de contadores,
calibración y comunicación. Este cambio no modifica formatos, IDs, CPR,
telemetría ni preferencias. No se atribuye prioridad por rueda al orden físico.

## Alimentación y montaje comunicado

- Hall: VCC a +5 V regulados, GND común con Mega; S1=A naranja y S2=B verde.
  El manual usa blanco para V y amarillo para G. OSOYOO advierte variantes
  invertidas; se debe seguir la identificación del ejemplar, no solo el color.
- OLED y panel del encoder: usuario confirma montaje a +5 V. Encoder mecánico
  sin breakout con comunes a GND; VCC del panel alimenta su red/pull-ups.
- El KiCad guardado mantiene +3V3 y R2/R3 a esa red. No se edita CAD en esta
  entrega; esta ficha no afirma que el esquema/PCB esté actualizado a 5 V ni
  acredita tolerancia del OLED o de sus pull-ups por el montaje comunicado.
- La confirmación de alimentación no acredita las ocho señales Hall ya
  conectadas ni lectura correcta. Se conserva AFR2_HALL_CONNECTED=0.

## Entrega documental

- [Pinout completo](../hardware/PINOUT.md): motores, Hall, alimentación,
  voltímetro, línea, ultrasonido, servo, OLED, encoder UI, conectores y UART.
- [PDF imprimible A4 de dos páginas](../hardware/output/pdf/PINOUT_DEL_AUTO.pdf),
  con referencias y autoría en ambas páginas. Se conserva como entrega para
  compartir junto al vehículo, generada desde la misma fuente que el Markdown.
- [Fuente editable](../hardware/PINOUT_DEL_AUTO.json) y
  [generador](../tools/generate_pinout_auto.py), que usa reportlab.
- CSV ampliado y contrato de interrupciones alineados al nuevo agrupamiento.
  Documentacion/ original y fichas históricas se preservan.

## Fundamento documental

- [Arduino Mega: A8-A15 = D62-D69 / PK0-PK7](https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf).
- [Microchip ATmega2560: puerto K, PCINT16-PCINT23 y PCMSK2](https://ww1.microchip.com/downloads/en/devicedoc/atmega640-1280-1281-2560-2561-datasheet-ds40002211a.pdf).
- [OSOYOO: Hall a 5 V y colores V/G/S1/S2](https://osoyoo.com/2020/07/01/robot-tank-car-kit-v2-lesson8-use-encoder-to-synchronize-motor-speed/).
- [OSOYOO: variantes blanco/amarillo](https://osoyoo.com/2019/11/08/how-to-test-motor-encoder-with-arduino/).
- Manual local supplied/ManualOsoyoo.pdf y fuente board_pinmap.h; confirmación
  del usuario en esta conversación. Las referencias de otro kit no sustituyen
  la identificación física de los sensores instalados.

## Estado real

| Módulo | Estado |
| --- | --- |
| atmega | Pines y máscara Hall actualizados en fuente; deshabilitado; sin compilación ni flasheo |
| esp | Sin cambio de fuente, build ni paquete |
| web | Sin cambio de fuente, build ni paquete |
| contracts | Pinout, PDF, fuente/generador, CSV y documentación actualizados |
| cad | No editado por esta entrega; red +3V3 histórica no actualizada |
| hardware | Alimentación reportada por el usuario; funcionamiento no comprobado |

Sin tests, validadores, compilaciones, flasheos, ERC/DRC, comprobación física ni
validación visual del PDF, conforme a la instrucción del proyecto. La ejecución
del generador es autoría documental, no una compilación o validación de producto.
No se crea commit ni se publica Git en esta entrega.
