#!/usr/bin/env python3
"""Genera los diagramas SVG de la presentación AutoFinalRobo2 (carpeta presentacion/).

Solo escribe archivos .svg junto a este script. No toca firmware, Web ni hardware.
Cada diagrama es un SVG de fondo blanco, apto para insertar en PowerPoint
(Insertar > Imágenes) o abrir en cualquier navegador. Los textos son editables
si se abre el SVG en Inkscape/PowerPoint ("Convertir en forma").

Uso:  python generar_diagramas.py
"""
import html
import os

OUT = os.path.dirname(os.path.abspath(__file__))
FONT = "Segoe UI, Arial, sans-serif"
INK = "#1f2430"
LINE = "#4a5163"
ACC = "#b81d33"      # carmesí del sistema visual del proyecto
KINDS = {            # relleno, trazo
    "proc": ("#f3f5f9", "#4a5163"),
    "term": ("#1f2430", "#1f2430"),
    "dec":  ("#fff4d6", "#b9831c"),
    "io":   ("#e4f0ff", "#2d6cb5"),
    "bad":  ("#fde7ea", "#b81d33"),
    "ok":   ("#e3f5ea", "#2a8a52"),
    "note": ("#ffffff", "#9aa1b2"),
    "hw":   ("#efe9ff", "#6a4bb5"),
}
COLW, ROWH = 250, 96


def esc(t):
    return html.escape(str(t), quote=False)


class Diagram:
    def __init__(self, name, title, cols, rows, colw=COLW, rowh=ROWH):
        self.name, self.title = name, title
        self.cols, self.rows, self.colw, self.rowh = cols, rows, colw, rowh
        self.nodes, self.parts, self.m = {}, [], 40
        self.top = 70
        self.w = cols * colw + 2 * self.m + 60
        self.h = rows * rowh + self.top + 50

    # --- posiciones -------------------------------------------------------
    def center(self, col, row):
        return (self.m + col * self.colw + self.colw / 2,
                self.top + row * self.rowh + self.rowh / 2)

    def node(self, nid, col, row, kind, text, w=None, h=None, dx=0, dy=0):
        w = w or getattr(self, 'defw', self.colw - 30)
        h = h or 58
        cx, cy = self.center(col, row)
        self.nodes[nid] = dict(cx=cx + dx, cy=cy + dy, w=w, h=h, kind=kind, text=text)

    # --- dibujo -----------------------------------------------------------
    def _text(self, cx, cy, text, color=INK, size=13, weight="400"):
        lines = str(text).split("\n")
        y0 = cy - (len(lines) - 1) * 8 + 5
        out = []
        for i, ln in enumerate(lines):
            out.append(
                f'<text x="{cx:.1f}" y="{y0 + i * 16:.1f}" text-anchor="middle" '
                f'font-family="{FONT}" font-size="{size}" font-weight="{weight}" fill="{color}">{esc(ln)}</text>')
        return "".join(out)

    def _draw_node(self, n):
        fill, stroke = KINDS[n["kind"]]
        cx, cy, w, h = n["cx"], n["cy"], n["w"], n["h"]
        tcol = "#ffffff" if n["kind"] == "term" else INK
        if n["kind"] == "dec":
            pts = f'{cx},{cy - h / 2 - 8} {cx + w / 2 + 8},{cy} {cx},{cy + h / 2 + 8} {cx - w / 2 - 8},{cy}'
            shape = f'<polygon points="{pts}" fill="{fill}" stroke="{stroke}" stroke-width="1.6"/>'
        elif n["kind"] == "term":
            shape = f'<rect x="{cx - w / 2}" y="{cy - h / 2}" width="{w}" height="{h}" rx="{h / 2}" fill="{fill}" stroke="{stroke}"/>'
        elif n["kind"] == "note":
            shape = f'<rect x="{cx - w / 2}" y="{cy - h / 2}" width="{w}" height="{h}" rx="4" fill="{fill}" stroke="{stroke}" stroke-dasharray="5 3"/>'
        else:
            shape = f'<rect x="{cx - w / 2}" y="{cy - h / 2}" width="{w}" height="{h}" rx="8" fill="{fill}" stroke="{stroke}" stroke-width="1.6"/>'
        return shape + self._text(cx, cy, n["text"], tcol)

    def _anchor(self, nid, side):
        n = self.nodes[nid]
        extra = 8 if n["kind"] == "dec" else 0
        return {
            "b": (n["cx"], n["cy"] + n["h"] / 2 + extra),
            "t": (n["cx"], n["cy"] - n["h"] / 2 - extra),
            "r": (n["cx"] + n["w"] / 2 + extra, n["cy"]),
            "l": (n["cx"] - n["w"] / 2 - extra, n["cy"]),
        }[side]

    def edge(self, a, b, label="", sa="b", sb="t", via=None, color=LINE, lpos=None):
        """Conector ortogonal a->b. via = lista de puntos intermedios absolutos."""
        p0, p1 = self._anchor(a, sa), self._anchor(b, sb)
        pts = [p0]
        if via:
            pts += via
        else:
            if sa in "bt" and sb in "bt" and abs(p0[0] - p1[0]) > 1:
                ym = (p0[1] + p1[1]) / 2
                pts += [(p0[0], ym), (p1[0], ym)]
            elif sa in "rl" and sb in "tb" and abs(p0[0] - p1[0]) > 1:
                pts += [(p1[0], p0[1])]
            elif sa in "bt" and sb in "rl" and abs(p0[1] - p1[1]) > 1:
                pts += [(p0[0], p1[1])]
            elif sa in "rl" and sb in "rl" and abs(p0[1] - p1[1]) > 1:
                xm = (p0[0] + p1[0]) / 2
                pts += [(xm, p0[1]), (xm, p1[1])]
        pts.append(p1)
        d = "M" + " L".join(f"{x:.1f},{y:.1f}" for x, y in pts)
        self.parts.append(
            f'<path d="{d}" fill="none" stroke="{color}" stroke-width="1.8" marker-end="url(#arr)"/>')
        if label:
            if lpos:
                lx, ly = lpos
            elif sa in "rl" and not via:
                lx, ly = ((p0[0] + p1[0]) / 2, p0[1] - 8) if sb in "rl" else (p0[0] + (24 if sa == "r" else -24), p0[1] - 8)
            else:
                lx, ly = p0[0] + (10 if sa in "bt" else 0), p0[1] + (-6 if sa in "rl" else 16)
            self.parts.append(
                f'<text x="{lx:.1f}" y="{ly:.1f}" font-family="{FONT}" font-size="12" font-weight="700" '
                f'fill="{ACC}" text-anchor="{"start" if sa in "bt" else "middle"}">{esc(label)}</text>')

    def label(self, x, y, text, size=12, color=LINE, anchor="start", weight="400"):
        self.parts.append(
            f'<text x="{x}" y="{y}" font-family="{FONT}" font-size="{size}" font-weight="{weight}" '
            f'fill="{color}" text-anchor="{anchor}">{esc(text)}</text>')

    def raw(self, svg):
        self.parts.append(svg)

    def save(self):
        body = "".join(self.parts) + "".join(self._draw_node(n) for n in self.nodes.values())
        svg = (
            f'<svg xmlns="http://www.w3.org/2000/svg" width="{self.w}" height="{self.h}" viewBox="0 0 {self.w} {self.h}">'
            f'<defs><marker id="arr" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="8" markerHeight="8" orient="auto-start-reverse">'
            f'<path d="M0,0 L10,5 L0,10 z" fill="{LINE}"/></marker></defs>'
            f'<rect width="100%" height="100%" fill="#ffffff"/>'
            f'<text x="{self.m}" y="40" font-family="{FONT}" font-size="22" font-weight="700" fill="{INK}">{esc(self.title)}</text>'
            f'<rect x="{self.m}" y="48" width="64" height="3" fill="{ACC}"/>'
            f'{body}</svg>')
        path = os.path.join(OUT, self.name + ".svg")
        with open(path, "w", encoding="utf-8") as f:
            f.write(svg)
        print("escrito", path)


