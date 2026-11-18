# Diagramas de la presentación v3 (paleta del informe maestro, fondo #0B1220).
# Cada número que aparece sale de una fuente del repo citada en evidence/verification_matrix.csv.
# Uso: python -I diagramas_v3.py <salida_dir>
import sys, os, math
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import FancyBboxPatch, FancyArrowPatch, Rectangle, Circle, Polygon

OUT = sys.argv[1] if len(sys.argv) > 1 else '.'
os.makedirs(OUT, exist_ok=True)
MEC, ELE, CTL, PRG = '#42B6FF', '#FFB454', '#A78BFA', '#55D6AA'
BG, TX, T2, GR = '#0B1220', '#F4F7FC', '#A9B4C4', '#2A3448'
plt.rcParams.update({'font.family': 'Segoe UI', 'font.size': 15, 'text.color': TX,
                     'axes.edgecolor': T2, 'axes.labelcolor': T2, 'xtick.color': T2, 'ytick.color': T2,
                     'svg.fonttype': 'none'})


def canvas(w=19.2, h=10.8, transparent=True):
    fig = plt.figure(figsize=(w / 1.5, h / 1.5), dpi=150)
    ax = fig.add_axes([0, 0, 1, 1]); ax.set_xlim(0, w); ax.set_ylim(0, h); ax.axis('off')
    if not transparent:
        fig.patch.set_facecolor(BG)
    return fig, ax


def save(fig, name):
    for ext in ('png', 'svg'):
        fig.savefig(os.path.join(OUT, f'{name}.{ext}'), facecolor=BG)
    plt.close(fig)
    print('OK', name)


def box(ax, x, y, w, h, text, color, sub=None, fs=17, fill=None, lw=2.2, ha='center', r=0.18):
    ax.add_patch(FancyBboxPatch((x, y), w, h, boxstyle=f'round,pad=0,rounding_size={r}',
                                fc=fill or (color + '22'), ec=color, lw=lw, zorder=3))
    if sub:
        ax.text(x + w / 2, y + h * 0.64, text, ha='center', va='center', fontsize=fs, weight='bold', color=TX, zorder=4)
        ax.text(x + w / 2, y + h * 0.30, sub, ha='center', va='center', fontsize=fs - 4.5, color=T2, linespacing=1.25, zorder=4)
    else:
        ax.text(x + w / 2, y + h / 2, text, ha='center', va='center', fontsize=fs, weight='bold', color=TX, linespacing=1.25, zorder=4)


def arrow(ax, a, b, color=T2, lw=2.2, both=False, dashed=False, text=None, toff=(0, 0.28), fs=12.5, rad=0.0):
    st = '<|-|>' if both else '-|>'
    ax.add_patch(FancyArrowPatch(a, b, arrowstyle=st, mutation_scale=18, color=color, lw=lw,
                                 linestyle=(0, (4, 3)) if dashed else '-', connectionstyle=f'arc3,rad={rad}'))
    if text:
        ax.text((a[0] + b[0]) / 2 + toff[0], (a[1] + b[1]) / 2 + toff[1], text, ha='center', va='center',
                fontsize=fs, color=T2, linespacing=1.2)


# ---------- F02 cuatro ramas
fig, ax = canvas()
cx, cy = 9.6, 5.4
ax.add_patch(Circle((cx, cy), 1.75, fc='#16203A', ec=TX, lw=2))
ax.text(cx, cy, 'SISTEMA\nMECATRÓNICO', ha='center', va='center', fontsize=22, weight='bold')
for ang, (t, s, c) in zip([135, 45, -45, -135], [('MECÁNICA', 'estructura y movimiento', MEC), ('ELECTRÓNICA', 'medir y actuar', ELE),
                                                 ('CONTROL', 'regular comportamiento', CTL), ('PROGRAMACIÓN', 'procesar y comunicar', PRG)]):
    x = cx + 5.6 * math.cos(math.radians(ang)); y = cy + 3.3 * math.sin(math.radians(ang))
    ax.plot([cx + 1.75 * math.cos(math.radians(ang)), x - 0 * 1], [cy + 1.75 * math.sin(math.radians(ang)), y], color=c, lw=3)
    box(ax, x - 2.4, y - 0.8, 4.8, 1.6, t, c, s, fs=21, fill=BG)
save(fig, 'F02_A01_VECTOR_cuatro_ramas_v001')

