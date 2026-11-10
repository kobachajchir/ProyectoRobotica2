> **Montaje confirmado por el usuario, AFR2-20261029-15:** motores y Hall ya cableados. Se corrige hall_wired en defaults/migración EEPROMv3; no se cambian pines ni se presume calibración. [Contrato de migración](../contracts/EEPROM_HALL_MIGRATION_3.md).

> **Build AFR2-20261027-10:** captura Hall habilitada por pedido humano y compilada. Preferencia Hall conectado/EEPROM conservada; activarla en menú si estaba0. Aún sin flasheo. [Evidencia](../documentation/builds/2026-10-27-10/README.md), [guía de carga](CARGA_USB_COM16.md). La nota de Hall deshabilitado en la hoja PDF07 describe esa entrega anterior.

> **AFR2-20261106-14:** incorporado el [PDF de encoders BioLog](supplied/Pinout_encoders_AGV_BioLog.pdf). El cableado coincide; su descripción x4, CPR1500, signos y orden lógico pertenecen a otra implementación. Nuestro firmware conserva x1 y calibración por rueda. Ver [cotejo con fuentes](COTEJO_ENCODERS_BIOLOG_20261106.md). Solo documentación, sin pruebas físicas.

> **AFR2-20261106-15:** [informe completo actualizado](INFORME_PINOUT_ROBOT_20261106.md) y [PDF para compartir](output/pdf/INFORME_PINOUT_ROBOT_20261106.pdf), con agregados propios identificados: OLED, encoder de menú, batería A5, D32 AutoKEY y D33 AutoRESET mediante NPN. IR digitales según fuente actual. El PDF07 enlazado abajo es histórico.

# Pinout del auto

AutoFinalRobo2 | OSOYOO Mecanum 4WD | Arduino Mega 2560

**AFR2-20261027-07 - 27/10/2026**

[Hoja PDF para imprimir](output/pdf/PINOUT_DEL_AUTO.pdf). Fuente editable: [PINOUT_DEL_AUTO.json](PINOUT_DEL_AUTO.json); generador: [generate_pinout_auto.py](../tools/generate_pinout_auto.py).

DEL/TRAS e IZQ/DER se interpretan mirando en el sentido de avance del auto.

## Motores - control desde el Mega

| Rueda | Model Y / salida | IN1 | IN2 | PWM |
| --- | --- | --- | --- | --- |
| DEL DER (FR) | M_B / BK1-BK2 | D22 | D24 | D9 / ENA |
| DEL IZQ (FL) | M_B / BK3-BK4 | D26 | D28 | D10 / ENB |
| TRAS DER (RR) | M_A / AK1-AK2 | D5 | D6 | D11 / ENA |
| TRAS IZQ (RL) | M_A / AK3-AK4 | D7 | D8 | D12 / ENB |

M+/M- van al puente H de su rueda; no al Mega. Polaridad de giro según montaje y calibración.

## Hall de ruedas - dos headers de 4 señales

| Rueda | A = S1 / naranja | B = S2 / verde |
| --- | --- | --- |
| DEL DER | A8 (D62) | A12 (D66) |
| DEL IZQ | A9 (D63) | A13 (D67) |
| TRAS DER | A10 (D64) | A14 (D68) |
| TRAS IZQ | A11 (D65) | A15 (D69) |

Header A: A8, A9, A10, A11. Header B: A12, A13, A14, A15. Ambos: DEL DER, DEL IZQ, TRAS DER, TRAS IZQ.

## Hall de ruedas - alimentación

| En cada encoder Hall | Conexión |
| --- | --- |
| VCC / V (blanco en manual) | +5 V regulados |
| GND / G (amarillo en manual) | GND común con el Mega |

Los cuatro VCC comparten +5 V y los cuatro GND comparten masa. OSOYOO advierte variantes blanco/amarillo invertidos: identificar V/G en la placa. No usar tensión de batería en VCC Hall.

## Potencia y voltímetro del kit

| Origen / conexión | Destino |
| --- | --- |
| Portabaterías + / - | Model Y VIN+ / VIN- |
| Model Y VOUT+ / VOUT- | Entrada POWER del shield |
| Voltímetro VCC / VT / GND | Cabecera homónima del Model Y |

Ruta de potencia del manual; VOUT no se considera una salida regulada de 5 V. VT mide tensión; no es un GPIO del Mega. GND lógico común; no usar una salida M- del puente H como masa.

## Sensores de línea, ultrasonido y servo

**Fuente AFR2-20261030-03:** IR1..IR5 conservan A4..A0 y pasan por pedido humano a muestreo ADC10 (0..1023), `line_kind=2`. Batería conserva A5. Journal6 migra configuración anterior en RAM; sin calibración ni prueba física de la barra. [Contrato ADC](../contracts/EEPROM_IR_ADC_6.md).

**Fuente AFR2-20261029-26 — batería:** A5/PF5/ADC5 queda asignado al nodo del divisor10k/10k, con capacitor104/100nF a GND. Tomar batería desde voltímetro después de llave, corte comunicado por usuario; no llevar BAT+ directo a A5 ni usar VT como salida ya adaptada sin identificar. Sistema → Batería inicia adquisición. Fuente sin compilar/cargar ni probar físicamente; PDF07 aún no actualizado. [Contrato9](../contracts/BATTERY_MEASUREMENT_9.md).

