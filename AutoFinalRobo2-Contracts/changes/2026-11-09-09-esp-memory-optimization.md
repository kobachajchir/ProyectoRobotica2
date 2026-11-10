# AFR2-20261109-09 — Menos RAM y tráfico repetido en el ESP

Fecha:09/11/2026. Pedido: aplicar los cinco puntos y comparar cuánto bajó.
Versión:1.10.5 (subversión mínima, optimización compatible).

## Cambios por archivo

- `ESP/src/broker.cpp`: `system.alive` devuelve solo `megaLink` y `brokerTs`.
  Conserva frecuencia500 ms, presencia interactiva, supervisión, expiración y STOP.
- `ESP/src/broker.h`, `broker.cpp`, `broker_product.cpp`: una instantánea por
  sesión compara enlace, frescura, features, extensión, máscara y límites.
  Hello siempre incluye capacidades; el estado periódico solo si cambiaron.
  Solo se recuerda la instantánea si el buffer completo fue admitido. Se usa
  el mismo tiempo para construir y recordar el catálogo.
- `ESP/src/broker.h`, `broker_product.cpp`: unión entre credenciales Wi-Fi e
  IDs de secuencia, mutuamente excluyentes. La rama de secuencia activa su
  miembro explícitamente. Conserva cuerpo, generación, cola4 y STOP dedicado.
- `ESP/src/main.cpp`, `broker.cpp`, `broker_product.cpp`, `broker_system.cpp`
  y `broker_wifi.cpp`: HTML fijo y descripciones/errores largos pasan a flash.
  Diagnóstico en PROGMEM consulta `/api/health` y muestra JSON con `textContent`;
  sin JS indica la ruta JSON. Errores leen el texto elegido a pila acotada.
- `ESP/src/broker_wifi.cpp`, `broker.cpp`, `broker.h`: búsquedas solo Web no
  duplican SSID para OLED. Listas OLED se liberan al reemplazar, perder
  soporte/enlace, iniciar otro trabajo o tras120 s inactivas, sin interrumpir
  el envío de páginas. Otra página renueva vigencia; vencida requiere buscar.
- `ESP/src/http_assets.h/.cpp`, `main.cpp`: archivos gzip de64 KiB o más se
  sirven de a uno. Hasta3 solicitudes pausadas con observadores débiles,
  sin archivo/buffer de respuesta hasta su turno; cancelación, margen de heap
  y bloque contiguo y timeout30 s con503/Retry-After. Pequeños y304 independientes.
  `/api/health` informa activo/espera/servidos/rechazos.
- `Web/src/protocol/afr2Client.ts`: documenta la conservación existente de
  capacidades ante payloads parciales; comprobada con el cliente real y
  transporte simulado. No requirió alterar su lógica de fusión.

## Contrato y compatibilidad

`afr2-ws-1`, comandos, capacidades y permisos conservados. Omitir capacidades
significa conservar la última instantánea de esta sesión, no mapa vacío.
Reconectar invalida permisos anteriores y exige hello. Sin cambios CMD UART,
EX01 o EEPROM. Header compartido y versiones raíz Web sincronizados1.10.5.

## Comparación y estado

RAM estática ESP51172→46644 B: **−4528 B (4,42 KiB;8,85 %)**.
Flash459699→462319 B (+2620 B). Ejemplo latido3758→195 B y estado sin cambios
3679→602 B; catálogo completo idéntico cuando corresponde.
[Evidencia, fixtures y límites](../documentation/builds/2026-11-09-09/README.md).

- esp: compilado antes/después, sin cargar; RAM dinámica de placa sin medir.
- web: tipos/protocolo parcial comprobados; sin Vite/paquete/LittleFS nuevos.
- atmega: solo versión compartida, sin compilar/cargar en esta ficha.
- CAD/Loader: sin intervención. Sin puertos, navegador, flasheos o pruebas físicas.
- Sin commit/publicación. Cambios previos/concurrentes conservados; base incluye08.
