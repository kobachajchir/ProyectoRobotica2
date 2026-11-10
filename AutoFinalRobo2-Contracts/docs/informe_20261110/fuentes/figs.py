"""Figuras propias del informe D2. Solo geometria/modelos; no representan ensayos."""
import numpy as np, matplotlib, json, sys
matplotlib.use('Agg'); import matplotlib.pyplot as plt
from matplotlib.patches import FancyBboxPatch, FancyArrowPatch, Rectangle
from numpy import sin, cos
OUT = sys.argv[1]
plt.rcParams.update({'font.family': 'Arial', 'font.size': 9, 'axes.spines.top': False, 'axes.spines.right': False})
INK = '#1f2933'; ACC = '#9b1b30'; BLUE = '#2b6cb0'; GREEN = '#2f855a'; GREY = '#718096'; LIGHT = '#edf2f7'


def box(ax, x, y, w, h, t, fc=LIGHT, ec=INK, fs=8):
    ax.add_patch(FancyBboxPatch((x, y), w, h, boxstyle='round,pad=0.02,rounding_size=0.08', fc=fc, ec=ec, lw=1))
    ax.text(x + w / 2, y + h / 2, t, ha='center', va='center', fontsize=fs, color=INK)


def arr(ax, x1, y1, x2, y2, t='', c=INK, both=False, off=(0, 0.12)):
    ax.add_patch(FancyArrowPatch((x1, y1), (x2, y2), arrowstyle='<|-|>' if both else '-|>', mutation_scale=10, color=c, lw=1.1))
    if t:
        ax.text((x1 + x2) / 2 + off[0], (y1 + y2) / 2 + off[1], t, ha='center', va='bottom', fontsize=7, color=c)


# 1. Arquitectura
fig, ax = plt.subplots(figsize=(7.2, 4.0)); ax.set_xlim(0, 12); ax.set_ylim(0, 7); ax.axis('off')
box(ax, 0.2, 5.4, 2.4, 1.1, 'Operador\nnavegador / móvil')
box(ax, 3.4, 5.4, 2.8, 1.1, 'Web React/Vite\n(servida desde LittleFS)', fc='#e6f0fa')
box(ax, 7.2, 5.4, 2.6, 1.1, 'Loader Windows (C#)\ncompilación y carga', fc='#f3eefc')
box(ax, 3.4, 3.3, 2.8, 1.2, 'ESP-12S / ESP8266\nWi-Fi AP/STA · HTTP · WS\nbroker y cola', fc='#e6f0fa')
box(ax, 3.4, 1.0, 2.8, 1.4, 'ATmega2560 (C/avr-libc)\ncontrol 100 Hz · odometría\nGOTO · línea · fallas · UI', fc='#fbeaec')
box(ax, 0.1, 1.0, 2.4, 1.4, 'Panel local\nOLED 0,96" I²C\nencoder KY-040')
box(ax, 7.2, 2.6, 2.6, 1.0, 'Model Y (2 × puente H)\n4 × GM25-370')
box(ax, 7.2, 1.2, 2.6, 1.0, 'Sensores: 4 Hall A/B, 5 IR\nHC-SR04 + servo, batería')
box(ax, 7.2, 0.0, 2.6, 0.9, 'Brazo 6 ejes (bus STS TTL)\nintegración pendiente', fc='#fffbea', ec=GREY)
box(ax, 10.3, 3.2, 1.6, 2.6, 'Contracts\nwire_catalog\nmotion_ref\ngeometría\nversión única', fc='#f0fff4', fs=7.5)
arr(ax, 2.6, 5.95, 3.4, 5.95, 'HTTP/WS', both=True)
arr(ax, 4.8, 5.4, 4.8, 4.5, 'JSON afr2-ws-1', both=True, off=(0.9, -0.05))
arr(ax, 4.8, 3.3, 4.8, 2.4, 'UART ROBO2\nUSART1 D18/D19', both=True, off=(-0.85, -0.25))
arr(ax, 2.5, 1.7, 3.4, 1.7, 'I²C / IRQ', both=True)
arr(ax, 6.2, 2.0, 7.2, 3.1, 'PWM/dir', off=(0.45, -0.2))
arr(ax, 7.2, 1.7, 6.2, 1.7, 'pulsos/ADC', off=(0, 0.05))
arr(ax, 6.2, 1.2, 7.2, 0.45, '', c=GREY)
arr(ax, 7.2, 5.6, 6.2, 3.9, 'USB / flasheo', c='#6b46c1', off=(0.75, -0.1))
fig.tight_layout(); fig.savefig(OUT + '/fig_arquitectura.png', dpi=220); plt.close(fig)