# =============================================================================
# D-01 Arquitectura general
# =============================================================================
def d01():
    d = Diagram("D-01_arquitectura_general", "D-01 · Arquitectura del sistema AutoFinalRobo2", 5, 4, colw=300)
    d.defw = 200
    d.node("loader", 1, 0, "io", "AFR2 Loader (Windows, C#)\ncompila y carga ATmega / ESP")
    d.node("usb", 2, 0, "note", "Cable USB (COM)\npuente USART0 ↔ USART1")
    d.node("op", 0, 1, "io", "Operador\n(celular / PC)")
    d.node("web", 1, 1, "io", "Web React + Vite\n(servida por el ESP)")
    d.node("esp", 2, 1, "proc", "ESP8266 · ESP-12S\nHTTP :80 · /ws · LittleFS\nbroker afr2-ws-1")
    d.node("mega", 3, 1, "proc", "ATmega2560\nC / avr-libc, sin Arduino\nautoridad de salidas")
    d.node("act", 4, 1, "hw", "Actuadores\n4 motores DC + 2 puentes H\nservo del radar")
    d.node("oled", 3, 3, "hw", "Panel local\nOLED 128×64 + encoder", w=215, dx=-125)
    d.node("sens", 3, 3, "hw", "Sensores\nHall ×4 · IR ×5\nHC-SR04 · batería A5", w=215, dx=125)
    d.node("con", 0, 3, "note", "Contracts: wire_catalog.json,\nheaders, commands.json, versión", w=235)
    d.node("cad", 1, 3, "note", "CAD: Fusion / Blender / KiCad\nauto · brazo v17 · PCB panel", w=235, dx=20)
    d.edge("loader", "usb", "", "r", "l")
    d.edge("usb", "mega", "avrdude / esptool", "r", "t", lpos=(d.nodes["usb"]["cx"] + 180, d.nodes["usb"]["cy"] - 8))
    d.edge("op", "web", "Wi-Fi", "r", "l", lpos=(d.nodes["op"]["cx"] + 142, d.nodes["op"]["cy"] - 8))
    d.edge("web", "esp", "WS · JSON", "r", "l", lpos=(d.nodes["web"]["cx"] + 142, d.nodes["web"]["cy"] - 8))
    d.edge("esp", "mega", "UART ROBO2", "r", "l", lpos=(d.nodes["esp"]["cx"] + 142, d.nodes["esp"]["cy"] - 8))
    d.edge("mega", "act", "PWM / IN1-IN2", "r", "l", lpos=(d.nodes["mega"]["cx"] + 142, d.nodes["mega"]["cy"] - 8))
    d.edge("oled", "mega", "I2C · IRQ", "t", "b")
    d.edge("sens", "mega", "", "t", "b")
    d.label(40, d.h - 14, "Navegador → JSON afr2-ws-1 → ESP → UART ROBO2 perfil 2 + EX01 → USART1 del Mega (D18/D19).", 12)
    d.save()


# =============================================================================
# D-02 Conexionado
# =============================================================================
def hline(d, x0, x1, y, label=""):
    d.raw(f'<path d="M{x0:.1f},{y:.1f} L{x1:.1f},{y:.1f}" fill="none" stroke="{LINE}" stroke-width="1.8" marker-end="url(#arr)" marker-start="url(#arr)"/>')
    if label:
        d.label((x0 + x1) / 2, y - 6, label, 11.5, ACC, "middle", "700")


def d02():
    d = Diagram("D-02_conexionado", "D-02 · Conexionado del auto: Arduino Mega 2560 + shield Wi-Fi OSOYOO", 3, 7, colw=430, rowh=84)
    mega_h = 7 * 84 - 24
    cx, cy = d.center(1, 3)
    d.nodes["mega"] = dict(cx=cx, cy=cy, w=250, h=mega_h, kind="proc",
                           text="ARDUINO MEGA 2560\nATmega2560\n\nGND común\n+5 V para Hall, IR,\nHC-SR04, servo, OLED")
    d.node("esp", 0, 1, "hw", "Shield Wi-Fi OSOYOO + ESP-12S\nselector E_RX / E_TX", w=250, h=52)
    d.node("bat", 0, 3, "hw", "Batería 2×18650 → Model Y VIN\ndivisor 10k/10k + 100 nF → A5", w=250, h=52)
    d.node("npn", 0, 5, "hw", "2× NPN (Q1 KEY · Q2 RST)\nbase 1 kΩ + 10 kΩ a GND", w=250, h=52)
    d.node("mot", 2, 0, "hw", "Motores · puentes H (IN1 IN2 PWM)\nDEL DER D22 D24 D9 · DEL IZQ D26 D28 D10\nTRAS DER D5 D6 D11 · TRAS IZQ D7 D8 D12", w=330, h=60)
    d.node("hall", 2, 1, "hw", "Hall ×4 · A: A8..A11 · B: A12..A15\norden físico FR / FL / RR / RL", w=330, h=52)
    d.node("ir", 2, 2, "hw", "Barra IR ×5: IR1..IR5 = A4..A0", w=330, h=44)
    d.node("hc", 2, 3, "hw", "HC-SR04: TRIG D30 · ECHO D31\nServo radar: S = D13", w=330, h=52)
    d.node("oled", 2, 4, "hw", "OLED 0,96\" I2C: SDA D20 · SCL D21", w=330, h=44)
    d.node("enc", 2, 5, "hw", "Encoder KY-040: CLK D2 · DT D3 · SW D42\ncomunes a GND (sin breakout)", w=330, h=52)
    d.node("usb", 2, 6, "note", "D0/D1 = USB (no es el enlace del ESP)", w=330, h=40)
    mx0 = d.nodes["mega"]["cx"] - 125
    mx1 = d.nodes["mega"]["cx"] + 125
    for k, lab in (("esp", "UART1 D18 / D19"), ("bat", "A5 (ADC)"), ("npn", "D32 · D33")):
        n = d.nodes[k]
        hline(d, n["cx"] + n["w"] / 2 + 6, mx0 - 6, n["cy"], lab)
    for k, lab in (("mot", "6 pines + 4 PWM"), ("hall", "PCINT2"), ("ir", "ADC10 / digital"), ("hc", "D30 D31 D13"), ("oled", "I2C Manager"), ("enc", "INT4 / INT5")):
        n = d.nodes[k]
        hline(d, mx1 + 6, n["cx"] - n["w"] / 2 - 6, n["cy"], lab)
    d.label(40, d.h - 14, "DEL/TRAS e IZQ/DER mirando en el sentido de avance. Hall: +5 V regulado, nunca batería. Panel OLED/encoder a 5 V (montaje confirmado).", 12)
    d.save()


