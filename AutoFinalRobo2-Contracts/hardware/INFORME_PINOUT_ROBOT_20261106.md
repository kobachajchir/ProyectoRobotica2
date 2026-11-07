# Pinout completo del robot

AutoFinalRobo2 | OSOYOO Mecanum 4WD | Arduino Mega 2560 + ESP-12S

06/11/2026 | AFR2-20261106-15 | Informe de conexiones y agregados propios

## 1. Alcance y agregados del proyecto

Este informe reúne las conexiones del robot documentadas y su asignación en las fuentes actuales. DEL/TRAS e IZQ/DER se interpretan mirando en el sentido de avance. FL = delantera izquierda; FR = delantera derecha; RL = trasera izquierda; RR = trasera derecha.

**BASE** identifica componentes del kit; **AGREGADO PROPIO** identifica incorporaciones de nuestro proyecto; **SERVICIO** identifica conexiones de programación o diagnóstico. El agrupamiento Hall y el enlace UART del Mega son adaptaciones de conexión de componentes base, no sensores nuevos.

| Elemento | Origen | Señales principales del Mega |
| --- | --- | --- |
| Cuatro motores y dos drivers Model Y | BASE | D5 a D12; D22, D24, D26, D28 |
| Cuatro encoders Hall de las ruedas | BASE; agrupamiento adaptado | A8 a A15 |
| Cinco sensores de línea IR | BASE | A4, A3, A2, A1, A0 |
| HC-SR04 y servo de radar | BASE | D30, D31 y D13 |
| Shield Wi-Fi ESP-12S | BASE; UART adaptada al Mega | D18/TX1 y D19/RX1 |
| Pantalla OLED I2C | AGREGADO PROPIO | D20/SDA y D21/SCL |
| Encoder de menú con pulsador | AGREGADO PROPIO | D2/CLK, D3/DT y D42/SW |
| Medición de batería con divisor y filtro | AGREGADO PROPIO | A5/ADC5 |
| AutoKEY del ESP mediante NPN | AGREGADO PROPIO; SERVICIO | D32/PC5 |
| AutoRESET del ESP mediante NPN | AGREGADO PROPIO; SERVICIO | D33/PC4 |

**Estado:** cotejo documental y lectura de fuentes. Este informe no identifica el binario instalado ni comprueba continuidad, niveles eléctricos, precisión o funcionamiento físico. ATmega, ESP y Web no cambian por esta entrega. No se ejecutaron tests, compilaciones, validadores, flasheos ni pruebas físicas.

El usuario comunicó motores/Hall cableados y alimentación del panel a 5 V. AutoKEY/AutoRESET están implementados en fuente de servicio, con montaje y ensayo pendientes. Los apartados siguientes distinguen estas situaciones.

<!-- pagebreak -->

## 2. Tracción y encoders Hall de las ruedas

### Motores: componentes base

Cada motor se conecta a su puente H. IN1/IN2 fijan dirección y PWM controla la salida. M+ y M- son conexiones de potencia del motor: no van a GPIO ni se usan como masa lógica.

| Rueda | Driver / salida motor | IN1 del Mega | IN2 del Mega | PWM del Mega |
| --- | --- | --- | --- | --- |
| FR / delantera derecha | M_B / BK1-BK2 | D22 / PA0 | D24 / PA2 | D9 / PH6 / OC2B |
| FL / delantera izquierda | M_B / BK3-BK4 | D26 / PA4 | D28 / PA6 | D10 / PB4 / OC2A |
| RR / trasera derecha | M_A / AK1-AK2 | D5 / PE3 | D6 / PH3 | D11 / PB5 / OC1A |
| RL / trasera izquierda | M_A / AK3-AK4 | D7 / PH4 | D8 / PH5 | D12 / PB6 / OC1B |

Los signos del motor son configurables. El pinout no determina por sí solo qué signo produce avance en el montaje real.

### Hall: sensores base, cableado agrupado por nuestro proyecto

| Rueda | A = S1 / naranja | B = S2 / verde | Puerto K A/B |
| --- | --- | --- | --- |
| FR | A8 / D62 | A12 / D66 | PK0 / PK4 |
| FL | A9 / D63 | A13 / D67 | PK1 / PK5 |
| RR | A10 / D64 | A14 / D68 | PK2 / PK6 |
| RL | A11 / D65 | A15 / D69 | PK3 / PK7 |

