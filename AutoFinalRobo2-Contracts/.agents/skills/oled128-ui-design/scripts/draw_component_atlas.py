"""Generate a design atlas, not a firmware/hardware validation.

Original font/bitmap authors remain in assets. Mini numeral rows below are
from AutoMicro reference oled_utils.c:OLED_MiniGlyphRow, attributed to kobac.
"""
import json
from pathlib import Path
import re
import struct
import zlib

ASSETS = Path(__file__).resolve().parents[1] / 'assets'
SPEC = json.loads((ASSETS / 'components.json').read_text(encoding='utf-8'))
T = SPEC['tokens']
NUMERALS = json.loads((ASSETS / 'numerals5x7.json').read_text(encoding='utf-8'))


def read_array(source, name):
    body = re.search(r'\b' + re.escape(name) + r'\s*\[\s*\]\s*\w*\s*=\s*\{(.*?)\}', source, re.S).group(1)
    body = re.sub(r'/\*.*?\*/|//[^\n]*', '', body, flags=re.S)
    return [int(value, 16) for value in re.findall(r'0x[0-9a-fA-F]+', body)]


FONT = read_array((ASSETS / 'font7x10.c').read_text(encoding='utf-8'), 'Font7x10')
SRC = (ASSETS / 'afr2_icons.c').read_text(encoding='utf-8')
ICONS = {name: (int(w), int(h), read_array(SRC, name)) for name, w, h in
         re.findall(r'\{\s*(Icon_\w+_bits),\s*(\d+),\s*(\d+)\s*\}', SRC)}
GESTURES = json.loads((ASSETS / 'encoder-gestures.json').read_text(encoding='utf-8'))
ICONS.update({name: (item['width'], item['height'], item['bytes'])
              for name, item in GESTURES['icons'].items()})
MINI = dict(zip('0123456789-', [
    [7, 5, 5, 5, 7], [2, 6, 2, 2, 7], [7, 1, 7, 4, 7], [7, 1, 7, 1, 7],
    [5, 5, 7, 1, 1], [7, 4, 7, 1, 7], [7, 4, 7, 5, 7], [7, 1, 2, 2, 2],
    [7, 5, 7, 5, 7], [7, 5, 7, 1, 7], [0, 0, 7, 0, 0]]))


