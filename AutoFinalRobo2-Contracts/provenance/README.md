AFR2-20261027-24: [Loader0.7, alta y prueba del servidor MCP](loader-mcp-connection-20261027.json),
fuente propia, indicador por sesión/cliente y casilla siempre visible. Sin
compilación, ejecución GUI/MCP, tests, validadores, COM ni cargas de esta revisión.

AFR2-20261027-23: [Paquete AT solo en familia AT](loader-at-visibility-20261027.json),
corrección propia de visibilidad en fuente, sin compilación ni ejecución de esta revisión.

AFR2-20261027-22: [Loader0.6, Web/AT por USB y reglas de memoria](loader-esp-modes-20261027.json),
referencia AutoMicro en solo lectura y adaptaciones propias por archivo. Solo
fuente/documentación; sin builds, tests, validadores, GUI/MCP, COM ni cargas.

AFR2-20261027-21: [Loader0.5, autoaceptación MCP, perfiles y copia](loader-usability-20261027.json),
adaptación por archivo sobre fuentes locales conservadas. Solo fuente/documentación;
sin compilar, ejecutar GUI/MCP, tests, validadores, COM ni flasheos de esta revisión.

AFR2-20261025-01: [control Web directo7](direct-web-control-20261025.json), eliminación funcional del permiso confirmado por el usuario, menús C/TS y contratos coordinados. Compilados los tres módulos, sin flasheo/pruebas físicas. Baselines y adaptaciones por archivo; autorías preservadas.

AFR2-20261024-15: [alineación del maestro](master-alignment-20261024.json), fuentes complementarias y brechas corregidas FS/Calibración/health. Se preservan seis planes anteriores por hash. Web/ESP compilados, AVR conserva12/gates0; aceptación física y CAD nuevo pendientes.

AFR2-20261024-14: [Desarrollador y árbol OLED](developer-oled-tree-20261024.json), referencia AutoMicro de solo lectura y adaptaciones por archivo; biblioteca/píxeles sin cambios. Web compilada/promovida, sin prueba visual/diferencial/física.

AFR2-20261024-11: [adaptaciones por archivo del encoder/Hall](encoder-hall-interrupts-20261024.json). Fuente AVR sin compilar ni probar; pines/IRQ y estados documentados, autores previos conservados.

AFR2-20261024-10: [simulador GOTO y recorrido](goto-map-queue-20261024.json) registra adaptación propia del mapa/cinemática existente, cola local y movimiento con animate/RECIPES. Sin assets binarios/dependencias nuevos; GLB/fuentes/autorías anteriores conservados. Fuente/build no acreditan gesto ni hardware.

AFR2-20261024-09: [biblioteca OLED vinculada](linked-oled-library-20261024.json) registra fuentes AVR activas de solo lectura en esta ficha, copia Web de primitivas/layouts y recursos literales, catálogo compartido y actualización de skill. Autorías previas/F4 kobac conservadas. Hashes no acreditan comparación diferencial ni hardware. [Build09](../documentation/builds/2026-10-24-09/README.md).

# Procedencia

AFR2-20261027-25: [diagnóstico USB–TTL ESP aportado](esp-usbttl-diagnosis-20261027.json),
consultas del usuario y registro por archivo. ESP8266EX/flash 4 MiB identificados;
sin carga ni verificación de contenido, ni nuevas pruebas del agente para documentarlo.

AFR2-20261027-20: [interfaz y flujo Loader 0.4](loader-workflow-20261027.json),
adaptaciones por archivo de barra/etapas, consolas, destino y guía K/RST.
Autoría de Koba Chajchir conservada. Solo fuente/documentación; sin ejecución,
compilación, tests, validadores, COM ni flasheos.

AFR2-20261025-12: [proxy LAN de desarrollo](lan-development-proxy-20261025.json),
herramienta Node propia y documentación por archivo. Servicio local iniciado;
sin dependencia nueva, cambios de firmware o acceso remoto comprobado.

AFR2-20261024-08: [Red OLED, iconos F4 y reinicio](oled-network-restart-20261024.json)
registra hashes,14recursos seleccionados y adaptaciones por archivo. Fuentes
F4 de solo lectura. Compilado sin paridad de píxeles ni prueba física.

AFR2-20261024-07: [Control, sistema y secuencias](control-system-sequence-20261024.json)
registra fuentes de solo lectura y adaptaciones/hashes por archivo; builds y
promoción en documentation/builds/2026-10-24-07. Compilado, sin render autorizado
ni prueba física. No se atribuyen avances previos a este cambio.

[Maestro](../docs/INFORME_MAESTRO.md). files.json registra SHA-256 leído/copiado y transformaciones; origins.json HEAD y cambios previos. Inventario documental, no test ni validación de interoperabilidad.

Defaults de contraseña y SSID de estación identificados se vaciaron. Sin .env/config persistida/.git/dependencias/binarios firmware. Manifests Web históricos son *.upstream.txt, para no ejecutar postbuild hacia AutoMicro. reference/ no entra en builds nuevos.