# 2. Cadena cinematica, vista lateral
def chain(q):
    q1, q2, q3, q4, q5, q6 = np.radians(q)
    p = np.array([0, 0.209]); P = [p.copy()]
    a = q2; p = p + 0.070 * np.array([sin(a), cos(a)]); P.append(p.copy())
    a = q2 + q3; p = p + 0.115 * np.array([sin(a), cos(a)]); P.append(p.copy())
    a = q2 + q3 + q5; pt = p + 0.1435 * np.array([sin(a), cos(a)])
    return P, pt


fig, axs = plt.subplots(1, 2, figsize=(7.2, 3.6), gridspec_kw={'width_ratios': [1, 1.25]})
ax = axs[0]; ax.set_aspect('equal'); ax.set_xlim(-0.13, 0.17); ax.set_ylim(-0.01, 0.58)
zs = [0, 0.096, 0.209, 0.279, 0.339, 0.394, 0.459, 0.5375]
ax.plot([0] * len(zs), zs, color=INK, lw=2.2, marker='o', ms=4, mfc='white')
for z, t in [(0.096, 'J1  (eje z)'), (0.209, 'J2  (eje y)'), (0.279, 'J3  (eje y)'), (0.339, 'J4  (eje z)'),
             (0.394, 'J5  (eje y)'), (0.459, 'J6  (eje z)'), (0.5375, 'TCP')]:
    ax.text(0.012, z, t, va='center', fontsize=7.5)
for a, b, t in [(0, 0.096, '96'), (0.096, 0.209, '113'), (0.209, 0.279, '70 (L1)'), (0.279, 0.339, '60'),
                (0.339, 0.394, '55'), (0.394, 0.459, '65'), (0.459, 0.5375, '78,5')]:
    ax.annotate('', (-0.035, a), (-0.035, b), arrowprops=dict(arrowstyle='<->', color=ACC, lw=0.8))
    ax.text(-0.04, (a + b) / 2, t, ha='right', va='center', fontsize=7, color=ACC)
ax.text(-0.125, 0.575, 'q = 0: eslabones verticales; cotas en mm', fontsize=7, color=GREY, va='top')
ax.set_xlabel('x [m]'); ax.set_ylabel('z [m] (marco de montura)')
ax = axs[1]; ax.set_aspect('equal'); ax.set_xlim(-0.05, 0.36); ax.set_ylim(-0.01, 0.56)
for q, c, lab in [([0, 15, -40, 0, 25, 0], GREY, 'transporte [0, 15, −40, 0, 25, 0]°'),
                  ([0, 60, 30, 0, 40, 0], BLUE, 'ejemplo [0, 60, 30, 0, 40, 0]°'),
                  ([0, 90, 0, 0, 0, 0], ACC, 'extendido [0, 90, 0, 0, 0, 0]°')]:
    P, pt = chain(q)
    xs = [0, 0] + [p[0] for p in P] + [pt[0]]; zz = [0, 0.096] + [p[1] for p in P] + [pt[1]]
    ax.plot(xs, zz, color=c, lw=2, marker='o', ms=3.5, mfc='white', label=lab)
ax.legend(fontsize=7, loc='upper right', frameon=False); ax.set_xlabel('x [m]'); ax.set_ylabel('z [m]')
ax.text(0.33, 0.185, 'alcance 328,5 mm desde J2', fontsize=7, ha='right', color=ACC)
fig.tight_layout(); fig.savefig(OUT + '/fig_cadena.png', dpi=220); plt.close(fig)

# 3. Mecanum
L = 0.170
Am = np.array([[1, -1, -L], [1, 1, L], [1, 1, -L], [1, -1, L]])
pos = {'FL': (0.0622, 0.10764), 'FR': (0.0622, -0.10764), 'RL': (-0.0622, 0.10764), 'RR': (-0.0622, -0.10764)}
roll = {'FL': -1, 'FR': 1, 'RL': 1, 'RR': -1}
fig, axs = plt.subplots(1, 3, figsize=(7.2, 2.9))
for ax, (t, V) in zip(axs, [('Avance  vx = 0,15 m/s', [0.15, 0, 0]), ('Lateral  vy = 0,15 m/s', [0, 0.15, 0]),
                             ('Giro  ω = 0,5 rad/s', [0, 0, 0.5])]):
    u = Am @ np.array(V); ax.set_aspect('equal'); ax.axis('off'); ax.set_xlim(-0.2, 0.2); ax.set_ylim(-0.2, 0.2)
    ax.add_patch(Rectangle((-0.085, -0.09), 0.17, 0.18, fc='#f7fafc', ec=INK, lw=0.8))
    for i, k in enumerate(['FL', 'FR', 'RL', 'RR']):
        x, y = pos[k]; X, Y = -y, x
        ax.add_patch(Rectangle((X - 0.016, Y - 0.035), 0.032, 0.07, fc='#4a5568', ec=INK))
        s = roll[k]
        for dd in (-0.02, 0, 0.02):
            ax.plot([X - 0.012, X + 0.012], [Y + dd - 0.012 * s, Y + dd + 0.012 * s], color='white', lw=1)
        if abs(u[i]) > 1e-9:
            ax.add_patch(FancyArrowPatch((X, Y), (X, Y + u[i] * 0.35), arrowstyle='-|>', mutation_scale=8, color=ACC, lw=1.4))
        ax.text(X + (0.035 if X > 0 else -0.035), Y - 0.055, f'{k}\n{u[i] * 1000:+.0f}', ha='center', fontsize=6.5)
    ax.add_patch(FancyArrowPatch((0, 0), (0, 0.06), arrowstyle='-|>', mutation_scale=8, color=BLUE)); ax.text(0.005, 0.065, 'x', color=BLUE, fontsize=7)
    ax.add_patch(FancyArrowPatch((0, 0), (-0.06, 0), arrowstyle='-|>', mutation_scale=8, color=BLUE)); ax.text(-0.07, 0.008, 'y', color=BLUE, fontsize=7)
    ax.set_title(t, fontsize=9)
