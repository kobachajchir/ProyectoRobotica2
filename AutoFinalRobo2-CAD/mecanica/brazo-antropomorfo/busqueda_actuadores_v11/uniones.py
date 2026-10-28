"""Mismas secciones y ecuaciones v10; nuevos conjuntos de masas v11."""
from pathlib import Path
H=Path(__file__).resolve().parent
src=(H.parent/'preseleccion_actuadores_v10'/'uniones.py').read_text(encoding='utf-8')
assert src.count("'analisis-actuadores-v10'")==1
src=src.replace("'analisis-actuadores-v10'","'analisis-actuadores-v11'")
old="FM=np.c_[F+fixedF,M+np.cross([0,p['v05_axis_y'],p['base_top']],F)+fixedM]"
new="extraF=np.array([0,0,-.200*9.80665]) if str(name)=='G_reforzado_206g_GX' else np.zeros(3)\n    "+old.replace('F+fixedF','F+fixedF+extraF')
assert src.count(old)==1
src=src.replace(old,new)
exec(compile(src,str(H/'uniones_heredadas_v10.py'),'exec'))