Autorías conservadas. La copia privada no concede una licencia nueva al material ajeno; revisar permisos antes de redistribuir. Font7x10 derivada identifica fuente/hash. Corpus AMDOC/AMI importado es histórico y conserva IDs; enlazar su código mediante manifest y repos locales.

[esp-firmware.json](esp-firmware.json) registra AFR2-20261023-11: procedencia y adaptación por archivo de servidor/LittleFS/Wi-Fi, perfil UART compartido, endpoint AVR y ajustes Web. Los hashes identifican seis fuentes conservadas y no certifican funcionamiento. No se reescribe files.json ni se modifica AutoMicro.

[wifi-shield.json](wifi-shield.json) es el manifest incremental AFR2-20261023-03: tres imágenes aportadas y dos descargas oficiales. Los 1113 archivos iniciales de files.json no se reescriben. Imágenes/documentos originales en Documentacion permanecen intactos.

AFR2-20261023-04 conserva los historiales de esos módulos dentro del Git global; no reescribe los manifests anteriores. Los insumos de CADS tienen un [inventario separado](../../AutoFinalRobo2-CAD/reference/insumos-locales.json) con ruta anterior/actual, tamaño y SHA-256, sin atribuirles autoría o licencia desconocidas. [Organización Git](../docs/ESTRUCTURA_Y_GIT.md).

[web-floating-ui.json](web-floating-ui.json) registra AFR2-20261023-14: fuentes ST2026-Radal 335d3ed preservadas en Web/reference/st2026 y adaptaciones activas por archivo. SHA256 identifica procedencia; no certifica renderizado, compilación ni compatibilidad de hardware. No modifica registros de AutoMicro ni manifiestos históricos.


AFR2-20261023-15: [adaptaciones ROBO2 y composición AVR por archivo](atmega-robo2.json), referencias F4 hash/autoría conservados. Sin compilación/tests/validadores/flasheos ni cambios en reference/AutoMicro.

[web-startup.json](web-startup.json) registra AFR2-20261023-16: fuentes ST conservadas, intro adaptada, PNG RGBA generado/prompt, original archivado y ubicación Sonner. La evidencia de imagen/alpha no acredita ejecución Web.

[web-control-map.json](web-control-map.json): AFR2-20261023-18 registra fuentes ST/Sonner/referencias Web y adaptación por archivo. No se reescriben manifests anteriores; hashes son procedencia, no ejecución probada. Sin cambios en AutoMicro o ST.

[atmega-product.json](atmega-product.json):19, producto AVR y adaptaciones por archivo;
hashes de entrega y fuentes previas preservadas en atmega19-before/. Sin reescribir
files.json ni reference/. No certifica compilación, memoria ni ejecución física.

AFR2-20261025-02: [táctil local y ruedas simuladas](touch-simulation-wheels-20261025.json). Fuentes propias adaptadas por archivo, GLB final conservado, sin nuevas dependencias o CAD. Web compilada; visual/placas no probadas.

AFR2-20261025-03: [captura/foco y flecha activa](touch-input-feedback-20261025.json). Adaptación propia de cuatro fuentes, contrato/documentación y paquete03. Compilado; sin reproducción en navegador o pruebas físicas.

AFR2-20261025-04: [entrada local independiente](local-input-independent-20261025.json). Corrección del efecto de Canvas.fallback y disponibilidad local, tres fuentes propias adaptadas; Web compilada, sin reproducción en navegador ni pruebas físicas.

AFR2-20261025-05: [estela direccional de degradé](directional-gradient-trail-20261025.json). SVG/CSS propios sobre dos fuentes, sin nuevas dependencias/activos. Compilado/promovido; visual no comprobada.

AFR2-20261025-06: [chevrones grandes bajo el auto](underbody-gradient-trail-20261025.json). SVG/CSS propios, capas/origen central y degradé/fade. Compilada/promovida; sin aceptación visual ni nueva dependencia/CAD.

AFR2-20261025-07: [cintas anchas con punta intensa](wide-ribbon-tips-20261025.json). Tres fuentes propias SVG/CSS/cámara manual adaptadas; Web compilada/promovida, sin cambio de escala CAD/mapas ni aceptación visual.

AFR2-20261025-08: [difusión suave por brazo y punta aguda](soft-pointed-ribbons-20261025.json). Componente visual nuevo y adaptación propia de Overlay/CSS; fuente compilada/promovida, sin nueva dependencia ni aceptación visual.

AFR2-20261025-09: [restauración07 con color suave](restore-previous-trail-20261025.json). Hashes de fuente y cambios por archivo; componente08 retirado con copia local, sin cambios ajenos. Compilada/promovida; sin revisión visual.

AFR2-20261025-10: [modelo manual mayor](larger-manual-model-20261025.json). Una fuente cambia dos factores de cámara, chevrones/malla intactos. Compilada/promovida, sin revisión visual.

AFR2-20261025-11: [giro local con dos dedos](two-finger-local-rotation-20261025.json), hook propio, integración/rig/lectura; sin comandos nuevos ni CAD. Compilado/promovido, gesto/captura no comprobados en teléfono.
