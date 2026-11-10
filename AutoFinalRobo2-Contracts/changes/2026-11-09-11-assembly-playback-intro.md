# AFR2-20261109-11 — Play/Pausa y presentación inicial de Armado

Versión: **1.12.0 (subversión)**.

Pedido humano: reproducción de20s junto a la barra de secciones, botón de pausa durante reproducción, secciones separadas para OLED/encoder y sensor analógico de batería, título inicial grande centrado que se desplaza al lateral con scroll y nuevo subtítulo.

Fuente implementada:

- Play avanza el mismo scroll nativo que controla la cámara, piezas y textos. La línea de tiempo completa dura20.000ms; al pausar conserva posición y reanuda con el tiempo proporcional restante. Al terminar se detiene y un nuevo Play comienza desde0.
- Scroll, gesto táctil, teclado o selección manual pausan. También se pausa al ocultar la pestaña o perder foco. Se cancela el RAF al desmontar. Sin nuevas dependencias ni reproducción automática al entrar. Botón deshabilitado durante carga de modelos.
- OLED/encoder y sensor analógico de batería tienen capítulos, títulos y accesos independientes, conservando el montaje y cableado existentes. Ahora15secciones.
- «De las piezas al movimiento» arranca grande y centrado. Durante el primer2,5% se desplaza y reduce hacia el lateral; desde allí empieza a descender la base y sigue el montaje. La preferencia de movimiento reducido evita esa transición espacial.
- Subtítulo exacto: «Un robot recolector que toma forma con cada gesto».

Archivos Web: `features/assembly/useAssemblyPlayback.ts`, `AssemblyPage.tsx`, `assemblyTimeline.ts`, `assemblyCar.ts`, `AssemblyScene.tsx` y `styles/assembly.css`. Versión compartida actualizada, sin modificar lógica ATmega/ESP por esta ficha.

Estado: Web=fuente sin ejecutar; ATmega/ESP=solo versión; CAD/Loader=sin cambios. Sin tests, compilaciones, validadores, navegador, renders, puertos ni flasheos por instrucción del usuario. No se declara duración observada, rendimiento ni validación visual;20s es el tiempo programado con performance.now/requestAnimationFrame y se pausa si la pestaña deja de estar activa.

## Ajuste posterior del mismo cambio, antes de cargar

Duración total ajustada a20s, incluidas las etiquetas del botón. La etiqueta roja de los capítulos OLED/encoder y sensor analógico dice «Nuestros agregados». El subtítulo conserva centrado cuando la introducción está centrada y pierde el margen automático al desplazarse al lateral, alineándose con el título. Versión1.12.0 conservada según regla3 de VERSIONADO.md. Sin tests, compilaciones, validadores ni navegador.

## Elevación final de la caja

Por corrección del usuario, la pose final pasa a J1/J2/J3/J4/J5/J6 =55/62/30/0/88/0 grados. Mantiene orientación descendente de la herramienta y apertura cerrada sobre la caja; se eleva mediante las articulaciones existentes. Durante la elevación final la cámara lleva gradualmente la caja al centro vertical de la imagen y conserva margen para el conjunto completo. Es coreografía de presentación, sin órdenes al robot ni comprobación de colisiones/cargas. Versión1.12.0 conservada como ajuste del mismo trabajo antes de cargar. Sin tests, builds, validadores o navegador.

## Duración final: 30 segundos

Por pedido posterior del usuario, el recorrido completo pasa a30.000ms. Etiquetas visibles y accesibles del Play actualizadas a30s; pausa y reanudación conservan el progreso. Sustituye la duración de20s indicada arriba. Ajuste de la misma entrega1.12.0, sin tests, compilaciones ni validaciones.

## Duración solicitada para narración técnica: 35 segundos

El usuario solicita35s para la presentación. Reproducción completa ajustada a35.000ms y etiquetas de Play a35s; conserva pausa/reanudación y avance proporcional. Sustituye30s. Mismo cambio1.12.0 antes de cargar; sin tests, compilación, validaciones ni ejecución en navegador.

## Sincronización con la toma de ElevenLabs entregada

El audio aportado dura 43,885714 s y reemplaza el temporizador fijo de 35 s. Play reproduce voz y escena con `audio.currentTime` como reloj: una espera de carga no adelanta la animación. Pausa, scroll, selección de capítulo, pérdida de foco y pestaña oculta detienen la voz; Play retoma desde el instante correspondiente al progreso. El final visual llega a 43,54 s y se conserva mientras termina la cola del audio.

Se incorpora `assemblyNarration.ts` con interpolación reversible entre segundos y progreso, basada en marcas de palabras de una transcripción local de la toma. La grabación describe ruedas antes de electrónica: se adelanta el montaje de ruedas y sus fijaciones, manteniendo el resto de conjuntos instalado. OLED/encoder comienza en 16,62 s, divisor resistivo en 21,96 s, montura en 26,995 s, base giratoria en 29,10 s, hombro/codo en 32,56 s, muñeca en 36,949 s y agarre desde 41,06 s. Los sensores y la placa superior conservan transiciones breves porque no tienen una frase dedicada en esta toma.

Audio servido desde `public/audio/armado-narracion.opus` (35.887 B, mono 6 kb/s), con alternativa AAC `armado-narracion.m4a` (138.706 B) para navegadores sin Opus. Se conserva velocidad y voz originales. Los originales de Downloads no se modifican. Dependencias de transcripción/conversión instaladas en el Python del usuario, sin incorporarlas a Web.

Versión 1.12.0 conservada como ajuste del mismo recorrido antes de cargar. Web=fuente y audio; ATmega/ESP/CAD/Loader=sin modificaciones por este ajuste. Sin tests, builds, validaciones, ejecución en navegador ni carga al ESP. Sincronización programada según marcas de voz; no se declara revisión audiovisual en navegador.