Header A: A8-A9-A10-A11. Header B: A12-A13-A14-A15. Ambos siguen el orden físico FR-FL-RR-RL. El código, la telemetría y los índices de ruedas usan FL-FR-RL-RR.

| Cable de cada encoder Hall | Destino |
| --- | --- |
| VCC / V; blanco según manual | +5 V regulados, común a los cuatro |
| GND / G; amarillo según manual | GND común del Mega |
| Rojo / negro; M+ / M- | Salida del puente H correspondiente |

Hay variantes con blanco y amarillo invertidos: prevalece la serigrafía V/G. La alimentación Hall no se toma directamente de la batería.

La fuente actual habilita pull-ups en A8-A15 y captura por PCINT2. Cuenta **x1: subida de A y sentido leído en B**, con máscara 0x0F. El CPR se calibra por rueda y se guarda en EEPROM; no se fija 1500 por el PDF BioLog, que describe otra implementación x4. Hall de ruedas y encoder de menú son dispositivos distintos.

<!-- pagebreak -->

## 3. Sensores base y medición de batería añadida

### Barra de línea, ultrasonido y servo

| Elemento / señal | Pin Mega | Puerto / función | Origen |
| --- | --- | --- | --- |
| IR1 | A4 / D58 | PF4, entrada digital | BASE |
| IR2 | A3 / D57 | PF3, entrada digital | BASE |
| IR3 | A2 / D56 | PF2, entrada digital | BASE |
| IR4 | A1 / D55 | PF1, entrada digital | BASE |
| IR5 | A0 / D54 | PF0, entrada digital | BASE |
| HC-SR04 TRIG | D30 | PC7, salida | BASE |
| HC-SR04 ECHO | D31 | PC6, entrada | BASE |
| Servo de radar, señal S | D13 | PB7, pulsos por software | BASE |

Alimentación documental de barra IR, HC-SR04 y servo: +5 V y GND común. El cable S del servo es señal, no alimentación. La fuente actual usa IR digitales; las descripciones anteriores de ADC para la barra son históricas. El ADC se utiliza para batería. No se adopta A4/A5 como I2C: en el Mega el bus está en D20/D21.

HC-SR04 y radar se conservan para sensado. Según AFR2-20261106-13, no son requisito ni causa de parada por distancia frontal en los modos físicos vigentes. D13 está ocupado por el servo: no usarlo como LED de estado.

### Batería: AGREGADO PROPIO

Se añadió una rama de medición; el voltímetro del kit es un componente base. La toma acordada es tensión de batería después de la llave, desde la conexión del voltímetro. No se supone que el terminal VT ya entregue una tensión adaptada al ADC.

| Conexión | Destino / componente |
| --- | --- |
| Batería + después de llave | Resistencia superior de 10 kΩ |
| Otro extremo de resistencia superior | Nodo de medida A5 / D59 / PF5 / ADC5 |
| Nodo A5 a GND común | Resistencia inferior de 10 kΩ |
| Nodo A5 a GND común | Capacitor 104 / 100 nF, en paralelo con resistencia inferior |

```text
BAT+ tras llave -- 10 kohm --+-- A5 / ADC5
                           +-- 10 kohm -- GND
                           +-- 100 nF --- GND
```

Circuito acordado sin diodo. La división nominal es 2:1: A5 recibe aproximadamente Vbat/2. El firmware usa AVCC nominal de 5 V, adquisición ADC de 10 bits y promedio de ocho muestras. La medición se inicia explícitamente; no representa corriente, autonomía ni porcentaje de carga.

A5 es entrada sin pull-up; **BAT+ no se conecta directamente a A5**. La lectura continúa sin calibración física acreditada. La toma conmutada fue comunicada por el usuario; el informe no certifica margen ante transitorios ni funcionamiento con el Mega sin alimentación.

<!-- pagebreak -->

## 4. Panel de usuario: agregados propios

### OLED I2C

| Señal OLED | Conexión al Mega | Observación |
| --- | --- | --- |
| SDA | D20 / PD1 / SDA | Tomar de la cabecera SDA expuesta por el shield |
| SCL | D21 / PD0 / SCL | Tomar de la cabecera SCL expuesta por el shield |
| VCC | +5 V del panel, montaje comunicado | No inferir tolerancia del módulo por una foto |
| GND | GND común | Referencia del panel y del Mega |

