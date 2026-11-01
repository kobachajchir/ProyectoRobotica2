"""Predimensionamiento mecanico del tensor, no ensayo ni masa final del brazo."""
from pathlib import Path
import json,math
H=Path(__file__).resolve().parent
data=json.loads((H/'predimensionado_resultados.json').read_text(encoding='utf-8'))
layout=next(row for row in data['rows'] if row['layout']=='junto_a_J2')
joint=layout['axes'][2]['peak_Nm']
ratio=2;eta=.8;minimum=1.2
input_torque=joint/(ratio*eta)
r=20*3/(2*math.pi) # mm
# Envelope elegida para dimensionar apoyos; no pretension recomendada.
F=200.0
rear=(-61.1-56.1)/2;front=(-42-37)/2;pulley=(-54-45)/2
span=front-rear;a=pulley-rear;b=front-pulley
Rrear=F*b/span;Rfront=F*a/span
M=Rrear*a
d=5.0
sigma=32*M/(math.pi*d**3)
tau=16*(input_torque*1000)/(math.pi*d**3)
vm=math.sqrt(sigma*sigma+3*tau*tau)
notch=3.0 # presupuesto conservador; requiere revisar radio de chavetero real
required_yield=minimum*notch*vm
# Tabla OMEGA HP3M a20rpm,20T,ancho9:6.4W; ancho6:0.61;
# longitud210:0.9; >=6 dientes engranados:1. Comparacion condicionada.
belt_torque_at_table=6.4*.61*.9/(2*math.pi*20/60)
out=dict(scope='Predimensionamiento del tensor con cargas del presupuesto70/60. No describe capacidad del brazo largo actualmente en Fusion ni tension real de montaje.',
 minimum_margin=minimum,joint_peak_Nm=joint,input_peak_Nm=input_torque,
 pulley_pitch_radius_mm=r,belt_tension_difference_N=input_torque*1000/r,
 assumed_radial_envelope_N=F,bearing_centres_y_mm=[rear,front],bearing_reactions_N=[Rrear,Rfront],
 SKF625_C0_N=380,pure_radial_static_bearing_margin=380/max(Rrear,Rfront),
 shaft_diameter_mm=d,bending_Nmm=M,nominal_bending_MPa=sigma,nominal_torsion_MPa=tau,
 nominal_von_mises_MPa=vm,assumed_keyway_concentration=notch,required_yield_MPa=required_yield,
 proposed_minimum_yield_MPa=600,conditional_shaft_margin=600/(notch*vm),
 belt_table_speed_rpm=20,commanded_input_speed_rpm=30*ratio/6,
 conditional_belt_torque_Nm=belt_torque_at_table,conditional_belt_margin=belt_torque_at_table/input_torque,
 limitations=['Pretension y fuerza radial no medidas;200N es una envolvente de calculo elegida.',
 'No hay analisis de fatiga, radio real de chavetero ni material certificado de eje.',
 'Tabla de correa a20rpm; operacion propuesta a10rpm y frenado requieren confirmacion del proveedor. No es aprobacion de correa generica.',
 'Recalcular con masas finales e inercias de rotores, cargas de frenado y conexiones reales.'])
(H/'calculo_previo_tensor.json').write_text(json.dumps(out,indent=2),encoding='utf-8')
lines=['# Predimensionamiento del tensor de J3','',
 'Estos numeros corresponden al presupuesto de eslabones de70/60mm y pinza V7 conservada. **No son resultados del brazo largo que todavia esta siendo modificado en Fusion.**', '',
 f'Par pico de J3: {joint:.4f} N m. Con reduccion2:1 y eficiencia0.8, el par exigido en la entrada es {input_torque:.4f} N m. Radio primitivo de la polea20T: {r:.3f} mm; diferencia entre tensiones de ramas: {out["belt_tension_difference_N"]:.1f} N.', '',
 f'Para predimensionar se impone una envolvente radial de200 N en la polea, no una tension de montaje. Con la polea entre dos apoyos separados{span:.2f} mm, las reacciones son{Rrear:.1f} y{Rfront:.1f} N. Frente a C0=380 N del SKF625-2Z, el cociente estatico radial es{out["pure_radial_static_bearing_margin"]:.2f}. No incluye carga axial ni vida de fatiga.', '',
 f'Eje5mm: momento flector maximo{M:.1f} N mm; tension flectora nominal{sigma:.1f} MPa; torsion nominal{tau:.1f} MPa; equivalente de von Mises{vm:.1f} MPa. Usando provisionalmente una concentracion3 por chavetero, el requisito de fluencia para margen1.2 es{required_yield:.1f} MPa. Se propone especificar acero certificado con Rp minimo600 MPa, que daria margen{out["conditional_shaft_margin"]:.2f} bajo estas hipotesis. No se atribuye esa resistencia a una varilla sin grado conocido.', '',
 f'La tabla Optibelt OMEGA HP3M,20T,20rpm, da una estimacion corregida de{belt_torque_at_table:.3f} N m para ancho6mm y longitud210mm. Cociente respecto a la demanda:{out["conditional_belt_margin"]:.3f}. **La entrada operaria a10rpm, por debajo de la tabla citada**: este cociente es condicional y no cierra el comportamiento de frenado ni la seleccion comercial. No sustituir por HTD generica usando esta capacidad.', '',
 'Faltan pretension de fabricante, comprobacion de alojamientos impresos, retenciones, radios de chavetero y las masas finales. No se hicieron ensayos fisicos ni tests de software.', '',
 'Fuentes: [catalogo SKF](https://cdn.skfmediahub.skf.com/api/public/0901d196802809de/pdf_preview_medium/0901d196802809de_pdf_preview_medium.pdf), [manual Optibelt](https://a.storyblok.com/f/192292/x/2d92ee5443/optibelt-tm-rubber-timing-belt-drives.pdf).']
(H/'CHEQUEO_PREVIO_TENSOR.md').write_text('\n'.join(lines)+'\n',encoding='utf-8')
print(json.dumps({k:v for k,v in out.items() if k not in ['limitations','scope']},indent=2))
