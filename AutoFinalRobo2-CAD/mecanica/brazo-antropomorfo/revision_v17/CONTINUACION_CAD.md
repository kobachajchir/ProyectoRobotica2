# Continuacion v17: alcance y estado real

## Alcance vigente

29/10/2026. El usuario suspendio nuevos calculos de vuelco. No volver a
preguntar por50ms frente a300ms para estabilidad: esa pregunta fue superada.
Concentrarse en actuadores para200g mas peso propio y en terminar montajes.
Revisar Shape Optimization; el estudio de aligerado sigue sin resolver,
diferido. No usar ahorros hipoteticos para hacer pasar los actuadores.
Conservar pinzaV7 y antropomorfo; no retomar pinzas v15/v16 descartadas.
Sin tests/builds/flasheos. CAD y calculos mecanicos expresamente pedidos.
Commit/push de trabajo propio autorizados, preservar adjuntos ajenos.

## Modelo y herramientas

Activo: AFR2_Brazo_v17_Base_V7_J3_Remoto. F3D del mismo nombre en esta carpeta.
Maestros v07/v09 intactos. El otro F3D Correa_J2 es solo copia inicial
historica. Conexion MCP local127.0.0.1:27182/mcp, cliente revision_v15.
Runner ejecutar_fase_J3.py, timeout1200; un timeout no cancela la mutacion.
No repetir fases aplicadas. Fusion revierte la transaccion completa si
un script falla: sus mensajes de operaciones no prueban que se aplicaran.
Leer progreso_J3.jsonl y estado_cad_v17.json, no suponer por este texto.

## Aplicado y guardado

J3 remoto sobre primer eslabon junto aJ2, poleasAl20/50=2.5, HP3M210x6,
centros50.4672mm, entrada con dos625. Carro completo4M4 en oblongas4.5x12.5,
ajusteM3, anclajesM4x40, eje/acople/chavetas/collares/retenciones.
Transmision desplazada10mm hacia-Y, sin elevar base. Eje de codo fijo.
AuxiliarT_J3_Correa_2a1 tiene nombre heredado, relacion real2.5 y+-225deg.

Ejes70/60, PVC25x1.2 y20x1, cortes42/28.5. A05/A06/A11 reconstruidos,
asientos608 completos enA06/A12, taladrosX del codo corregidos.
Oreja negativaA01 tiene corredor y diagonal fuera del barrido muestreado.
La correa se reconstruyo con centros vinculados: no usar el sketch viejo.
Para reemplazar piezas usar removeFeatures sobre ocurrencia, no deleteMe
solo: antes quedaban ocurrencias historicas duplicadas activas.

Dos servos pequenos sustituidos por STS3032 A0:21.6g cada uno. Nombres
historicosMG90S conservados en los componentes, atributos indicanSTS3032.
Soportes y tapas deJ6/J7 reconstruidos; la fase de pilares, horns y
herrajes debe comprobarse en el journal antes de declararla terminada.

## Seleccion antes de modelar los otros actuadores

SELECCION_ACTUADORES.md y preseleccion_actuadores_integrados.json:
STS3250J1..J5,75.5g; STS3032J6/pinza,21.6g. J2=4 con18/72modulo0.8,
centros36mm, engranajesAl2024SDP-SI. J3=2.5, J4=1. Conserva todas las
masas estructurales, agrega reservas de montajes y10g cableado distal.
Presupuesto2424g sin pieza. Margenes1.348/1.356/1.274/1.579/1.517/12.33.
1760 estados,30deg/s cada eje y frenada cubica50ms,900deg/s2pico: hipotesis,
no maximos nominales ni respuesta fisica certificada del servo.

Geometria actual y SHA en geometria_v17.json y resultados.
La base anterior sigue preservada en geometria_pre_actuadores_v17.json. No confundir
estos contrafactuales con actuadores ya integrados en CAD. No reutilizar
el presupuesto1.17/1.45kg como masa final: CAD anterior daba2207g.

PinzaV7 conSTS3032 necesita friccion efectiva>=0.516 para margen1.2 en
el caso dinamico. No esta probado el agarre de los pads.
STS requieren busTTL y12V/6V; PCA9685 anterior no sirve como interfaz.
Cuenta multivuelta no persiste al apagar; referencia de arranque pendiente.

## Pendientes nativos

