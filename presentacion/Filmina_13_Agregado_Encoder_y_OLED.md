# Filmina 13 · Nuestro agregado: encoder y OLED

**Mensaje clave:** un panel local propio (OLED 0,96" + encoder KY-040) con su PCB, soporte y firmware: interrupciones sin pérdida de
pasos, una cola, un renderizado diferencial cooperativo y navegación coherente con la Web.

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «Panel local: encoder + OLED»                       │
├──────────────────────┬──────────────────────┬────────────────────────────┤
│ A  HARDWARE          │ B  ENCODER           │ C  OLED                    │
│ FOTO-65 panel real   │ cadena de captura    │ cadena de render           │
│ FOTO-66 PCB (render) │ (texto + fórmulas)   │ (texto) + FOTO-69          │
│ c1–c4 · f2–f5        │ c5–c8 · f2–f5        │ c9–c12 · f2–f5             │
├──────────────────────┴──────────────────────┴────────────────────────────┤
│ D  Conectores y pines (tabla ancha) c1–c8 · f6    E  FOTO-67 / 68 c9–c12 │
└──────────────────────────────────────────────────────────────────────────┘
```

## Contenido por zona

### A · Hardware
- **FOTO-65** foto real del panel montado en el auto (✱); **FOTO-66** render de la PCB desde KiCad 3D (✱); **FOTO-67** soporte y PCB en Fusion (✱).
- PCB de **20 × 35 × 2 mm**, componentes pasantes, pistas en B.Cu y plano GND; el encoder va directo y el OLED en socket hembra 1×4.
- Soporte impreso con bisel y torretas M3 × 15 para el OLED; fijación a la cubierta con **dos tornillos y tuercas**; torretas de 5 mm hacia el auto.
- **Sin placa breakout** del KY-040: los comunes (giro y pulsador) van a GND; CLK/DT con R de 10 kΩ de pull-up.

### B · Encoder (captura por interrupción)
- **CLK → D2 (INT4) · DT → D3 (INT5) · SW → D42** (muestreo en `main` cada ≈ 1 ms, sin interrupción).
- Las ISR **solo capturan** el estado AB de ambos pines (una lectura de PINE) en una **cola circular de 32 B / 31 entradas**; nada de decodificar,
  I2C ni PID dentro de la ISR.
- `main` consume hasta **32 entradas por vuelta**, decodifica la cuadratura (2 o 4 transiciones por paso, inversión configurable; por defecto 4 e invertido)
  y ante desborde resincroniza sin inventar pasos.
- **Botón:** corto y largo se deciden **al soltar**; umbral ≥ 800 ms (800–3000 configurable); un largo nunca produce además un corto.
- Tabla de cuadratura (opcional en notas): secuencia 00→01→11→10→00 = un sentido, la inversa = el otro; transiciones inválidas se descartan.

### C · OLED (renderizado cooperativo)
- **SSD1306 128 × 64**, I2C en **D20 (SDA) / D21 (SCL)**, dirección 0x3C; **único dispositivo I2C**, accedido por un **I2C Manager** (`start/poll/abort`).
- Fuente 7 × 10 en PROGMEM; framebuffer en RAM (se conservan los dos buffers para diferencial).
- Envío **diferencial exacto en tramos ≤ 16 columnas**, con una etapa cada 2 ms como mínimo; el redibujo se limita a ≥ 40 ms entre cuadros y no bloquea el lazo
  de control (render es el último paso del ciclo, ver D-03a).
- Recuperación sin apagar ante error aislado; reconfiguración tras fallos persistentes.
- Dashboard con cabecera invertida: modo, SSID/IP desplazables, ícono de batería por bandas de voltaje y tarjeta de estados; **FOTO-69** OLED encendido (o reutilizar FOTO-48).
- La Web contiene el **mismo rasterizador** (paridad comprobada en host: 1680/1680 en la entrega del 06/11/2026) y puede mostrar el framebuffer remoto.

### D · Conectores y pines
| Conector | Pines |
| --- | --- |
| **J1** hembra 1×5 · 2,54 mm | 1 CLK · 2 DT · 3 SW · 4 SDA · 5 SCL |
| **J2** JST 1×2 · 2,00 mm | 1 VCC (5 V en el montaje) · 2 GND |
| OLED (de frente, pines arriba) | GND · VCC · SCL · SDA |

### E · FOTO-67 / FOTO-68
Soporte y PCB en Fusion (render) y captura del esquema KiCad (`PlacaOLED-Encoder-AutoRobo2.kicad_sch`).

## Guion del orador (≈ 75 s)

«El panel local es un OLED y un encoder con pulsador. En el encoder las interrupciones solo guardan el estado de los dos canales en una
cola; el programa principal los decodifica, así no se pierden pasos ni se bloquea nada. El pulsador distingue corto y largo al soltar. En
la pantalla, un único gestor I2C manda solo las diferencias de la imagen en tramos de 16 columnas, para que dibujar nunca retrase al
control de los motores. Diseñamos la placa en KiCad y el soporte en Fusion, y la Web reproduce exactamente las mismas pantallas.»

## Fuentes

`AutoFinalRobo2-Contracts/hardware/INTERRUPCIONES_ENCODER_HALL.md`, `PCB_OLED_ENCODER.md`, `PINOUT.md`, `OLED_I2C_DIAGNOSTICO.md` ·
`AutoFinalRobo2-CAD/kicad/oled-encoder/README.md` · `AutoFinalRobo2-CAD/docs/PANEL_UI_20261025.md` · `AutoFinalRobo2-Atmega/src/main.c`,
`src/input/afr2_encoder_ui.c`, `src/display/afr2_oled.c` · `README.md` del ATmega (fichas 20261029-11/-14).

## Advertencias

- **Tensión:** el montaje se informó a **5 V**, pero el CAD/KiCad conserva +3V3 en J2 y las pull-ups del encoder; no certificar compatibilidad de niveles
  ni rendimiento físico. Decir «a 5 V según el montaje comunicado».
- PCB y soporte: archivos de diseño; **sin ERC/DRC, sin comprobación de interferencias y sin fabricación** documentada. Tornillería y ranura M3 pendientes.
- Configuración validada con el diagnóstico independiente (OLED 0x3C, pulls como Wire, encoder invertido) que el usuario confirmó funcional el 29/10/2026.
