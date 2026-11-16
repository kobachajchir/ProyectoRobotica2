# Filmina 3 · Pinout y conexionado

**Mensaje clave:** cada señal del auto tiene un pin documentado; el Mega concentra motores, Hall, sensores y panel, y habla con el
ESP por USART1 (D18/D19). Los agregados propios están identificados.

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «Pinout y conexionado (Mega 2560 + shield OSOYOO)» │
├────────────────────────────────┬─────────────────────────────────────────┤
│ A  TABLA DE PINOUT             │ B  DIAGRAMA D-02 (conexionado)          │
│ c1–c6 · f2–f6                  │ c7–c12 · f2–f4                          │
│ 3 bloques: Potencia/Motores,   ├───────────────────┬─────────────────────┤
│ Hall+Sensores, Panel+ESP+NPN   │ C  FOTO-12        │ D  FOTO-14 / FOTO-15│
│                                │ pinout shield     │ foto real cableado  │
│                                │ c7–c9 · f5–f6     │ c10–c12 · f5–f6     │
└────────────────────────────────┴───────────────────┴─────────────────────┘
```

## Contenido por zona

### A · Tabla de pinout (3 bloques; resaltar en carmesí lo agregado por nosotros)

**Bloque 1 · Motores (IN1 · IN2 · PWM)**

| Rueda | Salida del puente H | IN1 | IN2 | PWM |
| --- | --- | --- | --- | --- |
| DEL DER (FR) | M_B · BK1-BK2 | D22 | D24 | D9 |
| DEL IZQ (FL) | M_B · BK3-BK4 | D26 | D28 | D10 |
| TRAS DER (RR) | M_A · AK1-AK2 | D5 | D6 | D11 |
| TRAS IZQ (RL) | M_A · AK3-AK4 | D7 | D8 | D12 |

**Bloque 2 · Hall (x1: flanco ↑ de A, B da el sentido) y sensores**

| Elemento | Pines |
| --- | --- |
| Hall A (S1, naranja) DER-DEL / IZQ-DEL / DER-TRAS / IZQ-TRAS | A8 / A9 / A10 / A11 |
| Hall B (S2, verde) en el mismo orden | A12 / A13 / A14 / A15 |
| Barra IR ×5 (IR1…IR5) | A4 / A3 / A2 / A1 / A0 |
| HC-SR04 TRIG / ECHO | D30 / D31 |
| Servo del radar (señal) | D13 |
| Batería (divisor 10 kΩ/10 kΩ + 100 nF) ⭐ | A5 |

**Bloque 3 · Enlace y agregados propios** ⭐

| Elemento | Pines |
| --- | --- |
| UART al ESP (a E_RX / E_TX del selector) | D18 (TX1) · D19 (RX1) |
| OLED I2C ⭐ | SDA D20 · SCL D21 |
| Encoder ⭐ | CLK D2 (INT4) · DT D3 (INT5) · SW D42 |
| KEY / RST automáticos (vía NPN) ⭐ | D32 → KEY · D33 → RST |
| USB (carga/depuración) | D0 / D1 (no es el enlace del ESP) |

Leyenda: ⭐ = agregado propio sobre el kit OSOYOO. Nota al pie: *DEL/TRAS e IZQ/DER mirando en el sentido de avance.
Orden físico de los headers Hall: FR, FL, RR, RL; orden lógico del firmware: FL, FR, RL, RR.*

### B · D-02 ([D-02_conexionado.svg](diagramas/D-02_conexionado.svg))
Mega al centro; a la izquierda enlaces con el ESP, la batería y los NPN; a la derecha motores, Hall, IR, ultrasonido, OLED y
encoder. Sobre el diagrama, tres notas breves:
1. Alimentación: Hall a **+5 V regulados** (nunca la batería) · GND común.
2. Potencia: portabaterías → Model Y VIN → VOUT → POWER del shield.
3. ESP: usar E_RX/E_TX del selector (el puerto ESP UART directo es de 3,3 V).

### C · FOTO-12 / FOTO-13
**FOTO-12** pinout oficial del shield OSOYOO V1.3 (arriba/abajo). Pie: *Fuente: OSOYOO; el mapa ilustrado es de UNO, no usar A4/A5
como I2C del Mega.* **FOTO-13** (mapa de pines Wi-Fi) como alternativa o apoyo.

### D · FOTO-14 / FOTO-15 (por tomar ✱)
Foto real del Mega + shield visto desde arriba con cables identificados; foto de motores y Hall cableados. Detalle en [FOTOS.md](FOTOS.md).
**FOTO-16**: exportar la primera página de `Documentacion/PINOUT_DEL_AUTO.pdf` si se prefiere mostrar la hoja oficial.

## Guion del orador (≈ 60 s)

«El Mega gobierna cuatro motores con dos pines de dirección y uno de PWM cada uno; los cuatro encoders Hall entran por el
puerto K y generan interrupción por cambio de A, mientras B da el sentido. A eso sumamos la barra de cinco IR, el ultrasonido con
su servo de radar y el divisor de batería en A5. El shield Wi-Fi se une por D18 y D19. Los agregados propios son el panel OLED con
encoder, la medición de batería y los dos transistores que simulan los botones KEY y RESET del ESP. Todo con masa común y los Hall
a cinco volts regulados.»

## Fuentes

`AutoFinalRobo2-Contracts/hardware/PINOUT.md` · `PINOUT_DEL_AUTO.json` · `INFORME_PINOUT_ROBOT_20261106.md` ·
`WIFI_SHIELD.md` · `INTERRUPCIONES_ENCODER_HALL.md` · `AutoFinalRobo2-Atmega/include/board_pinmap.h`.

## Advertencias

- Pines = **implementación en código y montaje comunicado**; no hay verificación eléctrica documentada de todos.
- El CAD de la PCB del panel conserva +3V3; el montaje real se informó a **5 V** (ver filmina 13).
- Calibración de Hall y signos: ver filmina 5; no afirmar que está «calibrado» sin medición.