# ---------- F03 recorridos curriculares (solo aristas citadas en el informe desde C.D. 726/23)
fig, ax = canvas()
C0, C1, C2, C3 = 2.0, 7.3, 12.4, 17.35
N = {'Ciencias de los Materiales': (C0, 9.4, MEC), 'Estática y Resist. de Mat.': (C0, 8.2, MEC),
     'Ec. Dif. y Cálc. Numérico': (C0, 7.0, CTL), 'Sistemas y Señales': (C0, 5.8, CTL),
     'Lab. de Mediciones': (C0, 4.6, ELE), 'Electrónica Digital': (C0, 3.4, ELE),
     'Electrónica Básica': (C0, 2.2, ELE), 'Computación II': (C0, 1.0, PRG),
     'Mecánica Racional': (C1, 8.4, MEC), 'Sistemas de Control': (C1, 5.9, CTL), 'Microcontroladores': (C1, 4.1, ELE),
     'Robótica I': (C2, 7.2, CTL), 'Sistemas Embebidos': (C2, 2.5, PRG), 'Automatización Ind.': (C2, 0.9, CTL),
     'SO y Redes de Comunicación': (C3, 1.9, PRG)}
W_, H_ = 3.3, 0.9
for n, (x, y, c) in N.items():
    box(ax, x - W_ / 2, y - H_ / 2, W_, H_, n, c, fs=12.5, fill='#111A2E', r=0.12)
E = [('Estática y Resist. de Mat.', 'Mecánica Racional', 'R'), ('Ec. Dif. y Cálc. Numérico', 'Mecánica Racional', 'R'),
     ('Ciencias de los Materiales', 'Mecánica Racional', 'A'),
     ('Electrónica Digital', 'Microcontroladores', 'R'), ('Lab. de Mediciones', 'Microcontroladores', 'R'),
     ('Electrónica Básica', 'Microcontroladores', 'A'), ('Computación II', 'Microcontroladores', 'A'),
     ('Electrónica Digital', 'Sistemas de Control', 'R'), ('Lab. de Mediciones', 'Sistemas de Control', 'R'),
     ('Sistemas y Señales', 'Sistemas de Control', 'R'),
     ('Microcontroladores', 'Sistemas Embebidos', 'R'), ('Electrónica Digital', 'Sistemas Embebidos', 'A'),
     ('Computación II', 'Sistemas Embebidos', 'A'),
     ('Sistemas de Control', 'Robótica I', 'R'), ('Mecánica Racional', 'Robótica I', 'R'),
     ('Sistemas Embebidos', 'SO y Redes de Comunicación', 'R'), ('Automatización Ind.', 'SO y Redes de Comunicación', 'R')]
inc = {}
for e in E: inc.setdefault(e[1], []).append(e)
for tgt_, es in inc.items():
    es.sort(key=lambda e: -N[e[0]][1])
    for k, (a_, b_, lab) in enumerate(es):
        (x1, y1, _), (x2, y2, _) = N[a_], N[b_]
        off = (k - (len(es) - 1) / 2) * min(0.22, 0.75 / max(1, len(es) - 1)) if len(es) > 1 else 0
        p1, p2 = (x1 + W_ / 2, y1), (x2 - W_ / 2, y2 + off)
        ax.add_patch(FancyArrowPatch(p1, p2, arrowstyle='-|>', mutation_scale=12, color='#C9D3E3', lw=1.7, alpha=.9))
        t = 0.45; mx, my = p1[0] + (p2[0] - p1[0]) * t, p1[1] + (p2[1] - p1[1]) * t
        ax.add_patch(Circle((mx, my), 0.17, fc=BG, ec='#C9D3E3', lw=1.2, zorder=5))
        ax.text(mx, my, lab, ha='center', va='center', fontsize=9.5, weight='bold', zorder=6)
x2, y2, _ = N['Robótica I']; x1, y1, _ = N['Sistemas Embebidos']
ax.plot([x1, x2], [y1 + H_ / 2, y2 - H_ / 2], color=T2, lw=1.3, ls=(0, (2, 3)))
ax.text(x1 + 0.15, (y1 + y2) / 2, 'vínculo conceptual\ndel proyecto', fontsize=11, color=T2, va='center')
ax.text(C3, 1.3, '+ A: Electrónica Digital,\nSistemas y Señales, Computación II', ha='center', va='top', fontsize=10.5, color=T2)
ax.text(9.6, 10.35, 'R = regularizada · A = aprobada (para cursar, C.D. 726/23) · punteada = vínculo conceptual, no correlatividad',
        ha='center', fontsize=12.5, color=T2)
