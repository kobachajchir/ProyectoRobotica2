# AFR2-20261031-12 — Interfaz del Simulador UI

Pedido humano: mejorar la UI del simulador de pantallas de Desarrollador.
Se conserva el desarrollo local01 y se integra completo junto con esta mejora.

La herramienta aparece antes del perfil. Cabecera, equipo de ejemplo y reinicio
preceden al area de pantalla/encoder y a la guia contextual. Seleccion, modo,
gesto y requisitos tienen jerarquia propia; el scroll de opciones sigue al
encoder dentro de la lista. Catalogo filtrable por nombre/codigo y explicacion
de alcance se agrupan en un Disclosure. Las vistas sin opciones tienen ayuda.

Reutiliza OLED128x64, catalogo compartido, EncoderControl y los tres gestos.
No agrega botones Menu/Atras, comandos al equipo ni recursos de dibujo.
La captura conectada sigue siendo remote-snapshot; local-ui-simulation identifica
valores ficticios. El motor local de01 se conserva sin cambios.

VIS-047: manual0.1.9 autorizado y habilidades Emil/Apple/identidad; la skill
local de pantallas conserva contratos y raster. Fuente y estilos implementados;
aspecto/interaccion en ambos temas y dispositivos No verificados. Sin tests,
builds, validadores, navegador ni flasheo. atmega/esp/Loader sin modificaciones;
dist/artifacts/ESP no promovidos.

[Comparacion y uso](../../AutoFinalRobo2-Web/docs/modules/ui-simulator.md).
[Cobertura](../docs/design-system/coverage-20261031-12.json).
