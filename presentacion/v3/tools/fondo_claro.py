# Fondos claros con manchas difusas teñidas de las cuatro ramas (base del estilo glass).
# Uso: python -I fondo_claro.py <salida_dir>
import sys, os
from PIL import Image, ImageDraw, ImageFilter

OUT = sys.argv[1] if len(sys.argv) > 1 else '.'
os.makedirs(OUT, exist_ok=True)
W, H = 1920, 1080
BASE = (246, 248, 252)
MEC, ELE, CTL, PRG = (59, 130, 246), (245, 158, 11), (139, 92, 246), (16, 185, 129)


def fondo(name, blobs, blur=170):
    img = Image.new('RGB', (W, H), BASE)
    layer = Image.new('RGBA', (W, H), (0, 0, 0, 0))
    d = ImageDraw.Draw(layer)
    for (x, y, r, c, a) in blobs:
        d.ellipse((x - r, y - r, x + r, y + r), fill=(*c, a))
    layer = layer.filter(ImageFilter.GaussianBlur(blur))
    img.paste(layer, (0, 0), layer)
    img.save(os.path.join(OUT, name), optimize=True)
    print('OK', name)


fondo('bg_claro.png', [(1750, 80, 330, MEC, 70), (1880, 900, 300, CTL, 55), (120, 1040, 320, PRG, 50), (60, 120, 260, ELE, 40)])
fondo('bg_hero.png', [(1500, 300, 520, MEC, 85), (1750, 900, 420, CTL, 75), (300, 950, 420, PRG, 60), (200, 150, 360, ELE, 55)], blur=200)