for x, t in [(C0, 'Bases'), (C1, 'Integración'), (C2, 'Aplicación'), (C3, 'Sistemas')]:
    ax.text(x, 9.95 if x != C0 else 10.0, '', fontsize=1)
save(fig, 'F03_A01_VECTOR_recorridos_curriculares_v001')

# ---------- F04 / F26 arquitectura
fig, ax = canvas()
box(ax, 0.4, 6.6, 4.0, 2.9, 'Navegador', PRG, 'React 19 + Vite 7\nThree.js (modelo GLB)\ncliente WS afr2-ws-1', fs=22)
box(ax, 7.6, 6.6, 4.0, 2.9, 'ESP8266 · ESP-12', PRG, 'Arduino core · PlatformIO\nAsyncWebServer · LittleFS\nbroker JSON ↔ binario', fs=21)
box(ax, 14.8, 6.6, 4.0, 2.9, 'ATmega2560', ELE, 'C avr-libc, sin Arduino\ncontrol PID · odometría\nUI OLED · sensores', fs=21)
arrow(ax, (4.5, 8.05), (7.5, 8.05), PRG, both=True, text='Wi-Fi · HTTP\nWebSocket /ws\nJSON', toff=(0, 0.85), fs=13)
arrow(ax, (11.7, 8.05), (14.7, 8.05), ELE, both=True, text='UART 115200\nROBO2 + XOR\nUSART1 D18/D19', toff=(0, 0.85), fs=13)
items = [('Drivers OSOYOO\nModel Y V2', 'PWM 976 Hz', ELE), ('4 × GM25-370\n+ Hall A/B', 'PCINT2 A8–A15', ELE),
         ('HC-SR04\nsobre SG90', 'Timer3 / Timer5', ELE), ('Línea 5 IR', 'A0–A4 digital', ELE),
         ('OLED 128×64\n+ encoder KY-040', 'I²C 100 kHz · INT4/5', ELE), ('Batería 2×18650', 'divisor → ADC5', ELE)]
for i, (t, s, c) in enumerate(items):
    x = 0.4 + i * 3.15
    box(ax, x, 1.0, 2.9, 2.3, t, c, s, fs=14.5, fill='#111A2E')
    arrow(ax, (16.8, 6.5), (x + 1.45, 3.4), '#5B6780', lw=1.4)
ax.text(4.6, 5.3, 'Hardware del auto (modelo CAD: AutoFinalRobo2.glb)', ha='center', fontsize=14, color=T2)
save(fig, 'F04_A02_VECTOR_arquitectura_v001')

# ---------- F13 divisor de batería
fig, ax = canvas()
ax.add_patch(Rectangle((1.4, 3.0), 1.6, 4.8, fc='#111A2E', ec=ELE, lw=2.5))
ax.text(2.2, 6.6, '18650', ha='center', fontsize=14); ax.text(2.2, 4.2, '18650', ha='center', fontsize=14)
ax.plot([1.4, 3.0], [5.4, 5.4], color=ELE, lw=1.5)
ax.text(2.2, 8.2, 'BAT+ (después de llave)', ha='center', fontsize=13, color=T2)
ax.plot([2.2, 2.2, 7.0, 7.0], [7.8, 9.0, 9.0, 8.3], color=TX, lw=2.4)
for y0, lab in [(6.1, 'R1 = 10 kΩ'), (2.9, 'R2 = 10 kΩ')]:
    ax.add_patch(Rectangle((6.6, y0), 0.8, 2.2, fc='#111A2E', ec=ELE, lw=2.5)); ax.text(7.8, y0 + 1.1, lab, va='center', fontsize=17)
