# AFR2-20261024-01 — Retoma de firmware e integración Web

Pedido humano: revisar el plan ya implementado, contrastar AFR2-20261023-20 y
completar la parte pendiente del firmware. Se leyeron fichas19/20/21, fuentes
actuales y contratos. La Web de21 contiene avisos con X, controles OLED,
gráficos de giro, editor/persistencia y simulador; no se vuelven a implementar.
Wi-Fi20 ya está en fuente y tiene build histórico: se conserva su servicio.

## Resultado de la revisión

Se detectó colisión real entre el borrador UART_PROFILE_3 de21 y la extensión
EX01 de19: ambos usaban0x10..0x15 para operaciones diferentes. Se conserva
ROBO2/perfil2, EX01 producto y su autoridad/calibración. Secuencias se agregan
en0x40..0x45, EX01v2. La especificación3 queda como borrador histórico sustituido,
sin cambiar framing ni asumir compatibilidad por nombres de comando.

## Implementación

- ESP: negociación EX01, cuerpos UART y validación de respuestas; estado/pose,
  adquisición/renovación/liberación por dueño, drive/prueba/modo; captura OLED
  coherente por páginas y acción ligada a revisión; adaptación de secuencias.
- ATmega: ejecutor estático32 segmentos, commit de carga completa y temporización
  local; comparte PID/STOP/motores de19. Caducidad, fallo o STOP cancelan y no
  reanudan. Preserva servicios, UI ampliada y configuración del producto.
- Web: framebuffer del menú ampliado, revisión por acción, lease inmediato y
  defensa ante respuestas tardías entre cancelación/commit/start. Conserva
  estética y editor existentes; ajusta referencias al rango de IDs vigente.
- Wi-Fi: servicio20 intacto; el broker rechaza NUL embebido antes de convertir
  credenciales JSON a C strings. Scan, persist/RAM y fallback no dependen de EX01.

## Estados reales y límites

| Módulo | Estado |
|---|---|
| atmega | Fuente adaptada; secuencias bloqueadas por defecto hasta revisar SRAM/pila y cumplir calibración/Hall. Sin build/ensayo de esta ficha. |
| esp | Puente EX01 y secuencias adaptado en fuente. Wi-Fi20 conservado. Sin build/runtime de esta revisión. |
| web | Consumidor adaptado sobre21. Sin nuevo build/render/pruebas. |
| shared | Contrato corregido, catálogo y capacidades documentados, procedencia por archivo. |

Se aplica la skill local afr2-screen-authoring al reporte/acciones, sin modificar
renderers ni navegación. Corto confirma; largo800ms abre menú; Volver por ítem.
Captura coherente en memoria no acredita barrido físico atómico de SSD1306.

No se ejecutaron tests, compilaciones, validadores, flasheos, servidor ni pruebas
físicas. Los builds históricos de20/21 no cubren este cambio. LittleFS y paquetes
no se regeneran/promueven. GOTO con todos los parámetros Web y jobs/borradores
Wi-Fi solicitados desde OLED siguen pendientes y no se anuncian como disponibles.

[Contrato y requisitos](../contracts/EX01_WEB_INTEGRATION.md) ·
[Procedencia](../provenance/firmware-web-integration-20261024.json).
