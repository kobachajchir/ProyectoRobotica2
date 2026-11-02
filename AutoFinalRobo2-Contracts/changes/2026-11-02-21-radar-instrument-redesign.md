# AFR2-20261102-21 — Radar de la Web como instrumento

02/11/2026. Pedido humano: reformar la parte visual del radar, con un estilo más
lindo y más en línea con el manual, y revisar cómo se ve en todas las escalas.
Continúa AFR2-20261102-18 (alcances 25 cm · 50 cm · 1 m · 2 m), sin cambiar datos,
comandos ni escalas.

## Cambios (solo presentación)

- **Etiquetas HTML** en lugar de texto pintado en la escena: distancias de los
  anillos sobre la línea base, ángulos 0–180° fuera del arco y «90° · frente»
  destacado. Se proyectan desde la cámara solo cuando cambian alcance, vista o
  tamaño (no hay bucle de dibujo continuo). Usan tipografía y tokens del manual en
  claro y oscuro. Distancias legibles por escala (`formatRadarDistance`): cm por
  debajo de 1 m, m desde 1 m (6,25 cm … 2 m).
- **Plato** semicircular con brillo radial cerca del sensor, borde suave y grilla de
  puntos que se apaga afuera. Anillos principales con jerarquía (el exterior más
  marcado), anillos intermedios tenues, rayos cada 30°, línea base y marcas cada 5°,
  10° y 30°.
- **Sector de escaneo:** relleno rubí con borde exterior marcado y límites. En vista
  previa queda más tenue y el chip lo indica («Sin aplicar»).
- **Haz del sensor:** cono de ±9° con caída angular y radial, línea central con halo
  y punta luminosa. Sigue la interpolación existente; con movimiento reducido salta
  directo.
- **Ecos:** punto con aro del color de la superficie y halo; el último eco es más
  grande y tiene un pulso de 450 ms (ninguno con movimiento reducido). Una línea de
  contorno une ecos vecinos (≤ 10°) para leer la silueta. Los históricos se atenúan.
- **Escala uniforme:** puntos, líneas y punta escalan con el alcance, así un eco se ve
  igual en 25 cm que en 2 m. Desde 1 m el encuadre muestra el auto completo; en
  alcances cortos, solo su frente, para que mande el plato.
- **Superposiciones:**
  - un único chip «Sector de escaneo 0°–180°», que reemplaza las dos cajas anteriores
    («Frente 90°» ahora está sobre el plato);
  - leyenda con muestras de eco, último eco, sector y anillos, abajo a la izquierda
    para no tapar el auto;
  - estado vacío como tarjeta.
- **Celular:** visor de `clamp(17rem, 80vw, 22rem)`, que antes quedaba en 58svh con
  el radar chico. Solo dos etiquetas de anillo y leyenda en una fila.

## Adaptación por archivo

Web/src: `features/sensors/RadarScene.tsx` (reescrito), `features/sensors/SensorsPage.tsx`
(chip, leyenda e import de `formatRadarDistance`) y `styles/radar.css` (bloque
AFR2-20261102-21). Contracts: CHANGELOG.md y README.md.

## Estado por repo

- **atmega / esp:** sin cambios.
- **web:** fuente modificada. **Verificado en el navegador local**, sin robot, con
  ecos de muestra temporales solo de desarrollo (ya retirados del código):
  - alcances 25 cm, 50 cm, 1 m y 2 m en vista superior;
  - 50 cm en perspectiva;
  - claro y oscuro a 1440 px, 1000 px y 390 px, sin desplazamiento horizontal;
  - estado sin datos.

  No verificado: con el robot real (barrido, pulso y aguja con telemetría),
  compilación TypeScript, `build:esp` ni hardware.
- Sin commit.
