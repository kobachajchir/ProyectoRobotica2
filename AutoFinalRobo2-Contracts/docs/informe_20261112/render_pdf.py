"""Render documental del PDF y planchas de lectura para revisión visual."""
from pathlib import Path
import pypdfium2 as pdfium
from PIL import Image, ImageDraw, ImageFont
HERE=Path(__file__).resolve().parent
pdf=pdfium.PdfDocument(str(HERE/'entrega/AutoFinalRobo2_Informe_Proyecto_D2.pdf'))
out=HERE/'revision_visual';out.mkdir(exist_ok=True)
for i,page in enumerate(pdf):
    im=page.render(scale=1.5).to_pil();im.save(out/f'pagina-{i+1:02}.png')
font=ImageFont.truetype('C:/Windows/Fonts/arial.ttf',22)
for start in range(0,len(pdf),4):
    sheet=Image.new('RGB',(1800,2600),'#DDE3EA');draw=ImageDraw.Draw(sheet)
    for j in range(4):
        idx=start+j
        if idx>=len(pdf):break
        im=Image.open(out/f'pagina-{idx+1:02}.png');im.thumbnail((865,1235))
        x=20+(j%2)*900;y=35+(j//2)*1300
        sheet.paste(im,(x,y));draw.text((x,y-27),f'Página {idx+1}',fill='#182B42',font=font)
    sheet.save(out/f'planchas-{start+1:02}-{min(start+4,len(pdf)):02}.jpg',quality=90)
print(f'{len(pdf)} páginas PDF renderizadas.')