# =============================================================================
# D-03a Ciclo principal del ATmega (main.c)
# =============================================================================
def d03a():
    d = Diagram("D-03a_ciclo_principal_ATmega", "D-03a · Ciclo principal del ATmega2560 (main.c)", 2, 7, colw=360, rowh=92)
    d.node("ini", 0, 0, "term", "Arranque: Board, Debug, Product, UART1, UI,\nApp, Link, Encoder, TWI, OLED (último)", w=320, h=52)
    d.node("wd", 0, 1, "proc", "Watchdog ON · sei()\nlas salidas quedan apagadas", w=320)
    d.node("app", 0, 2, "proc", "① App_Task(now)\nservicios del producto (ver D-03b)", w=320)
    d.node("link", 0, 3, "proc", "② Enlace ESP: Link_Tick, ≤ 64 bytes de USART1\nparser ROBO2 → comando → respuesta\n(si falta, envía el BT01 de arranque)", w=330, h=64)
    d.node("enc", 0, 4, "proc", "③ Encoder: drena ≤ 32 transiciones → giro UI\nbotón corto/largo, ambos al soltar (≥ 800 ms)", w=330)
    d.node("ctl", 0, 5, "proc", "④ Si hay marcha y el control venció (≥ 10 ms):\nApp_Task otra vez, antes de dibujar", w=330)
    d.node("tw", 0, 6, "proc", "⑤ TWI + I2C Manager (tick)", w=320, h=44)
    d.node("rd", 1, 6, "dec", "⑥ ¿redraw y ≥ 40 ms\ndesde el último cuadro?", w=220, h=40)
    d.node("rnd", 1, 5, "proc", "Render UI → framebuffer 128×64", w=270)
    d.node("ol", 1, 4, "proc", "⑦ Oled_Task: ≤ 16 bytes por turno,\n1 etapa cada 2 ms (no bloquea)", w=300)
    d.node("kick", 1, 3, "term", "⑧ Watchdog_Kick", w=250, h=44)
    d.edge("ini", "wd")
    d.edge("wd", "app")
    d.edge("app", "link")
    d.edge("link", "enc")
    d.edge("enc", "ctl")
    d.edge("ctl", "tw")
    d.edge("tw", "rd", "", "r", "l")
    d.edge("rd", "rnd", "sí", "t", "b", lpos=(d.nodes["rd"]["cx"] + 10, d.nodes["rd"]["cy"] - 42))
    d.edge("rnd", "ol")
    d.edge("rd", "ol", "no", "r", "r", via=[(d.nodes["rd"]["cx"] + 175, d.nodes["rd"]["cy"]), (d.nodes["rd"]["cx"] + 175, d.nodes["ol"]["cy"])], lpos=(d.nodes["rd"]["cx"] + 135, d.nodes["rd"]["cy"] - 6))
    d.edge("ol", "kick")
    d.edge("kick", "app", "vuelve al ciclo", "l", "r", via=[(d.nodes["kick"]["cx"] - 190, d.nodes["kick"]["cy"]), (d.nodes["kick"]["cx"] - 190, d.nodes["app"]["cy"]), (d.nodes["app"]["cx"] + 165, d.nodes["app"]["cy"])], lpos=(d.nodes["kick"]["cx"] - 125, d.nodes["kick"]["cy"] - 34))
    d.label(40, d.h - 14, "Con reinicio diferido pendiente y el UART libre, el ciclo termina en Watchdog_Reboot. Ninguna ISR hace trabajo caro: todo ocurre en main.", 12)
    d.save()


# =============================================================================
# D-03b Product_Task (control)
# =============================================================================
def d03b():
    d = Diagram("D-03b_servicio_de_control", "D-03b · Afr2Product_Task: sensores, odometría, control y PWM", 3, 7, colw=350, rowh=92)
    d.node("s", 0, 0, "term", "Afr2Product_Task(now)", w=260, h=44)
    d.node("sen", 0, 1, "proc", "Sensores (Hall, IR, HC-SR04, batería)\nmedición Hall manual · EEPROM", w=300)
    d.node("to", 0, 2, "proc", "Vencimientos: origen local / Web (1,5 s) /\nconsigna manual → Stop (salidas a 0)", w=310)
    d.node("seq", 0, 3, "proc", "Secuencias · ¿pasaron ≥ 10 ms?\n(si no, termina el servicio)", w=300)
    d.node("late", 0, 4, "dec", "¿dt > 100 ms con\nPWM aplicado?", w=190, h=40)
    d.node("f1", 1, 4, "bad", "Corta salidas · F1\n3 en 2 s = F1 enclavado\n(PWM manual: solo registra)", w=270, h=62)
    d.node("od", 0, 5, "proc", "Odometría (ver D-10)\nΔn → Δs → (Δx, Δy, Δψ) → pose", w=300)
    d.node("tgt", 0, 6, "proc", "Modo LÍNEA / GOTO / MANUAL / PWM / TEST\n→ (vx, vy, ωz) → Drive → u_i por rueda", w=330)
    d.node("pid", 1, 6, "proc", "PID por rueda (Q8): e = u_ref − u_med\nsalida = (Kp·e + Ki·∫e + Kd·Δe)/256\nrampa ±4 · límite 255", w=290, h=70)
    d.node("chk", 1, 5, "dec", "¿signo PWM/Hall cruzado (F4)\no sin pulsos 500 ms (F3)?", w=240, h=40)
    d.node("mw", 2, 5, "term", "Motor_Write(pwm, signos)", w=250, h=44)
    d.node("st", 2, 4, "bad", "Stop + fallo", w=170, h=40)
    d.edge("s", "sen")
    d.edge("sen", "to")
    d.edge("to", "seq")
    d.edge("seq", "late")
    d.edge("late", "f1", "sí", "r", "l")
    d.edge("late", "od", "no", "b", "t")
    d.edge("od", "tgt")
    d.edge("tgt", "pid", "", "r", "l")
    d.edge("pid", "chk", "", "t", "b")
    d.edge("chk", "mw", "no", "r", "l")
    d.edge("chk", "st", "sí", "t", "l", via=[(d.nodes["chk"]["cx"], d.nodes["st"]["cy"])], lpos=(d.nodes["chk"]["cx"] + 8, d.nodes["chk"]["cy"] - 40))
    d.label(40, d.h - 14, "Un solo lazo cooperativo de 10 ms. STOP, TTL de 400 ms por consigna y corte de 1,5 s sin pulso Web apagan las salidas.", 12)
    d.save()


