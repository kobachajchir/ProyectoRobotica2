# Logos de la aplicacion

## Emblema vigente — AFR2-20261031-09

`Logo-neon-robot-source.png` es el PNG RGBA entregado por el usuario como nuevo
logo oficial. Se conserva sin modificar, tambien como `assets/Logo.png` en la
raiz del proyecto. Los derivados activos son `Logo-robot-768.webp` (antes 1024, reducido por el presupuesto del paquete ESP en AFR2-20261110-18),
`Logo-robot-256.webp`, `favicon-robot.webp` y `apple-touch-icon-robot.png` bajo
`public/brand`. La conversion mantiene proporciones y transparencia; no genera
ni reinterpreta el dibujo. Color WebP con perdida, alpha conservado.

`tools/optimize-images.py` reproduce las versiones de uso. El componente
BrandMark las comparte entre navegacion, Inicio y Desarrollador; la bienvenida
usa la version grande. Los nombres nuevos renuevan la cache del emblema.

## Referencias anteriores preservadas

El texto siguiente describe la entrega anterior; `assets/Logo.png` ahora contiene
el nuevo emblema. Los archivos de referencia antiguos se conservan.

Logo-original.png conserva íntegro el PNG opaco anterior (también en D:/ProyectoRobotica2/assets/Logo.png). El activo Web ahora es public/brand/Logo-transparent.png, derivado con image_gen para quitar el fondo; no se presenta como identidad de píxeles exacta. El original se retira de public para no empaquetar ambos PNG en LittleFS. Procedencia: Contracts/provenance/web-startup.json.
