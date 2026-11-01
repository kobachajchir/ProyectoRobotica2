# AFR2-20261030-09 — estado de red, búsqueda y operaciones Wi-Fi

Implementación en fuentes bajo D:/ProyectoRobotica2. ATmega, ESP y Web: source-integrated-uncompiled-runtime-unverified. Contracts actualizado; Loader y CAD sin cambios. EX01v11 coordinado; EEPROMv8 sin cambios. Sin pruebas, compilaciones, validadores, generadores, navegador, promociones, cargas ni pruebas físicas. La ficha08 de la pill táctil es independiente y se conserva.

| Pedido | Implementación y puntos de intervención |
|---|---|
| Estado de red | afr2_menu_catalog.c, afr2_screens.c, afr2_app.c, afr2_network_screen.c; resumen real y opciones anidadas, conexión en dos páginas y datos de red con seguridad |
| Cambiar modo | nodo67/030043; STA/AP/AP-STA con subrayado y gestos; trabajo10, WifiService.changeMode; modo de sesión confirmado sin reemplazar credenciales |
| Buscar/refrescar/listado | nodo66/030042, Refresh existente; paquetes de nombres reales/seguridad/id; destino diferido tras Redes listas; filas dinámicas y guardas de revisión |
| Credenciales Web | readyWifiClients comprueba sesión lista y transporte vivo; resultado6, aviso sin clientes y retorno a Redes encontradas; desaparece revisión como fila |
| Configurar y acciones | Configurar dentro de Estado, Modificar credenciales; Conectar/Desconectar dinámicos, Reintentar solo desconectado; eliminadas filas Revisar/Cancelar borrador y el Conectar duplicado de raíz; AP-fallback conservado |
| Desconexión real | trabajo9, WifiService.disconnectStation, observación no bloqueante; éxito solo con STA desconectada, timeout5s e inhibición de reconexión automática hasta reintento/configuración explícita |
| Información | SDK entrega IP/máscara/gateway; enum de seguridad y candado propio12bytes; datos desconocidos/caducados explícitos, sin señal STA inventada para AP |
| Web completa | WifiPage, wsEnvelope, firmwareRenderer/SensorRenderer, firmwareHints, wifiCatalogue, reformResources y FirmwareLibraryPreview; render y recursos vinculados, visor sigue framebuffer real; preview no ejecuta radio |
| Ayudas generales | Helper común de clic/largo e iniciales capitalizadas en AVR y TypeScript, incluidas pantallas de sensores, pruebas, teclado, avisos, editores y dashboard |
| Contratos/procedencia | WIFI_UI_11.md, commands.json, screens.json, conexiones, navegación, informes de módulo y procedencia por archivo; sincronizador preparado, sin ejecutarse |

Detalle de bytes, permisos, tiempos, fuente de datos y comprobaciones futuras: [WIFI_UI_11.md](../contracts/WIFI_UI_11.md). Procedencia: [wifi-ui-20261030-09.json](../provenance/wifi-ui-20261030-09.json).

Límites específicos: seguridad STA conocida solo por BSSID exacto de un escaneo reciente (120s); de lo contrario desconocida. Raster ASCII conserva fallback para caracteres no incluidos. OLED primeras8 redes; Web primeras16. La pérdida de WebSocket durante una operación deja el resultado sin confirmar en la Web. Modo de sesión no persistente. Cuenta nominal de campos AVR24+2+3bytes y candado12bytes, sin medida enlazada de memoria/pila. Biblioteca generada y paquete ESP conservan su entrega previa hasta regeneración/build/promoción autorizados. Fuente revisada manualmente por lectura y diff; no hay evidencia visual, de transporte real ni hardware de esta ficha.