class Oled:
    def __init__(self):
        self.bits = [[False] * 128 for _ in range(64)]

    def pixel(self, x, y):
        if 0 <= x < 128 and 0 <= y < 64:
            self.bits[y][x] = True

    def fill(self, x, y, w, h):
        for py in range(y, y + h):
            for px in range(x, x + w):
                self.pixel(px, py)

    def rect(self, x, y, w, h):
        self.fill(x, y, w, 1)
        self.fill(x, y + h - 1, w, 1)
        self.fill(x, y, 1, h)
        self.fill(x + w - 1, y, 1, h)

    def invert(self, x, y, w, h):
        for py in range(max(0, y), min(64, y + h)):
            for px in range(max(0, x), min(128, x + w)):
                self.bits[py][px] = not self.bits[py][px]

    def panel(self, x, y, w, h, inverse=False):
        cut = T['panelChamfer']
        for py in range(h):
            inset = max(0, cut - min(py, h - py - 1))
            for px in range(inset, w - inset):
                if inverse:
                    self.bits[y + py][x + px] = not self.bits[y + py][x + px]
                elif py == 0 or py == h - 1 or px == inset or px == w - inset - 1:
                    self.pixel(x + px, y + py)

    def numeral(self, x, y, text):
        scale = NUMERALS['metricScale']
        for char in str(text):
            for py, bits in enumerate(NUMERALS['rows'][char]):
                for px in range(NUMERALS['width']):
                    if bits & (1 << (NUMERALS['width'] - px - 1)):
                        self.fill(x + px * scale, y + py * scale, scale, scale)
            x += NUMERALS['advance'] * scale

    def text(self, x, y, text, mini=False):
        for char in str(text):
            if mini:
                rows, columns, mask, advance = MINI.get(char, [0] * 5), 3, 4, 4
            else:
                code = ord(char) if 32 <= ord(char) <= 126 else 63
                rows, columns, mask, advance = FONT[(code - 32) * 10:(code - 31) * 10], 7, 128, 7
            for py, bits in enumerate(rows):
                for px in range(columns):
                    if bits & (mask >> px):
                        self.pixel(x + px, y + py)
            x += advance

    def centered(self, y, text, mini=False, left=0, width=128):
        w = len(str(text)) * (4 if mini else 7) - (1 if mini and text else 0)
        self.text(left + (width - w) // 2, y, text, mini)

    def icon(self, name, x, y, box):
        name = SPEC['iconAliases'].get(name, name)
        w, h, data = ICONS[SPEC['icons'][name]]
        ratio = min(1, box / max(w, h))
        dw, dh = max(1, int(w * ratio)), max(1, int(h * ratio))
        x, y = x + (box - dw) // 2, y + (box - dh) // 2
        for py in range(dh):
            for px in range(dw):
                sx, sy = px * w // dw, py * h // dh
                if data[sy * ((w + 7) // 8) + sx // 8] & (1 << (sx % 8)):
                    self.pixel(x + px, y + py)

    def header(self, icon, text, page=''):
        self.icon(icon, T['edge'], 0, T['headerIconBox'])
        self.text(T['edge'] + T['headerIconBox'] + T['iconGap'], 2, text)
        if page:
            self.text(126 - len(page) * 7, 2, page)
        if T['headerStyle'] == 'inverse-band':
            self.invert(0, 0, 128, T['headerHeight'])

    def metric_panel(self, value, unit, secondary_label, secondary_value):
        y, h, w = T['dashboardBodyY'], T['dashboardBodyHeight'], T['dashboardHeroWidth']
        number_width = (len(str(value)) * NUMERALS['advance'] - 1) * NUMERALS['metricScale']
        self.numeral((w - number_width) // 2, y + 4, value)
        self.centered(y + 20, unit, left=0, width=w)
        self.panel(0, y, w, h, inverse=True)
        sx, sw = w + T['dashboardPanelGap'], T['dashboardSecondaryWidth']
        self.centered(y + 4, secondary_label, left=sx, width=sw)
        self.centered(y + 19, secondary_value, left=sx, width=sw)
        self.panel(sx, y, sw, h)

    def status_panel(self, x, icon, state, label, emphasis=False):
        y, w, h = T['dashboardBodyY'], 62, T['dashboardBodyHeight']
        self.icon(icon, x + 4, y + 3, 14)
        self.text(x + 22, y + 5, state)
        self.centered(y + 20, label, left=x, width=w)
        self.panel(x, y, w, h, inverse=emphasis)

    def quick_action_card(self, card, index, count, selected=0):
        self.header(card['icon'], card['title'], f'{index + 1}/{count}')
        self.panel(0, T['quickCardY'], 128, T['quickCardHeight'])
        first = max(0, selected - SPEC['quickActions']['visibleRows'] + 1)
        for row, action in enumerate(card['actions'][first:first + SPEC['quickActions']['visibleRows']]):
            y = T['quickCardY'] + 1 + row * T['quickRowHeight']
            self.icon(action['icon'], T['quickRowIconX'], y + 2, T['quickRowIconBox'])
            self.text(T['quickRowTextX'], y + 3, action['label'])
            if action.get('available', True) is False:
                self.text(116, y + 3, '!')
            if first + row == selected:
                self.panel(T['quickFocusX'], y + 1, T['quickFocusWidth'], T['quickFocusHeight'], inverse=True)
        self.footer()

    def hint(self, x, y, long=False):
        box, gap = T['footerIconBox'], T['iconGap']
        self.icon('input.encoder.long' if long else 'input.encoder', x, y, box)
        if long:
            self.icon('nav.back', x + box + gap, y, box)
        else:
            self.text(x + box + gap, y + (box - 10) // 2, 'OK')

    def footer(self, ok=True, back=True):
        if ok:
            self.hint(2, T['footerY'])
        if back:
            self.hint(126 - (2 * T['footerIconBox'] + T['iconGap']), T['footerY'], True)

    def progress(self, fraction=None):
        x, y, w, h, pad = 2, 29, T['progressWidth'], T['progressHeight'], T['progressInset']
        self.rect(x, y, w, h)
        iw, ih = w - 2 * pad, h - 2 * pad
        if fraction is None:
            self.fill(x + pad + iw // 3, y + pad, iw // 4, ih)
        else:
            self.fill(x + pad, y + pad, int(iw * fraction), ih)

    def sensors(self, values, maximum):
        self.header('sensor.group', 'IR')
        self.text(126 - len(str(maximum)) * 4, 4, maximum, True)
        self.rect(*T['sensorFrame'])
        n = len(values)
        bw = min(12, 124 // n - 6)
        for i, value in enumerate(values):
            left, right = 2 + i * 124 // n, 2 + (i + 1) * 124 // n
            self.centered(T['sensorIdY'], str(i + 1), True, left, right - left)
            x, y, h = left + (right - left - bw) // 2, T['sensorBarY'], T['sensorBarHeight']
            self.rect(x, y, bw, h)
            if value is not None:
                fill = int(max(0, min(1, value / maximum)) * (h - 2))
                self.fill(x + 1, y + h - 1 - fill, bw - 2, fill)
            self.centered(T['sensorValueY'], '--' if value is None else str(value), True, left, right - left)
        self.footer(ok=False)


def diagrams():
    cards = []
    o = Oled()
    o.header('system.core', 'Iconos')
    for row, (icon, label) in enumerate([('actuator.servo', 'Servo'), ('system.core', 'Sistema')]):
        y = 16 + row * 16
        o.icon(icon, 2, y, 16)
        o.text(22, y + 3, label)
    o.footer()
    cards.append(('01 ICONOS CONSISTENTES', 'Mismo recurso / separacion 4px', o))
    o = Oled()
    o.header('input.encoder', 'Gestos')
    o.text(2, 20, 'Corto')
    o.text(72, 20, 'Largo')
    o.hint(2, 33)
    o.hint(76, 33, True)
    cards.append(('02 PARES ALINEADOS', 'Corto OK / mantener + retorno', o))
    o = Oled()
    o.header('system.core', 'Control', '1/3')
    for i, (icon, label) in enumerate([('actuator.servo', 'Angulo'), ('system.core', 'Sistema')]):
        y = 16 + i * T['rowHeight']
        if i == 0:
            o.rect(0, y, 128, 16)
        o.icon(icon, 2, y, 16)
        o.text(22, y + 3, label)
        if i == 0:
            o.text(120, y + 3, '>')
    o.footer()
    cards.append(('03 MENU DESPLAZABLE', 'Dos filas y pie / mismo MenuRow', o))
    o = Oled()
    o.header('actuator.servo', 'Posicion', '2/3')
    o.metric_panel('90', 'GRADOS', 'PWM', '1500us')
    o.footer()
    cards.append(('04 DASHBOARD SERVO', 'Lectura principal / dato secundario', o))
    for known in [True, False]:
        o = Oled()
        o.header('tool.calibration' if known else 'network.wifi', 'Calibrar' if known else 'Conectar')
        o.centered(17, 'Muestras' if known else 'Esperando')
        o.progress(0.65 if known else None)
        if known:
            o.centered(39, '65%')
        o.footer(ok=False)
        cards.append(('05 PROGRESO CONOCIDO' if known else '06 CARGA INDETERMINADA', 'Misma barra 124x8 / cambia relleno', o))
    for title, note, values, maximum in [
        ('07 CINCO SENSORES', 'Numero arriba / valor abajo', [182, 320, 850, 300, 160], 1023),
        ('08 OCHO SENSORES', 'Todos juntos / numeros mini3x5', [24, 512, 1830, 4095, 3240, 99, 1580, 2401], 4095),
        ('09 VALIDEZ DEL DATO', 'Cero valido / -- sin lectura', [0, None, 832, None, 215, 3600, 4095, 120], 4095)]:
        o = Oled()
        o.sensors(values, maximum)
        cards.append((title, note, o))
    o = Oled()
    o.header('status.warning', 'Atencion')
    o.centered(21, 'Sin datos')
    o.centered(36, 'Revisar sensor')
    o.footer()
    cards.append(('10 AVISO DE FAMILIA', 'Misma cabecera y mismo pie', o))
    cards.extend(dashboard_diagrams()[::2])
    cards.extend(quick_action_diagrams())
    return cards


def dashboard_diagrams():
    cards = []
    o = Oled()
    o.header('system.core', 'Resumen', '1/3')
    o.status_panel(0, 'actuator.motor', 'OFF', 'MOTOR', emphasis=True)
    o.status_panel(66, 'network.wifi', 'STA', 'WI-FI')
    o.footer()
    cards.append(('RESUMEN / 1 DE 3', 'Dos estados / iconos vinculados', o))
    o = Oled()
    o.header('actuator.servo', 'Posicion', '2/3')
    o.metric_panel('90', 'GRADOS', 'PWM', '1500us')
    o.footer()
    cards.append(('SERVO / 2 DE 3', 'Lectura protagonista / pulso secundario', o))
    o = Oled()
    o.header('actuator.motor', 'Velocidad', '3/3')
    o.metric_panel('150', 'MM/S', 'PWM', '40')
    o.footer()
    cards.append(('MOVIMIENTO / 3 DE 3', 'Consigna de ejemplo / PWM de ejemplo', o))
    return cards


def quick_action_diagrams():
    cards = SPEC['quickActions']['cards']
    result = []
    for index, card in enumerate(cards):
        oled = Oled()
        selected = 1 if index == 1 else 0
        oled.quick_action_card(card, index, len(cards), selected)
        result.append((f"CARD {index + 1}/{len(cards)} / {card['title'].upper()}",
                       'Titulo arriba / franja de seleccion', oled))
    return result


def draw_board(cards, filename, columns=2, scale=3, title='OLED128-4 / COMPONENTES'):
    panel_width, panel_height, gap, margin = 128 * scale, 64 * scale, 24, 24
    row_step = panel_height + 58
    rows = (len(cards) + columns - 1) // columns
    width = margin * 2 + panel_width * columns + gap * (columns - 1)
    height = 90 + rows * row_step + 30
    pixels = bytearray([13, 18, 27] * (width * height))

    def pixel(x, y, color):
        if 0 <= x < width and 0 <= y < height:
            index = (y * width + x) * 3
            pixels[index:index + 3] = bytes(color)

    def caption(x, y, text, color=(196, 212, 228), factor=1):
        for char in text:
            code = ord(char) if 32 <= ord(char) <= 126 else 63
            for row in range(10):
                for col in range(7):
                    if FONT[(code - 32) * 10 + row] & (128 >> col):
                        for dy in range(factor):
                            for dx in range(factor):
                                pixel(x + col * factor + dx, y + row * factor + dy, color)
            x += 7 * factor

    caption(24, 18, title, (230, 240, 250), 2)
    caption(24, 48, 'Diagramas de diseno - 128x64 - valores ficticios - sin integrar')
    for i, (title, note, oled) in enumerate(cards):
        x, y = margin + (i % columns) * (panel_width + gap), 90 + (i // columns) * row_step
        caption(x, y, title, (106, 205, 220))
        for py, row in enumerate(oled.bits):
            for px, on in enumerate(row):
                color = (228, 241, 246) if on else (0, 0, 0)
                for dy in range(scale):
                    for dx in range(scale):
                        pixel(x + px * scale + dx, y + 20 + py * scale + dy, color)
        caption(x, y + panel_height + 28, note)
    caption(24, height - 20, 'Borrador de diseno / sin integrar al firmware.')

    def chunk(kind, data):
        return struct.pack('!I', len(data)) + kind + data + struct.pack('!I', zlib.crc32(kind + data) & 0xffffffff)
    raw = b''.join(b'\x00' + bytes(pixels[y * width * 3:(y + 1) * width * 3]) for y in range(height))
    png = b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('!2I5B', width, height, 8, 2, 0, 0, 0))
    (ASSETS / filename).write_bytes(png + chunk(b'IDAT', zlib.compress(raw)) + chunk(b'IEND', b''))


def main():
    draw_board(diagrams(), 'component-atlas.png')
    draw_board(dashboard_diagrams(), 'dashboard-proposal.png', columns=3, scale=2, title='OLED128-3 / DASHBOARD')
    draw_board(quick_action_diagrams(), 'quick-actions-proposal.png', columns=3, scale=2, title='OLED128-4 / ACCIONES RAPIDAS')
    draw_board([diagrams()[1]], 'encoder-gestures-proposal.png', columns=1, scale=3, title='ENCODER / GESTOS')


if __name__ == '__main__':
    main()