| Elemento | Señal | Pin Mega / alimentación |
| --- | --- | --- |
| Línea | IR1 / IR2 / IR3 / IR4 / IR5 | A4 / A3 / A2 / A1 / A0 |
| Línea | VCC / GND | +5 V / GND |
| HC-SR04 | TRIG / ECHO | D30 / D31 |
| HC-SR04 | VCC / GND | +5 V / GND |
| Servo radar | S / VCC / GND | D13 / +5 V / GND |
| KEY/RST ESP (opcional, AFR2-20261106-11) | KEY / RST → base de NPN por 1 kΩ | D32 / D33 (PC5 / PC4); solo flasher de servicio, entrada en alta impedancia el resto del tiempo. [Circuito](PLAN_KEY_RST_AUTOMATICO.md) |

S del servo es señal. Su +5 V es de alimentación, no un GPIO. A4 está ocupado por IR1; no usar A4/A5 como I2C del Mega.

## Panel añadido - OLED y encoder de menú

| Elemento | Señal | Pin Mega / alimentación |
| --- | --- | --- |
| OLED | SDA / SCL | D20 (SDA) / D21 (SCL) |
| OLED | VCC / GND | +5 V / GND |
| Encoder menú | CLK / DT / SW | D2 / D3 / D42 |
| Panel encoder | VCC / GND | +5 V / GND |
| Encoder sin breakout | Comunes giro / pulsador | GND / GND |

5 V en el montaje: confirmado por Koba. El encoder mecánico cierra contactos a GND; VCC alimenta el panel/pull-ups. OLED con pines arriba, de frente: GND, VCC, SCL, SDA. Usar serigrafía real.

## Conectores del panel

| Conector | Orden de señales |
| --- | --- |
| J1 hembra 1x5 / 2,54 mm | 1 CLK, 2 DT, 3 SW, 4 SDA, 5 SCL |
| J2 JST 1x2 / 2,00 mm | 1 VCC (+5 V montaje), 2 GND |

Numeración del diseño; identificar pin 1 físicamente. El CAD guardado conserva +3V3: esta hoja registra el montaje a 5 V, no una actualización del esquema/PCB.

## Wi-Fi - UART de operación

| Origen | Destino |
| --- | --- |
| Mega D18 / TX1 | E_RX del selector del shield |
| E_TX del selector del shield | Mega D19 / RX1 |
| GND Mega | GND shield |

Usar E_RX/E_TX del selector. El puerto directo ESP UART es de 3,3 V. Aislar selección D4/D5 y D0/D1; Bluetooth desconectado de UART1. D5 ya controla un motor.

## Compatibilidad del nuevo agrupamiento Hall

A8-A11 son PK0-PK3 y A12-A15 son PK4-PK7. Las ocho señales pertenecen a PINK y al grupo PCINT2; se conserva una captura simultánea del puerto. No se necesita adyacencia entre cada A y B.

El firmware usa x1: cuenta flancos ascendentes de A y lee B para el sentido. La máscara cambia de 0x55 a **0x0F** para interrumpir solo con PK0-PK3. Los contadores y el protocolo mantienen el orden lógico **FL, FR, RL, RR**; el orden físico de los headers es **FR, FL, RR, RL**. No cambia CPR ni formato wire.

## Estado de esta entrega

Build AFR2-20261027-10: captura Hall habilitada y firmware compilado. Aún sin flashear. Si la preferencia estaba en 0, activar Hall conectado=1 en Geometria y guardar EEPROM. Funcionamiento físico no probado.

El usuario confirma alimentación del panel OLED/encoder a 5 V. Se registra como montaje comunicado, sin certificar tolerancia del OLED, pull-ups, continuidad ni rendimiento físico. No se modifica el CAD que conserva la red +3V3. El build AFR2-20261027-10 habilita captura Hall por pedido humano; la preferencia Hall conectado y la EEPROM se conservan. La versión flasheada previa puede usar otro mapa.

D0/D1 corresponden a USB/USART0; no son el enlace operativo ESP. La carga temporal tiene cableado propio en [FLASHING.md](FLASHING.md). Para el selector y niveles, ver [WIFI_SHIELD.md](WIFI_SHIELD.md). Para IRQ y límites, ver [INTERRUPCIONES_ENCODER_HALL.md](INTERRUPCIONES_ENCODER_HALL.md).

## Referencias

- [Manual OSOYOO del kit](supplied/ManualOsoyoo.pdf): Páginas PDF 5, 14-17, 26-27, 33-34 y 43.
- [Mapa de pines del firmware](../../AutoFinalRobo2-Atmega/include/board_pinmap.h): Asignación activa en fuente; Hall agrupados en revisión 07.
- [Arduino Mega 2560 - pinout oficial](https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf): Correspondencia A8-A15, D62-D69 y puerto K.
- [Microchip ATmega2560 - hoja de datos](https://ww1.microchip.com/downloads/en/devicedoc/atmega640-1280-1281-2560-2561-datasheet-ds40002211a.pdf): Puerto K / PCINT16-PCINT23 y PCMSK2.
- [OSOYOO - alimentación y colores Hall](https://osoyoo.com/2020/07/01/robot-tank-car-kit-v2-lesson8-use-encoder-to-synchronize-motor-speed/): Hall V a 5 V; G a GND; S1 naranja; S2 verde.
- [OSOYOO - variantes de alimentación del encoder](https://osoyoo.com/2019/11/08/how-to-test-motor-encoder-with-arduino/): Advierte variantes con blanco y amarillo intercambiados.
- [LCDwiki - OLED de referencia MC096VX](https://www.lcdwiki.com/0.96inch_OLED_Module_%28IIC-4P_SKU%3AMC096VX%29): Referencia 3,3/5 V; no identifica por sí sola la unidad instalada.
- [Montaje comunicado por Koba Chajchir](../changes/2026-10-27-07-complete-pinout-grouped-hall.md): 27/10/2026: panel a 5 V; Hall en dos grupos A y B, orden FR/FL/RR/RL.

**Hecho por Koba Chajchir**
