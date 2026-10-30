"""Par y velocidad simultaneos de la preseleccion, sin nuevo calculo de vuelco.

Reutiliza picos externos ya calculados. La curva lineal STS3250 es una
aproximacion de la grafica del fabricante; no identifica inercia interna.
"""
from pathlib import Path
import json

H=Path(__file__).resolve().parent

def main():
    selected=json.loads((H/'preseleccion_actuadores_integrados.json').read_text())
    rows=[]
    for axis in selected['axes']:
      load=axis['external_peak_Nm']/(axis['ratio']*axis['assumed_efficiency'])
      row=dict(joint=axis['joint'],motor=axis['motor'],motor_required_torque_Nm=load,
          motor_required_torque_kgfcm=load/.0980665,
          motor_required_speed_deg_s=axis['required_servo_speed_deg_s'],
          rated_torque_margin=axis['margin'])
      if axis['motor']=='STS3250':
        # Fabricante75rpm sin carga y50kgfcm de bloqueo, ambos+-10%.
        # Se toma el extremo inferior de ambas magnitudes para la recta.
        speed=405*(1-load/(45*.0980665))
        row.update(steady_speed_estimate_deg_s=speed,
            estimated_speed_margin=speed/axis['required_servo_speed_deg_s'],
            interpretation='Filtro de velocidad por recta aproximada conservadora. No valida aceleracion/frenado real.')
      else:
        row.update(no_load_speed_deg_s=60/.09,
            interpretation='STS3032: no se atribuye la velocidad sin carga a la carga nominal; curva cargada pendiente.')
      rows.append(row)
    out=dict(source='preseleccion_actuadores_integrados.json',geometry_sha256=selected['geometry_sha256'],
        axes=rows,
        power=dict(rated_current_12V_A=5*1.4,stall_current_12V_A=5*4.2,
            rated_current_6V_A=2*.4,stall_current_6V_A=2*1.2,
            interpretation='Sumas de catalogo; no consumo medido ni dimensionamiento electrico final. La tolerancia superior de bloqueo STS3250 exige23.1A si se presupone bloqueo simultaneo de los cinco.'),
        status='Preseleccion externa con200g y masas nuevas/reservas. Montajes CAD incompletos. Sin vuelco, sin ensayos.')
    (H/'capacidad_actuadores_resultados.json').write_text(json.dumps(out,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
    print(json.dumps(rows))

if __name__=='__main__':main()
