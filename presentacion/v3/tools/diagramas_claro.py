# Diagramas de la presentación en tema claro (glass). Derivado de diagramas_v3.py; fondo transparente sobre #F6F8FC.
# Cada número que aparece sale de una fuente del repo citada en evidence/verification_matrix.csv.
# Uso: python -I diagramas_claro.py <salida_dir>
import sys, os, math
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import FancyBboxPatch, FancyArrowPatch, Rectangle, Circle, Polygon

OUT = sys.argv[1] if len(sys.argv) > 1 else '.'
os.makedirs(OUT, exist_ok=True)
MEC, ELE, CTL, PRG = '#3B82F6', '#F59E0B', '#8B5CF6', '#10B981'
MECd, ELEd, CTLd, PRGd = '#1D4ED8', '#B45309', '#6D28D9', '#047857'
BG, TX, T2, GR = '#FFFFFF', '#132033', '#4F6178', '#CBD5E1'
PANEL, SUB, LINE = '#FFFFFF', '#EEF3F8', '#64748B'
plt.rcParams.update({'font.family': ['Inter', 'Segoe UI'], 'font.size': 15, 'text.color': TX,
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
        fig.savefig(os.path.join(OUT, f'{name}.{ext}'), transparent=True)
    plt.close(fig)
    print('OK', name)


def box(ax, x, y, w, h, text, color, sub=None, fs=17, fill=None, lw=2.2, ha='center', r=0.18):
    ax.add_patch(FancyBboxPatch((x, y), w, h, boxstyle=f'round,pad=0,rounding_size={r}',
                                fc=fill or (color + '1F'), ec=color, lw=lw, zorder=3))
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


# ---------- F02 Venn de las cuatro ramas (materias del Plan 2023 nombradas en la charla)
fig, ax = canvas(19.2, 10.8)
cx, cy, d, R = 9.6, 5.25, 1.95, 3.35
VEN = [('MECÁNICA', MEC, MECd, (-1, 1), ['Sistemas de\nRepresentación I/II', 'Estática y Resist.\nde Materiales', 'Ciencias de\nlos Materiales']),
       ('CONTROL', CTL, CTLd, (1, 1), ['Sistemas\ny Señales', 'Sistemas de\nControl', 'Ec. Diferenciales y\nCálculo Numérico']),
       ('PROGRAMACIÓN', PRG, PRGd, (1, -1), ['Computación I/II', 'Sistemas Operativos\ny Redes']),
       ('ELECTRÓNICA', ELE, ELEd, (-1, -1), ['Electrónica\nBásica', 'Electrónica\nDigital', 'Laboratorio de\nMediciones'])]
for t, c, cd, (sx, sy), mats in VEN:
    x0, y0 = cx + sx * d, cy + sy * d
    ax.add_patch(Circle((x0, y0), R, fc=c + '26', ec=c, lw=2.6, zorder=2))
    ax.add_patch(Circle((x0, y0), R - 0.06, fc='none', ec='#FFFFFFAA', lw=1.2, zorder=2))
    ux, uy = sx / math.sqrt(2), sy / math.sqrt(2)
    tx, ty = x0 + ux * 1.55, y0 + uy * 1.35
    ax.text(tx, ty, '\n\n'.join(mats), ha='center', va='center', fontsize=12.2, color=TX, linespacing=1.12, zorder=5)
    lx = 0.25 if sx < 0 else 18.95
    ly = y0 + sy * 1.9
    ax.text(lx, ly, t, ha='left' if sx < 0 else 'right', va='center', fontsize=19, weight='bold', color=cd, zorder=6)
    ax.annotate('', (x0 + ux * R * 0.98, y0 + uy * R * 0.98), (lx - sx * 1.4, ly - sy * 0.42),
                arrowprops=dict(arrowstyle='-|>', color=cd, lw=2, mutation_scale=18), zorder=6)
PAIRS = [((0, 1), 'Mecánica Racional\nRobótica I'), ((1, 0), 'Sistemas\nEmbebidos\nAutomatización\nIndustrial'),
         ((0, -1), 'Micro-\ncontroladores'), ((-1, 0), 'Máquinas\nEléctricas')]
for (px, py), t in PAIRS:
    ax.text(cx + px * 3.05, cy + py * 2.95, t, ha='center', va='center', fontsize=12, weight='bold', color=TX, linespacing=1.1, zorder=5)
ax.add_patch(FancyBboxPatch((cx - 1.55, cy - 0.78), 3.1, 1.56, boxstyle='round,pad=0,rounding_size=0.75', fc='#FFFFFFE6', ec='#13203333', lw=1.4, zorder=7))
ax.text(cx, cy + 0.33, 'PROYECTO DE\nINGENIERÍA MECATRÓNICA', ha='center', va='center', fontsize=10.5, weight='bold', color=TX, linespacing=1.05, zorder=8)
ax.text(cx, cy - 0.38, 'Sistemas Mecatrónicos I/II\nRobótica II', ha='center', va='center', fontsize=10, color=T2, linespacing=1.1, zorder=8)
save(fig, 'F02_A01_VECTOR_venn_ramas_v002')

# ---------- F03 árbol vertical por años (aristas de C.D. 726/23 citadas en el informe; ubicación por año a confirmar)
fig, ax = canvas(19.2, 10.8)
YR = {2: 8.85, 3: 6.55, 4: 4.25, 5: 1.95}
N = {'Ciencias de los\nMateriales': (1.9, 2, MEC), 'Estática y Resist.\nde Materiales': (4.5, 2, MEC), 'Ec. Dif. y Cálculo\nNumérico': (7.1, 2, CTL), 'Computación II': (16.0, 2, PRG),
     'Sistemas y\nSeñales': (8.4, 3, CTL), 'Lab. de\nMediciones': (11.0, 3, ELE), 'Electrónica\nDigital': (13.6, 3, ELE), 'Electrónica\nBásica': (17.9, 3, ELE),
     'Mecánica\nRacional': (4.5, 4, MEC), 'Automatización\nIndustrial': (7.9, 4, CTL), 'Sistemas de\nControl': (11.1, 4, CTL), 'Microcontroladores': (14.1, 4, ELE),
     'Robótica II': (1.6, 5, MEC), 'Robótica I': (4.5, 5, CTL), 'SO y Redes de\nComunicación': (12.9, 5, PRG), 'Sistemas\nEmbebidos': (17.6, 5, PRG)}
RAD = {}
W_, H_ = 2.45, 1.0
for y in [9.95, 7.7, 5.4, 3.1, 0.75]:
    ax.plot([0.25, 18.95], [y, y], color='#94A3B8', lw=1.3, ls=(0, (5, 4)), zorder=1)
for k, yy in YR.items():
    ax.text(0.35, yy + 0.95, f'{k}.º AÑO', fontsize=13.5, weight='bold', color=T2, va='center')
ax.text(0.35, 10.35, '1.er AÑO · Ciencias Básicas: Análisis Matemático, Álgebra, Física I, Química, Computación I, Sistemas de Representación I',
        fontsize=12.5, color=T2, va='center')
P = {n: (x, YR[yr]) for n, (x, yr, c) in N.items()}
E = [('Estática y Resist.\nde Materiales', 'Mecánica\nRacional', 'R'), ('Ec. Dif. y Cálculo\nNumérico', 'Mecánica\nRacional', 'R'),
     ('Ciencias de los\nMateriales', 'Mecánica\nRacional', 'A'),
     ('Electrónica\nDigital', 'Microcontroladores', 'R'), ('Lab. de\nMediciones', 'Microcontroladores', 'R'),
     ('Electrónica\nBásica', 'Microcontroladores', 'A'), ('Computación II', 'Microcontroladores', 'A'),
     ('Electrónica\nDigital', 'Sistemas de\nControl', 'R'), ('Lab. de\nMediciones', 'Sistemas de\nControl', 'R'),
     ('Sistemas y\nSeñales', 'Sistemas de\nControl', 'R'),
     ('Microcontroladores', 'Sistemas\nEmbebidos', 'R'), ('Electrónica\nDigital', 'Sistemas\nEmbebidos', 'A'), ('Computación II', 'Sistemas\nEmbebidos', 'A'),
     ('Sistemas de\nControl', 'Robótica I', 'R'), ('Mecánica\nRacional', 'Robótica I', 'R'),
     ('Sistemas\nEmbebidos', 'SO y Redes de\nComunicación', 'R'), ('Automatización\nIndustrial', 'SO y Redes de\nComunicación', 'R')]
for a_, b_, lab in E:
    (x1, y1), (x2, y2) = P[a_], P[b_]
    p1, p2 = (x1, y1 - H_ / 2), (x2, y2 + H_ / 2)
    if y1 == y2:
        p1, p2 = (x1 - W_ / 2, y1), (x2 + W_ / 2, y2)
    ax.add_patch(FancyArrowPatch(p1, p2, arrowstyle='-|>', mutation_scale=13, color=LINE, lw=1.6, alpha=.85,
                                 connectionstyle=f'arc3,rad={RAD.get((a_, b_), 0.0)}', zorder=2, shrinkA=0, shrinkB=2))
    t = 0.62 if (a_, b_) not in RAD else 0.5
    mx, my = p1[0] + (p2[0] - p1[0]) * t, p1[1] + (p2[1] - p1[1]) * t
    ax.add_patch(Circle((mx, my), 0.19, fc='white', ec=LINE, lw=1.1, zorder=6))
    ax.text(mx, my, lab, ha='center', va='center', fontsize=9.5, weight='bold', color=TX, zorder=7)
(x1, y1), (x2, y2) = P['Robótica I'], P['Robótica II']
ax.add_patch(FancyArrowPatch((x1 - W_ / 2, y1), (x2 + 1.1, y2), arrowstyle='-|>', mutation_scale=13, color=T2, lw=1.4, ls=(0, (2, 3)), zorder=2))
for n, (x, yr, c) in N.items():
    y = YR[yr]
    w = 2.0 if n in ('Robótica II', 'Computación II') else W_
    rs = H_ / 2
    ax.add_patch(FancyBboxPatch((x - w / 2 + 0.05, y - H_ / 2 - 0.08), w, H_, boxstyle=f'round,pad=0,rounding_size={rs}', fc='#13203314', ec='none', zorder=3))
    ax.add_patch(FancyBboxPatch((x - w / 2, y - H_ / 2), w, H_, boxstyle=f'round,pad=0,rounding_size={rs}', fc='#FFFFFFF0', ec=c, lw=2.2, zorder=4))
    ax.add_patch(FancyBboxPatch((x - w / 2, y - H_ / 2), w, H_, boxstyle=f'round,pad=0,rounding_size={rs}', fc=c + '1A', ec='none', zorder=4))
    ax.text(x, y, n, ha='center', va='center', fontsize=11.5, weight='bold', color=TX, linespacing=1.05, zorder=5)
ax.text(18.95, 0.35, 'R regularizada · A aprobada (para cursar, C.D. 726/23) · punteada: vínculo del proyecto, no correlatividad',
        ha='right', fontsize=11.5, color=T2)
save(fig, 'F03_A01_VECTOR_arbol_vertical_v002')


# ---------- F06 matrices de cinemática (Mecanum en X, orden FL, FR, RL, RR; afr2_product.c:130 y :295-304)
def matrix(ax, x, y, rows, fs=17, cw=0.62, rh=0.62, color=TX):
    nr, nc = len(rows), max(len(r) for r in rows)
    w, h = nc * cw, nr * rh
    for side in (0, 1):
        xx = x if side == 0 else x + w
        k = 0.12 if side == 0 else -0.12
        ax.plot([xx + k, xx, xx, xx + k], [y + h / 2, y + h / 2, y - h / 2, y - h / 2], color=color, lw=1.8)
    for i, r in enumerate(rows):
        for j, v in enumerate(r):
            ax.text(x + (j + 0.5) * cw, y + h / 2 - (i + 0.5) * rh, v, ha='center', va='center', fontsize=fs, color=color)
    return x + w


def mt(ax, x, y, s, fs=19, color=TX, ha='left'):
    ax.text(x, y, s, ha=ha, va='center', fontsize=fs, color=color)


def head(ax, x, y, t, sub):
    ax.text(x, y, t, fontsize=16, weight='bold', color=MECd, va='center')
    ax.text(x, y - 0.45, sub, fontsize=11.5, color=T2, va='center')


fig, ax = canvas(19.2, 9.6)
head(ax, 0.3, 9.1, '1 · Cinemática inversa  [1][3]', 'velocidad del chasis → velocidad de cada rueda')
mt(ax, 0.3, 6.9, r'$\dot{\varphi}=\dfrac{1}{r}$', 20)
x = matrix(ax, 1.75, 6.9, [[r'$1$', r'$-1$', r'$-L$'], [r'$1$', r'$1$', r'$L$'], [r'$1$', r'$1$', r'$-L$'], [r'$1$', r'$-1$', r'$L$']], cw=0.75, rh=0.6)
x = matrix(ax, x + 0.2, 6.9, [[r'$v_x$'], [r'$v_y$'], [r'$\omega_z$']], cw=0.75, rh=0.6)
mt(ax, 0.3, 5.1, r'$\dot{\varphi}=[\dot\varphi_{FL}\ \dot\varphi_{FR}\ \dot\varphi_{RL}\ \dot\varphi_{RR}]^T$' + '\n' + r'$L=a+b=170\,\mathrm{mm},\ \ r=40\,\mathrm{mm}$', 13.5, T2)
head(ax, 6.75, 9.1, '2 · Cinemática directa  [1][3]', 'velocidad de las ruedas → velocidad del chasis')
mt(ax, 6.75, 6.9, r'$\xi_B=r\,J^{+}\dot{\varphi}=\dfrac{r}{4}$', 19)
x = matrix(ax, 9.55, 6.9, [[r'$1$', r'$1$', r'$1$', r'$1$'], [r'$-1$', r'$1$', r'$1$', r'$-1$'], [r'$\frac{-1}{L}$', r'$\frac{1}{L}$', r'$\frac{-1}{L}$', r'$\frac{1}{L}$']], cw=0.72, rh=0.72)
mt(ax, x + 0.15, 6.9, r'$\dot{\varphi}$', 20)
mt(ax, 6.75, 5.1, r'$J^{+}=(J^TJ)^{-1}J^T$' + '\nrango 3: sin singularidades si $L>0$', 13.5, T2)
head(ax, 13.6, 9.1, '3 · Transformación SE(2)  [2]', 'pose del chasis {B} en el mundo {W}')
mt(ax, 13.6, 6.9, r'$^{W}T_{B}=$', 20)
matrix(ax, 14.9, 6.9, [[r'$\cos\theta$', r'$-\sin\theta$', r'$x$'], [r'$\sin\theta$', r'$\cos\theta$', r'$y$'], [r'$0$', r'$0$', r'$1$']], cw=1.25, rh=0.62, fs=16)
mt(ax, 13.6, 5.1, r'bloque de rotación $R_z(\theta)$' + '\n' + r'+ traslación $(x,\ y)$', 13.5, T2)
head(ax, 0.3, 4.2, '4 · Jacobiano ruedas → mundo  [1]', 'velocidades de las ruedas a velocidad de la pose')
mt(ax, 0.3, 2.3, r'$\dot{\mathbf{p}}=$', 20)
x = matrix(ax, 1.05, 2.3, [[r'$\cos\theta$', r'$-\sin\theta$', r'$0$'], [r'$\sin\theta$', r'$\cos\theta$', r'$0$'], [r'$0$', r'$0$', r'$1$']], cw=1.15, rh=0.6, fs=15)
mt(ax, x + 0.15, 2.3, r'$r\,J^{+}\,\dot{\varphi}\;=\;\mathcal{J}(\theta)\,\dot{\varphi}$', 19)
mt(ax, 0.3, 0.65, r'$\mathcal{J}(\theta)\in\mathbb{R}^{3\times4}$: 4 ruedas, 3 GDL · modo interno $\mathcal{N}(J^{+})=[1\ 1\ -1\ -1]^T$', 13.5, T2)
head(ax, 10.6, 4.2, '5 · Odometría discreta (punto medio)  [3]', 'lo que integra el ATmega cada ciclo')
mt(ax, 10.6, 2.75, r'$\Delta\mathbf{q}_B=J^{+}\,\mathbf{d},\qquad d_i=\Delta N_i\,\dfrac{\pi D}{N_{rev}}$', 18)
mt(ax, 10.6, 1.65, r'$\mathbf{p}_{k+1}=\mathbf{p}_k+R_z\!\left(\theta_k+\dfrac{\Delta\theta}{2}\right)\Delta\mathbf{q}_B$', 18)
mt(ax, 10.6, 0.65, r'$D=80\,\mathrm{mm},\ N_{rev}=370$ pulsos/vuelta', 13.5, T2)
save(fig, 'F06_A01_VECTOR_matrices_cinematica_v002')

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
    box(ax, x, 1.0, 2.9, 2.3, t, c, s, fs=14.5, fill=PANEL)
    arrow(ax, (16.8, 6.5), (x + 1.45, 3.4), '#94A3B8', lw=1.4)
ax.text(4.6, 5.3, 'Hardware del auto (modelo CAD: AutoFinalRobo2.glb)', ha='center', fontsize=14, color=T2)
save(fig, 'F04_A02_VECTOR_arquitectura_v001')

# ---------- F13 divisor de batería
fig, ax = canvas()
ax.add_patch(Rectangle((1.4, 3.0), 1.6, 4.8, fc=PANEL, ec=ELE, lw=2.5))
ax.text(2.2, 6.6, '18650', ha='center', fontsize=14); ax.text(2.2, 4.2, '18650', ha='center', fontsize=14)
ax.plot([1.4, 3.0], [5.4, 5.4], color=ELE, lw=1.5)
ax.text(2.2, 8.2, 'BAT+ (después de llave)', ha='center', fontsize=13, color=T2)
ax.plot([2.2, 2.2, 7.0, 7.0], [7.8, 9.0, 9.0, 8.3], color=TX, lw=2.4)
for y0, lab in [(6.1, 'R1 = 10 kΩ'), (2.9, 'R2 = 10 kΩ')]:
    ax.add_patch(Rectangle((6.6, y0), 0.8, 2.2, fc=PANEL, ec=ELE, lw=2.5)); ax.text(7.8, y0 + 1.1, lab, va='center', fontsize=17)
ax.plot([7.0, 7.0], [6.1, 5.1], color=TX, lw=2.4); ax.plot([7.0, 7.0], [2.9, 1.6], color=TX, lw=2.4)
ax.plot([2.2, 2.2, 7.0], [3.0, 1.6, 1.6], color=TX, lw=2.4); ax.text(4.6, 1.15, 'GND', ha='center', fontsize=13, color=T2)
ax.plot([7.0, 11.0], [5.6, 5.6], color=TX, lw=2.4); ax.add_patch(Circle((7.0, 5.6), 0.09, color=TX))
ax.plot([9.3, 9.3], [5.6, 4.4], color=TX, lw=2.2); ax.plot([8.9, 9.7], [4.4, 4.4], color=ELE, lw=3); ax.plot([8.9, 9.7], [4.15, 4.15], color=ELE, lw=3)
ax.plot([9.3, 9.3], [4.15, 1.6], color=TX, lw=2.2); ax.plot([7.0, 9.3], [1.6, 1.6], color=TX, lw=2.4)
ax.text(9.9, 4.3, '100 nF', fontsize=14)
box(ax, 11.0, 4.6, 4.2, 2.0, 'ADC5 · A5 (PF5)', ELE, '10 bits · ref. AVCC 5 V\nprescaler 128 · 8 muestras', fs=17, fill=PANEL)
ax.text(11.0, 9.1, r'$V_{A5}=V_B\,\dfrac{R_2}{R_1+R_2}=\dfrac{V_B}{2}$', fontsize=26, va='center')
ax.text(11.0, 7.6, r'$mV=\dfrac{raw\cdot 10000+512}{1024}$', fontsize=24, va='center')
ax.text(15.6, 6.5, 'Rango útil: V_B ≤ 10 V\n(2×4,2 V = 8,4 V → 4,2 V en A5)', fontsize=14, color=T2, va='top', linespacing=1.4)
ax.text(11.0, 3.6, 'Sin calibrar: referencia nominal 5000 mV.\nEl dato es tensión, no % de carga.', fontsize=13, color=T2, va='top', linespacing=1.4)
save(fig, 'F13_A01_VECTOR_divisor_bateria_v001')

# ---------- F14 cuadratura Hall (señal pin A, señal pin B, sentidos ±1 y matriz binaria)
fig, ax = canvas(19.2, 8.2)
import numpy as np
# Panel izquierdo superior: Giro sentido +1 (A adelanta a B: en flanco ↑ de A, B = 0)
t = np.linspace(0, 4.2, 1200)
A1 = (np.sin(2 * np.pi * t / 2) >= 0).astype(float)
B1 = (np.sin(2 * np.pi * (t - 0.5) / 2) >= 0).astype(float)
ax.add_patch(FancyBboxPatch((0.2, 4.35), 11.4, 3.65, boxstyle='round,pad=0,rounding_size=0.22', fc='#FFFFFFD8', ec=PRGd + '66', lw=1.6))
ax.text(0.45, 7.72, 'Sentido +1  (↻ ej. horario) · A adelanta 90° a B → al subir A, B = 0', fontsize=12.5, weight='bold', color=PRGd, va='center')
ax.plot(1.5 + t * 2.25, 6.25 + A1 * 0.85, color=ELE, lw=2.8)
ax.plot(1.5 + t * 2.25, 4.75 + B1 * 0.85, color=CTL, lw=2.8)
ax.text(0.85, 6.67, 'Pin A\n(A8–A11)', fontsize=11, weight='bold', color=ELEd, ha='center', va='center')
ax.text(0.85, 5.17, 'Pin B\n(A12–A15)', fontsize=11, weight='bold', color=CTLd, ha='center', va='center')
for k in range(3):
    x = 1.5 + (2 * k) * 2.25
    ax.plot([x, x], [4.65, 7.25], color=PRGd, lw=1.2, ls=':', zorder=1)
    ax.add_patch(Circle((x, 4.75), 0.12, fc=PRG, ec='white', lw=1.2, zorder=5))
    ax.text(x, 7.38, 'A↑ y B=0 ⇒ +1', ha='center', fontsize=10.5, weight='bold', color=PRGd)

# Panel izquierdo inferior: Giro sentido -1 (B adelanta a A: en flanco ↑ de A, B = 1)
A2 = (np.sin(2 * np.pi * t / 2) >= 0).astype(float)
B2 = (np.sin(2 * np.pi * (t + 0.5) / 2) >= 0).astype(float)
ax.add_patch(FancyBboxPatch((0.2, 0.35), 11.4, 3.65, boxstyle='round,pad=0,rounding_size=0.22', fc='#FFFFFFD8', ec=CTLd + '66', lw=1.6))
ax.text(0.45, 3.72, 'Sentido −1  (↺ ej. antihorario) · B adelanta 90° a A → al subir A, B = 1', fontsize=12.5, weight='bold', color=CTLd, va='center')
ax.plot(1.5 + t * 2.25, 2.25 + A2 * 0.85, color=ELE, lw=2.8)
ax.plot(1.5 + t * 2.25, 0.75 + B2 * 0.85, color=CTL, lw=2.8)
ax.text(0.85, 2.67, 'Pin A\n(A8–A11)', fontsize=11, weight='bold', color=ELEd, ha='center', va='center')
ax.text(0.85, 1.17, 'Pin B\n(A12–A15)', fontsize=11, weight='bold', color=CTLd, ha='center', va='center')
for k in range(3):
    x = 1.5 + (2 * k) * 2.25
    ax.plot([x, x], [0.65, 3.25], color=CTLd, lw=1.2, ls=':', zorder=1)
    ax.add_patch(Circle((x, 1.60), 0.12, fc=CTL, ec='white', lw=1.2, zorder=5))
    ax.text(x, 3.38, 'A↑ y B=1 ⇒ −1', ha='center', fontsize=10.5, weight='bold', color=CTLd)

# Panel derecho: Matriz de verdad binaria (A, B) con flechas de giro horario / antihorario
ax.add_patch(FancyBboxPatch((11.95, 0.35), 7.05, 7.65, boxstyle='round,pad=0,rounding_size=0.22', fc='#FFFFFFE8', ec=GR, lw=1.6))
ax.text(15.48, 7.62, 'Matriz de estados binarios (A, B)', ha='center', fontsize=14, weight='bold', color=TX)
ax.text(13.05, 7.02, '↻ Horario\n(+1)', ha='center', va='center', fontsize=11.5, weight='bold', color=PRGd)
ax.text(17.90, 7.02, '↺ Antihorario\n(−1)', ha='center', va='center', fontsize=11.5, weight='bold', color=CTLd)
ax.text(14.85, 7.02, 'Pin A', ha='center', va='center', fontsize=12, weight='bold', color=ELEd)
ax.text(16.10, 7.02, 'Pin B', ha='center', va='center', fontsize=12, weight='bold', color=CTLd)
states = [('0', '0'), ('1', '0'), ('1', '1'), ('0', '1')]
sy_pos = [6.15, 5.25, 4.35, 3.45]
for idx, (va, vb) in enumerate(states):
    yy = sy_pos[idx]
    ax.add_patch(FancyBboxPatch((14.25, yy - 0.3), 2.45, 0.6, boxstyle='round,pad=0,rounding_size=0.12',
                                fc=ELE + '22' if idx == 1 else SUB, ec=PRGd if idx == 1 else GR, lw=1.4))
    ax.text(14.85, yy, va, ha='center', va='center', fontsize=14, weight='bold', color=TX, family='Consolas')
    ax.text(16.10, yy, vb, ha='center', va='center', fontsize=14, weight='bold', color=TX, family='Consolas')
# Flechas de secuencia izquierda (↻ +1: abajo 00 -> 10 -> 11 -> 01) y retorno
for idx in range(3):
    ax.annotate('', (13.35, sy_pos[idx + 1] + 0.12), (13.35, sy_pos[idx] - 0.12),
                arrowprops=dict(arrowstyle='-|>', color=PRGd, lw=2.2, mutation_scale=15))
ax.add_patch(FancyArrowPatch((13.35, sy_pos[3] - 0.15), (13.35, sy_pos[0] + 0.15),
                             arrowstyle='-|>', mutation_scale=15, color=PRGd, lw=1.8, ls='--', connectionstyle='arc3,rad=-0.55'))
ax.text(12.38, 4.8, '↓\n↻\n+1', ha='center', va='center', fontsize=12, weight='bold', color=PRGd)
# Flechas de secuencia derecha (↺ -1: arriba 01 -> 11 -> 10 -> 00) y retorno
for idx in range(3, 0, -1):
    ax.annotate('', (17.60, sy_pos[idx - 1] - 0.12), (17.60, sy_pos[idx] + 0.12),
                arrowprops=dict(arrowstyle='-|>', color=CTLd, lw=2.2, mutation_scale=15))
ax.add_patch(FancyArrowPatch((17.60, sy_pos[0] + 0.15), (17.60, sy_pos[3] - 0.15),
                             arrowstyle='-|>', mutation_scale=15, color=CTLd, lw=1.8, ls='--', connectionstyle='arc3,rad=-0.55'))
ax.text(18.58, 4.8, '↑\n↺\n−1', ha='center', va='center', fontsize=12, weight='bold', color=CTLd)

# Tabla de decisión del firmware (PCINT2 x1)
ax.text(15.48, 2.55, 'Decisión en el firmware (flanco ↑ de A)', ha='center', fontsize=12, weight='bold', color=TX)
ax.add_patch(FancyBboxPatch((12.35, 0.55), 6.25, 1.72, boxstyle='round,pad=0,rounding_size=0.14', fc=SUB, ec=GR, lw=1.2))
ax.text(12.55, 1.98, 'A: 0→1  &  B = 0   ⇒   +1   (↻ horario / un sentido)', fontsize=11.2, weight='bold', color=PRGd, va='center')
ax.text(12.55, 1.45, 'A: 0→1  &  B = 1   ⇒   −1   (↺ antihorario / opuesto)', fontsize=11.2, weight='bold', color=CTLd, va='center')
ax.text(12.55, 0.88, 'El signo físico de avance depende de motor_signs y lado', fontsize=9.8, color=T2, va='center')
save(fig, 'F14_A02_VECTOR_cuadratura_hall_v001')

# ---------- F15 ultrasonido (cotas en TRIG y ECHO + velocidad del sonido)
fig, ax = canvas(19.2, 7.8)
def sig(y, segs, c, lab):
    xs, ys = [], []
    for x0, x1, v in segs:
        xs += [x0, x1]; ys += [y + v, y + v]
    ax.plot(xs, ys, color=c, lw=3.2)
    ax.text(0.35, y + 0.55, lab, fontsize=15.5, weight='bold', color=c, va='center')
sig(5.05, [(2.5, 3.6, 0), (3.6, 5.1, 1.15), (5.1, 18.6, 0)], ELE, 'TRIG\nD30/PC7')
sig(1.95, [(2.5, 6.8, 0), (6.8, 13.8, 1.15), (13.8, 18.6, 0)], PRG, 'ECHO\nD31/PC6')
# Cotas del pulso TRIG (todo el ancho del pulso)
for xx in (3.6, 5.1):
    ax.plot([xx, xx], [6.25, 6.95], color=ELEd, lw=1.4, ls='--')
ax.annotate('', (3.6, 6.72), (5.1, 6.72), arrowprops=dict(arrowstyle='<|-|>', color=ELEd, lw=2.2, mutation_scale=15))
ax.text(4.35, 7.12, 'ancho pulso TRIG\n25 µs (1 tick · mín. ≥ 10 µs)', ha='center', va='bottom', fontsize=12.5, weight='bold', color=ELEd)
# Ráfaga 40 kHz entre TRIG y ECHO
ax.annotate('', (6.65, 4.15), (5.2, 4.95), arrowprops=dict(arrowstyle='-|>', color=T2, lw=1.6, ls='--'))
ax.text(6.9, 4.25, 'Emite 8 pulsos a 40 kHz → viaja a c ≈ 343 m/s → rebota en el obstáculo → vuelve', fontsize=12.5, color=TX, va='center')
# Cotas del pulso ECHO (igual estilo, bien nítido)
for xx in (6.8, 13.8):
    ax.plot([xx, xx], [1.15, 1.9], color=PRGd, lw=1.4, ls='--')
ax.annotate('', (6.8, 1.35), (13.8, 1.35), arrowprops=dict(arrowstyle='<|-|>', color=TX, lw=2.2, mutation_scale=16))
ax.text(10.3, 0.72, 'Δt = ancho del pulso ECHO (tiempo de ida y vuelta del sonido)', ha='center', fontsize=14.5, weight='bold', color=TX)
ax.text(18.6, 6.85, 'Timer3 CTC: 50 × 0,5 µs = 25 µs/tick\nTimeout 1280 ticks = 32 ms (no bloquea)', ha='right', va='top', fontsize=12.5, color=T2, linespacing=1.35)
save(fig, 'F15_A02_VECTOR_tof_tiempos_v001')

# ---------- F16 línea: pesos
fig, ax = canvas(19.2, 7.2)
ax.add_patch(Rectangle((6.3, 4.85), 3.7, 1.5, fc='#000000', ec='#94A3B8', lw=1, zorder=0))
ax.text(8.15, 6.45, 'cinta oscura', ha='center', fontsize=12, color=T2)
for i in range(5):
    x = 4.2 + i * 2.6
    on = i in (1, 2)
    ax.add_patch(Circle((x, 5.6), 0.55, fc=(ELE if on else SUB), ec=ELE, lw=2))
    ax.text(x, 5.6, f'IR{i + 1}', ha='center', va='center', fontsize=12, weight='bold', color='white' if on else TX)
    ax.text(x, 4.55, f'peso {2 - i:+d}', ha='center', fontsize=13.5, color=T2)
ax.text(9.6, 6.75, 'bit i = 1 si el sensor ve cinta', ha='center', fontsize=14, color=T2)
ax.text(9.6, 2.4, r'$e=\dfrac{\sum_{i\,activos}(2-i)}{n}\qquad \omega_z=\dfrac{e\cdot K_{p,línea}}{n}$', ha='center', fontsize=24)
ax.text(9.6, 0.4, 'Sin línea (0 bits) → STOP · 5 bits activos ≥150 ms → STOP (cruce)', ha='center', fontsize=13.5, color=T2)
save(fig, 'F16_A02_VECTOR_linea_pesos_v001')

# ---------- F16-B conversión analógica → digital en el módulo 5-IR (Electrónica Básica → Electrónica Digital)
fig, ax = canvas(19.2, 7.2)
# Bloque 1: etapa óptica analógica
box(ax, 0.3, 4.1, 5.3, 2.55, '1 · Sensor óptico IR\n(Electrónica Básica)', ELE,
    'LED IR emite luz al piso\nFototransistor varía corriente\nsegún reflectancia → V_sens continua', fs=15, fill='#FFFFFFE8')
# Bloque 2: trimmer + 74HC14D
box(ax, 6.7, 4.1, 5.8, 2.55, '2 · Trimmer + 74HC14D\n(Umbral con histéresis)', CTL,
    'Trimmer fija punto de disparo (0–3 cm)\nInversor Schmitt 74HC14D convierte\nla señal analógica en bit sin rebotes', fs=15, fill='#FFFFFFE8')
# Bloque 3: salida digital TTL
box(ax, 13.6, 4.1, 5.3, 2.55, '3 · Salida digital TTL\n(Electrónica Digital)', PRG,
    'Bit limpio 0 / 1 en pines IR1..IR5\nCinta negra = LOW (0 V, LED rojo ON)\nPiso blanco = HIGH (5 V) → PINF', fs=15, fill='#FFFFFFE8')
arrow(ax, (5.65, 5.38), (6.65, 5.38), ELEd, lw=2.6)
arrow(ax, (12.55, 5.38), (13.55, 5.38), CTLd, lw=2.6)

# Gráfico comparativo inferior: señal analógica vs umbrales del Schmitt trigger vs salida digital
ax.add_patch(FancyBboxPatch((0.3, 0.35), 18.6, 3.35, boxstyle='round,pad=0,rounding_size=0.2', fc='#FFFFFFE0', ec=GR, lw=1.4))
tt = np.linspace(0, 10, 600)
vs = 2.5 + 1.85 * np.sin(0.85 * tt) + 0.16 * np.sin(9.0 * tt)  # señal analógica con ruido
v_hi, v_lo = 3.1, 1.9
out = np.zeros_like(tt)
state = 1.0
for i, v in enumerate(vs):
    if v >= v_hi: state = 0.0   # inversor Schmitt
    elif v <= v_lo: state = 1.0
    out[i] = state
# Eje izquierdo: señal analógica
ax.plot(1.1 + tt * 0.72, 0.75 + vs * 0.45, color=ELEd, lw=2.4)
ax.plot([1.1, 8.3], [0.75 + v_hi * 0.45, 0.75 + v_hi * 0.45], color=CTLd, lw=1.4, ls='--')
ax.plot([1.1, 8.3], [0.75 + v_lo * 0.45, 0.75 + v_lo * 0.45], color=CTL, lw=1.4, ls='--')
ax.text(1.1, 3.32, 'Señal analógica continua V_sens(t) + ruido', fontsize=12.5, weight='bold', color=ELEd)
ax.text(8.45, 0.75 + v_hi * 0.45, 'V_T+ (umbral alto)', fontsize=10.5, color=CTLd, va='center')
ax.text(8.45, 0.75 + v_lo * 0.45, 'V_T− (umbral bajo)', fontsize=10.5, color=CTL, va='center')
ax.text(8.45, 1.15, 'Banda de histéresis\n(fijada por 74HC14D\ny ajustada con trimmer)', fontsize=10, color=T2, va='center')
# Flecha central
arrow(ax, (10.5, 2.0), (11.5, 2.0), TX, lw=2.2)
# Eje derecho: salida digital limpia
ax.plot(11.8 + tt * 0.68, 0.95 + out * 1.75, color=PRGd, lw=2.8)
ax.text(11.8, 3.32, 'Salida digital binaria limpia (0 / 1) → ATmega2560', fontsize=12.5, weight='bold', color=PRGd)
ax.text(11.45, 2.7, '5 V\n(1)', fontsize=10, color=PRGd, ha='right', va='center', weight='bold')
ax.text(11.45, 0.95, '0 V\n(0)', fontsize=10, color=PRGd, ha='right', va='center', weight='bold')
save(fig, 'F16_A03_VECTOR_ir_analog_digital_v001')

# ---------- F17 I2C (con explicación de TWBR y fórmula del datasheet)
fig, ax = canvas(19.2, 7.6)
box(ax, 0.7, 2.1, 4.5, 3.1, 'ATmega2560', ELE, 'TWI maestro\nSDA D20 · SCL D21', fs=20)
box(ax, 14.0, 2.1, 4.5, 3.1, 'OLED 128×64', ELE, 'controlador tipo SSD1306\núnico dispositivo I²C', fs=20)
for y, n in [(4.25, 'SDA (datos)'), (2.95, 'SCL (reloj · 100 kHz)')]:
    ax.plot([5.2, 14.0], [y, y], color=PRG if 'SDA' in n else CTL, lw=3.2)
    ax.text(9.6, y + 0.28, n, ha='center', fontsize=15, weight='bold', color=PRGd if 'SDA' in n else CTLd)
ax.text(9.6, 6.65, r'$f_{\mathrm{SCL}} = \dfrac{f_{\mathrm{CPU}}}{16 + 2 \cdot \mathrm{TWBR} \cdot 4^{\mathrm{TWPS}}} = \dfrac{16\ \mathrm{MHz}}{16 + 2 \cdot 72 \cdot 1} = 100\ \mathrm{kHz}$', ha='center', fontsize=18.5, color=TX)
ax.text(9.6, 5.65, 'TWBR (TWI Bit Rate Register) = 72 · registro de 8 bits que fija la frecuencia del reloj SCL (TWPS = 0 ⇒ prescaler 1)', ha='center', fontsize=12.8, color=T2, weight='bold')
ax.text(9.6, 0.95, 'I²C Manager: un dueño del bus, escrituras con deadline, nunca desde la interrupción', ha='center', fontsize=13.5, color=T2)
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
ax.add_patch(Circle((4.2, 2.6), 0.45, fc=PANEL, ec=TX, lw=2)); ax.text(4.2, 2.6, 'Σ', ha='center', va='center', fontsize=20)
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
    ax.barh(i, cap, color='#E2E8F0', height=0.55); ax.barh(i, u, color=c, height=0.55)
    ax.text(cap * 1.01, i, f'{u:,} / {cap:,} B ({u / cap * 100:.0f} %)'.replace(',', '.'), va='center', fontsize=13, color=TX)
ax.set_xscale('log'); ax.set_yticks(range(4)); ax.set_yticklabels([r[0] for r in rows[::-1]], fontsize=13, color=TX)
ax.set_xlim(3000, 2.4e7); ax.tick_params(axis='x', labelsize=11)
for s in ax.spines.values(): s.set_visible(False)
ax.set_xlabel('bytes (escala log) · capacidades: datasheet chip / presupuesto del paquete', fontsize=12)
fig.tight_layout(); fig.savefig(os.path.join(OUT, 'F30_A01_CHART_memoria_v001.png'), transparent=True); fig.savefig(os.path.join(OUT, 'F30_A01_CHART_memoria_v001.svg'), transparent=True); plt.close(fig); print('OK F30')

# ---------- F32 monorepo y contratos
fig, ax = canvas()
ax.add_patch(Circle((9.6, 5.3), 2.0, fc=SUB, ec=TX, lw=2.2))
ax.text(9.6, 5.65, 'Contracts', ha='center', fontsize=24, weight='bold'); ax.text(9.6, 4.75, 'arquitectura · integración\n· validación', ha='center', fontsize=13.5, color=T2)
mods = [('Atmega', 'C avr-libc · Makefile', ELE), ('ESP', 'PlatformIO · Arduino', PRG), ('Web', 'React · Vite · Three', PRG),
        ('CAD', 'Fusion · Blender · KiCad', MEC), ('Loader', 'C# Windows · MCP', CTL), ('Documentación', 'fichas AFR2 · CHANGELOG', T2)]
for i, (t, s, c) in enumerate(mods):
    a = math.radians(90 - i * 60); x = 9.6 + 6.4 * math.cos(a); y = 5.3 + 3.6 * math.sin(a)
    box(ax, x - 2.2, y - 0.8, 4.4, 1.6, t, c, s, fs=20, fill=PANEL)
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


# ======================================================================================
# Mecánica v4: cargas del brazo, esfuerzos internos, fórmulas anotadas y vuelco esquemático
import textwrap
from matplotlib.patches import Arc, Ellipse


def eq_annot(name, tokens, w=19.2, h=4.2, fs=44, y0=None, rows=2, lfs=15, wrap=None):
    """tokens: (tex, etiqueta|None). Las etiquetas quedan debajo, con flecha al símbolo."""
    fig, ax = canvas(w, h)
    r = fig.canvas.get_renderer()
    y0 = h - 1.15 if y0 is None else y0
    items, x = [], 0.0
    for tex, lab in tokens:
        t = ax.text(x, y0, tex, fontsize=fs, color=TX, va='center', ha='left')
        bb = t.get_window_extent(r).transformed(ax.transData.inverted())
        items.append((t, bb, lab)); x = bb.x1 + 0.22
    total = x - 0.22; off = (w - total) / 2
    for t, bb, lab in items:
        t.set_x(t.get_position()[0] + off)
    labs = [(bb.x0 + off, bb.x1 + off, bb.y0, lab) for _, bb, lab in items if lab]
    k = len(labs)
    if wrap is None:  # caracteres que entran en el ancho de cada etiqueta (1 unidad = 48 pt)
        wrap = max(10, int(((w - 0.8) / k - 0.35) / (lfs * 0.56 / 48)))
    for i, (xa, xb, yb, lab) in enumerate(labs):
        cx = (xa + xb) / 2
        lx = 0.4 + (w - 0.8) * (i + 0.5) / k
        ly = 1.35 if (i % rows == 0 or rows == 1) else 0.55
        if rows == 1: ly = 1.75
        txt = '\n'.join(textwrap.wrap(lab, wrap))
        ax.text(lx, ly, txt, ha='center', va='top', fontsize=lfs, color=TX, linespacing=1.15,
                bbox=dict(boxstyle='round,pad=0.35,rounding_size=0.3', fc='#FFFFFFE8', ec=MEC + '66', lw=1.2))
        ax.annotate('', (cx, yb - 0.05), (lx, ly + 0.12), arrowprops=dict(arrowstyle='-|>', color=MECd, lw=1.5, mutation_scale=14,
                    connectionstyle='arc3,rad=0.0', shrinkA=2, shrinkB=2))
    save(fig, name)


eq_annot('EQ_momento_vectorial', [(r'$\mathbf{M}$', 'Momento: tendencia a hacer girar (N·m)'), (r'$=$', None),
         (r'$\mathbf{r}$', 'Vector desde el punto de giro hasta donde actúa la fuerza (m)'), (r'$\times$', 'Producto vectorial: M sale perpendicular a r y a F'),
         (r'$\mathbf{F}$', 'Fuerza aplicada (N)')], w=13, h=4.6, rows=1)
eq_annot('EQ_momento_magnitud', [(r'$|\mathbf{M}|$', 'Magnitud del momento (N·m)'), (r'$=$', None), (r'$F$', 'Módulo de la fuerza (N)'), (r'$\cdot$', None),
         (r'$d$', 'Brazo de palanca: distancia perpendicular del eje a la línea de la fuerza (m)')], w=13, h=4.6, rows=1)
eq_annot('EQ_centro_masa', [(r'$\mathbf{r}_G$', 'Posición del centro de masa del conjunto'), (r'$=$', None),
         (r'$\dfrac{\sum\, m_i\,\mathbf{r}_i}{\sum\, m_i}$', 'Promedio de las posiciones r_i de cada pieza, pesado por su masa m_i')], w=12, h=4.8, rows=1)
eq_annot('EQ_inercia', [(r'$J$', 'Momento de inercia: cuánto le cuesta girar'), (r'$=$', None), (r'$\int$', 'Suma sobre toda la masa del cuerpo'),
         (r'$r_\perp^{2}$', 'Distancia de cada punto al eje, al cuadrado'), (r'$dm$', 'Pedacito de masa')], w=12, h=4.8, rows=1)
eq_annot('EQ_dalembert', [(r'$\mathbf{F}_i$', 'Fuerza de inercia equivalente (D’Alembert)'), (r'$=$', None), (r'$-$', 'Opuesta a la aceleración'),
         (r'$m$', 'Masa del conjunto (kg)'), (r'$\mathbf{a}$', 'Aceleración del auto al arrancar o frenar')], w=12, h=4.8, rows=1)
eq_annot('EQ_vuelco', [(r'$m\,g\,e$', 'Momento estabilizador: peso × distancia del CG al borde de apoyo'), (r'$\geq$', 'No vuelca mientras se cumpla'),
         (r'$P\,d$', 'Momento volcador de la carga P en la pinza, a distancia d del borde'), (r'$+$', None),
         (r'$m\,a\,h_G$', 'Momento de la fuerza de inercia: altura h_G del CG × m·a')], w=16, h=4.8, rows=1)

# ---------- Brazo: cargas en sus centros de masa y momento flector (análisis estático 200 g, pose horizontal, v06)
fig, ax = canvas(19.2, 9.6)
J = [('J2\nhombro', 0), ('J3\ncodo', 130), ('J4', 250), ('J5', 305), ('J6', 370), ('Carga', 457)]
Mg = [(390, 70, 'Eslabón 1\n+ motor codo'), (254, 178, 'Eslabón 2\n+ motor J4'), (194, 291, 'Giro J4\n+ J5'), (88, 326, 'Núcleo J5'), (89, 395, 'Palma\n+ servo'), (200, 457, 'Carga\n200 g')]
X = lambda mm: 1.2 + mm / 457 * 16.6
yb = 5.55
ax.plot([X(0), X(457)], [yb, yb], color='#475569', lw=7, solid_capstyle='round', zorder=2)
for t, mm in J:
    ax.add_patch(Circle((X(mm), yb), 0.17, fc=TX, ec='white', lw=1.5, zorder=3))
    ax.text(X(mm), yb - 0.45, t, ha='center', va='top', fontsize=12.5, color=T2, linespacing=1.05)
for g, mm, lab in Mg:
    L = 0.55 + g / 390 * 1.85
    ax.annotate('', (X(mm), yb + 0.2), (X(mm), yb + 0.2 + L), arrowprops=dict(arrowstyle='-|>', color='#DC2626', lw=2.6, mutation_scale=20), zorder=4)
    ax.text(X(mm), yb + 0.35 + L, f'{g} g', ha='center', va='bottom', fontsize=13, weight='bold', color='#B91C1C')
    ax.text(X(mm) + 0.12, yb + 0.25 + L * 0.45, lab, ha='left', va='center', fontsize=10, color=T2, linespacing=1.05)
ax.text(0.3, 9.25, 'A · Cargas: cada flecha es el peso de un tramo, aplicado en su centro de masa', fontsize=15, weight='bold', color=MECd)
# momento flector aproximado M(x) = Σ_{x_i > x} m_i g (x_i − x)
import numpy as np
xs = np.linspace(0, 457, 400)
Mx = np.array([sum(g / 1000 * 9.80665 * (mm - x) / 1000 for g, mm, _ in Mg if mm > x) for x in xs])
y0, sy = 0.5, 3.0 / max(Mx)
ax.fill_between([X(x) for x in xs], y0, y0 + Mx * sy, color=MEC + '33', lw=0)
ax.plot([X(x) for x in xs], y0 + Mx * sy, color=MECd, lw=2.6)
ax.plot([X(0), X(457)], [y0, y0], color='#94A3B8', lw=1.2)
ax.text(0.3, 4.15, 'B · Momento flector M(x): suma de cada peso × su distancia a la sección', fontsize=15, weight='bold', color=MECd)
ax.annotate(f'≈ {Mx[0]:.1f} N·m en el hombro (J2):'.replace('.', ',') + '\nla sección más exigida', (X(0), y0 + Mx[0] * sy), (X(70), y0 + Mx[0] * sy - 0.1),
            fontsize=12.5, color=TX, va='center', arrowprops=dict(arrowstyle='-|>', color=TX, lw=1.4))
ax.text(X(457), y0 + 0.25, 'M = 0 en la punta', ha='right', fontsize=11.5, color=T2)
save(fig, 'F_brazo_cargas_momento_v001')

# ---------- Esfuerzos internos en una sección: N, V, M, T
fig, ax = canvas(19.2, 8.4)
# tramo de tubo cortado
ax.add_patch(Rectangle((1.2, 3.2), 5.6, 1.8, fc=SUB, ec='#475569', lw=2, zorder=2))
ax.add_patch(Ellipse((6.8, 4.1), 0.9, 1.8, fc=MEC + '55', ec=MECd, lw=2, zorder=3))
ax.add_patch(Ellipse((1.2, 4.1), 0.9, 1.8, fc=SUB, ec='#475569', lw=2, zorder=1))
ax.annotate('sección cortada', (6.5, 4.9), (4.2, 6.1), fontsize=12.5, color=MECd, ha='center',
            arrowprops=dict(arrowstyle='-|>', color=MECd, lw=1.4))
ax.annotate('', (9.1, 4.1), (6.9, 4.1), arrowprops=dict(arrowstyle='-|>', color='#DC2626', lw=3.2, mutation_scale=24), zorder=5)
ax.text(9.25, 4.1, 'N', fontsize=24, weight='bold', color='#DC2626', va='center')
ax.annotate('', (6.8, 6.7), (6.8, 5.05), arrowprops=dict(arrowstyle='-|>', color=ELEd, lw=3.2, mutation_scale=24), zorder=5)
ax.text(6.95, 6.65, 'V', fontsize=24, weight='bold', color=ELEd, va='center')
ax.add_patch(Arc((6.8, 4.1), 3.4, 3.4, theta1=200, theta2=330, color=CTLd, lw=3))
ax.annotate('', (8.25, 3.2), (8.05, 2.85), arrowprops=dict(arrowstyle='-|>', color=CTLd, lw=3, mutation_scale=22))
ax.text(8.5, 2.75, 'M', fontsize=24, weight='bold', color=CTLd, va='center')
ax.add_patch(Arc((7.9, 4.1), 0.9, 2.6, theta1=-70, theta2=250, color=PRGd, lw=3))
ax.annotate('', (7.75, 2.85), (7.95, 2.81), arrowprops=dict(arrowstyle='-|>', color=PRGd, lw=3, mutation_scale=20))
ax.text(7.55, 5.75, 'T', fontsize=24, weight='bold', color=PRGd, va='center')
rows = [('N', 'Axial', 'estira o comprime a lo largo del eje', 'peso que baja por la torre hacia J1', '#DC2626'),
        ('V', 'Cortante', 'intenta deslizar una cara sobre la otra', 'peso de cada tramo, perpendicular al tubo', ELEd),
        ('M', 'Flector', 'dobla el tramo: tracción arriba, compresión abajo', 'crece hacia el hombro: J2 y el primer eslabón', CTLd),
        ('T', 'Torsor', 'retuerce el tramo alrededor de su eje', 'carga fuera del plano del brazo, giros de muñeca', PRGd)]
for i, (k, t, qh, dn, c) in enumerate(rows):
    y = 7.3 - i * 1.75
    ax.add_patch(FancyBboxPatch((10.6, y - 1.35), 8.3, 1.5, boxstyle='round,pad=0,rounding_size=0.3', fc='#FFFFFFE6', ec=c, lw=1.6))
    ax.text(10.95, y - 0.6, k, fontsize=26, weight='bold', color=c, va='center')
    ax.text(11.8, y - 0.25, t, fontsize=15, weight='bold', color=TX, va='center')
    ax.text(11.8, y - 0.68, qh, fontsize=12, color=TX, va='center')
    ax.text(11.8, y - 1.08, 'En el brazo: ' + dn, fontsize=11.5, color=T2, va='center')
ax.text(0.4, 1.3, 'Fuerzas (N): N axial y V cortante · Momentos (N·m): M flector y T torsor.\nNo se suman entre sí: cada uno se dimensiona por separado y después se combinan.', fontsize=13, color=T2, va='center', linespacing=1.4)
save(fig, 'F_esfuerzos_seccion_v001')

# ---------- Vuelco esquemático (lateral y frontal) con las cargas sobre el brazo
fig, ax = canvas(19.2, 9.0)
def car(x0, w, label):
    ax.add_patch(FancyBboxPatch((x0, 2.0), w, 1.0, boxstyle='round,pad=0,rounding_size=0.15', fc=SUB, ec='#475569', lw=2))
    for xw in (x0 + 0.55, x0 + w - 0.55):
        ax.add_patch(Circle((xw, 1.55), 0.55, fc='#64748B', ec='#334155', lw=2))
    ax.plot([0.2 + x0 - 0.4, x0 + w + 0.4], [1.0, 1.0], color='#94A3B8', lw=2)
    ax.text(x0 + w / 2, 8.0, label, ha='center', fontsize=14, weight='bold', color=T2)
# lateral: torre + brazo estirado hacia adelante (+x)
car(1.0, 5.2, 'Vista lateral · vuelco hacia adelante')
ax.add_patch(Rectangle((2.3, 3.0), 0.7, 2.6, fc='#E2E8F0', ec='#475569', lw=2))
ax.plot([2.65, 6.6, 8.9], [5.6, 6.3, 6.0], color='#475569', lw=6, solid_capstyle='round')
ax.add_patch(Circle((2.65, 5.6), 0.2, fc=TX)); ax.add_patch(Circle((6.6, 6.3), 0.16, fc=TX))
ax.annotate('', (8.9, 4.4), (8.9, 5.85), arrowprops=dict(arrowstyle='-|>', color='#DC2626', lw=3, mutation_scale=22)); ax.text(9.05, 5.0, 'P\ncarga', fontsize=13, weight='bold', color='#DC2626')
ax.annotate('', (2.65, 3.15), (2.65, 4.4), arrowprops=dict(arrowstyle='-|>', color=ELEd, lw=3, mutation_scale=22)); ax.text(1.0, 4.2, 'N axial\nen la torre', fontsize=12.5, weight='bold', color=ELEd)
ax.add_patch(Circle((3.9, 3.1), 0.13, fc=MECd)); ax.annotate('', (3.9, 1.75), (3.9, 3.0), arrowprops=dict(arrowstyle='-|>', color=MECd, lw=3, mutation_scale=22)); ax.text(4.05, 2.35, 'm g', fontsize=13, weight='bold', color=MECd)
ax.annotate('', (5.6, 3.1), (3.95, 3.1), arrowprops=dict(arrowstyle='-|>', color=CTLd, lw=2.6, mutation_scale=20)); ax.text(4.3, 3.3, 'm a (frenada)', fontsize=12, color=CTLd)
ax.add_patch(Circle((5.65, 1.0), 0.13, fc='#DC2626')); ax.text(5.65, 0.7, 'borde de vuelco', ha='center', fontsize=12, color='#B91C1C', va='top')
ax.add_patch(Arc((5.65, 1.0), 2.4, 2.4, theta1=20, theta2=80, color='#DC2626', lw=2.4, ls='--'))
# frontal: brazo girado hacia el costado → momento de giro sobre J1 y vuelco lateral
car(11.3, 4.4, 'Vista frontal · giro del brazo al costado')
ax.add_patch(Rectangle((13.15, 3.0), 0.7, 2.6, fc='#E2E8F0', ec='#475569', lw=2))
ax.plot([13.5, 16.4, 18.2], [5.6, 6.2, 5.9], color='#475569', lw=6, solid_capstyle='round')
ax.annotate('', (18.2, 4.3), (18.2, 5.75), arrowprops=dict(arrowstyle='-|>', color='#DC2626', lw=3, mutation_scale=22)); ax.text(17.3, 4.6, 'P', fontsize=13, weight='bold', color='#DC2626')
ax.add_patch(Arc((13.5, 6.4), 1.8, 0.7, theta1=200, theta2=520 - 180, color=PRGd, lw=2.8))
ax.annotate('', (14.35, 6.25), (14.2, 6.15), arrowprops=dict(arrowstyle='-|>', color=PRGd, lw=2.8, mutation_scale=20)); ax.text(12.2, 7.0, 'M de giro sobre J1\n(T en la torre)', fontsize=12.5, weight='bold', color=PRGd)
ax.add_patch(Circle((15.15, 1.0), 0.13, fc='#DC2626')); ax.text(15.15, 0.7, 'borde de vuelco lateral', ha='center', fontsize=12, color='#B91C1C', va='top')
ax.text(9.6, 8.75, 'Esquema didáctico: proporciones ilustrativas, sin valores medidos', ha='center', fontsize=12, color=T2)
save(fig, 'F_vuelco_esquema_v001')
