# AFR2-20261023-14 — Navegación flotante y visor OLED global

Fecha: 2026-10-23. Alcance autorizado: interfaz Web de AutoFinalRobo2; ST2026-Radal es fuente de lectura. Fuentes guardadas, sin pruebas ni compilación de esta revisión.

Productores: ATmega para estado/OLED/UI; ESP para broker/capabilities; Web para presentación. Consumidor nuevo: visor remoto compartido y modo de cabecera. No se crean IDs UART ni se presenta el endpoint OLED pendiente como disponible.

| Módulo | Estado de esta entrega |
| --- | --- |
| atmega | Sin cambios; reporte remoto OLED y acciones aún sin integrar al broker |
| esp | Sin cambios; robot.screen.get/robot.ui.action conservan rechazo CAPABILITY_UNAVAILABLE |
| web | Fuente adaptada y guardada; sin compilar, renderizar ni probar esta revisión |
| shared | Manual 0.1.5/VIS-015, perfil consumidor, catálogo de pantallas/conexiones y procedencia actualizados |

## Resultado

- AppShell y sidebar adaptados de ST2026-Radal, revisión `335d3edb2f5f39457ae82c82ded6f0403bbbec65`: rail flotante persistente, iconos/nombres, selección de sección y preferencia de colapso. En móvil conserva rail visible y expansión superpuesta.
- FloatingModal común adapta PublicCatalogSearch: dialog nativo, material translate/clip, backdrop neutro blur24, apertura240/salida200, teclado inmediato y reduced fade120. Se usa en OLED global y Ajustes; STOP/cerrar quedan dentro del top layer, con cabecera sticky y retorno de foco.
- Inicio ofrece seis accesos con contenido izquierdo e ilustraciones simbólicas propias a derecha. Superficies de trabajo sólidas, profundidad discreta y acento rubí conservado.
- Modo de cabecera recibido de robot.state con edad/vencimiento; no inferido del lease o del botón seleccionado. Sonner único desplaza toasts bajo el alto real de la cabecera.
- Visor remoto compartido entre modal y ruta OLED: captura estricta, pausa/limpieza por sesión/visibilidad, edad, capability y errores explícitos, sin snapshot simulado. SW único cancelable; acción remota sin actualización optimista ni reintento automático.
- Apertura modal interrumpe hold manual y solicita STOP; toda emisión de STOP cancela el emisor periódico de la página. No se acredita parada física por estas defensas Web.

## Contratos y procedencia

[Diseño de interfaz](../docs/WEB_FLOATING_UI.md), [perfil consumidor](../contracts/WEB_OLED_CONSUMER.md), [manual](../docs/design-system/index.md) y [manifest por archivo](../provenance/web-floating-ui.json). Se guardaron 19 fuentes Web, seis archivos de referencia (incluye README), dos informes y el manifest Web: 28 archivos. Referencias ST conservadas íntegras/inactivas; no se modificó ST, AutoMicro ni sus registros. Logo, GLB/Draco y dependencias no cambian.

La forma semántica de state sin framebuffer es perfil **Propuesta** del consumidor; requiere acuerdo/productor ESP/ATmega. firstVisible distinto de cero o una pantalla no soportada no se reconstruyen por aproximación. No se asignan IDs UART ni se elevan capabilities. Los comandos mantienen sobre afr2-ws-1, correlación, timeout2500 para consultas y timeout2000 para ui.action. Solo completed habilita lectura del resultado; accepted no se anuncia como cambio confirmado.

## Evidencia y límites

Lectura de componentes/ST, imágenes aportadas, fuentes resultantes y hashes de procedencia. Sin tests, compilaciones, validadores, servidor, renderizado ni ensayos físicos ejecutados por esta entrega. Pendientes: cotejo móvil/escritorio/claro/oscuro, teclado/foco, fluidez/scroll y casos de red cuando se autorice; fidelidad física OLED y productores siguen pendientes.

La compilación concurrente [AFR2-20261023-15](2026-10-23-15-esp-web-build-fixes.md) se completó antes de promover estas fuentes. Se preservaron sus apéndices README/INFORME_ADAPTACION y su empaquetador. Sus artefactos no certifican ni incluyen la nueva interfaz de esta ficha, aunque su ID sea mayor. No se reemplazaron dist, artifacts/LittleFS ni ESP/data/dist desde este cambio.