# =============================================================================
# D-04 GOTO
# =============================================================================
def d04():
    d = Diagram("D-04_flujo_GOTO", "D-04 · Programa GOTO: del pedido a la llegada (ATmega, cada 10 ms)", 3, 8, colw=330, rowh=92)
    d.node("req", 0, 0, "io", "Pedido GOTO (0x16)\nX, Y [mm] relativos al rumbo,\nθ [mrad] opcional, v [mm/s]", w=270, h=62)
    d.node("val", 0, 1, "dec", "¿|X|,|Y| ≤ 5000 · |θ| ≤ 3140\n10 ≤ v ≤ límite · sin fallo ·\npose válida · motores calibrados?", w=270, h=52)
    d.node("rej", 1, 1, "bad", "Rechazo: BAD / DENIED /\nUNAVAILABLE / BUSY", w=230, h=52)
    d.node("obj", 0, 2, "proc", "Objetivo en el mundo:\n(xm, ym) = (x, y) + R(ψ)·(X, Y)\nψm = ψ + θ", w=270, h=62)
    d.node("tm", 0, 3, "dec", "¿pose válida y\nt < 60 s?", w=190, h=40)
    d.node("tof", 1, 3, "bad", "Estado 4 (fallo) · Stop", w=230, h=44)
    d.node("er", 0, 4, "proc", "Error: ex = xm − x · ey = ym − y\neψ = ψm − ψ  (normalizado ±π)", w=290)
    d.node("ar", 0, 5, "dec", "¿|ex|,|ey| ≤ 10 mm\ny |eψ| ≤ 25 mrad?", w=200, h=40)
    d.node("ok", 1, 5, "ok", "Llegó: estado 2 · Stop\naviso OLED/Web", w=230, h=52)
    d.node("p", 0, 6, "proc", "Al marco del robot:\nvx = ex·cosψ + ey·sinψ\nvy = −ex·sinψ + ey·cosψ · ωz = eψ", w=290, h=62)
    d.node("lim", 0, 7, "proc", "Mínimo 40 mm/s · normaliza\n|vx|+|vy|+|ωz|·L ≤ v  → Drive(vx, vy, ωz)", w=300, h=52)
    d.edge("req", "val")
    d.edge("val", "rej", "no", "r", "l")
    d.edge("val", "obj", "sí", "b", "t")
    d.edge("obj", "tm")
    d.edge("tm", "tof", "no", "r", "l")
    d.edge("tm", "er", "sí", "b", "t")
    d.edge("er", "ar")
    d.edge("ar", "ok", "sí", "r", "l")
    d.edge("ar", "p", "no", "b", "t")
    d.edge("p", "lim")
    ymid = (d.nodes["obj"]["cy"] + 31 + d.nodes["tm"]["cy"] - 28) / 2
    d.edge("lim", "tm", "siguiente ciclo (10 ms)", "r", "t", via=[(d.nodes["lim"]["cx"] + 185, d.nodes["lim"]["cy"]), (d.nodes["lim"]["cx"] + 185, ymid), (d.nodes["tm"]["cx"], ymid)], lpos=(d.nodes["lim"]["cx"] + 290, d.nodes["lim"]["cy"] - 12))
    d.node("web", 2, 6, "note", "Web (1.23.x): la ruta simulada se\nenvía como tramos consecutivos\nrobot.goto.start; espera robot.state\nantes del siguiente tramo.", w=290, h=82)
    d.save()


