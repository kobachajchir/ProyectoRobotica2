# AFR2-20261102-18 — Reforma visual del radar y cuatro alcances coordinados

Pedido humano: aplicar el manual visual vigente al radar, mejorar su animación
y coordinar cuatro alcances nuevos entre ATmega y Web.

## Fuente implementada

- Web/src/features/sensors/SensorsPage.tsx: vista superior inicial, cabecera con
  estado y escala visual, medición principal con icono, unidades localizadas.
- Web/src/features/sensors/RadarScene.tsx: haz de 15 grados por capas, posición
  inicial directamente en el ángulo recibido, interpolación existente con delta
  acotado y pulso de anillo de 300 ms por eco nuevo válido dentro de la escala.
  Sigue frameloop demand; no hay barrido de reloj que simule datos ausentes.
  Movimiento reducido elimina la expansión del pulso; la aguja se actualiza
  directamente. Los puntos conservan posición medida y estado histórico.
- Web/src/styles/radar.css: paneles sólidos, tipografía por tokens, lectura
  principal destacada, zona de estado, visor delimitado y controles agrupados;
  composiciones responsive, leyenda y sector con jerarquía del manual 0.1.11.
- Web/src/styles/tokens.css: alias semántico `--radar-floor` al suelo del mapa.

Fuentes: manual 0.1.11, componentes y tokens actuales; dirección Confirmada por
pedido explícito, extensión de composición Propuesta. Cobertura de fuente en
`docs/design-system/coverage-20261102-18.json`. No verificada en ejecución.

## Alcances confirmados y aplicados

El usuario confirma **25 cm · 50 cm · 1 m · 2 m**. Web ofrece esas cuatro
etiquetas en orden ascendente. El menú OLED conserva el orden descendente y los
índices existentes: 0 = 2 m, 1 = 1 m, 2 = 50 cm, 3 = 25 cm. Se mantiene el
formato del campo persistido y no se escribe EEPROM automáticamente: una
preferencia existente conserva su índice y pasa a interpretar la nueva escala
(por ejemplo, índice 0: antes 4 m, ahora 2 m).

- `Atmega/src/ui/afr2_sensor_screens.c`: radio de escala `2000 >> índice`,
  aplicado tanto al radar como al editor compacto del servo. Etiquetas cortas
  `2m/1m/50cm/25cm`, manteniendo el buffer de cinco bytes.
- `Atmega/src/ui/afr2_menu_catalog.c`: opciones `2 m/1 m/50 cm/25 cm`.
- `Atmega/include/afr2_product.h`: significado de índices documentado.
- `Web/src/features/oled/firmwareSensorRenderer.ts`: mismo radio y etiquetas.
- `Web/src/features/oled/generated/firmwareLibrary.ts`: copia literal de las
  cuatro etiquetas del menú, sin modificar bytes de fuentes o iconos.
- `Web/src/features/oled/firmwareRenderer.sources.json`: procedencia actualizada
  de los dos C portados; sus metadatos coincidentes también se actualizan en
  la copia de biblioteca. No se ejecutó el generador ni un validador.
- `Web/src/features/sensors/SensorsPage.tsx`: selector y badge usan cm para las
  dos escalas cortas y metros para las otras dos. Cálculos internos en metros.

El contrato de esta extensión está en `contracts/RADAR_SCALES_20261102.md`.
Los límites de validez HC-SR04 no cambian: son independientes de la escala
gráfica. No se añaden comandos ni cambia el transporte ESP.

## Estado real

atmega: fuentes de escala y menú actualizadas. esp: sin cambios. web: reforma
visual y cuatro alcances implementados, con copia OLED vinculada.
Sin tests, compilación, validadores, navegador, ejecución del generador OLED,
promoción, puertos ni flasheos. Sin commit/publicación. Firmware y paquete
instalados no se actualizan por esta edición de fuentes.
