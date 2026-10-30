# J2: motor fijo, reduccion 4:1 y apoyo externo

Montaje integrado y revisado30/10/2026. Las fases realmente aplicadas figuran en
`progreso_J3.jsonl`; el ultimo exportado esta en `estado_cad_v17.json`.
Ver REVISION_MONTAJE_J2.md para alcance de la revision CAD muestreada;
no es aprobacion de resistencia fisica ni de fabricacion.

Se conserva el hombro a `tower_h` y su eje8mm sobre dos608. El STS3250
permanece en la torre, debajo del hombro: su masa no se lleva al codo.
La pinzaV7 y la correa de J3 no se reemplazan. No se eleva la montura.

## Transmision

18/72dientes, modulo0.8, presion20grados, centros36mm. EngranajesAl2024,
cara5mm y cubos7mm; referencias comerciales en REFERENCIAS_REDUCCION_J2.md.
Se reconstruyen sus perfiles, no se cambia solamente la cantidad de un
patron anterior. Los chaveteros2mm son adaptaciones de mecanizado propias.

El pinon usa eje5mm sobre dosSKF625-2Z; la rueda usa eje8mm del hombro.
Horn comercial25T, cuatroM3PCD14 y brida torneada solidaria al eje5.
El tornillo central del horn se monta antes de cerrar la brida.
El pinon no descarga su fuerza radial directamente sobre el servo.

Coordenadas respecto al conjunto de giro de base, enmm:

| Elemento | Intervalo axialY | Observacion |
|---|---:|---|
| Travesano impreso |24.5..32.5|Una pieza con A01|
| SeparadoresAl |32.5..39.5|Cuatro,7mm|
| Placa verticalAl del modulo |39.5..42|Alojamiento comparte diseno conJ3|
| 625delantero |41..46|Eje de entrada|
| Separador delantero |46..48|Al,agujero5.1|
| Caras dentadas |48..53|Pinon y rueda|
| Cubos |53..60|Chavetas,prisionero solo auxiliar|
| 625trasero |60.1..65.1|0.1mm de juego junto a cubo|
| Separador trasero |65.2..69.1|0.1mm de juego junto a625|
| Brida integral del eje |69.1..73.1|OD21mm, acero condicionadoRp>=600MPa|
| Horn comercial |73.1..77.6|PCD14, plano base estriado77.4|

Eje de entrada aZ=`tower_h-gear_centers`. El travesano termina16.5mm
encima del eje; deja0.5mm respecto a la tapa608 del hombro, en el diseno
nominal. Esa separacion exige verificar tolerancias y poses nativas.

## Montaje y parametrizacion

CuatroM4x25ISO7380, centrosX+-17 yZ=-8/+10respecto al eje de entrada.
Tres arandelasDIN125traseras por tornillo, una delantera y tuercaDIN934.
No se propone cortar un tornillo comercial a una longitud especial.
La envolvente de cabeza baja es una referencia: confirmar el lote comprado.

Motor, alojamiento, placaAl, tapa,625 y piezas del rotor se reutilizan
como instancias de los componentes deJ3. Sus interfaces rigidas son
origenes nativos parametrizados, con compensacion de `j3_motor_z` y
`j3_transverse_shift`: tensarJ3 no debe desplazar el motor fijo deJ2.
Las masas comerciales se cuentan por ocurrencia, no por definicion comun.
Los patrones de anclajesM4 perdieron referencias nativas y se retiraron.
Se usan cuatro instancias fijadas al padre con paso fijo34x18mm; su
geometria comparte v17_j2_input_z y sigue la altura del montaje.

J2 conserva limites-10..125grados. Reduccion4requiere540grados del servo.
El modo multivuelta y su referencia al encender siguen pendientes de
implementar en controlTTL. No equivale a posicion absoluta por PWM.

Despues del montaje: extraer cuerpos/masas, revisar contactos con J2 en
ambos extremos y con codo/muneca plegados, comprobar eje/engranajes y
recalcular pares. No calcular vuelco ni ejecutar estudio en nube.