ax.plot([7.0, 7.0], [6.1, 5.1], color=TX, lw=2.4); ax.plot([7.0, 7.0], [2.9, 1.6], color=TX, lw=2.4)
ax.plot([2.2, 2.2, 7.0], [3.0, 1.6, 1.6], color=TX, lw=2.4); ax.text(4.6, 1.15, 'GND', ha='center', fontsize=13, color=T2)
ax.plot([7.0, 11.0], [5.6, 5.6], color=TX, lw=2.4); ax.add_patch(Circle((7.0, 5.6), 0.09, color=TX))
ax.plot([9.3, 9.3], [5.6, 4.4], color=TX, lw=2.2); ax.plot([8.9, 9.7], [4.4, 4.4], color=ELE, lw=3); ax.plot([8.9, 9.7], [4.15, 4.15], color=ELE, lw=3)
ax.plot([9.3, 9.3], [4.15, 1.6], color=TX, lw=2.2); ax.plot([7.0, 9.3], [1.6, 1.6], color=TX, lw=2.4)
ax.text(9.9, 4.3, '100 nF', fontsize=14)
box(ax, 11.0, 4.6, 4.2, 2.0, 'ADC5 · A5 (PF5)', ELE, '10 bits · ref. AVCC 5 V\nprescaler 128 · 8 muestras', fs=17, fill='#111A2E')
ax.text(11.0, 9.1, r'$V_{A5}=V_B\,\dfrac{R_2}{R_1+R_2}=\dfrac{V_B}{2}$', fontsize=26, va='center')
ax.text(11.0, 7.6, r'$mV=\dfrac{raw\cdot 10000+512}{1024}$', fontsize=24, va='center')
ax.text(15.6, 6.5, 'Rango útil: V_B ≤ 10 V\n(2×4,2 V = 8,4 V → 4,2 V en A5)', fontsize=14, color=T2, va='top', linespacing=1.4)
ax.text(11.0, 3.6, 'Sin calibrar: referencia nominal 5000 mV.\nEl dato es tensión, no % de carga.', fontsize=13, color=T2, va='top', linespacing=1.4)
save(fig, 'F13_A01_VECTOR_divisor_bateria_v001')

# ---------- F14 cuadratura Hall
fig, ax = canvas(19.2, 7.2)
import numpy as np
t = np.linspace(0, 8, 2000)
A = (np.sin(2 * np.pi * t / 2) >= 0).astype(float); B = (np.sin(2 * np.pi * (t - 0.5) / 2) >= 0).astype(float)
ax.plot(1.8 + t * 1.9, 4.6 + A * 1.3, color=ELE, lw=3); ax.plot(1.8 + t * 1.9, 1.8 + B * 1.3, color=CTL, lw=3)
ax.text(1.2, 5.25, 'A', fontsize=24, weight='bold', color=ELE, ha='center'); ax.text(1.2, 2.45, 'B', fontsize=24, weight='bold', color=CTL, ha='center')
for k in range(4):
    x = 1.8 + (2 * k) * 1.9
    ax.annotate('', (x, 6.25), (x, 6.95), arrowprops=dict(arrowstyle='-|>', color=TX, lw=2))
    ax.text(x, 7.0, '+1', ha='center', fontsize=13)
ax.text(9.6, 0.6, 'Se cuenta solo el flanco ascendente de A (PCINT2); el nivel de B en ese instante da el sentido → ±1 cuenta',
        ha='center', fontsize=14, color=T2)
save(fig, 'F14_A02_VECTOR_cuadratura_hall_v001')

# ---------- F15 ultrasonido
fig, ax = canvas(19.2, 7.2)
def sig(y, segs, c, lab):
    xs, ys = [], []
    for x0, x1, v in segs:
        xs += [x0, x1]; ys += [y + v, y + v]
    ax.plot(xs, ys, color=c, lw=3); ax.text(0.5, y + 0.5, lab, fontsize=17, weight='bold', color=c, va='center')
sig(5.0, [(2.5, 4, 0), (4, 4.6, 1.1), (4.6, 18.5, 0)], ELE, 'TRIG\nD30/PC7')
sig(2.0, [(2.5, 6.5, 0), (6.5, 13.5, 1.1), (13.5, 18.5, 0)], PRG, 'ECHO\nD31/PC6')
ax.annotate('', (6.5, 1.4), (13.5, 1.4), arrowprops=dict(arrowstyle='<|-|>', color=TX, lw=2)); ax.text(10, 0.85, 'Δt  (ancho del eco)', ha='center', fontsize=16)
ax.text(10, 6.7, r'$d=\dfrac{c\,\Delta t}{2}$', fontsize=26, ha='center')
ax.text(12.0, 3.95, 'Timer3 CTC: 50 × 0,5 µs = 25 µs por tick\nTimeout 1280 ticks = 32 ms, sin bloquear', fontsize=13.5, color=T2, va='center', linespacing=1.4)
save(fig, 'F15_A02_VECTOR_tof_tiempos_v001')