# =============================================================================
# D-05 Misión recolectora
# =============================================================================
def d05():
    d = Diagram("D-05_ciclo_mision_recolectora", "D-05 · Ciclo de la misión del recolector (simulador)", 3, 8, colw=330, rowh=92)
    d.m = 90; d.w += 60; d.h += 40
    d.node("a", 0, 0, "term", "Inicio: robot detenido, brazo recogido", w=300, h=44)
    d.node("b", 0, 1, "proc", "Barrido del HC-SR04 con base quieta\n37 direcciones a 5° · 300 ms de posicionado\n+ 250 ms por dirección", w=300, h=62)
    d.node("c", 0, 2, "dec", "¿candidato de cubo\n(eco + tamaño conocido)?", w=210, h=40)
    d.node("x1", 1, 2, "bad", "Fin: objetivo perdido", w=220, h=44)
    d.node("d", 0, 3, "proc", "Orientar / centrar al candidato\n+ eco frontal reciente", w=270)
    d.node("e", 0, 4, "proc", "Aproximar (Mecanum) → detener\n→ medir otra vez → preagarre", w=270)
    d.node("f", 0, 5, "dec", "¿IK vertical con trayectoria\nválida y alcanzable?", w=220, h=40)
    d.node("x2", 1, 5, "bad", "Fin: inalcanzable /\ntrayectoria bloqueada", w=230, h=50)
    d.node("g", 1, 6, "proc", "Tomar → ¿agarre OK?\nElevar → llevar sobre celda", w=250, h=52)
    d.node("x3", 2, 6, "bad", "Fin: agarre fallido", w=210, h=44)
    d.node("h", 1, 7, "proc", "Soltar en la celda del depósito\n→ recoger brazo", w=250, h=52)
    d.node("i", 0, 7, "dec", "¿quedan cubos\ny hay lugar libre?", w=200, h=40)
    d.node("x4", 2, 7, "ok", "Fin: misión completa /\ndepósito lleno (con motivo)", w=240, h=50)
    d.edge("a", "b")
    d.edge("b", "c")
    d.edge("c", "x1", "no", "r", "l")
    d.edge("c", "d", "sí", "b", "t")
    d.edge("d", "e")
    d.edge("e", "f")
    d.edge("f", "x2", "no", "r", "l")
    d.edge("f", "g", "sí", "b", "l", via=[(d.nodes["f"]["cx"], d.nodes["g"]["cy"])])
    d.edge("g", "x3", "no", "r", "l")
    d.edge("g", "h")
    d.edge("h", "i", "", "l", "r")
    d.edge("i", "b", "sí", "l", "l", via=[(d.nodes["i"]["cx"] - 185, d.nodes["i"]["cy"]), (d.nodes["i"]["cx"] - 185, d.nodes["b"]["cy"]), (d.nodes["b"]["cx"] - 150, d.nodes["b"]["cy"])], lpos=(d.nodes["i"]["cx"] - 160, d.nodes["i"]["cy"] - 8))
    d.edge("i", "x4", "no", "b", "l", via=[(d.nodes["i"]["cx"], d.nodes["i"]["cy"] + 60), (d.nodes["x4"]["cx"] - 140, d.nodes["i"]["cy"] + 60), (d.nodes["x4"]["cx"] - 140, d.nodes["x4"]["cy"])], lpos=(d.nodes["i"]["cx"] + 10, d.nodes["i"]["cy"] + 56))
    d.label(40, d.h - 14, "La base permanece detenida mientras el brazo manipula. Es simulación local de la Web: no hay misión física completa verificada.", 12)
    d.save()


# =============================================================================
# D-06 Circuito KEY / RST
# =============================================================================
def d06():
    w, h = 1180, 660
    s = []
    s.append(f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}" height="{h}" viewBox="0 0 {w} {h}">')
    s.append(f'<rect width="100%" height="100%" fill="#ffffff"/>')
    s.append(f'<text x="40" y="40" font-family="{FONT}" font-size="22" font-weight="700" fill="{INK}">D-06 · KEY y RST automáticos del ESP con transistores NPN (propuesta, sin armar/medir)</text>')
    s.append(f'<rect x="40" y="48" width="64" height="3" fill="{ACC}"/>')

    def canal(x0, y0, nombre, pin, destino, qn, r1, r3):
        o = []
        stroke = f'stroke="{LINE}" stroke-width="2" fill="none"'
        # Mega pin
        o.append(f'<rect x="{x0}" y="{y0 + 50}" width="110" height="46" rx="6" fill="#efe9ff" stroke="#6a4bb5" stroke-width="1.6"/>')
        o.append(f'<text x="{x0 + 55}" y="{y0 + 70}" text-anchor="middle" font-family="{FONT}" font-size="13" fill="{INK}">Mega</text>')
        o.append(f'<text x="{x0 + 55}" y="{y0 + 88}" text-anchor="middle" font-family="{FONT}" font-size="14" font-weight="700" fill="{ACC}">{pin}</text>')
        # resistencia R1 (zigzag) horizontal
        xs = x0 + 110
        o.append(f'<path d="M{xs},{y0 + 73} L{xs + 25},{y0 + 73}" {stroke}/>')
        zz = f'M{xs + 25},{y0 + 73} l8,-12 l12,24 l12,-24 l12,24 l12,-24 l8,12'
        o.append(f'<path d="{zz}" {stroke}/>')
        o.append(f'<text x="{xs + 65}" y="{y0 + 48}" text-anchor="middle" font-family="{FONT}" font-size="13" fill="{INK}">{r1} 1 kΩ</text>')
        xb = xs + 112
        o.append(f'<path d="M{xs + 89},{y0 + 73} L{xb + 30},{y0 + 73}" {stroke}/>')
        # transistor
        bx = xb + 30
        o.append(f'<path d="M{bx},{y0 + 50} L{bx},{y0 + 96}" stroke="{INK}" stroke-width="4" fill="none"/>')
        o.append(f'<path d="M{bx},{y0 + 62} L{bx + 42},{y0 + 28} L{bx + 42},{y0}" {stroke}/>')
        o.append(f'<path d="M{bx},{y0 + 84} L{bx + 42},{y0 + 118} L{bx + 42},{y0 + 160}" {stroke}/>')
        o.append(f'<path d="M{bx + 28},{y0 + 108} L{bx + 42},{y0 + 118} L{bx + 29},{y0 + 120}" stroke="{INK}" stroke-width="2" fill="{INK}"/>')
        o.append(f'<text x="{bx + 8}" y="{y0 + 140}" font-family="{FONT}" font-size="14" font-weight="700" fill="{INK}">{qn}</text>')
        o.append(f'<text x="{bx + 52}" y="{y0 + 14}" font-family="{FONT}" font-size="12" fill="{INK}">C (colector)</text>')
        o.append(f'<text x="{bx + 52}" y="{y0 + 168}" font-family="{FONT}" font-size="12" fill="{INK}">E (emisor)</text>')
        o.append(f'<text x="{bx - 24}" y="{y0 + 66}" font-family="{FONT}" font-size="12" fill="{INK}">B</text>')
        # R3 a GND
        o.append(f'<path d="M{xb + 12},{y0 + 73} L{xb + 12},{y0 + 100}" {stroke}/>')
        zz2 = f'M{xb + 12},{y0 + 100} l-9,6 l18,12 l-18,12 l18,12 l-9,6'
        o.append(f'<path d="{zz2}" {stroke}/>')
        o.append(f'<text x="{xb - 20}" y="{y0 + 142}" text-anchor="end" font-family="{FONT}" font-size="13" fill="{INK}">{r3} 10 kΩ</text>')
        o.append(f'<path d="M{xb + 12},{y0 + 148} L{xb + 12},{y0 + 170} L{bx + 42},{y0 + 170}" {stroke}/>')
        # GND
        gx = bx + 42
        o.append(f'<path d="M{gx},{y0 + 160} L{gx},{y0 + 176}" {stroke}/>')
        for i, wd in enumerate((26, 18, 10)):
            o.append(f'<path d="M{gx - wd / 2},{y0 + 176 + i * 6} L{gx + wd / 2},{y0 + 176 + i * 6}" stroke="{INK}" stroke-width="2"/>')
        o.append(f'<text x="{gx + 20}" y="{y0 + 194}" font-family="{FONT}" font-size="12" fill="{INK}">GND común (Mega = shield)</text>')
        # al pulsador
        o.append(f'<path d="M{bx + 42},{y0} L{bx + 42},{y0 - 18} L{bx + 150},{y0 - 18}" {stroke}/>')
        o.append(f'<rect x="{bx + 150}" y="{y0 - 40}" width="190" height="44" rx="6" fill="#e4f0ff" stroke="#2d6cb5" stroke-width="1.6"/>')
        o.append(f'<text x="{bx + 245}" y="{y0 - 22}" text-anchor="middle" font-family="{FONT}" font-size="13" fill="{INK}">Pata de señal del</text>')
        o.append(f'<text x="{bx + 245}" y="{y0 - 6}" text-anchor="middle" font-family="{FONT}" font-size="13" font-weight="700" fill="{INK}">{destino}</text>')
        return "".join(o)

    s.append(canal(40, 140, "KEY", "D32 (PC5)", "pulsador KEY (GPIO0)", "Q1", "R1", "R3"))
    s.append(canal(40, 410, "RST", "D33 (PC4)", "pulsador RST (reset)", "Q2", "R2", "R4"))
    s.append(f'<text x="740" y="150" font-family="{FONT}" font-size="13" fill="{INK}">• NPN en paralelo con cada pulsador del shield:</text>')
    for i, t in enumerate([
        "  el pulsador sigue funcionando a mano.",
        "• Pin en alto = pulsador \"apretado\" (colector a ~0 V).",
        "• Pin en bajo o alta impedancia = suelto; los 10 kΩ",
        "  aseguran \"suelto\" durante el arranque.",
        "• Al arrancar D32/D33 son entradas sin pull-up.",
        "• Solo hay carga automática si el usuario la pide."]):
        s.append(f'<text x="740" y="{172 + i * 20}" font-family="{FONT}" font-size="13" fill="{INK}">{esc(t)}</text>')
    s.append(f'<text x="740" y="340" font-family="{FONT}" font-size="13" font-weight="700" fill="{ACC}">Secuencia de entrada en modo carga</text>')
    for i, t in enumerate([
        "1  KEY alto (GPIO0 a masa)",
        "2  espera 20 ms",
        "3  RST alto 100 ms → RST bajo (reinicio)",
        "4  ~200 ms con KEY apretado",
        "5  puente transparente + esptool",
        "6  se suelta KEY y el ESP arranca"]):
        s.append(f'<text x="740" y="{362 + i * 20}" font-family="{FONT}" font-size="13" fill="{INK}">{esc(t)}</text>')
    s.append(f'<text x="40" y="{h - 16}" font-family="{FONT}" font-size="12" fill="{LINE}">Transistor 2N3904 (E-B-C) o BC547 (C-B-E): verificar patas con la hoja de datos. Estado: implementado en fuente (AFR2-20261106-11); circuito sin armar ni medir.</text>')
    s.append("</svg>")
    with open(os.path.join(OUT, "D-06_circuito_KEY_RST_NPN.svg"), "w", encoding="utf-8") as f:
        f.write("".join(s))
    print("escrito D-06")


