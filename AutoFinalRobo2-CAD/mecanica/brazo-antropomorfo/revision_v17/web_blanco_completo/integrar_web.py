"""Mismo generador de descriptor, exportacion completa y presupuesto800k."""
from pathlib import Path
HERE=Path(__file__).resolve().parent
source=(HERE.parent/'web_blanco/integrar_web.py').read_text(encoding='utf-8')
source=source.replace('600000','800000').replace('20261031-v17-white','20261031-v17-complete')
exec(compile(source,'integrar_web_completo.py','exec'))
