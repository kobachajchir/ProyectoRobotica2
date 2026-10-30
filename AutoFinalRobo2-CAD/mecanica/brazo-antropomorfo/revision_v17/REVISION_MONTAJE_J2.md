# J2 y J3: cierre de correcciones mecanicas30/10

Se conserva el brazo V7, pinza de dos cremalleras, PVC25/20 y ejes70/60.
J2 lleva STS3250,18/72modulo0.8, eje5 sobre dos625 y rueda sobre eje8.
J3 mantiene su motor junto al hombro y la correa20/50. No se elevaron la
base ni los ejes. J1/J4/J5 nuevos siguen pendientes: no es brazo terminado.

## Correcciones aplicadas

- CarcasaSTS3250 contrastada con STEP: eje12.5mm desde centro de caja,
  extremos10.11/35.11mm. Horn comercial19.95x4.5 y4M3PCD14.
- Brida21x4 y eje5 torneados en una pieza, horn montable sin tornillo
  central atrapado. Eje/horn/apoyos reales enJ2 yJ3.
- CuatroM4 con separadoresAl7mm. Se sustituyen patrones nativos que
  perdian referencias por instancias fijadas a la torre. El paso34x18mm
  es fijo; la altura se mueve con `v17_j2_input_z` en todas las piezas.
- Alojamientos9.4mm en el dorso: arandelas y cabezas ya no invaden la torre.
- Travesano limitado aX29mm: despeja el encastre del codo aJ2=125grados,
  conservando7.5mm desde el borde de la arandela hasta el borde recortado.
- Puente625 acortado0.5mm: se elimina invasion0.2mm de esquinas del servo.
- Ventana posterior27x21 para conectoresTTL. Corte nativo por herramienta
  y combinacion, con retirada de volumen comprobada; no queda corte fallido.
  La reserva es inferida del dibujo: confirmar plug y radio de cable reales.
- Caras planas en ejes de collares: las puntasM2/M3 apoyan sobre plano,
  sin penetrar acero. Las chavetas conservan la transmision principal.

## Revision geometrica muestreada

`interferencias_montaje_J2.json`:9poses, incluyendo hombro-10/125grados,
codo+-90, muneca combinada+-90, pinza cerrada y abierta180grados.
`interferencias_barrido_J2.json`:28posiciones de hombro cada5grados entre
-10 y125, con codo y muneca en cero y agarre50mm. Poses restauradas.

Los13resultados brutos por pose se conservan completos. Clasificacion:

| Cantidad | Contacto | Interpretacion |
|---:|---|---|
|4|M2 contra pilotos1.6 de tapas625J2/J3|Envolventes de rosca; no modelado helicoidal|
|2|Correa contra discos primitivos20/50|Representacion conservadora sin dientes comerciales|
|3|Prisionero contra rosca de collar|Envolventes de rosca; puntas contra eje ya despejadas|
|1|M2 contra piloto del pinon18|Envolvente de rosca|
|3|Disipador contra borneraXL4016|Simplificacion interna de modulos heredados|

No aparecen contactos entre engranajes18/72, rodamientos/ejes, servo y
carcasa, arandelas y soporte, ni travesano y encastre en esas muestras.
Esto no demuestra trayectorias continuas ni todas las combinaciones de6ejes.
LosXL4016 son referencias antiguas: la alimentacion12V/6V sigue pendiente.

El historial actual no tiene errores ni impresos con multiples solidos
desconectados. Conserva4avisos anteriores:2taladros sin cuerpo en el viejo
MG90SdeJ4 y2reservas de arandelaA11 que terminan al ras. No son avisos nuevos
deJ2. Los resultados anteriores a las correcciones permanecen identificados.

## Masas y par

CAD actual aproximadamente2352g sin pieza, PLA macizo rho1.24 y masas
comerciales por ocurrencia. Propuesta con J1/J4/J5 y reservas restantes:
2495g, carga200g, margen minimo externo1.272 enJ3. Ver SELECCION_ACTUADORES.md
y JSON conSHA de geometria. No confundir propuesta completa con CAD actual.
Las densidades nativas ya estan sincronizadas: materiales equivalentes
de motores/rodamientos/electronica son SOLO MASA, excluidos de FEA.

No se calculo vuelco, no se resolvio Shape Optimization/FEA ni se hicieron
tests de software, compilaciones, flasheos o ensayos fisicos.