OLED de referencia 0,96 pulgadas. Con pines arriba, de frente, el orden comunicado es GND-VCC-SCL-SDA; prevalece la serigrafía de la unidad. Es el único dispositivo I2C del producto y el firmware accede mediante I2C Manager.

### Encoder de menú y pulsador

| Señal | Pin Mega | Puerto / uso |
| --- | --- | --- |
| CLK | D2 | PE4 / INT4, giro |
| DT | D3 | PE5 / INT5, giro |
| SW | D42 | PL7, pulsador activo bajo |
| Común de contactos de giro | GND | Encoder mecánico sin breakout |
| Común del pulsador | GND | Cierra SW a masa |

VCC alimenta el panel/pull-ups; no se conecta al común del encoder mecánico. La fuente configura pull-ups internos en CLK/DT/SW; el CAD también documenta resistencias externas en CLK/DT. El giro y el pulsador se procesan como entradas de interfaz, independientes de los Hall de tracción.

No hay botón de menú adicional. Desde dashboard, corto abre menú completo y largo, ejecutado al soltar a partir de 800 ms, abre controles rápidos. En el resto, largo vuelve por contexto; la decisión de cambios pendientes sigue siendo obligatoria.

### Conectores del panel

| Conector | Pin 1 | Pin 2 | Pin 3 | Pin 4 | Pin 5 |
| --- | --- | --- | --- | --- | --- |
| J1 señales, hembra 1x5, 2,54 mm | CLK / D2 | DT / D3 | SW / D42 | SDA / D20 | SCL / D21 |
| J2 alimentación, 1x2 | VCC | GND | - | - | - |

Identificar pin 1 físicamente antes de cablear. El requisito inicial indicaba J2 de 2,54 mm; la revisión CAD AFR2-20261025-12 documenta JST de 2,00 mm. Este informe conserva ambas referencias y no afirma cuál está instalado.

La alimentación del panel a 5 V fue comunicada por el usuario; los archivos CAD documentados conservan red +3V3. Esta diferencia queda señalada y no se corrige eléctricamente por redactar el informe. Confirmar red/pull-ups de la placa real antes de tomar el CAD como plano de cableado definitivo.

<!-- pagebreak -->

## 5. Wi-Fi, AutoKEY, AutoRESET y alimentación

### UART del shield ESP-12S

| Conexión | Pin / destino | Origen |
| --- | --- | --- |
| Mega transmite al ESP | D18 / PD3 / TX1 hacia E_RX del selector | BASE; enlace adaptado |
| Mega recibe del ESP | E_TX del selector hacia D19 / PD2 / RX1 | BASE; enlace adaptado |
| Masa de comunicación | GND Mega y GND shield | BASE |
| USB del Mega / servicio | D0 / PE0 / RX0 y D1 / PE1 / TX0 | SERVICIO |

El ESP utiliza UART0, GPIO1/TX y GPIO3/RX. E_RX/E_TX pertenecen al selector del shield; la cabecera directa ESP UART, rotulada GND/ESP TX/ESP RX/3V3, es una conexión distinta de lógica 3,3 V. Aislar las selecciones D4/D5 y D0/D1 del enlace operativo y desconectar Bluetooth de esas redes. D5 ya controla un motor.

### AutoKEY y AutoRESET: AGREGADOS PROPIOS / SERVICIO

| Función | Pin Mega | Etapa hacia el shield | Destino |
| --- | --- | --- | --- |
| AutoKEY | D32 / PC5 | 1 kΩ a base de Q1 NPN | Colector a señal KEY / GPIO0 |
| AutoRESET | D33 / PC4 | 1 kΩ a base de Q2 NPN | Colector a señal RST / reset ESP |

En cada canal: emisor a GND común; resistencia de 10 kΩ desde base a GND del lado del transistor. El colector se conecta en paralelo con el pulsador del shield, que se conserva. D32/D33 **no van directamente a GPIO0 ni RST**. El patillaje E/B/C depende del transistor concreto.

Control alto en D32/D33 hace conducir al NPN y simula pulsador apretado. En reposo, el flasher deja los pines como entradas sin pull-up; las resistencias de base los mantienen inactivos. El firmware de producto no los acciona. El modo automático requiere activación explícita; manual sigue disponible.

