# Pinout y PCB de interfaz

[Maestro](../docs/INFORME_MAESTRO.md). Base: supplied/Pinout_OSOYOO_Mecanum.csv y ManualOsoyoo.pdf, páginas PDF 14-17, 26-27, 33-34, 38, 43 (no numeración impresa). [CSV ampliado](PINOUT_PROPUESTO.csv).

| Función | Señales documentadas |
| --- | --- |
| FR / BK1-BK2 | Dirección D22/D24, PWM D9 |
| FL / BK3-BK4 | Dirección D26/D28, PWM D10 |
| RR / AK1-AK2 | Dirección D5/D6, PWM D11 |
| RL / AK3-AK4 | Dirección D7/D8, PWM D12 |
| Servo ultrasonido | D13 reservado |
| HC-SR04 | TRIG D30, ECHO D31 |
| Línea IR1..IR5 | A4, A3, A2, A1, A0 |
| ESP operativo | TX1 D18 → E_RX; E_TX → RX1 D19 |

Bluetooth comparte UART1 en el manual y queda fuera de esta arquitectura; no colocar sus salidas en paralelo con ESP. Polaridad por rueda pendiente de comprobar.

## Adiciones propuestas

| Terminal PCB | Destino | Función |
| --- | --- | --- |
| GND | Masa común | Referencia eléctrica |
| VCC | Alimentación por definir según módulos | No asumir OLED 5 V |
| CLK | D40 / PG1 | Encoder UI canal A |
| DT | D41 / PG0 | Encoder UI canal B |
| SW | D42 / PL7 | Pulsador activo bajo con pull-up |
| SDA | D20 / PD1 | OLED TWI |
| SCL | D21 / PD0 | OLED TWI |

Asignación de puertos: [pinout oficial Mega](https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf) y [variante Mega AVR](https://github.com/arduino/ArduinoCore-avr/blob/master/variants/mega/pins_arduino.h). Son propuestas sin cableado físico confirmado. D40-D42 no se usan en la documentación recibida. SDA/SCL no son A4/A5 en Mega; A4 ya pertenece a IR1. No usar D13 como LED diagnóstico: señal de servo.

La cabecera de siete señales puede compartir VCC solo si encoder/OLED/pull-ups admiten esa tensión. Si difieren, separar VCC_OLED/VCC_ENCODER y adaptar niveles I2C. No unir pull-ups a tensiones distintas. Un encoder pasivo puede usar GND y pull-ups del MCU; un módulo con resistencias/LED requiere revisar circuito. Prever desacople y antirrebote.

Los Hall de ruedas son otros sensores. El manual los deja sin conectar; su asignación permanece pendiente. Un banco PCINT como PK/A8..A15 es candidato a estudiar para ocho señales, no cableado aprobado. Comprobar niveles, acceso del shield y carga CPU; nunca adquirirlos con el polling lento del encoder UI.
