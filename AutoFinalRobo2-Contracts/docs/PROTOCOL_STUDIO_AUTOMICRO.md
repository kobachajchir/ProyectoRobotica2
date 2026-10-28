# Continuidad del Protocol Studio de AutoMicro

AFR2-20261028-10 · 28/10/2026. Pedido humano: mantener funcionalidad, ideas y
esencia de Protocol Studio, con presentación alineada a AutoFinalRobo2.

Cotejo **manual de código**, sin ejecutar ninguna de las dos aplicaciones.
Referencia directa de solo lectura:
`D:/AutoProyecto/Web ESP01/WebESP01AutoMicro2025/src/features/protocolStudio/`
y `src/pages/ProtocolSection.tsx`. La copia histórica en Web/reference conserva
sus autorías; no se actualizan registros, scripts ni fuentes de AutoMicro.

| Idea de AutoMicro | Adaptación AFR2 vigente |
|---|---|
| Constructor / Traductor, una tarea a la vez | TaskSections; Constructor inicial y Traductor / escáner |
| Origen y destino | Selector con las dos rutas ROBO2 permitidas, nodos3/1 |
| Catálogo por grupos y descripción | Siete familias de comandos y ficha del comando seleccionado |
| Campos por comando y payload automático | Formularios tipados de solicitudes conocidas, little-endian, constantes y reserved en cero |
| Payload manual que reemplaza parámetros | HEX avanzado con prioridad explícita; vaciar recupera el borrador guiado |
| Respuestas con dirección inversa y payload manual | Ruta13, resultado0..4 y cuerpo manual; prefijo agregado por constructor |
| Estructura visual por campos y detalles | Tarjetas con offsets ROBO2, bytes, notas y explicación desplegable |
| Bytes coloreados por función | Grupos de bytes con etiquetas y separación; acentos de tokens AFR2 |
| Cuatro salidas visibles y copiables | HEX, RealTerm, C y Python simultáneos; decimal adicional |
| Abrir / cargar paquete generado en Traductor | Acción disponible desde ambas tareas, sin perder los borradores |
| Nombre, CMD, HEX, arrays C y Python | Entradas conservadas; asignación Python y arrays mixtos decimal/0xHH |
| Escaneo automático de bloques largos o múltiples frames | Scanner común de todas las capturas, sin modo manual adicional |
| Mapa visual, paquetes seleccionables | Tarjetas por rango y detalle del bloque seleccionado |
| Stream continuo | Todos los bytes de todos los rangos en orden, con selección del bloque |
| Estado, payload y comprobaciones | Conteos, análisis de campos, observaciones y copia del payload/cuerpo |
| Detección de texto de arranque ESP | Reconocimiento literal de rst cause / boot mode, junto a binarios retenidos |
| Ayuda integrada | Modal compartido AFR2 con flujo, formatos y límites |

La primera adaptación01 ya tenía conversiones, framing y constructor manual.
La revisión02 recupera los formularios, el desglose visual del paquete, las
salidas simultáneas, el mapa seleccionable y la ayuda que faltaban para mantener
el flujo de AutoMicro. Los formatos de texto UTF-8/ASCII/Latin-1/escapado, los
límites de captura, los metadatos de monitor y la copia manual alternativa de01
se conservan.

## Diferencias necesarias del producto

- ROBO2/perfil2 y EX01v8 sustituyen UNER. Sesión, transacción, rutas válidas,
  máximo64 y evento BT01 se presentan con su formato actual. No se trasladan
  comandos STM/F4, rutas arbitrarias ni catálogo legado.
- Los formularios no sustituyen validación, negociación o límites reales del
  firmware. Identidades de secuencia, revisión de framebuffer y estado del
  robot se escriben como valores de ejemplo; no se obtienen del hardware.
- El estudio sigue siendo local. Los controles preparados no ejecutan acciones,
  reinicios ni secuencias. No se añade transporte por imitación de la referencia.
- El texto de arranque no demuestra que el ATmega provocó el reinicio ESP ni
  permite inferir modo SPI, flash o cableado. Se conserva el contenido observado.
- El editor OLED de la referencia no forma parte de este alcance de Protocol
  Studio; la ruta OLED AFR2 existente conserva su biblioteca y navegación.

## Identidad y cobertura

APLICAR `visual-identity-system`, borrador propio0.1.9 previamente autorizado y
pedido actual de consistencia. AutoMicro aporta estructura y flujo; AFR2 aporta
marca, superficies, controles y tokens de tema. No existe una skill local con
nombre exacto «Visual Consistency»; se aplicó la disponible para ese propósito.

Dirección Confirmada. Organización de referencia Observada en código.
Composición adaptada Propuesta. Cobertura: fuente e inspección manual;
apariencia/interacción en escritorio/móvil, temas y contraste **No verificados**.
No se ejecutaron tests, compilaciones, validadores, navegador, COM ni flasheos.
Atmega/ESP sin cambios; Web en fuente, paquete ESP anterior.

[Ficha02](../changes/2026-10-28-10-protocol-studio-essence.md) ·
[Contrato](../contracts/WEB_PROTOCOL_STUDIO.md) ·
[Adaptación por archivo](../provenance/web-protocol-studio-essence-20261028.json).