Horns y fijaciones de los dos pequenos terminados y guardados.
Se revisaron siete poses mas dos aproximaciones al tope exacto180.
Ver REVISION_PINZA_Y_J6.md; quedan13contactos clasificados por pose.
Los tornillos de unionJ6 ahora sonM2x4avellanados enY+-6, noM2x5debajo.
Base2.6,tapa3,tuercasDIN439M2bajas1.2 y bolsillos abiertos por debajo.
Se reabrieron paso del ejeD y prisionero tras agregar material al piso.
act_mini_refuerzo_pilares separa centrosY a+-8.3 y pilares a6.3..10.3:
conserva0.85mm de pared junto a agujero2.3 y orejas del motor. No7.7mm.
Horns18mm, disco2/cubo2.5,8M2PCD12,25T4.95,M2.3x5 central asentado en
su rebaje1.2mm. El material de los agujerosPCD11 se reconstruyo al final del historial.
No suprimir cortes antiguos: rompen referencias cruzadas heredadas.
J6 final: tornillosM2x4avellanados enY+-6 desde la brida hacia las roscas
del horn. Los pasosX+-6 quedan sin tornillos. Ver INTERFACES_HORN.md.
J7: pinon con paso central4.8 ya despeja cabeza, evitar corte4.6 vacio.

Despues: integrarJ1/J2/J4/J5 con motores nuevos y montajes reales.
J2: rehacer18/72modulo0.8, no solo cambiar parametro del patron viejo.
Usar dos625, eje5 de entrada, chavetas y retencion; no todo el par enM2.
J4: candidato ejeX35.0999,Y8, centro36; fase7.5deg+2atan2(8,35.0999).
Mover cuerpo y origen del pinon solidariamente. Conservar24/24 y1:1.
J5: conservar ejes/625 y ruedaJ4 integrada enA15; no borrarA15 entera.
J1: conservar altura baja y apoyos6000; adaptar motor sin alzar montaje.
No estan aplicadas propuestas de bajar hombro, colas/contrapesos o azimut
limitado para estabilidad; esas rutas ya no son objetivo.

## Revision solicitada y cierre

Revision localJ3 tras cuatro correcciones:13contactos en tresposes,
clasificados en REVISION_CONTACTOS_J3.md; bruto conservado. No sinchoques
universal. Nuevos montajes pequenos necesitan su propia revision.
revisar_geometria_v17.py permite gripper_absolute_deg por muestra y
exclude_payload para cerrar pinza sin atravesar el cubo de referencia.
Restaura poses. Scopearm excluye80/81/82;82 es envolvente, no obstaculo.

Extraer geometria e historial activos tras terminar. Actualizar masas,
recalcular par con montajes reales, clasificar contactos concretos y
revisar aperturas0/50/79.4mm y extremos de muneca. No calcular vuelco.
Guardar/exportar, registrar estados reales en ContractsAFR2-20261029-03
y publicar con GitLFS. No llamar terminado al brazo si faltan actuadores.
No generar aun despieces/laminas finales. Shape Optimization preparado
sin estudio resuelto: ver PENDIENTE_OPTIMIZACION_MATERIAL.md.

## Resultado actualizado de esta continuacion

Los dos montajes pequenos estan completos y guardados. Revision en siete
poses mas dos al extremo:13contactos clasificados por pose; cierre4.007mm,
agarre50.000mm y apertura exacta180deg=79.405mm. Se restauran poses.
Actual359cuerpos,2240.34g sin carga. Preseleccion restante conreservas:
2423.94g y margenes1.348/1.356/1.274/1.579/1.517/12.33. Ver informes nuevos.
No volver a usar la preseleccion anterior2437g como dato actualizado.

Se recupero STEPcomercialSTS3250de2021 desdeRobotopian, conURL/SHA en
referencia_STEP_STS3250.json. Se inspecciona solo en documento temporal;
no insertarlo ciegamente ni asumir identidad con ficha2024. La primera
figura dehorn es25T5.9,total4.5,disco2.5 y4M3PCD14. La segunda figura
es un accesorio liso6.05,espesor3.1: no confundirlas. INTERFACES_HORN.md.


## Actualizacion30/10: J2 cerrado, pasos que siguen

