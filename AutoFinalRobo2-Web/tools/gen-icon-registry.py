"""Genera AutoFinalRobo2-Web/src/components/ui/icons/registry.ts desde lucide-static@0.460.0.
Convierte circle/rect/line/polygon a un unico atributo d por elemento. Uso: python -I gen_registry.py <dir_svg> <salida>"""
import re, sys, os

SRC, OUT = sys.argv[1], sys.argv[2]

def num(v):
    f = float(v)
    return ('%g' % f)

def rect(a):
    x, y = float(a.get('x', 0)), float(a.get('y', 0))
    w, h = float(a['width']), float(a['height'])
    rx = float(a.get('rx', a.get('ry', 0)))
    if not rx:
        return f"M{num(x)} {num(y)}h{num(w)}v{num(h)}h{num(-w)}Z"
    r = num(rx)
    return (f"M{num(x+rx)} {num(y)}h{num(w-2*rx)}a{r} {r} 0 0 1 {r} {r}v{num(h-2*rx)}"
            f"a{r} {r} 0 0 1 -{r} {r}h{num(-(w-2*rx))}a{r} {r} 0 0 1 -{r} -{r}v{num(-(h-2*rx))}a{r} {r} 0 0 1 {r} -{r}Z")

def circle(a):
    cx, cy, r = float(a['cx']), float(a['cy']), float(a['r'])
    return f"M{num(cx-r)} {num(cy)}a{num(r)} {num(r)} 0 1 0 {num(2*r)} 0a{num(r)} {num(r)} 0 1 0 {num(-2*r)} 0"

def convert(tag, a):
    if tag == 'path': return a['d']
    if tag == 'circle': return circle(a)
    if tag == 'rect': return rect(a)
    if tag == 'line': return f"M{a['x1']} {a['y1']} {a['x2']} {a['y2']}"
    if tag in ('polygon', 'polyline'):
        pts = a['points'].split()
        pairs = [f"{pts[i]} {pts[i+1]}" for i in range(0, len(pts), 2)]
        return 'M' + ' '.join(pairs) + ('Z' if tag == 'polygon' else '')
    raise ValueError(tag)

def load(name):
    text = open(os.path.join(SRC, name + '.svg'), encoding='utf-8').read()
    out = []
    for tag, attrs in re.findall(r'<(path|circle|rect|line|polyline|polygon)\s([^>]*)/>', text):
        out.append(convert(tag, dict(re.findall(r'([\w-]+)="([^"]*)"', attrs))))
    return out

# Nombre semantico AFR2 -> icono Lucide. Los nombres historicos se conservan para no cambiar llamadas.
LUCIDE = {
    'home': 'house', 'control': 'gamepad-2', 'sensors': 'activity', 'radar': 'radar',
    'wifi': 'wifi', 'wifi-high': 'wifi-high', 'wifi-low': 'wifi-low', 'wifi-zero': 'wifi-zero', 'wifi-off': 'wifi-off',
    'cube': 'box', 'book': 'book-open', 'settings': 'settings-2',
    'panel-open': 'panel-left-open', 'panel-close': 'panel-left-close',
    'back': 'arrow-left', 'arrow': 'arrow-right', 'close': 'x', 'check': 'check',
    'stop': 'octagon-x',
    'arrow-up': 'arrow-up', 'arrow-down': 'arrow-down', 'arrow-left': 'arrow-left', 'arrow-right': 'arrow-right',
    'arrow-up-left': 'arrow-up-left', 'arrow-up-right': 'arrow-up-right',
    'arrow-down-left': 'arrow-down-left', 'arrow-down-right': 'arrow-down-right',
    'rotate-ccw': 'rotate-ccw', 'rotate-cw': 'rotate-cw',
    'play': 'play', 'pause': 'pause', 'square': 'square',
    'external': 'arrow-up-right', 'refresh': 'refresh-cw',
    'chevron-down': 'chevron-down', 'chevron-up': 'chevron-up', 'lock': 'lock',
    'success': 'circle-check', 'warning': 'triangle-alert', 'error': 'octagon-alert', 'info': 'info',
    'touch': 'pointer', 'trash': 'trash-2', 'undo': 'undo-2', 'redo': 'redo-2', 'board': 'cpu',
    'programs': 'route', 'setup': 'wrench', 'help': 'circle-help', 'more': 'ellipsis', 'plus': 'plus', 'minus': 'minus', 'crosshair': 'crosshair', 'move': 'move',
}

