"""AFR2-20261031-09. Deterministic image conversion; not part of firmware builds.
Requires Pillow with WebP support. Preserves the original RGBA master in reference/.
"""
from pathlib import Path
from PIL import Image

root = Path(__file__).resolve().parent.parent
source = root / "reference/brand/Logo-neon-robot-source.png"
image = Image.open(source).convert("RGBA")
out = root / "public/brand"
out.mkdir(parents=True, exist_ok=True)
for edge, name in [(1024, "Logo-robot-1024.webp"), (256, "Logo-robot-256.webp"), (64, "favicon-robot.webp")]:
    resized = image.copy()
    resized.thumbnail((edge, edge), Image.Resampling.LANCZOS)
    if edge == 64:
        square = Image.new("RGBA", (edge, edge))
        square.alpha_composite(resized, ((edge - resized.width) // 2, (edge - resized.height) // 2))
        resized = square
    target = out / name
    resized.save(target, "WEBP", quality=88, method=6, alpha_quality=100, exact=True)
    print(f"{name}: {target.stat().st_size} bytes ({resized.width}x{resized.height}, alpha preserved)")

touch = image.copy()
touch.thumbnail((180, 180), Image.Resampling.LANCZOS)
square = Image.new("RGBA", (180, 180))
square.alpha_composite(touch, ((180 - touch.width) // 2, (180 - touch.height) // 2))
square.save(out / "apple-touch-icon-robot.png", optimize=True)
