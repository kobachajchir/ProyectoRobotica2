> AFR2-20261024-11: CLK/DT pasan de D40/D41 a D2/D3; requiere recableado. SW D42. Hall PCINT2 A8–A15 revisado en fuente, deshabilitado. [Contrato IRQ](INTERRUPCIONES_ENCODER_HALL.md). Sin compilación ni prueba física.

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
| ESP operativo | Mega TX1 D18 → E_RX del selector; E_TX del selector → Mega RX1 D19 |

Bluetooth comparte UART1 en el manual y queda fuera de esta arquitectura; no colocar sus salidas en paralelo con ESP. Retirar/aislar jumpers hacia D4/D5 y D0/D1 al usar D18/D19; D5 ya es dirección RR. [Shield, selector y niveles](WIFI_SHIELD.md). El puerto directo ESP UART de 3,3 V no se sustituye por E_RX/E_TX sin revisar la ruta de niveles. Polaridad por rueda pendiente de comprobar.

## Adiciones acordadas y conectores

El usuario y las imágenes identifican las cabeceras SDA/SCL. Usarlas; en Mega equivalen a D20/PD1 y D21/PD0, sin exigir esos números impresos. El mapa OSOYOO aportado usa equivalencias UNO D18/D19 y A4/A5: no copiarlas a Mega. A4 permanece IR1; la continuidad del shield real aún no se ha medido.

La placa OLED/encoder tiene J1 hembra 1x5 de paso 2,54 mm para CLK→D2, DT→D3, SW→D42, SDA→cabecera SDA y SCL→cabecera SCL. J2 1x2 JST de **2,00 mm** lleva +3V3 y GND; el OLED va en socket hembra 1x04 de 2,54 mm. Encoder sin breakout con comunes a GND. [Detalle eléctrico y mecánico vigente AFR2-20261025-12](PCB_OLED_ENCODER.md).

Asignaciones AVR: CLK PE4/INT4, DT PE5/INT5, SW PL7, SDA PD1 y SCL PD0; SW activo bajo con pull-up. +3V3 confirmado por el usuario; pull-ups y compatibilidad de niveles I2C con Mega siguen pendientes de identificar en los módulos reales. No se ha conectado ni probado la nueva PCB.

Fuente de correspondencias: [pinout oficial Mega](https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf). A4 ya pertenece a IR1 y D13 al servo. D2/D3/D42 no figuran ocupados en el pinout recibido; confirmar montaje real antes de cablear.

Los Hall de ruedas siguen sin conectar. La asignación de fuente es FL A8/A9, FR A10/A11, RL A12/A13 y RR A14/A15: PK0..PK7 / PCINT16..23, banco PCINT2. Se conserva x1: IRQ por A (máscara0x55), flanco ascendente contado y B para dirección. Sin superposición en el mapa actual; niveles, acceso físico y correspondencia S1/S2 pendientes. AFR2_HALL_CONNECTED=0 se conserva. [Detalle de captura y prioridad](INTERRUPCIONES_ENCODER_HALL.md).

En el CSV, las señales UART se nombran `MEGA_TX1_TO_ESP_RX` y `ESP_TX_TO_MEGA_RX1` para explicitar el emisor/receptor. Esta corrección documental no renombra símbolos C ni cambia asignaciones eléctricas.