# =============================================================================
# D-07 Navegación OLED
# =============================================================================
def d07():
    d = Diagram("D-07_navegacion_OLED_encoder", "D-07 · Navegación del panel OLED + encoder (sin botón menú)", 4, 4, colw=330, rowh=110)
    d.node("ini", 0, 0, "io", "INICIO 010001\ndashboard: modo, red,\nbatería, tarjeta de estados", w=235, h=70)
    d.node("men", 1, 0, "proc", "MENÚ PRINCIPAL 010002\nEstado · Wi-Fi · Sensores ·\nPruebas · Calibración · Sistema", w=250, h=70)
    d.node("sub", 2, 0, "proc", "SUBMENÚ / EDITOR / VISTA\ngiro = mover · corto = abrir\no aplicar · Volver = un ítem", w=250, h=70)
    d.node("sn", 3, 0, "dec", "¿cambios\npendientes?", w=150, h=44)
    d.node("gu", 3, 1, "ok", "Guardar cambios 030038\nSí / No obligatorio\n(el largo no lo evita)", w=235, h=64)
    d.node("rap", 1, 2, "proc", "RÁPIDAS 010003\n7 tarjetas: servicios, radar,\nmenú y modo. Largo → Inicio", w=250, h=70)
    d.node("ret", 2, 2, "proc", "RETORNO CONTEXTUAL\nal contexto anterior\n(menú o pantalla de origen)", w=240, h=64)
    d.node("tou", 0, 3, "note", "Largo = ≥ 800 ms (configurable\n800..3000) y se ejecuta AL SOLTAR.\nUn largo nunca genera un corto.", w=270, h=64)
    d.edge("ini", "men", "corto", "r", "l")
    d.edge("ini", "rap", "largo", "b", "l", via=[(d.nodes["ini"]["cx"], d.nodes["rap"]["cy"])], lpos=(d.nodes["ini"]["cx"] + 8, d.nodes["ini"]["cy"] + 90))
    d.edge("men", "sub", "giro + corto", "r", "l")
    d.edge("sub", "sn", "", "r", "l")
    d.edge("sn", "gu", "sí", "b", "t")
    d.edge("sub", "ret", "largo", "b", "t", lpos=(d.nodes["sub"]["cx"] + 8, d.nodes["sub"]["cy"] + 90))
    d.label(40, d.h - 14, "Corto confirma/alterna. No hay botón de menú: volver es el ítem Volver. Sí/No de cambios pendientes se mantiene en firmware y Web.", 12)
    d.save()


