"""Compone banners AFR2: degradado vertical tomado del render de Inicio + render RGBA de Blender.
Uso: python -I compose_banner.py <renders_dir> <out_dir> <modelo> [quality]"""
import sys, os
from PIL import Image, ImageDraw, ImageFilter

RENDERS, OUT, MODEL = sys.argv[1], sys.argv[2], sys.argv[3]
QUALITY = int(sys.argv[4]) if len(sys.argv) > 4 else 86
# Paradas medidas en home-hero-1600.webp (borde izquierdo): 0 %, 25 %, 50 %, 75 %, 100 %.
STOPS = [(0.0, (23, 30, 39)), (0.25, (63, 77, 91)), (0.5, (127, 141, 155)), (0.75, (150, 162, 174)), (1.0, (156, 168, 177))]

def gradient(w, h, glow_x):
    img = Image.new('RGB', (w, h))
    px = img.load()
    for y in range(h):
        t = y / (h - 1)
        for i in range(len(STOPS) - 1):
            (t0, c0), (t1, c1) = STOPS[i], STOPS[i + 1]
            if t0 <= t <= t1:
                k = (t - t0) / (t1 - t0); c = tuple(round(a + (b - a) * k) for a, b in zip(c0, c1)); break
        for x in range(w): px[x, y] = c
    # Halo suave detrás del robot, como el estudio del render original.
    glow = Image.new('L', (w, h), 0)
    ImageDraw.Draw(glow).ellipse((glow_x - h * .9, h * .05, glow_x + h * .9, h * 1.25), fill=60)
    glow = glow.filter(ImageFilter.GaussianBlur(h * .25))
    return Image.composite(Image.new('RGB', (w, h), (190, 200, 212)), img, glow)

os.makedirs(OUT, exist_ok=True)
for name, size, glow in (('wide', (1200, 500), .70), ('narrow', (640, 400), .5)):
    render = Image.open(os.path.join(RENDERS, f'{MODEL}-{name}.png')).convert('RGBA').resize(size, Image.LANCZOS)
    base = gradient(size[0], size[1], size[0] * glow).convert('RGBA')
    out = Image.alpha_composite(base, render).convert('RGB')
    label = '1200' if name == 'wide' else '640'
    path = os.path.join(OUT, f'model-{MODEL}-{label}.webp')
    out.save(path, 'WEBP', quality=QUALITY, method=6)
    print(path, os.path.getsize(path))