fig.text(0.5, 0.02, 'u_i = (A·V)_i en mm/s, base geométrica canónica (antes de S). 2·lx = 124,4 mm; 2·ly = 215,3 mm; L = lx + ly = 170 mm.',
         ha='center', fontsize=7.5, color=GREY)
fig.tight_layout(rect=(0, 0.06, 1, 1)); fig.savefig(OUT + '/fig_mecanum.png', dpi=220); plt.close(fig)

# 4. Diagrama de control
fig, ax = plt.subplots(figsize=(7.2, 2.7)); ax.set_xlim(0, 14); ax.set_ylim(0, 5); ax.axis('off')
box(ax, 0.1, 2.6, 1.7, 1.0, 'Consigna\nWeb/OLED\nGOTO/línea', fs=7)
box(ax, 2.3, 2.6, 1.9, 1.0, 'Ley de pose\nP + saturación\n(GOTO)', fc='#e6f0fa', fs=7)
box(ax, 4.7, 2.6, 1.9, 1.0, 'Mezclador\nMecanum\nu = S·A·V', fc='#e6f0fa', fs=7)
box(ax, 7.1, 2.6, 1.9, 1.0, 'PID por rueda\nQ8 · 100 Hz\nsat + rampa', fc='#fbeaec', fs=7)
box(ax, 9.5, 2.6, 1.9, 1.0, 'Model Y +\nGM25-370\n(4 ruedas)', fs=7)
box(ax, 11.9, 2.6, 1.9, 1.0, 'Chasis\n(x, y, ψ)', fs=7)
for a, b in [(1.8, 2.3), (4.2, 4.7), (6.6, 7.1), (9.0, 9.5), (11.4, 11.9)]:
    arr(ax, a, 3.1, b, 3.1)
for x, t in [(2.05, 'V*'), (4.45, 'V'), (6.85, 'u_ref'), (9.25, 'PWM')]:
    ax.text(x, 3.75, t, fontsize=7, ha='center', color=BLUE)
box(ax, 9.5, 0.4, 1.9, 1.0, 'Hall A/B × 4\n370 cuentas/vuelta', fs=7)
box(ax, 4.7, 0.4, 1.9, 1.0, 'Odometría\npunto medio', fc='#e6f0fa', fs=7)
arr(ax, 10.45, 2.6, 10.45, 1.4); arr(ax, 9.5, 0.9, 6.6, 0.9, 'Δn_i', off=(0, 0.05))
ax.add_patch(FancyArrowPatch((9.5, 1.15), (8.05, 2.6), arrowstyle='-|>', mutation_scale=9, color=INK, connectionstyle='arc3,rad=0.2'))
ax.text(8.35, 1.6, 'u_med', fontsize=7)
ax.add_patch(FancyArrowPatch((4.7, 0.9), (3.25, 2.6), arrowstyle='-|>', mutation_scale=9, color=INK, connectionstyle='arc3,rad=-0.2'))
ax.text(3.0, 1.5, '(x̂, ŷ, ψ̂)', fontsize=7)
ax.text(7.0, 4.55, 'Supervisión: F1 control tardío > 100 ms · F2 odometría inválida · F3 sin pulsos Hall 500 ms · F4 signo PWM/Hall cruzado · Web sin pulso 1,5 s → STOP',
        ha='center', fontsize=6.6, color=ACC)
fig.tight_layout(); fig.savefig(OUT + '/fig_control.png', dpi=220); plt.close(fig)


# 5. Simulacion GOTO: replica la ley de afr2_product.c sobre cinematica ideal
def clamp(x, l):
    return max(-l, min(l, x))