# ---------- F16 línea: pesos
fig, ax = canvas(19.2, 7.2)
ax.add_patch(Rectangle((6.3, 4.85), 3.7, 1.5, fc='#000000', ec='#3A4560', lw=1, zorder=0))
ax.text(8.15, 6.45, 'cinta oscura', ha='center', fontsize=12, color=T2)
for i in range(5):
    x = 4.2 + i * 2.6
    on = i in (1, 2)
    ax.add_patch(Circle((x, 5.6), 0.55, fc=(ELE if on else '#1A2438'), ec=ELE, lw=2))
    ax.text(x, 5.6, f'IR{i + 1}', ha='center', va='center', fontsize=12, weight='bold', color=BG if on else TX)
    ax.text(x, 4.55, f'peso {2 - i:+d}', ha='center', fontsize=13.5, color=T2)
ax.text(9.6, 6.75, 'bit i = 1 si el sensor ve cinta', ha='center', fontsize=14, color=T2)
ax.text(9.6, 2.4, r'$e=\dfrac{\sum_{i\,activos}(2-i)}{n}\qquad \omega_z=\dfrac{e\cdot K_{p,línea}}{n}$', ha='center', fontsize=24)
ax.text(9.6, 0.4, 'Sin línea (0 bits) → STOP · 5 bits activos ≥150 ms → STOP (cruce)', ha='center', fontsize=13.5, color=T2)
save(fig, 'F16_A02_VECTOR_linea_pesos_v001')

# ---------- F17 I2C
fig, ax = canvas(19.2, 7.2)
box(ax, 0.8, 2.0, 4.6, 3.2, 'ATmega2560', ELE, 'TWI maestro\nSDA D20 · SCL D21', fs=20)
box(ax, 13.8, 2.0, 4.6, 3.2, 'OLED 128×64', ELE, 'controlador tipo SSD1306\núnico dispositivo I²C', fs=20)
for y, n in [(4.3, 'SDA (datos)'), (2.9, 'SCL (reloj)')]:
    ax.plot([5.4, 13.8], [y, y], color=PRG if 'SDA' in n else CTL, lw=3); ax.text(9.6, y + 0.25, n, ha='center', fontsize=15)
ax.text(9.6, 6.3, 'TWBR = 72 → 16 MHz / (16 + 2·72) = 100 kHz', ha='center', fontsize=18)
ax.text(9.6, 1.0, 'I²C Manager: un dueño del bus, escrituras con deadline, nunca desde la interrupción', ha='center', fontsize=13.5, color=T2)
save(fig, 'F17_A02_VECTOR_i2c_oled_v001')

# ---------- F18 PWM motor vs servo
fig, ax = canvas(19.2, 7.2)
def pulses(y, period, width, n, c, sx=3.2, scale=1.0):
    xs, ys = [sx], [y]
    for k in range(n):
        x0 = sx + k * period * scale
        xs += [x0, x0, x0 + width * scale, x0 + width * scale, x0 + period * scale]; ys += [y, y + 1.1, y + 1.1, y, y]
    ax.plot(xs, ys, color=c, lw=2.6)
pulses(4.6, 1.024, 0.6, 14, ELE, scale=1.0)
ax.text(0.4, 5.15, 'Motor DC\nTimer1/2', fontsize=15, weight='bold', color=ELE, va='center')
ax.text(18.8, 6.15, 'Fast PWM 8 bits · ÷64 → 976 Hz (T ≈ 1,02 ms)\nduty = |PWM|/255 → tensión media', fontsize=13, color=T2, ha='right')
pulses(1.4, 20, 1.5, 1, CTL, scale=0.7)
ax.text(0.4, 1.95, 'Servo SG90\nTimer5', fontsize=15, weight='bold', color=CTL, va='center')
ax.text(18.8, 2.9, 'Periodo 20 ms (40000 × 0,5 µs) · pulso 1000–2000 µs\nel ANCHO codifica el ángulo 0–180°', fontsize=13, color=T2, ha='right')
save(fig, 'F18_A02_VECTOR_pwm_vs_servo_v001')