# =============================================================================
# D-08 Cadena cinemática del brazo
# =============================================================================
def d08():
    w, h = 1400, 330
    s = [f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}" height="{h}" viewBox="0 0 {w} {h}"><defs><marker id="arr" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="8" markerHeight="8" orient="auto-start-reverse"><path d="M0,0 L10,5 L0,10 z" fill="{LINE}"/></marker></defs>',
         '<rect width="100%" height="100%" fill="#ffffff"/>',
         f'<text x="40" y="40" font-family="{FONT}" font-size="22" font-weight="700" fill="{INK}">D-08 · Cadena cinemática del conjunto: mundo → base → montura → brazo → TCP</text>',
         f'<rect x="40" y="48" width="64" height="3" fill="{ACC}"/>']
    items = [
        ("Mundo\nW", "io"), ("T_W,B(x,y,ψ)\nbase Mecanum", "ok"), ("T_B,M\nmontura\n(0,0278; 0; 0,079) m", "proc"),
        ("Tz(h)\nh = 0,209 m", "proc"), ("Rz(q1)\nJ1 base", "hw"), ("Ry(q2)\nJ2 hombro", "hw"),
        ("Tz(L1)\nL1 = 0,070 m", "proc"), ("Ry(q3)\nJ3 codo", "hw"), ("Tz(L2)\nL2 = 0,115 m", "proc"),
        ("Rz(q4)\nJ4", "hw"), ("Ry(q5)\nJ5", "hw"), ("Rz(q6)\nJ6", "hw"), ("Tz(d)\nd = 0,1435 m", "proc"), ("TCP", "term")]
    n = len(items)
    bw, gap = 84, 14
    row_y = [100, 190]
    perrow = 7
    for i, (t, k) in enumerate(items):
        r, c = divmod(i, perrow)
        x = 40 + c * (bw + 36)
        y = row_y[r]
        fill, stroke = KINDS[k]
        col = "#ffffff" if k == "term" else INK
        s.append(f'<rect x="{x}" y="{y}" width="{bw + 22}" height="66" rx="8" fill="{fill}" stroke="{stroke}" stroke-width="1.6"/>')
        for j, ln in enumerate(t.split("\n")):
            yy = y + 33 - (len(t.split("\n")) - 1) * 8 + 4 + j * 16
            s.append(f'<text x="{x + (bw + 22) / 2}" y="{yy}" text-anchor="middle" font-family="{FONT}" font-size="11.5" fill="{col}">{esc(ln)}</text>')
        if c < perrow - 1:
            s.append(f'<path d="M{x + bw + 22},{y + 33} L{x + bw + 22 + 14 + 0},{y + 33}" stroke="{LINE}" stroke-width="1.8" fill="none" marker-end="url(#arr)"/>')
    s.append(f'<path d="M{40 + 6 * (bw + 36) + bw / 2 + 11},{row_y[0] + 66} L{40 + 6 * (bw + 36) + bw / 2 + 11},{row_y[0] + 80} L{40 + (bw + 22) / 2},{row_y[0] + 80} L{40 + (bw + 22) / 2},{row_y[1]}" stroke="{LINE}" stroke-width="1.8" fill="none" marker-end="url(#arr)"/>')
    s.append(f'<text x="40" y="{h - 44}" font-family="{FONT}" font-size="13" fill="{INK}">T_W,TCP(Q) = T_W,B(x, y, ψ) · T_B,M · Tz(h) Rz(q1) Ry(q2) Tz(L1) Ry(q3) Tz(L2) Rz(q4) Ry(q5) Rz(q6) Tz(d)      Q = (x, y, ψ, q1…q6)  → 9 grados de libertad de configuración</text>')
    s.append(f'<text x="40" y="{h - 20}" font-family="{FONT}" font-size="12" fill="{LINE}">h = 0,096 + 0,113 · L2 = 0,060 + 0,055 · d = 0,065 + 0,0785 (tramos colineales agrupados). Descriptor v17; unidades m / rad.</text>')
    s.append("</svg>")
    with open(os.path.join(OUT, "D-08_cadena_cinematica.svg"), "w", encoding="utf-8") as f:
        f.write("".join(s))
    print("escrito D-08")


# =============================================================================
# D-09 Flujo del Loader
# =============================================================================
def d09():
    d = Diagram("D-09_flujo_Loader", "D-09 · AFR2 Loader: de la fuente al equipo (Windows, C#)", 5, 2, colw=235, rowh=120)
    d.node("a", 0, 0, "io", "1 · Operación\nPuerto COM + destino:\nATmega o ESP", w=200, h=64)
    d.node("b", 1, 0, "proc", "2 · Perfil\nrutas {App}/módulos,\nconfig fuera del repo", w=200, h=64)
    d.node("c", 2, 0, "proc", "3 · Compilar / preparar\nAVR: avr-gcc + make\nESP: PlatformIO + LittleFS", w=200, h=64)
    d.node("d", 3, 0, "dec", "4 · Confirmaciones\ny backup opcional\nde flash", w=150, h=44)
    d.node("e", 4, 0, "hw", "5 · Flasher temporal en el Mega\npuente USB ↔ USART1 ↔ ESP\nKEY/RST manual o auto (NPN)", w=215, h=70)
    d.node("f", 4, 1, "hw", "6 · avrdude (Mega) /\nesptool (ESP) con hash\nverificado en la misma sesión", w=215, h=64)
    d.node("g", 3, 1, "ok", "7 · Restaura producto\nBT01 de arranque", w=200, h=52)
    d.node("h", 2, 1, "proc", "8 · Monitor serie\nASCII / HEX, TX/RX a archivo", w=200, h=58)
    d.node("i", 1, 1, "proc", "9 · Intérprete USB\ndecodifica el enlace con\nwire_catalog.json (TAP)", w=210, h=64)
    d.node("j", 0, 1, "proc", "10 · Servidor MCP\nrevisión manual de cada\nsolicitud (sin comandos libres)", w=210, h=64)
    d.edge("a", "b", "", "r", "l")
    d.edge("b", "c", "", "r", "l")
    d.edge("c", "d", "", "r", "l")
    d.edge("d", "e", "sí", "r", "l")
    d.edge("e", "f", "", "b", "t")
    d.edge("f", "g", "", "l", "r")
    d.edge("g", "h", "", "l", "r")
    d.edge("h", "i", "", "l", "r")
    d.edge("i", "j", "", "l", "r")
    d.label(40, d.h - 14, "Cualquier paso físico (KEY/RST, cableado D18/D19) se confirma en pantalla. El Loader no abre puertos ni flashea sin pedido del operador.", 12)
    d.save()


