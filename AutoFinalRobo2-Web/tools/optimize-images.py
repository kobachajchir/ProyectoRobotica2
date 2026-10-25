"""AFR2-20261024-03. Deterministic image conversion; not part of firmware builds.
Requires Pillow with WebP support. Preserves the original RGBA master in reference/.
"""
from pathlib import Path
from PIL import Image

root = Path(__file__).resolve().parent.parent
source = root / "reference/brand/Logo-transparent-source.png"
image = Image.open(source).convert("RGBA")
out = root / "public/brand"
out.mkdir(parents=True, exist_ok=True)
for edge, name in [(1024, "Logo-1024.webp"), (256, "Logo-256.webp"), (64, "favicon.webp")]:
    resized = image.resize((edge, edge), Image.Resampling.LANCZOS)
    target = out / name
    resized.save(target, "WEBP", quality=88, method=6, alpha_quality=100, exact=True)
    print(f"{name}: {target.stat().st_size} bytes ({edge}x{edge}, alpha preserved)")
