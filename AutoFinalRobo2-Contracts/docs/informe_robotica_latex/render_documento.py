"""Render del documento para inspección editorial, sin ejecutar código de producto."""
from pathlib import Path
import sys
import pypdfium2 as pdfium
from PIL import Image,ImageDraw,ImageFont
sys.stdout.reconfigure(encoding='utf-8')
ROOT=Path(__file__).resolve().parent
OUT=ROOT/'revision_visual';OUT.mkdir(exist_ok=True)
pdf=pdfium.PdfDocument(str(ROOT/'salida/AutoFinalRobo2.pdf'))
for i,page in enumerate(pdf):
    page.render(scale=1.5).to_pil().save(OUT/f'pagina-{i+1:02}.png')
font=ImageFont.truetype('C:/Windows/Fonts/arial.ttf',22)
for start in range(0,len(pdf),4):
    sheet=Image.new('RGB',(1800,2600),'#DDE3EA');d=ImageDraw.Draw(sheet)
    for offset in range(4):
        index=start+offset
        if index>=len(pdf):break
        im=Image.open(OUT/f'pagina-{index+1:02}.png');im.thumbnail((865,1230))
        x=20+(offset%2)*900;y=35+(offset//2)*1300
        sheet.paste(im,(x,y));d.text((x,y-28),f'Página PDF {index+1}',font=font,fill='#19334B')
    sheet.save(OUT/f'planchas-{start+1:02}-{min(start+4,len(pdf)):02}.jpg',quality=92)
print(f'{len(pdf)} páginas renderizadas para revisión editorial.')
