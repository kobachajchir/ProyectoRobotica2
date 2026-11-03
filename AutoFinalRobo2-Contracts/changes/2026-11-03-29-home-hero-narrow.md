# AFR2-20261103-29 — Banner de Inicio en anchos chicos

03/11/2026. Pedido humano: con la pantalla chica los rellenos y márgenes del banner de Inicio cambian y el contenido queda corrido.

## Causa
Entre 840 px y unos 1100 px el banner mantenía una proporción fija de 2,4:1 con `overflow: hidden`. La altura resultante era menor que la del texto (título de 3 líneas, descripción y chips), que se recortaba contra el borde inferior. Además el título escalaba con el ancho de la ventana sin descontar la barra lateral.

## Cambios en Web/src/styles
- `models.css`: desde 840 px el banner pasa de proporción fija a `min-height` según el ancho útil (aprox. 2,4:1, entre 300 y 460 px); la altura sigue al contenido. El texto se centra en vertical y se quita el relleno duplicado de ese bloque (queda el de `visual-language.css`, igual en los cuatro lados).
- `visual-language.css`: desde 840 px el título escala con `(100vw − 360px) / 17`, es decir con el ancho real del banner, y ya no se corta.

## Estado por repo
- **atmega / esp / loader:** sin cambios.
- **web:** fuente modificada. Verificado en el navegador local a 900 px (sin recorte ni desbordes, relleno parejo) y a 1440 px (proporción 2,30:1). No verificado: 840–1000 px intermedios, tablet vertical ni compilación TypeScript. El diseño de menos de 840 px no cambió.
- Sin commit.
