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

## Adiciones acordadas y conectores

El shield ya presenta SDA/SCL con nomenclatura I2C, según el usuario. Usar esas cabeceras; D20/PD1 y D21/PD0 son equivalencias de firmware, no números que deban estar impresos allí.

La placa OLED/encoder tendrá J1 1x5 de paso 2,54 mm para CLK→D40, DT→D41, SW→D42, SDA→cabecera SDA y SCL→cabecera SCL. J2 1x2, también 2,54 mm, tendrá VCC y GND comunes a ambos módulos. [Detalle eléctrico y mecánico](PCB_OLED_ENCODER.md).

Asignaciones AVR: CLK PG1, DT PG0, SW PL7, SDA PD1 y SCL PD0; SW activo bajo con pull-up. VCC común está acordado, tensión numérica/pull-ups pendientes de módulos reales. No asumir 5 V por ser Mega. No se ha conectado ni probado la nueva PCB.

Fuente de correspondencias: [pinout oficial Mega](https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf). A4 ya pertenece a IR1 y D13 al servo. D40-D42 no figuran ocupados en el pinout recibido; confirmar montaje real antes de cablear.

Los Hall de ruedas son otros sensores. El manual los deja sin conectar; su asignación permanece pendiente. Un banco PCINT como PK/A8..A15 es candidato a estudiar para ocho señales, no cableado aprobado. Comprobar niveles, acceso del shield y carga CPU; nunca adquirirlos con el polling lento del encoder UI.
