> **Sustituida por [AFR2-20261101-11](2026-11-01-11-web-accents-restored.md)** (01/11/2026): el usuario pidio recuperar las tildes en la Web.

# AFR2-20261026-03 - Textos de la Web sin tildes

Pedido humano: quitar los acentos de las palabras de toda la interfaz Web
porque se muestran mal; por ejemplo, configuracion.

Se reemplazaron las vocales con acento agudo por sus equivalentes sin acento,
en minusculas y mayusculas, en 51 archivos de fuente activa. Incluye menus,
botones, etiquetas accesibles, formularios, ayudas, documentacion dentro de
la aplicacion, errores, notificaciones, sensores, secuencias y fichas 3D.
Se corrigieron ademas secuencias de texto mal codificado en RouteNav y
LiveOledViewer, incluidos simbolos afectados por la misma conversion.
Se conserva la letra enie y los signos propios de unidades y controles.

`src/utils/displayText.ts` normaliza solamente texto de presentacion.
El catalogo Web aplica esa conversion al nombre y las acciones explicativas
que importa de Contracts; Calibracion la aplica a las etiquetas importadas.
Afr2CommandError la aplica al mensaje mostrado, conservando codigo y origen.
Los datos de formularios, SSID, claves, identificadores, comandos y payloads
no pasan por una conversion global del DOM ni se reescriben.

No se cambian las pantallas del firmware, el framebuffer OLED recibido,
fuentes/iconos binarios, recursos generados, hashes de fidelidad ni documentos
de referencia. Las leyendas HTML alrededor del visor OLED si quedan sin tildes.
No cambia la navegacion ni se crean nuevos ScreenCodes.

Procedencia por archivo: [inventario de edicion](../provenance/web-unaccented-text-20261026-03.json).
El inventario describe las escrituras realizadas; no acredita una validacion.

| Modulo | Estado de esta entrega |
|---|---|
| ATmega | Sin cambios |
| ESP | Sin cambios; paquete data/dist anterior |
| Web | Fuente editada; sin compilar ni empaquetar |
| CAD | Sin cambios |
| Contracts | Ficha y procedencia de la adaptacion Web |

No se ejecutaron tests, compilaciones, validaciones, navegador, generadores de
recursos OLED, promociones al ESP ni flasheos, conforme a la instruccion humana.
Los cambios concurrentes de radar, firmware, modelo y hardware se conservan.