J2 ya integrado y guardado: motorSTS3250,18/72Al2024, eje5 con dos625,
horn19.95/PCD14, brida torneada, chavetas, retencion y4M4 con separadores7.
37muestras CAD (9poses+28hombro cada5grados),13contactos por muestra;
poses restauradas. Ver REVISION_MONTAJE_J2.md. Esos13son4pilotosM2,
2correa/disco,3roscas de collar,1pilotoM2pinon y3referenciasXL4016.
Se eliminaron penetraciones reales de puntas con planos sobre ejes.
Historial0errores/4avisos antiguos, ningun impreso desconectado.
CAD2352.323g, presupuesto2495.423g sin pieza, min1.2722 enJ3; escenario
200g/30deg/s/50ms hipotetico, sin nuevo vuelco. SHA antes de sincronizar
materiales:d875f22625c7b32e9ea549d50cd17d6920ecb197de27bca92666fe499d5cbd86.

Hallazgos API importantes:
- transform2 de ocurrencia NATIVA compartida devolvio contexto del primer
  montaje. Proxy completo createForAssemblyContext + inversa del padre
  muestra ubicacion correcta. No volver a mover la geometria por ese dato.
- Patrones de componentes anidados perdieron referencias. Se retiraron;
 4instancias fijadas al padre con paso fijo34x18mm usan altura nativa
  v17_j2_input_z en el propio cuerpo. No restaurar patrones fallidos.
- ExtrudeCut sobre housing compartido dioNoTarget con puntos dentro del
  cuerpo. Se usa ExtrudeNewBody+CombineCut de herramienta explicita,
  se consume herramienta y se exige retirada real de volumen/unico solido.
- Travesano se recorto aX29 y cajeados9.4dorso; ya no choca a125grados.
  Puente625 acortado0.5 para esquinas de carcasa. VentanaTTL27x21 real.

Materiales: inicialmente390cuerposSteel/21PLA, aunque el calculo externo
ya usaba densidades correctas. Se lanzo act_sts_materiales_base para
sincronizar en Fusion:PLA1.24,Al2.70/2.78,PVC1.45 y equivalentes de masa
comercial. Materiales equivalentes NO sirven para FEA. Referencias del
AGV80/81/82 excluidas y conservadas. La comprobacion nativa suma brazo
+pieza200g, no masa del auto ni envolventeHCSR.

Scripts preparados PERO TODAVIA NO APLICADOS de J5/J4/J1:
montar_J5_STS3250_v17.py, montar_J4_STS3250_v17.py,
montar_J1_STS3250_v17.py. Integraciones con parametros/origenes y fases.
Secuencia propuesta despues de guardar/publicar J2:
J5_parametros,motor,housing,tapa,pasantes,fijaciones,horn,tornillos_horn.
J4_parametros,motor,housing,tapa,pasantes,fijaciones,horn,union.
J1_parametros,motor,housing,cartucho,horn,adapter,union.
Cada nombre lleva prefijoact_sts_. Revisar estado tras cada fase y guardar.

J5 conserva bridaY32 y625, motor outputY36.3/cuerpo33.7..68.7. CuatroM3
PCD14 desdeY27.85 con cabezasISO7380, paso frente22.6. Cuna integradaA15.
J4 conserva24/24 y1:1; centro36 enXsqrt(36^2-8^2),Y8; fase7.5+2atan(Y/X).
Casebottomlink2-32.4,toplink2+2.6; cunaA12 integrada con puentesx12..24.69
Zlink2-13..-10; tapa con despejeR19.5 para tapa608. Eje de pinon aZlink2+6,
hornfrente64.3,separador1.7,cuatroM3x8 y cabezaM3central libre por6.2.
J1 conserva altura96 y cartucho6000. Motoroutput59.15,bottom26.75,
cap61.75; rebajar SOLO underside cartucho hasta61.75. Hormfrente63.45
coincide adaptador; horn comercial adaptado2agujeros3.4 y cajeados6 para
cabezas desde abajo, cuerpo de cubo minimo1.05mm radial a verificar.
Prearmar horn+adaptador fuera del motor, M3central precolocado; llave2.5
larga por taladro3.4 del eje. Caja6.2x3.2 para cabeza central. Ver faseunion
para secuencia; introducir conjunto superior sobre motor ya en la cuna.

Falta comprobar choques y masa de estos montajes; no dar sus scripts como
CAD aplicado. Despues extraer/recalcular, materiales_final (no repetirbase
ya aplicado), guardar/exportar, revision nativa final y GitLFS. Sin tests de
software ni estudio ShapeOptimization/FEA. ControlTTL12V/6V sigue pendiente.
