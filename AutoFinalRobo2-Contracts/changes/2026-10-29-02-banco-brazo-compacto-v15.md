# AFR2-20261029-02 — Banco mecanico compacto v15

## Motivo y cambios

El presupuesto v14 necesitaba piezas concretas para evitar una reduccion de
masa solo nominal. Se crea un documento Fusion independiente del maestrov09,
con dos eslabones55/50mm de mejillas de aluminio2mm, ventanas, bordes y
separadores. Se estudia J2 con transmision5:1 y una pinza accionada porSTS3032.

La primera pinza simetrica concretada pesaba207g y se descarta. La nueva usa
una mordaza fijaU y una movil con paralelogramo, cable directo y tambor4mm.
Se escalonan bielas y pernos para evitar cruces; los apoyos impresos son
continuos, con tornilleria pasante, tuercas, diarios redondos, ranuras y
segurosDIN6799 de referencia. El eje4mm falla el filtro combinado conservador
de flexion/torsion y pasa a6mm con aceroRp>=600MPa requerido, sumando unos11g.
Se centran pads sobre el cubo50 y se incorpora un topeM3 ajustable cerca49.5mm.

## Evidencia y limites

[Informe y datos](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v15/README.md)
y [modelo nativo](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v15/AFR2_Compacto_v15_Modulos_mecanicos.f3d).
Masas calculadas desde volumen/material y catalogos, CG en coordenadas del
banco; no son mediciones ni el peso final integrado sobre el auto.

La revision CAD solicitada es local a la pinza y muestreada en cinco aperturas
50..60mm. El resultado concreto se conserva en `revision_pinza.json`; no
acredita recorrido continuo, tolerancias, cable/muelle reales ni resto del brazo.
El filtro de agarre depende de friccion/eficiencia supuestas y no cierra el
contacto distribuido con momentos. No hubo FEA ni ensayo fisico.

El primer reductorJ2 modulo0.8 exige50.6MPa en el filtroLewis y no se acepta
paraPLA PrintaLot FDM. Modulo1.25 reduce el requisito a20.7MPa, pero cambia
centros y masa: requiere otra geometria y soportes mas livianos. Es propuesta
analitica, no montaje terminado. No se mantienen como aprobados1.17kg/2.65 de
v14 sin integrar las nuevas masas. Vuelco, traccion, demas articulaciones,
curvas de frenado, acoples25T y compra/entrega aArgentina siguen pendientes.

## Estados compartidos

CAD: banco de modulos editable, no ensamble final ni fabricacion autorizada.
atmega/esp/web/Loader: sin cambios ni nuevas comprobaciones de software.
No se ejecutaron tests, compilaciones, flasheos ni pruebas de hardware.
Los despieces y planos finales siguen diferidos por indicacion humana.