# ---------- F19 lazo abierto / cerrado
fig, ax = canvas(19.2, 9.0)
ax.text(0.5, 8.3, 'Lazo abierto', fontsize=19, weight='bold', color=T2)
for i, (t, c) in enumerate([('PWM pedido', PRG), ('Driver', ELE), ('Motor + rueda', MEC), ('Velocidad', CTL)]):
    box(ax, 1.0 + i * 4.6, 6.1, 3.4, 1.5, t, c, fs=16)
    if i: arrow(ax, (1.0 + i * 4.6 - 1.2, 6.85), (1.0 + i * 4.6, 6.85))
ax.text(0.5, 4.8, 'Lazo cerrado (modos Manual, Línea, GOTO)', fontsize=19, weight='bold', color=CTL)
ax.add_patch(Circle((4.2, 2.6), 0.45, fc=BG, ec=TX, lw=2)); ax.text(4.2, 2.6, 'Σ', ha='center', va='center', fontsize=20)
box(ax, 0.3, 1.9, 2.8, 1.4, 'r: objetivo\nmm/s', PRG, fs=14); arrow(ax, (3.1, 2.6), (3.75, 2.6))
box(ax, 5.4, 1.9, 2.9, 1.4, 'PID', CTL, fs=17); arrow(ax, (4.65, 2.6), (5.4, 2.6), text='e', toff=(0, .3))
box(ax, 9.2, 1.9, 2.9, 1.4, 'Rampa +\nsaturación', CTL, fs=14); arrow(ax, (8.3, 2.6), (9.2, 2.6))
box(ax, 13.0, 1.9, 2.9, 1.4, 'Driver +\nmotor', ELE, fs=15); arrow(ax, (12.1, 2.6), (13.0, 2.6), text='PWM', toff=(0, .3))
box(ax, 16.4, 1.9, 2.5, 1.4, 'Hall', ELE, fs=16); arrow(ax, (15.9, 2.6), (16.4, 2.6))
ax.add_patch(FancyArrowPatch((17.65, 1.9), (4.2, 2.15), arrowstyle='-|>', mutation_scale=16, color=TX, lw=2, connectionstyle='bar,angle=180,fraction=-0.12'))
ax.text(10.9, 0.25, 'y: velocidad medida (Δcuentas → mm/s)', ha='center', fontsize=13.5, color=T2)
ax.text(4.6, 3.25, '−', fontsize=20)
save(fig, 'F19_A01_VECTOR_lazo_abierto_cerrado_v001')

# ---------- F23 capas
fig, ax = canvas(19.2, 8.0)
cols = [('ATmega2560 · C avr-libc', ['Aplicación: afr2_product.c (PID, modos)', 'Servicios propios: afr2_link, I²C Manager', 'Registros: TCCR1A, OCR2A, UBRR1L, TWBR', 'Periférico: Timer / USART1 / TWI'], ELE),
        ('ESP8266 · Arduino core', ['Aplicación: broker.cpp (JSON ↔ ROBO2)', 'Bibliotecas: ESPAsyncWebServer, ArduinoJson', 'Framework: WiFi, LittleFS, Serial', 'SDK Espressif + Xtensa LX106'], PRG)]
for j, (t, ls, c) in enumerate(cols):
    x = 0.7 + j * 9.4
    ax.text(x + 4.0, 7.3, t, ha='center', fontsize=19, weight='bold', color=c)
    for i, l in enumerate(ls):
        box(ax, x, 5.4 - i * 1.6, 8.0, 1.3, l, c, fs=14.5, fill=c + ('33' if i == 0 else '14'))
ax.text(9.6, 0.25, '↓ más cerca del hardware', ha='center', fontsize=14, color=T2)
save(fig, 'F23_A02_VECTOR_capas_abstraccion_v001')

# ---------- F24 secuencia HTTP
fig, ax = canvas(19.2, 9.0)
lanes = [('Navegador', 3.0, PRG), ('ESP8266 (AP o STA)', 16.0, PRG)]
for n, x, c in lanes:
    box(ax, x - 2.6, 7.5, 5.2, 1.2, n, c, fs=21); ax.plot([x, x], [7.5, 0.3], color=GR, lw=2, ls='--')
steps = [(7.0, '1. Asociación Wi-Fi (AP del robot o red del usuario)', True),
         (5.9, '2. GET /  (Accept-Encoding: gzip)', True), (4.9, '200 index.html.gz · no-store', False),
         (3.9, '3. GET /assets/*.js.gz, modelo .glb.gz', True), (2.9, '200 desde LittleFS · ETag', False),
         (1.9, '4. Upgrade: websocket → /ws', True), (0.9, '101 Switching Protocols → JSON afr2-ws-1', False)]