def sim(tx, ty, th=0, speed=150, R=170, tau=0.0, T=60):
    x = y = yaw = 0.0; dt = 0.01
    mx, my, myaw = tx, ty, th
    vxa = vya = wza = 0.0; log = []
    for k in range(int(T / dt)):
        ex = mx - x; ey = my - y; eyaw = myaw - yaw * 1000
        pure = (tx == 0 and ty == 0)
        if ((abs(ex) <= 10 and abs(ey) <= 10) or pure) and abs(eyaw) <= 25:
            break
        a = yaw
        vx = clamp(ex * cos(a) + ey * sin(a), speed); vy = clamp(-ex * sin(a) + ey * cos(a), speed)
        wz = clamp(eyaw, min(speed * 1000 / R, 3140))
        if (abs(ex) <= 10 and abs(ey) <= 10) or pure:
            vx = vy = 0
        mn = min(speed, 40); lin = abs(vx) + abs(vy)
        if lin and lin < mn:
            vx = vx * mn / lin; vy = vy * mn / lin
        wmin = mn * 1000 / R
        if not lin and abs(eyaw) > 25 and abs(wz) < wmin:
            wz = wmin if eyaw > 0 else -wmin
        peak = abs(vx) + abs(vy) + abs(wz) * R / 1000
        if peak > speed:
            vx *= speed / peak; vy *= speed / peak; wz *= speed / peak
        if tau > 0:
            al = dt / (tau + dt); vxa += al * (vx - vxa); vya += al * (vy - vya); wza += al * (wz - wza)
        else:
            vxa, vya, wza = vx, vy, wz
        ang = wza / 1000 * dt; hd = yaw + ang / 2
        x += (vxa * cos(hd) - vya * sin(hd)) * dt; y += (vxa * sin(hd) + vya * cos(hd)) * dt; yaw += ang
        log.append((k * dt, x, y, yaw, ex, ey, vx, vy, wz))
    return np.array(log)


res = {}
fig, axs = plt.subplots(1, 2, figsize=(7.2, 3.6), gridspec_kw={'width_ratios': [1.1, 1]})
ax = axs[0]; ax.set_aspect('equal')
for (tx, ty), lab, c in [((500, 0), 'avance 500 mm', INK), ((0, 400), 'lateral 400 mm', BLUE), ((400, 300), 'diagonal (400; 300) mm', ACC)]:
    for tau, ls in [(0.0, '-'), (0.15, ':')]:
        lg = sim(tx, ty, tau=tau)
        ax.plot(lg[:, 2], lg[:, 1], color=c, ls=ls, lw=1.4, label=lab if tau == 0 else None)
        res[f'{lab} | tau={tau}'] = dict(t_s=round(float(lg[-1, 0]), 2), x_mm=round(float(lg[-1, 1]), 1),
                                         y_mm=round(float(lg[-1, 2]), 1), err_mm=round(float(np.hypot(tx - lg[-1, 1], ty - lg[-1, 2])), 1))
    ax.plot(ty, tx, 'x', color=c)
ax.invert_xaxis(); ax.set_xlabel('y [mm] (izquierda +)'); ax.set_ylabel('x [mm] (adelante +)')
ax.legend(fontsize=7, frameon=False, loc='upper center', bbox_to_anchor=(0.5, -0.22), ncol=2); ax.set_title('Trayectoria en planta', fontsize=8)
ax = axs[1]
lg = sim(400, 300); ax.plot(lg[:, 0], np.hypot(lg[:, 4], lg[:, 5]), color=ACC, label='|e| ideal')
lg2 = sim(400, 300, tau=0.15); ax.plot(lg2[:, 0], np.hypot(lg2[:, 4], lg2[:, 5]), color=ACC, ls=':', label='|e| con retardo τ = 0,15 s')
ax.axhline(10, color=GREY, lw=0.8, ls='--'); ax.text(0.2, 16, 'tolerancia ±10 mm por eje', fontsize=6.5, color=GREY)
ax.set_xlabel('t [s]'); ax.set_ylabel('error de posición [mm]'); ax.legend(fontsize=6.5, frameon=False)
ax.set_title('Caso diagonal, velocidad de misión 150 mm/s', fontsize=8)
fig.tight_layout(); fig.savefig(OUT + '/fig_goto.png', dpi=220); plt.close(fig)
for tau in (0.0, 0.15):
    lg = sim(0, 0, th=1571, tau=tau)
    res[f'giro 90° | tau={tau}'] = dict(t_s=round(float(lg[-1, 0]), 2), yaw_deg=round(float(np.degrees(lg[-1, 3])), 2))
json.dump(res, open(OUT + '/goto_resultados.json', 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
print(json.dumps(res, indent=1, ensure_ascii=False))
