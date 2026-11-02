# AFR2-20261102-07 — Reforma visual de Infraestructura

02/11/2026. Pedido humano sobre la página de AFR2-20261102-04: «está muy linda, está
bien organizada, me gusta el orden». Pide una reforma visual para hacerla más
llamativa y notoria, con animaciones y flujos; permite reformar el layout si se
mantiene la idea.

## Cambios (solo presentación; textos, datos y orden se conservan)

- **Portada:** fondo oscuro de banner (`--banner-*`) en ambos temas, con grilla
  tenue, halos y un brillo que respira. El `h1` de la página pasa a la portada:
  «Del diseño al robot, del robot a tu pantalla», con la segunda línea en degradado.
  Ya no se usa `PageHeader`. Chips: cantidad y peso de los GLB, «Una aplicación
  dentro del robot» y «Sin depender de un CDN».
- **Mapa de flujo animado:** tres zonas con color propio (computadora, ESP8266 y tu
  dispositivo; tokens nuevos `--banner-zone-*`), unidas por cables discontinuos que
  avanzan y por los que viajan «paquetes» con su etiqueta (`.bin`, `gzip`). Los
  íconos de los nodos pulsan. Cada nodo es un botón que abre su etapa en el recorrido
  y desplaza hasta él. Desde 1200 px el mapa va en una fila; entre 600 y 1199 px las
  zonas se apilan con los nodos en fila; en celular todo es vertical.
- **Recorrido en «línea de metro»:** estaciones numeradas sobre una línea con el
  color de la zona de cada etapa. Las anteriores muestran un tilde, la actual se
  agranda con un halo y la línea se completa hasta ella. Navegación con flechas,
  Inicio y Fin (`aria-current="step"`). En celular, solo los números, y la línea se
  desplaza en horizontal hasta la estación elegida sin mover la página. La ficha de
  la etapa entra con un desplazamiento breve: zona, ícono grande, descripción,
  «Entra → Sale» con un paquete que cruza, fuente y, a la derecha, «Lo que conviene
  saber» como tarjetas numeradas que aparecen en cascada.
- **Cuánto pesa el 3D:** barras proporcionales que crecen al entrar en pantalla.
  El conjunto se muestra como barra apilada auto + brazo, y el cubo tiene un ancho
  mínimo visible.
- **El paquete que se sirve:** barras «Antes / Guardado» con el porcentaje de
  reducción destacado, anillo de ocupación de LittleFS cuando el ESP lo informa
  (el `<progress>` se conserva para lectores de pantalla) y tarjetas con ícono. La
  tabla por archivo no cambia, salvo el resaltado al pasar el mouse.
- **Glosario:** los cuatro conceptos pasan de desplegables a tarjetas con ícono y
  el texto completo a la vista.
- **Movimiento:** las secciones aparecen una vez al entrar en pantalla. Con
  `prefers-reduced-motion` se detienen cables, paquetes, pulsos y brillo, y las
  entradas quedan como fundidos.

## Adaptación por archivo

Web/src: `features/infrastructure/InfrastructurePage.tsx`, `styles/infrastructure.css`,
`styles/tokens.css` (`--banner-zone-pc|esp|link|client`). Sin cambios en
`infrastructureFlow.ts`, `useInfrastructurePackage.ts` ni el contrato
`WEB_INFRASTRUCTURE.md`. Contracts: CHANGELOG.md y README.md.

## Estado por repo

- **atmega / esp:** sin cambios.
- **web:** fuente modificada. **Verificado en el navegador local** (sin robot, con
  el paquete local preparado; sin dato de ocupación del ESP):
  - a 1440 px en oscuro: portada, mapa en una fila con paquetes en movimiento,
    línea de metro, ficha, barras, compresión (45 % menos), tabla y glosario;
  - a 1000 px en claro: zonas apiladas (alto del mapa 716 px, antes 915) y sin
    desplazamiento horizontal;
  - a 390 px en claro: sin desplazamiento horizontal, las 8 estaciones visibles;
  - al tocar un nodo se abre su etapa («Modelar en Fusion», «Guardar en la flash»)
    y la sección queda debajo de la barra superior; al elegir una estación, la
    página no se mueve.

  No verificado: compilación TypeScript, `build:esp`, el anillo de ocupación con
  datos reales del ESP ni hardware.