for y, t, right in steps:
    a, b = (3.0, y), (16.0, y)
    arrow(ax, a if right else b, b if right else a, PRG if right else T2, text=t, toff=(0, 0.36), fs=17.5)
save(fig, 'F24_A01_VECTOR_secuencia_http_v001')

# ---------- F25 OSI didáctico
fig, ax = canvas(19.2, 9.6)
L = [('7 Aplicación', 'HTTP · WebSocket · mensajes afr2-ws-1'), ('6 Presentación', 'JSON (texto) · gzip de archivos'),
     ('5 Sesión', '— (hello/requestId correlacionan; no es OSI-5)'), ('4 Transporte', 'TCP (ESPAsyncTCP)'),
     ('3 Red', 'IP'), ('2 Enlace', 'Wi-Fi 802.11 (MAC)'), ('1 Física', 'Radio 2,4 GHz')]
for i, (a, b) in enumerate(L):
    y = 8.2 - i * 1.18
    box(ax, 1.0, y, 5.0, 1.0, a, PRG, fs=16, fill=PRG + ('30' if i < 2 else '14'))
    ax.text(6.7, y + 0.5, b, va='center', fontsize=15.5, color=TX if i != 2 else T2)
box(ax, 13.6, 1.0, 5.0, 3.0, 'Enlace ESP ↔ Mega', ELE, 'UART 115200 8N1\ntrama ROBO2 + XOR\n(fuera del modelo TCP/IP)', fs=16)
ax.text(9.6, 0.25, 'Representación didáctica: asocia ejemplos del proyecto a capas; no es una implementación OSI', ha='center', fontsize=13, color=T2)
save(fig, 'F25_A01_VECTOR_osi_didactico_v001')

# ---------- F27 trama ROBO2
fig, ax = canvas(19.2, 7.2)
F = [('R', '52', 'cabecera'), ('O', '4F', ''), ('B', '42', ''), ('O', '4F', ''), ('2', '32', ''), ('N', 'u8', 'largo'),
     (':', '3A', 'token A'), ('·', 'A5', 'token B'), ('v', '01', 'versión'), ('RT', 'u8', 'ruta'), ('CMD', 'u8', 'comando'),
     ('payload', 'N ≤ 64', 'datos'), ('XOR', 'u8', 'check')]
x = 0.35
cols = [ELE] * 5 + [CTL, PRG, PRG, PRG, CTL, CTL, MEC, '#FF6B81']
for i, ((a, b, c), col) in enumerate(zip(F, cols)):
    w = 3.3 if a == 'payload' else 1.22
    box(ax, x, 3.2, w - 0.08, 1.9, a, col, fs=17, fill=col + '30', r=0.08)
    ax.text(x + w / 2, 2.75, b, ha='center', fontsize=12.5, color=T2, family='Consolas')
    ax.text(x + w / 2, 5.35, str(i) if i < 11 else ('11…' if a == 'payload' else '11+N'), ha='center', fontsize=12, color=T2)
    if c: ax.text(x + w / 2, 1.95, c, ha='center', fontsize=12, color=T2)
    x += w
ax.text(9.6, 6.5, 'Trama ROBO2 perfil 2: 12 + N bytes (máx. 76) · offsets en byte', ha='center', fontsize=18, weight='bold')
ax.text(9.6, 0.7, 'XOR = R ^ O ^ B ^ O ^ 2 ^ N ^ 3A ^ A5 ^ 01 ^ RT ^ CMD ^ payload   (todos los bytes anteriores)', ha='center', fontsize=14.5, family='Consolas')
save(fig, 'F27_A01_VECTOR_trama_robo2_v001')

# ---------- F28 evento → OLED
fig, ax = canvas(19.2, 6.0)
seq = [('ISR USART1', 'anillo RX 128 B', ELE), ('afr2_wire_push', 'máquina de estados\n+ XOR', PRG), ('dispatch', 'CMD → handler', PRG),
       ('estado', 'Afr2Product\nmodo / notice', CTL), ('UI OLED', 'notificación[4]\n3000 ms', ELE)]
for i, (t, s, c) in enumerate(seq):
    box(ax, 0.4 + i * 3.8, 1.8, 3.2, 2.4, t, c, s, fs=16)
    if i: arrow(ax, (0.4 + i * 3.8 - 0.55, 3.0), (0.4 + i * 3.8, 3.0))
