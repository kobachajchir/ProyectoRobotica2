"""Publicar el libro completo y las dos fichas generales como PDF separado."""
from pathlib import Path
import importlib.util,json
H=Path(__file__).resolve().parent
root=H.parents[4]
composer=root/'AutoFinalRobo2-Contracts/.agents/skills/fusion-technical-sheets/scripts/componer.py'
spec=importlib.util.spec_from_file_location('laminas',composer);m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
book=json.loads((H/'libro.json').read_text(encoding='utf-8'))
m.compose(book,H/'AFR2_Laminas_Iniciales_A3.pdf')
fichas={**book,'sheets':[s for s in book['sheets'] if s.get('render_cards')]}
(H/'libro_fichas.json').write_text(json.dumps(fichas,indent=2),encoding='utf-8')
m.compose(fichas,H/'AFR2_Fichas_Brazo_y_Robot_A3.pdf')
print(json.dumps({'book_sheets':len(book['sheets']),'overview_sheets':len(fichas['sheets'])}))
