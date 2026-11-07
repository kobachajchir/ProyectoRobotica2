# AFR2-20261106-14 — Pinout BioLog frente a AutoFinalRobo2

Fecha: 06/11/2026. Incorporación solicitada por el usuario y cotejo estático
de las dos páginas del [PDF original](supplied/Pinout_encoders_AGV_BioLog.pdf)
con las fuentes actuales. No se ejecutaron scripts del documento, tests,
compilaciones, validadores, cargas ni pruebas físicas. No se cambia firmware,
configuración EEPROM, protocolo ni CAD por incorporar esta referencia.

## Procedencia

Documento aportado: `Pinout_encoders_AGV_BioLog.pdf`, proyecto BioLog
(Robótica II, FCAL-UNER), fechado 06/11/2026. Conservado sin modificar,
94822 bytes, SHA-256:
`a354683c38e0f6c089ab6a585f5532b1292f3279a71121e5b41b8aa0b7f02ce7`.
El PDF atribuye sus fuentes a PINOUT_DEL_AUTO (K. Chajchir, 27/10/2026),
hoja OSOYOO 2024005800 y firmware BioLog, T. Mendevich y M. Baldoni.
Estas atribuciones se preservan; el cotejo AFR2 es una adaptación documental.
La medición de 1500 cuentas se registra como afirmación del documento,
sin trasladarla a una medición comprobada de nuestro auto.

## Cableado: coincidencia completa de A/B por rueda

| Rueda | A = S1 naranja | B = S2 verde | Bits puerto K | Índice AFR2 |
| --- | --- | --- | --- | --- |
| FR, delantera derecha | A8 / D62 | A12 / D66 | PK0 / PK4 | 1 |
| FL, delantera izquierda | A9 / D63 | A13 / D67 | PK1 / PK5 | 0 |
| RR, trasera derecha | A10 / D64 | A14 / D68 | PK2 / PK6 | 3 |
| RL, trasera izquierda | A11 / D65 | A15 / D69 | PK3 / PK7 | 2 |

Coincide con [board_pinmap.h](../../AutoFinalRobo2-Atmega/include/board_pinmap.h)
y [PINOUT.md](PINOUT.md). Headers físicos FR/FL/RR/RL, A8–A11 y A12–A15.
También coincide la alimentación documental: +5 V regulados, GND común,
blanco VCC/amarillo GND con advertencia de variantes invertidas y lectura de
serigrafía V/G. Rojo/negro M+/M- van al puente H. Este cotejo no comprueba
continuidad, tensión ni colores del ejemplar instalado.

## Diferencias que impiden copiar las constantes del PDF

| Tema | PDF BioLog | Fuente AFR2 actual | Consecuencia |
| --- | --- | --- | --- |
| Decodificación | x4, todos los flancos A/B | x1, subida A y sentido por B | CPR de escalas distintas |
| Interrupciones | Describe IRQ para los ocho pines | PCINT2 con PCMSK2=0x0F, solo PK0–PK3 | B se lee, no dispara IRQ |
| Orden lógico | FL, RL, RR, FR | FL, FR, RL, RR | No copiar arrays por posición |
| Cuentas/vuelta | CAL_CPR=1500, declarado medido | counts_per_rev[4], medidos y guardados por rueda | No fijar 1500 como default |
| Signos Hall | FL/RL -1, FR/RR +1 | hall_signs por bits y hall_cw_mask | Calibración normaliza CW exterior positivo |
| Signos motores | -1 en las cuatro ruedas | motor_signs por bits; default 0, EEPROM puede conservar otros | No inferir equivalencia de convenciones |
| Diámetro | 80,59 mm | AFR2_FIXED_WHEEL_MM=80 mm | Diferencia nominal de 0,7375 % respecto de 80 mm |
| Configuración | config.h y calibracion.h de biolog_agv | Configuración AFR2 y journal EEPROMv10 | Son implementaciones distintas |