ax.text(9.6, 5.2, 'La interrupción solo encola bytes; parseo y efectos corren en el lazo principal', ha='center', fontsize=15, color=T2)
save(fig, 'F28_A01_VECTOR_evento_oled_v001')

# ---------- F30 memoria
fig, ax = plt.subplots(figsize=(12.8, 6.4), dpi=150)
fig.patch.set_alpha(0); ax.set_facecolor('none')
rows = [('ATmega · Flash (.text+.data)', 124240, 262144, ELE), ('ATmega · SRAM estática (.data+.bss)', 5614, 8192, ELE),
        ('ESP · RAM estática', 46644, 81920, PRG), ('Web empaquetada (gzip) / presupuesto', 2487493, 2867200, PRG)]
for i, (n, u, cap, c) in enumerate(rows[::-1]):
    ax.barh(i, cap, color='#1C2639', height=0.55); ax.barh(i, u, color=c, height=0.55)
    ax.text(cap * 1.01, i, f'{u:,} / {cap:,} B ({u / cap * 100:.0f} %)'.replace(',', '.'), va='center', fontsize=13, color=TX)
ax.set_xscale('log'); ax.set_yticks(range(4)); ax.set_yticklabels([r[0] for r in rows[::-1]], fontsize=13, color=TX)
ax.set_xlim(3000, 2.4e7); ax.tick_params(axis='x', labelsize=11)
for s in ax.spines.values(): s.set_visible(False)
ax.set_xlabel('bytes (escala log) · capacidades: datasheet chip / presupuesto del paquete', fontsize=12)
fig.tight_layout(); fig.savefig(os.path.join(OUT, 'F30_A01_CHART_memoria_v001.png'), transparent=True); fig.savefig(os.path.join(OUT, 'F30_A01_CHART_memoria_v001.svg'), transparent=True); plt.close(fig); print('OK F30')

# ---------- F32 monorepo y contratos
fig, ax = canvas()
ax.add_patch(Circle((9.6, 5.3), 2.0, fc='#16203A', ec=TX, lw=2.2))
ax.text(9.6, 5.65, 'Contracts', ha='center', fontsize=24, weight='bold'); ax.text(9.6, 4.75, 'arquitectura · integración\n· validación', ha='center', fontsize=13.5, color=T2)
mods = [('Atmega', 'C avr-libc · Makefile', ELE), ('ESP', 'PlatformIO · Arduino', PRG), ('Web', 'React · Vite · Three', PRG),
        ('CAD', 'Fusion · Blender · KiCad', MEC), ('Loader', 'C# Windows · MCP', CTL), ('Documentación', 'fichas AFR2 · CHANGELOG', T2)]
for i, (t, s, c) in enumerate(mods):
    a = math.radians(90 - i * 60); x = 9.6 + 6.4 * math.cos(a); y = 5.3 + 3.6 * math.sin(a)
    box(ax, x - 2.2, y - 0.8, 4.4, 1.6, t, c, s, fs=20, fill=BG)
    arrow(ax, (9.6 + 2.0 * math.cos(a), 5.3 + 2.0 * math.sin(a)), (x - 2.2 * math.cos(a) * 0.9, y - 0.8 * math.sin(a)), c, both=True, lw=1.8)
ax.text(9.6, 0.25, 'Un solo Git · wire_catalog.json · versión de producto única (afr2_build_info.h = package.json)', ha='center', fontsize=14, color=T2)
save(fig, 'F32_A01_VECTOR_monorepo_contratos_v001')

# ---------- F32 caso batería
fig, ax = canvas(19.2, 3.6)
chain = [('Divisor\n10k/10k', 'hardware', ELE), ('ADC5', 'afr2_sensors.c', ELE), ('Millivolts()', 'firmware C', CTL),
         ('CMD 0x55', 'UART ROBO2', PRG), ('system.battery', 'WS JSON (ESP)', PRG), ('BatteryPill', 'estado React', PRG)]
for i, (t, s, c) in enumerate(chain):
    box(ax, 0.3 + i * 3.17, 0.4, 2.75, 2.8, t, c, s, fs=21)
    if i: arrow(ax, (0.3 + i * 3.17 - 0.42, 1.8), (0.3 + i * 3.17, 1.8))
save(fig, 'F32_A02_VECTOR_caso_bateria_v001')
