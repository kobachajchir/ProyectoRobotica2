# Brazo v17: arquitectura V7 y codo por correa

**Documentación 09/11/2026, AFR2-20261109-13:** [Libro complementario de 224 hojas y seis tomos](laminas_20261109/README.md): 66 definiciones fabricadas/adaptadas, incluidas las 30 impresas; interfaces, cortes, BOM y despieces de seis conjuntos. [Primer borrador del informe](../../../../AutoFinalRobo2-Contracts/docs/informe_20261109/README.md), separado del maestro. Documentación nominal desde la copia F3D del 02/11; ajustes y liberación de taller pendientes, sin modificar la geometría.

Entrega mecanica 3D, AFR2-20261029-03. **Montajes, ensambles y limites
completados; no aprobado para fabricar.** Se conserva el antropomorfo y la
pinza V7; se descartan la pinza compacta v15 y el banco alternativo v16.

Modelo seleccionado:
[AFR2_Brazo_v17_Base_V7_J3_Remoto.f3d](AFR2_Brazo_v17_Base_V7_J3_Remoto.f3d).
Los maestros v07/v09 no fueron modificados. El archivo Correa_J2 de esta
carpeta es la copia inicial historica, no el modelo seleccionado.

## Geometria aplicada

Ejes70/60mm, PVC25x1.2 y20x1, cortes42/28.5mm. Motor J3 junto a J2,
solidario al primer eslabon; correa HP3M210x6, poleas20/50 de aluminio
representadas como discos a diametro primitivo para calcular masa.
No son perfiles de dentado aptos para fabricar.

Tensor con cuatroM4 en ranuras oblongas4.5x12.5mm y ajuste finoM3.
Motor, acople, eje de entrada y ambos625 se desplazan juntos. Se mantienen
eje de codo fijo y recorridos de las articulaciones. Se reconstruyeron
encastres de hombro/codo y se corrigieron pasos de tornilleria, asientos
de rodamientos y holguras con la tapa estacionaria del hombro.

Se completaron los montajes de los dos servos pequenos STS3032 A0,
con horns, tapas, tornilleria pasante y tuercas accesibles.
Los montajes y fases realmente aplicados constan en `progreso_J3.jsonl` y
el ultimo guardado/exportado en `estado_cad_v17.json`. Una fase fallida
no cuenta como geometria aplicada aunque conserve mensajes de operaciones.
Tambien se completo el montaje nativo de J2: STS3250 fijo en la torre,
engranajes18/72modulo0.8 de aluminio, eje5 con dos625, chavetas y cuatroM4.
Comparte componentes conJ3. J5 ya tiene STS3250, cuna integrada, tapa con
tope trasero desmontable, horn y cuatro M3 avellanados. J4 integrado con
cuna, tapa, fijaciones y horn al pinon24. J1 integrado con cuna en la
montura baja, cartucho6000 conservado, horn adaptado y union al eje.
El journal identifica las fases aplicadas y los guardados.
Los [ensambles y limites](ENSAMBLES_Y_MOVIMIENTO.md) se registran tambien
en `ensambles_y_limites_3D.json`: 47 uniones y 5 vinculos de movimiento.
La entrada auxiliar de J3 queda sin topes adicionales: el codo conserva
su limite de -90 a +90 grados y el vinculo conserva la relacion2.5.

## Prioridad actual: entrega 3D

Por pedido del usuario se termina solamente la geometria, los ensambles
y los limites de movimiento. Los nuevos calculos de cargas, masas,
actuadores, tensiones, vuelco y el estudio de aligerado quedan diferidos
hasta que el usuario indique retomarlos. Las cifras siguientes pertenecen
al checkpoint anterior y no certifican el montaje final.

## Evidencia y limites

- [Seleccion de actuadores](SELECCION_ACTUADORES.md): STS3250 en J1..J5,
  STS3032 en J6/pinza; J2=4, J3=2.5. Preseleccion con pieza200g y masas
  nuevas/reservas, margen minimo1.272. Presupuesto2495g sin carga; no
  equivale a CAD integrado final. El CAD actual ronda2352g sin carga.
- [Revision local de J3](REVISION_CONTACTOS_J3.md): tres posiciones,
  cuatro choques corregidos;13 contactos brutos clasificados por pose.
  No representa revision continua del conjunto completo.
- [PVC comercial](SELECCION_PVC.md) y [tensor](TENSOR_CORREA.md).
- [MontajeJ2](MONTAJE_J2_STS3250.md): interfaces y correcciones del
  travesano, apoyos y fijaciones. [RevisionJ2](REVISION_MONTAJE_J2.md):
  nueveposes y28posiciones del hombro,13contactos clasificados por muestra.
- [Informe explicativo](INFORME_PESOS_Y_ESFUERZOS_v17.md): masas,CG,
  metodo de calculo, esfuerzos y motivo de las iteraciones.
- [Shape Optimization diferido](PENDIENTE_OPTIMIZACION_MATERIAL.md):
  herramienta identificada en la instalacion; sin estudio resuelto.

Por ultima instruccion del usuario **no se continua calculando vuelco**.
Los archivos de estabilidad anteriores son antecedentes superados y no
deben ejecutarse como paso de continuacion. Los50ms son una hipotesis de
frenada, no una parada fisica comprobada ni un maximo nominal del servo.

La preseleccion actual usa `geometria_v17.json`, cuyo SHA figura en sus
resultados. `geometria_pre_actuadores_v17.json` conserva la base historica
anterior a adaptar los dos servos pequenos.
Las masas de PLA se calculan como volumen macizo por densidad; faltan
perfil/orientacion de impresion y comprobacion resistente del conjunto.
Los nuevos STS requieren busTTL y alimentacion12V/6V: la electronica del
CAD anterior no representa una conexion electrica adaptada.

Sin tests de software, compilaciones, flasheos, FEA resuelto ni pruebas
fisicas. Solo CAD y calculos mecanicos expresamente solicitados. Los
despieces y laminas tecnicas finales siguen diferidos.

## Fuentes editables

`modelar_transmision_J3.py`, `reformar_v17.py` y
`montar_actuadores_v17.py` contienen fases parametricas explicitas;
`ejecutar_fase_J3.py` las envia al MCP local de Fusion. No lanzar todas
las fases a ciegas: el journal impide repetir las aplicadas. Un timeout
no cancela una operacion de Fusion.

`extraer_v17.py` obtiene volumen, centros de masa e inercias nativas;
`calcular_cad_v17.py` y `preseleccionar_actuadores_integrados.py` calculan
masas y cargas externas. Los scripts historicos de vuelco quedan inactivos.
Referencias y recuperacion de catalogos en los documentos de seleccion.

## Punto recuperable30/10

El F3D publicado guarda J2/J3 y pinza revisados. La sincronizacion masiva
de materiales fisicos termino despues del timeout, sin reiniciar Fusion,
y ya esta guardada/exportada. PLA/Al/PVC y masas comerciales coinciden
con el calculo externo: brazo mas pieza200g =2552.323g, excluyendo auto y
referencias80/81/82. Los materiales equivalentes SOLO MASA/NO FEA no
acreditan propiedades resistentes; ver materiales_fisicos_asignados.json.
Las fases de J1/J4/J5 estan preparadas en sus scripts, TODAVIA NO APLICADAS.
No relanzar una mutacion por timeout: consultar journal y estado nativo.