La correspondencia de índices BioLog → AFR2 es **0→0, 1→2, 2→3, 3→1**.
Solo como traducción de la tabla, sus bits A serían `{1,0,3,2}` y B
`{5,4,7,6}` en orden AFR2; coinciden con nuestros símbolos de pin.
Sus signos Hall serían `{-1,+1,-1,+1}` en ese orden, pero esto no demuestra
que deban aplicarse a nuestra convención de giro ni a calibraciones guardadas.

Para una señal ideal idéntica, x4 produce cuatro veces las cuentas de x1:
1500/4=375 sería una conversión aritmética, **no un CPR aceptado para AFR2**.
La teoría 11×56×4=2464 indicada en el PDF tampoco coincide con su 1500
declarado medido; a x1 correspondería nominalmente 616. No se resuelve esa
diferencia suponiendo reducción, pérdida de pulsos o motor distinto.
No se adopta modelo 2024005800, reducción 56:1, 12 V ni 11 pulsos como
identificación física confirmada del motor AFR2 por esta referencia.

## Evidencia en fuentes actuales

- [afr2_board.c](../../AutoFinalRobo2-Atmega/src/platform/afr2_board.c):
  `hall_capture()` cuenta subidas A y toma B del mismo snapshot PINK;
  `Afr2Board_HallInit()` configura entradas, pull-ups en las ocho señales
  (`PORTK=0xFF`) y máscara A. La coincidencia de pull-ups con el PDF no
  convierte el conteo en x4.
- [afr2_board.h](../../AutoFinalRobo2-Atmega/include/afr2_board.h):
  default `AFR2_HALL_CONNECTED=1`; disponibilidad también depende de
  `hall_wired`. No identifica qué binario está cargado.
- [afr2_sensors.c](../../AutoFinalRobo2-Atmega/src/platform/afr2_sensors.c):
  snapshot de cuatro contadores e inversión mediante `hall_signs`.
- [afr2_app.c](../../AutoFinalRobo2-Atmega/src/services/afr2_app.c):
  confirmar calibración guarda CPR de la rueda y marca `hall_cw_mask`;
  invierte el signo si la vuelta CW de referencia produjo conteo negativo.
- [afr2_product.c](../../AutoFinalRobo2-Atmega/src/services/afr2_product.c):
  defaults sin CPR supuesto y odometría por CPR individual; transforma las
  ruedas derechas calibradas a su base cinemática. Los signos del PDF no
  sustituyen esta normalización.
- [afr2_motor.c](../../AutoFinalRobo2-Atmega/src/platform/afr2_motor.c):
  inversión por máscara `motor_signs`, independiente de la captura Hall.
- [afr2_geometry.h](../contracts/afr2_geometry.h): diámetro nominal 80 mm
  y Lx+Ly nominal 170 mm, conservados.
- [broker_product.cpp](../../AutoFinalRobo2-ESP/src/broker_product.cpp) y
  [ayuda Web](../../AutoFinalRobo2-Web/src/features/docs/helpContent.ts):
  orden FL/FR/RL/RR; la ayuda describe x1 y máscara 0x0F correctamente.

## Decisión y pendientes

Se incorpora el PDF como referencia con este cotejo adjunto. El pinout AFR2
se conserva. No se migra a x4, ni se fijan CPR/signos/diámetro por implicación.
Las rutas `agv/firmware/biolog_agv/...` y `CARGAR_ROBOT.bat → opción 9`
del PDF pertenecen a BioLog; no son instrucciones para ejecutar en AFR2.
El valor especial `ENC_BIT_B=255` tampoco es una opción implementada aquí:
la lectura AFR2 espera los cuatro pares A/B definidos.

Para adoptar x4 haría falta un cambio explícito de captura, presupuesto de
interrupciones y escala de CPR/calibraciones persistidas. La calibración
vigente es [Hall por motor](../contracts/HALL_MANUAL_STREAM_20261106.md):
una vuelta manual CW vista desde el exterior, Confirmar/Descartar y EEPROM.
No se ha leído la EEPROM del auto ni comprobado el firmware instalado.

ATmega/ESP/Web: fuentes y artefactos sin cambios por AFR2-20261106-14.
Solo documentación y referencia; funcionamiento físico no acreditado.