# =============================================================================
# D-10 Odometría
# =============================================================================
def d10():
    d = Diagram("D-10_pipeline_odometria", "D-10 · Odometría por ruedas: de los pulsos Hall a la pose (x, y, ψ)", 2, 5, colw=360, rowh=92)
    d.node("h", 0, 0, "hw", "Hall ×4 · flanco ↑ de A, B = sentido\n(IRQ PCINT2, puerto K, máscara 0x0F)", w=320, h=56)
    d.node("n", 0, 1, "proc", "Δn_i = n_i(k) − n_i(k−1)\ncontadores en orden FL, FR, RL, RR", w=320)
    d.node("sg", 0, 2, "proc", "Normaliza signos: hall_signs / hall_cw_mask\n(derechas: CW exterior = avance)", w=320)
    d.node("s", 0, 3, "proc", "Δs_i = Δn_i · π · D / N_i\nD = 80 mm · N_i = 370  (0,679 mm/cuenta)", w=320)
    d.node("u", 0, 4, "proc", "u_i = Δs_i / Δt  → velocidad por rueda\n(realimenta el PID)", w=320)
    d.node("m", 1, 4, "proc", "Mezcla inversa (4 ruedas → 3 velocidades)\nΔx = ¼ΣΔs · Δy = ¼(−s1+s2+s3−s4)\nΔψ = ¼(−s1+s2−s3+s4)/L  (L = 170 mm)", w=340, h=70)
    d.node("k", 1, 3, "proc", "Factor direccional medido (odom_scale)\navance/retro ×1,000 · lateral ×0,775 · giro ×1,000", w=340)
    d.node("r", 1, 2, "proc", "Rumbo medio: ψm = ψ + Δψ/2\nx += Δx·cosψm − Δy·sinψm\ny += Δx·sinψm + Δy·cosψm", w=340, h=62)
    d.node("p", 1, 1, "ok", "Pose: x_mm, y_mm, yaw_mrad (±π)\npose_valid = 1", w=300, h=52)
    d.node("o", 1, 0, "io", "Usos: GOTO (error), telemetría Web,\nmapa y prueba de odometría", w=320, h=52)
    d.edge("h", "n")
    d.edge("n", "sg")
    d.edge("sg", "s")
    d.edge("s", "u")
    d.edge("u", "m", "", "r", "l")
    d.edge("m", "k", "", "t", "b")
    d.edge("k", "r", "", "t", "b")
    d.edge("r", "p", "", "t", "b")
    d.edge("p", "o", "", "t", "b")
    d.label(40, d.h - 14, "Sin giroscopio: la deriva de rumbo crece con la distancia. |Δn| > 100000 invalida la pose (F2).", 12)
    d.save()


# =============================================================================
# D-11 Patrones de rueda Mecanum
# =============================================================================
def d11():
    w, h = 1180, 450
    s = [f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}" height="{h}" viewBox="0 0 {w} {h}">',
         '<rect width="100%" height="100%" fill="#ffffff"/>',
         f'<text x="40" y="40" font-family="{FONT}" font-size="22" font-weight="700" fill="{INK}">D-11 · Patrones de rueda Mecanum (vista superior, signos geométricos canónicos)</text>',
         f'<rect x="40" y="48" width="64" height="3" fill="{ACC}"/>']
    motions = [
        ("Avance  (vx > 0)", (1, 1, 1, 1), "u = vx · (+1, +1, +1, +1)", "up"),
        ("Lateral izquierda  (vy > 0)", (-1, 1, 1, -1), "u = vy · (−1, +1, +1, −1)", "left"),
        ("Giro antihorario  (ω > 0)", (-1, 1, -1, 1), "u = L·ω · (−1, +1, −1, +1)", "ccw"),
    ]
    for k, (title, sg, formula, kind) in enumerate(motions):
        x0 = 60 + k * 370
        y0 = 110
        # chasis
        s.append(f'<rect x="{x0 + 40}" y="{y0 + 20}" width="170" height="210" rx="14" fill="#f3f5f9" stroke="{LINE}" stroke-width="1.8"/>')
        pos = [(x0 + 5, y0 + 30), (x0 + 215, y0 + 30), (x0 + 5, y0 + 150), (x0 + 215, y0 + 150)]
        names = ["FL", "FR", "RL", "RR"]
        for (wx, wy), sgn, nm in zip(pos, sg, names):
            col = "#2a8a52" if sgn > 0 else "#b81d33"
            s.append(f'<rect x="{wx}" y="{wy}" width="30" height="80" rx="6" fill="#1f2430"/>')
            cy1, cy2 = (wy + 62, wy + 18) if sgn > 0 else (wy + 18, wy + 62)
            s.append(f'<path d="M{wx + 15},{cy1} L{wx + 15},{cy2}" stroke="{col}" stroke-width="5" fill="none"/>')
            tip = f'{wx + 15},{cy2 - 8 if sgn > 0 else cy2 + 8} {wx + 6},{cy2 + 2 if sgn > 0 else cy2 - 2} {wx + 24},{cy2 + 2 if sgn > 0 else cy2 - 2}'
            s.append(f'<polygon points="{tip}" fill="{col}"/>')
            s.append(f'<text x="{wx + 15}" y="{wy - 6 if wy < y0 + 100 else wy + 98}" text-anchor="middle" font-family="{FONT}" font-size="12" font-weight="700" fill="{INK}">{nm} {"+" if sgn > 0 else "−"}</text>')
        # vector de movimiento
        cx, cy = x0 + 125, y0 + 160
        if kind == "up":
            s.append(f'<path d="M{cx},{cy + 40} L{cx},{cy - 100}" stroke="{ACC}" stroke-width="5" fill="none" marker-end="url(#a)"/>')
        elif kind == "left":
            s.append(f'<path d="M{cx + 50},{cy - 30} L{cx - 70},{cy - 30}" stroke="{ACC}" stroke-width="5" fill="none" marker-end="url(#a)"/>')
        else:
            s.append(f'<path d="M{cx + 45},{cy - 40} A45,45 0 1 0 {cx - 45},{cy - 40}" stroke="{ACC}" stroke-width="5" fill="none" marker-end="url(#a)"/>')
        s.append(f'<text x="{x0 + 125}" y="{y0 + 282}" text-anchor="middle" font-family="{FONT}" font-size="15" font-weight="700" fill="{INK}">{title}</text>')
        s.append(f'<text x="{x0 + 125}" y="{y0 + 304}" text-anchor="middle" font-family="{FONT}" font-size="13" fill="{INK}">{formula}</text>')
    s.insert(1, f'<defs><marker id="a" viewBox="0 0 10 10" refX="6" refY="5" markerWidth="5" markerHeight="5" orient="auto"><path d="M0,0 L10,5 L0,10 z" fill="{ACC}"/></marker></defs>')
    s.append(f'<text x="40" y="{h - 16}" font-family="{FONT}" font-size="12" fill="{LINE}">Verde ↑ = rueda con u &gt; 0 · rojo ↓ = u &lt; 0. El contrato del proyecto (motion_reference.json) aplica S = diag(−1, −1, −1) sobre estos signos; physicalVerification = false.</text>')
    s.append("</svg>")
    with open(os.path.join(OUT, "D-11_patrones_rueda_mecanum.svg"), "w", encoding="utf-8") as f:
        f.write("".join(s))
    print("escrito D-11")


if __name__ == "__main__":
    for fn in (d01, d02, d03a, d03b, d04, d05, d06, d07, d08, d09, d10, d11):
        fn()
