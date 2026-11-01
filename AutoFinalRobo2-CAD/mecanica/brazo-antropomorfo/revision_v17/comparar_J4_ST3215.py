"""Comparacion previa solicitada por el usuario, NO piezas ya modeladas.

Sustituir solo M04 por56g con envolventeST3215C047 y reservar20g extra de
montaje; no descontar ahorros de una horquilla de aluminio aun no hecha.
"""
import importlib.util
from pathlib import Path
import numpy as np
H=Path(__file__).resolve().parent
sp=importlib.util.spec_from_file_location('cad17',H/'calcular_cad_v17.py');c=importlib.util.module_from_spec(sp);sp.loader.exec_module(c)

def adjust(raw,parts,groups):
    out=[b for b in parts if b['component']!='V02_M04_MG90S_J4']
    dims=np.array([45.2,24.7,35])*.001;mass=.056
    I=mass/12*np.diag([dims[1]**2+dims[2]**2,dims[0]**2+dims[2]**2,dims[0]**2+dims[1]**2])
    out.append(dict(root='03_Antebrazo_PVC',component='PROPUESTO_M04_ST3215_C047',body='envolvente propuesta',mass_g=56,volume_cm3=0,material='Servo propuesto',basis='FichaST3215C047:55+1g; caja uniforme. No CAD nuevo.',cg_local_m=[.0461,.004,.0425],I_local_kg_m2=I.tolist()))
    out.append(dict(root='03_Antebrazo_PVC',component='RESERVA_housing_y_acople_J4',body='reserva de presupuesto',mass_g=20,volume_cm3=0,material='Reserva',basis='20g adicionales: no descontar el soporte anterior ni futuros ahorros.',cg_local_m=[.0461,.004,.0425],I_local_kg_m2=(I*20/56).tolist()))
    return out,c.regroup(out)

c.AJUSTAR_CAD=adjust;c.RESULT_PREFIX17='candidato_J4_ST3215_'
c.CANDIDATE_SCOPE17='ST3215C047 enJ4,56g +reserva20g; conservar todas las otras masasCAD actuales. Par nominal10kgfcm a12V; verificar luego alojamientos, interferencias e inercia con CAD nuevo.'
if __name__=='__main__':c.main()