# Iconos propios del dominio: misma grilla 24 px, trazo 1,75 px, extremos redondeados (VIS-050).
OWN = {
    'arm': ['M4 21h12', 'M10 21v-3', circle({'cx': 10, 'cy': 16, 'r': 2}), 'M11.4 14.6 15.6 9.4',
            circle({'cx': 17, 'cy': 8, 'r': 2}), 'M18.4 6.6 21 4', 'M19 9.2l3 1.3'],
    'servo': [rect({'x': 3, 'y': 10, 'width': 15, 'height': 9, 'rx': 2}), 'M1.5 13.5h1.5', 'M18 13.5h1.5',
              circle({'cx': 14, 'cy': 10, 'r': 2}), 'M14 8V3.5', 'M11 3.5h6'],
    'wheel': [rect({'x': 5, 'y': 2.5, 'width': 14, 'height': 19, 'rx': 4}), 'M8.5 7 15.5 10', 'M8.5 11 15.5 14', 'M8.5 15 15.5 18'],
    'antenna': ['M12 10v11', 'M8 21h8', 'M8.5 6.5a5 5 0 0 0 0 7', 'M15.5 6.5a5 5 0 0 1 0 7',
                'M5.6 3.6a9 9 0 0 0 0 12.8', 'M18.4 3.6a9 9 0 0 1 0 12.8', 'M12 10h.01'],
    'screen': [rect({'x': 2, 'y': 5, 'width': 20, 'height': 12, 'rx': 2}), 'M6 9h8', 'M6 13h5', 'M9 21h6', 'M12 17v4'],
    'encoder': [circle({'cx': 12, 'cy': 14, 'r': 7}), 'M12 7V2', 'M10 3.5h4', 'M12 14l3-3'],
    'battery-unknown': [rect({'x': 2, 'y': 7, 'width': 17, 'height': 10, 'rx': 2}), 'M22 11v2', 'M8.5 12h4'],
}
for level in range(5):
    OWN[f'battery-{level}'] = [rect({'x': 2, 'y': 7, 'width': 17, 'height': 10, 'rx': 2}), 'M22 11v2'] + \
        [f'M{6 + 3 * i} 10v4' for i in range(level)]

icons = {k: load(v) for k, v in LUCIDE.items()}
icons.update(OWN)
names = sorted(icons)

lines = [
    '/**',
    ' * AFR2-20261101-07 / VIS-050 - Registro unico de iconos de la Web.',
    ' *',
    ' * Trazos de Lucide (https://lucide.dev), copiados de lucide-static@0.460.0 y',
    ' * convertidos a un atributo `d` por elemento (circle/rect/line/polygon -> path).',
    ' * Lucide: ISC License. Copyright (c) for portions of Lucide are held by Cole Bemis',
    ' * 2013-2022 as part of Feather (MIT). All other copyright (c) for Lucide are held by',
    ' * Lucide Contributors 2022. Permission to use, copy, modify, and/or distribute this',
    ' * software for any purpose with or without fee is hereby granted, provided that the',
    ' * above copyright notice and this permission notice appear in all copies.',
    ' *',
    ' * Iconos propios del dominio (arm, servo, wheel, antenna, screen, encoder, battery-*):',
    ' * AutoFinalRobo2, dibujados en la misma grilla de 24 px. Sin paquete de iconos en el bundle.',
    ' * Generado; para agregar un icono, sumar su trazo aqui con el mismo formato.',
    ' */',
    '',
    '/** Nombre semantico AFR2 -> icono Lucide de origen (documentacion; no se usa en runtime). */',
    'export const ICON_SOURCES = {',
]
for k in sorted(LUCIDE):
    lines.append(f"  '{k}': 'lucide:{LUCIDE[k]}',")
for k in sorted(OWN):
    lines.append(f"  '{k}': 'afr2',")
lines += ['} as const;', '', 'export const ICON_PATHS = {']
for k in names:
    body = ', '.join("'" + d + "'" for d in icons[k])
    lines.append(f"  '{k}': [{body}],")
lines += ['} as const satisfies Record<string, readonly string[]>;', '',
          'export type UiIconName = keyof typeof ICON_PATHS;', '']
open(OUT, 'w', encoding='utf-8', newline='\n').write('\n'.join(lines))
print(len(names), 'icons')