Estado: reservado e implementado en fuente de servicio, sin montaje ni funcionamiento físico acreditados. Con el circuito conectado, D32/D33 no deben reutilizarse. La carga ESP vigente usa puente software USART0-USART1 por D18/D19 y RESET Mega libre; el método D0/D1 con RESET Mega a GND está retirado.

### Alimentación general

| Origen | Destino / uso |
| --- | --- |
| Portabaterías + / - | VIN+ / VIN- de Model Y |
| VOUT+ / VOUT- de Model Y | Entrada POWER del shield, según manual |
| Voltímetro VCC / VT / GND | Cabecera homónima del Model Y |
| +5 V regulados / GND común | Hall, sensores base y panel según conexiones documentadas |

VOUT no se interpreta como salida regulada de 5 V. El ESP se alimenta mediante el shield y su regulación; su pin 3V3 no equivale a 5 V. Las salidas M- de los puentes H no son masa lógica.

<!-- pagebreak -->

## 6. Índice de pines y trazabilidad

### Resumen por bloque de cabeceras

| Pines Mega | Asignación actual | Origen |
| --- | --- | --- |
| D0, D1 | USB / USART0 y puente software de servicio | SERVICIO |
| D2, D3 | CLK y DT del encoder de menú | AGREGADO PROPIO |
| D5-D8 | Direcciones RR y RL | BASE |
| D9-D12 | PWM de los cuatro motores | BASE |
| D13 | Señal del servo de radar | BASE |
| D18, D19 | UART1 hacia el ESP | BASE; enlace adaptado |
| D20, D21 | SDA y SCL del OLED | AGREGADO PROPIO |
| D22, D24, D26, D28 | Direcciones FR y FL | BASE |
| D30, D31 | TRIG y ECHO HC-SR04 | BASE |
| D32 | AutoKEY ESP mediante NPN | AGREGADO PROPIO / SERVICIO |
| D33 | AutoRESET ESP mediante NPN | AGREGADO PROPIO / SERVICIO |
| D42 | SW del encoder de menú | AGREGADO PROPIO |
| A0-A4 / D54-D58 | IR5 a IR1 | BASE |
| A5 / D59 | Divisor de batería | AGREGADO PROPIO |
| A8-A11 / D62-D65 | Hall A: FR, FL, RR, RL | BASE; agrupamiento adaptado |
| A12-A15 / D66-D69 | Hall B: FR, FL, RR, RL | BASE; agrupamiento adaptado |

Las denominaciones A0-A15 y D54-D69 son alias de los mismos pines; no son entradas adicionales. Los pines no listados no tienen uso identificado en este mapa; eso no certifica disponibilidad física bajo el shield. No hay MPU ni sensores de balance de dos ruedas en el producto. No se asignan pines al brazo futuro en este informe.

### Fuentes del informe

- AutoFinalRobo2-Atmega/include/board_pinmap.h: motores, Hall, panel, UART y reservas D32/D33.
- AutoFinalRobo2-Atmega/src/platform/afr2_board.c: pull-ups, captura Hall x1 e interrupciones del encoder.
- AutoFinalRobo2-Atmega/src/platform/afr2_sensors.c: IR digitales, ADC5, HC-SR04 y servo.
- AutoFinalRobo2-Atmega/src/platform/afr2_motor.c: dirección y PWM por rueda.
- AutoFinalRobo2-Atmega/service/esp_uart_bridge.c: control opcional de AutoKEY y AutoRESET.
- Contracts/hardware/PINOUT.md, PCB_OLED_ENCODER.md y WIFI_SHIELD.md: conexiones documentadas y diferencias de montaje/CAD.
- Contracts/contracts/BATTERY_MEASUREMENT_9.md y hardware/MEDICION_BATERIA.md: divisor y filtro de batería.
- Contracts/hardware/PLAN_KEY_RST_AUTOMATICO.md y FLASHING.md: circuito de servicio y método de carga vigente.
- Contracts/hardware/COTEJO_ENCODERS_BIOLOG_20261106.md: coincidencia de pines y diferencias con la referencia BioLog.

Autoría del proyecto: Koba Chajchir. Informe elaborado con asistencia de Codex a partir de fuentes propias y referencias conservadas. Las atribuciones originales del manual OSOYOO y de la referencia BioLog se mantienen en sus documentos. Esta entrega actualiza documentación; no realiza nuevas pruebas del robot.
