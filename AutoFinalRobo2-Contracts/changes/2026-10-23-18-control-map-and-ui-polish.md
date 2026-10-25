# AFR2-20261023-18 — Inicio fiel a Radal, toast y mapa de Control

Pedido humano: aproximar la introducción a Radal sin tarjeta flotante, corregir notificaciones recortadas y agregar un mapa con botones de vistas para observar el auto. Alcance Web/Contracts; fuentes ST/AutoMicro de lectura. Dirección visual autorizada para aplicar el manual borrador 0.1.7, sin dar por verificada su apariencia en ejecución.

| Módulo | Estado real de esta ficha |
| --- | --- |
| atmega | Sin cambios. Productor de posición pendiente; no se activa odometría por esta entrega. |
| esp | Sin cambios. Traducción/transporte de pose pendientes; no se modifica UART ni LittleFS. |
| web | Fuentes guardadas: inicio, notificaciones, mapa y lector compartido de estado. Sin compilar, renderizar ni probar esta revisión. |
| shared | Perfil consumidor `afr2-pose-1` Propuesta; manual 0.1.7/VIS-017, catálogo y procedencia actualizados. No constituye un contrato binario implementado. |

## Cambios aplicados

| Before | After | Why |
| --- | --- | --- |
| Introducción sin halo/sheen de referencia y controles con aspecto de panel | Logo libre, sin contenedor; velo acrílico de pantalla completa y controles secundarios transparentes | Recuperar la composición de Radal con la identidad del robot |
| Título truncado y cálculo de ancho móvil incompatible con los márgenes asimétricos | Texto multilínea, ancho limitado al viewport, cierre propio y Toaster en portal de `document.body` | Evitar recortes por texto, tamaño o ancestros con overflow/transform |
| Control sin espacio de observación | Mapa Superior SVG, Perspectiva y Seguir auto con Three.js bajo demanda; zoom, centrado y trazo | Mostrar contexto espacial con acciones de cámara independientes del mando |
| Lector de modo exclusivo de la cabecera | Un único `robot.state` compartido por cabecera y mapa | Evitar consultas simultáneas duplicadas y conservar una misma fuente de estado |

### Inicio

Fuente canónica: `D:/ST2026-Radal/src/presentation/components/ui/startup-welcome.tsx` y su CSS, revisión `335d3ed`. El velo usa mezcla de marca/ink, transparencia 86%, halo 28%, sheen estático y blur 24px; logo hasta `min(76vw,32rem)`, sin borde, tarjeta ni sombra añadida. Entrada 300ms, pausa 600ms, salida de logo 200ms y salida de velo 300ms, total 1300ms desde la imagen decodificada. Se conserva el diálogo nativo, Escape/Omitir, STOP, retorno de foco, fallo de imagen y fallback de 4s. Reduced motion usa fade de 120ms; contraste/transparencia reducida reciben fondo sólido. No se modifica el activo de marca en esta ficha.

### Notificaciones

Un único Sonner, siempre `bottom-right`. Se elimina `truncate`, se limita el ancho a 400px y al viewport y se usa una cuadrícula con columna de texto flexible. Márgenes con safe areas y ancho explícito del item corrigen la ecuación móvil del Sonner instalado 2.0.8. Una notificación visible a la vez evita lectura parcial de pilas; contenido excepcionalmente largo permite scroll interno manteniendo el cierre disponible. Persisten los IDs estables y los flujos existentes; el polling no genera toast.

### Mapa y estado

El mapa muestra un chasis esquemático de cuatro ruedas, identificado como referencia cuando no llega ubicación; no es un CAD validado. El GLB de prueba y su visor independiente se conservan. Superior usa SVG responsive; las otras cámaras cargan Three.js bajo demanda con fallback a la vista superior si falla WebGL. No se añade un bucle de animación continuo.

La trayectoria solo incorpora pose válida/fresca y capabilities de odometría completas. Se rechazan secuencias duplicadas/no crecientes, se reinicia por marco/conexión y se corta el trazo ante huecos; máximo 256 muestras. No se integra una orden del mando para inventar posición. La orden solicitada se informa aparte con sus valores originales; STOP o pérdida de mando la limpia. Cambiar cámara, zoom o centrado cancela un hold activo.

`ConnectionProvider` comparte `robot.state`: timeout 2500ms y siguiente lectura 1000ms tras terminar, página visible y ambos enlaces listos. No hay solapamiento ni segundo polling en cabecera. Visibilidad/sesión invalidan respuestas anteriores. Modo caduca a 6000ms y pose a 2000ms.

El [perfil consumidor](../contracts/WEB_CONTROL_MAP.md) precisa estructura, unidades, marcos, correlación, frescura, errores y aceptación futura. `robot.state.pose` es opcional y Propuesta. No existe nuevo ID UART ni productor AVR/ESP en esta entrega; sin él el mapa no puede mostrar recorrido físico. `stopped=false` se etiqueta como parada informada, sin inferir velocidad medida.

## Identidad, procedencia y cobertura

Skills aplicadas: `apple-design`, `emil-design-eng`, `animate`, `ask-sonner` y `visual-identity-system` en modo APLICAR. Se actualiza el [manual 0.1.7](../docs/design-system/index.md), decisión VIS-017. Fuentes, autores conservados, hashes previos/resultantes y adaptación por archivo en [procedencia](../provenance/web-control-map.json); copia junto a la Web en `docs/ui-control-map-20261023.json`.

Cinco reglas protectoras: logo libre sobre velo; notificación legible e inferior derecha; una tarea visible; cámaras independientes del movimiento; datos autoritativos con desconocido/caducado explícitos. El esquema y el perfil de pose son extensiones Propuesta, no observaciones de hardware ni aprobación estética final.

## Verificación y límites

Se leyeron fuentes propias, componentes/CSS canónicos de ST y CSS/implementación del Sonner instalado. Se inspeccionó el activo de logo existente. La copia comprobó rutas y hashes de destino antes de guardar los 18 archivos Web, para preservar cambios concurrentes; no se sobrescribieron referencias ni módulos de firmware.

No se ejecutaron tests, compilaciones, validadores, servidor, renderizado de la Web, flasheos ni ensayos físicos. Los builds anteriores no acreditan esta revisión; `dist`, paquete local y ESP/data no se regeneraron. Quedan pendientes revisión visual en ambos temas/tamaños, comportamiento del toast en navegador, las tres cámaras y casos de recepción de pose bajo pedido expreso. El siguiente paso para movimiento observado es implementar su productor acordando el perfil entre ATmega y ESP, sin habilitar capacidades anticipadamente.
