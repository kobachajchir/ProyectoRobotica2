> Verificación posterior: [AFR2-20261106-02](2026-11-06-02-regeneration-checks.md) regenera y compila; detecta bloqueo SRAM/pila. El texto inferior conserva el estado histórico de fuente 01.

# AFR2-20261106-01 — Conexión, IR digital, Wi-Fi y pantallas

06/11/2026. Implementación en fuente del plan aprobado tras la revisión del auto. Se preservan autorías, cambios locales del Loader y el archivo generado Web preexistente. No se modifican CAD ni repositorios de referencia.

## Estado real

| Módulo | Estado de esta entrega |
| --- | --- |
| ATmega | Fuente modificada: EEPROM v9, IR digital, calibración, paginación Wi-Fi, retorno por arranque, pantallas y tiempos de dibujo/envío |
| ESP | Fuente modificada: EX01v13, presión de salida WS, Wi-Fi persistente v2, reinicio tras respuesta, catálogo paginado y diagnóstico HTTP |
| Web | Fuente y copia raster modificadas; consultas coordinadas, consumidores visibles, formularios y escenarios locales |
| Loader | Reubicación de etapas ya escrita en AFR2-20261105-01 conservada; ejecutable abierto no actualizado |
| CAD | Sin cambios |

Revisión mediante lectura de fuentes y diff. **Sin compilar, ejecutar tests/validadores/generadores, abrir navegador/COM, promover paquete, crear imagen LittleFS, flashear ni probar físicamente.** La fuente no identifica lo que está instalado hoy en las placas. La compilación Web y el paquete del 04/11 son anteriores a esta entrega.

## Implementación

- **HTTP:** `/diagnostico` funciona con el firmware aunque el paquete Web no esté disponible; expone montaje, rechazo del paquete, modo/IPs, versión, arranque y causa de reset. `/api/health` agrega red e identidad. No incluye claves. El servidor sigue siendo el mismo para AP/STA. Se conserva el límite de 24 activos / 12 KiB del manifiesto.
- **WS:** cola Web única para comandos ordinarios, deduplicación de lecturas, heartbeat/STOP separados, limpieza por sesión/STOP y backoff que se restablece tras diez segundos estables con tráfico reciente. Cabezal solo con consumidores y pantalla solo con visor. Boot snapshots repetidos no rearman todos los lectores.
- **Salida ESP:** eventos periódicos dejan dos posiciones de transporte libres; una respuesta serializada diferida, límite de 8192 bytes y plazo de dos segundos. Se bloquea despacho ordinario mientras espera drenaje. Los avisos iniciales de identidad se escalonan. Se retiene el evento terminal del escaneo para la sesión que lo solicitó. Una ranura de metadatos conserva el error de una petición cancelada por STOP durante presión; no es una segunda respuesta serializada. Exceder esos límites de concurrencia sigue siendo un error de protocolo explícito, no descarte silencioso de ACK.
- **IR:** solo deshabilitado/digital, A4..A0, nivel eléctrico 0/1 separado de máscara activa. ADC reservado a batería. EEPROM v9 migra ADC a digital en RAM, invalida la calibración que cambia de tipo y conserva perfiles apagados y ajustes restantes. No hay escritura automática ni borrado. Muestras de fondo/línea y transición de los cinco canales son obligatorias para confirmar. Se retiran controles numéricos de ADC del producto.
- **HC-SR04/PWM:** se mejora el diagnóstico OLED de eco, timeout, movimiento, caducidad y edad desconocida. La vista de ruedas muestra la máscara de inversión junto a consignas; no se invierte globalmente el driver ni se afirma corregido un defecto físico no reproducido.
- **Hall:** «Rueda» es la entrada inicial. Guardar/Descartar aparecen con borradores; finalizar ocurre en la pantalla de conteo. Continúan la cancelación del intento, los borradores por rueda y el Sí/No obligatorio. Signos Hall y motor se mantienen separados.
- **Wi-Fi:** AP y STA se guardan por separado en `afr2-wifi-2`; lectura/migración de v1. Configurar o cambiar modo requiere persistencia y reinicia después de poder responder. Un modo sin credenciales abre configuración, sin aplicar una operación incompleta. Elegir una red desde AP no fuerza AP+STA. El AP de recuperación después de quince segundos sin STA se conserva. Un arranque ESP invalida borradores/resultados Wi-Fi y retorna al principal con guardas de cambios.
- **Escaneo:** se copian resultados antes de restaurar AP puro. El ESP conserva hasta dieciséis redes ordenadas por señal; OLED recibe páginas de ocho con job/scan/page y recepción completa. Hay estados buscando, vacío, fallo, resultados y truncamiento. La Web correlaciona resultados con el identificador aceptado. La simulación agrega resultados ficticios de dieciséis redes, vacío y fallo.
- **OLED/Web:** candado corregido también en el parcial; progreso del largo al soltar con icono L y dieciséis sectores. Slot inferior derecho reservado en menús, dashboard, IR, teclado y bandas de red. QR conserva la zona protegida sin superposición. Rápidas deja de forzar raster periódico de contenido estático; invalida por estado/input. Diagnóstico OLED separa duración de raster y última transferencia I2C, ambas con resolución de milisegundos.
- **Táctil:** mantiene requisitos de enlace/motores/modelo, y cancela también al cambiar el modo informado. No reproduce gestos tras reconexión o recuperación del modelo.

## Compatibilidad y procedencia

[Contrato EX01v13 / EEPROMv9 / Wi-Fi v2](../contracts/COORDINATED_CORRECTIONS_13.md). [Procedencia por archivo](../provenance/coordinated-corrections-20261106-01.json). [Casos de aceptación pendientes](../documentation/acceptance/2026-11-06-01.md).

Renderers C/TypeScript cotejados por lectura; hashes LF actualizados después de adaptar las rutas correspondientes. **No se ejecutó `sync-oled-library.mjs` ni se sobrescribió `generated/firmwareLibrary.ts`.** Su regeneración/copia de catálogo queda para una compilación autorizada. No se modificaron bitmaps de fuente/iconos ni los ScreenCodes existentes.

## Pendientes que no se presentan como resueltos

- Medir flash/SRAM/pila del ATmega y RAM/heap del ESP antes de desplegar. El último build conjunto documentado dejaba 773 bytes de SRAM AVR antes de pila; no demuestra suficiencia para esta fuente.
- Regenerar recursos Web, compilar el conjunto y crear/promover LittleFS únicamente con autorización; no usar la Web nueva con firmware EX01 anterior ni el paquete de 17 activos con la tabla antigua de 16.
- Comprobar HTTP por ambas interfaces y capturar el motivo real del corte inicial WS. No declarar estable el transporte por haber cambiado la cola.
- Investigar alimentación, superficie, distancia y LED del módulo IR sin transición; y el fallo HC-SR04 comunicado. Sin medición no se certifica sensor averiado ni reparación física.
- Comprobar referencia y giro de cada rueda antes de alterar inversiones. No se ejecutaron motores.
- Revisar visualmente todos los estados y medir respuesta de rápidas en el hardware; los contadores agregados no son resultados ya medidos.
