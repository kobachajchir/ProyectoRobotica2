> **AFR2-20261102-14:** el dashboard incorpora ahora el panel rotativo y se alinea la familia de pantallas. [Contrato vigente](OLED_DASHBOARD_STATUS_20261102.md). QR, privacidad y límites físicos se conservan.

> **Actualización AFR2-20261102-11:** por pedido del usuario, el dashboard vuelve a su disposición original con cabecera invertida. Las filas Volver de submenús usan flecha16px y texto; las demás páginas conservan sus ayudas. La OLED Web es blanca. QR y resto del flujo se conservan. [Ficha](../changes/2026-11-02-11-oled-original-dashboard-white-back.md).

# OLED128-5 y QRScreen en el auto

AFR2-20261102-05 · 02/11/2026. Integración en fuente; sin compilación,
tests, validadores, navegador, decodificador ni flasheo.

Lo diseñado el 01/11 en la skill no estaba conectado a todos los renderizadores.
Esta entrega lleva dashboard, menús de scroll, cards rápidas, barras de progreso
y gráfica IR a las fuentes AVR y su contraparte TypeScript. No modifica la
navegación vigente: corto abre menú en Inicio; largo al soltar abre rápidas en
Inicio y retorna por contexto en el resto. Sí/No obligatorio mantiene su guarda.

## Uso en Ayuda

Ayuda → OLED → Simular flujo abre el estado local con servicios ficticios.
Se reutilizan el reductor existente, el catálogo compartido, los recursos AVR,
los tres controles del encoder y el umbral configurado. Elegir una pantalla en
la lista salta a ella y reinicia el ejemplo; navegar con el encoder conserva el
contexto, selección y borradores. No invoca `ui.action`, serial, WebSocket ni
órdenes de robot. Biblioteca sin datos sigue disponible. El visor conectado
conserva la captura real de 1024 bytes y no sustituye datos con una simulación.

## Pantallas del auto

| Identidad | Ruta | Contenido |
| --- | --- | --- |
| 030045 | Menú → Wi-Fi → QR conectar AP | `WIFI:T:WPA;S:<SSID>;P:<clave>;;`, o `nopass` si la red es abierta |
| 030046 | Menú → Wi-Fi → QR abrir Web | `http://<IP>/`, STA conectada tiene prioridad; en otro caso usa AP |

Los datos proceden del estado de red ya recibido del ESP. La pantalla no lee
hardware ni consulta HTTP. Escapa `\ ; , : "` en los campos Wi-Fi. El QR Web
exige IP adquirida, red activa, datos actuales y servidor Web listo. Si faltan
datos, caducaron o el AP está apagado, muestra el motivo sin inventar una red.
Corto o largo retornan al contexto; girar no altera el QR.

QR AP es privado: bloquea inicio y lectura de captura remota, y entrada mediante
acción remota. Los ejemplos Web se identifican como ficticios: AFR2-DEMO,
demo1234 y las IP reservadas de documentación 192.0.2.1/192.0.2.2.

## Componente para otro contenido

`Atmega/include/afr2_qr_screen.h` expone
`Afr2Qr_DrawPayload(oled, bytes_en_RAM, longitud)`: acepta los bytes completos,
limpia y dibuja el QR; devuelve 0 si no caben. Su caller prepara el contenido,
las leyendas de la columna derecha y las guardas de captura/movimiento. El
adaptador AP/Web implementa esas guardas. La contraparte Web es
`renderQrPayload(buffer, texto)` en `features/oled/qrScreen.ts`.

Ejemplo de contenido elegido por otro wrapper de pantalla:

```c
const uint8_t texto[] = "AFR2:equipo-01";
if (snapshot->qr_allowed && Afr2Qr_DrawPayload(oled, texto, sizeof(texto)-1)) {
    /* Componer título y gestos en x67..127. */
}
```

## Geometría y presupuesto

QR Code Model 2, byte mode, versiones 1..9, máscara fija 0; parte de ECC Low y
eleva corrección dentro de la versión elegida cuando cabe. No recorta el
contenido. Caja blanca 62×62 en (1,1); módulos oscuros, quiet zone de cuatro
módulos, escala entera y centrado. Versión 1 admite 2 px/módulo; las siguientes
usan 1 px/módulo. Títulos, iconos y footer están fuera de la caja, desde x67.
La densidad de un Wi-Fi completo puede requerir 1 px/módulo; aún falta comprobar
su lectura con teléfonos en la OLED real. No se declara escaneabilidad física.

El Mega reutiliza 706 bytes del framebuffer como dos áreas temporales de 353
bytes; no reserva dos buffers globales adicionales. Después de generar, una
función noinline copia la matriz a 353 bytes de stack y restaura el frame. Las
tablas ECC del proveedor van a PROGMEM. Las fases de encoding y composición
son separadas; el stack total, Flash, SRAM restante y tiempos siguen sin medir.
La vista integrada exige auto y radar detenidos y conserva el frame entre
renders mientras no cambien revisión de red, estado o contexto.

## Procedencia y entrega

Encoder QR C y TypeScript: Project Nayuki, MIT, commit
`bcfe1b018cec5f1f91b5705eca5bb4cdd34e9f8d`.
[Biblioteca original](https://www.nayuki.io/page/qr-code-generator-library) ·
[fuentes](https://github.com/nayuki/QR-Code-generator/tree/bcfe1b018cec5f1f91b5705eca5bb4cdd34e9f8d).
Las cabeceras de licencia quedan en cada archivo. Adaptaciones locales: tablas
C en PROGMEM y namespace TS exportado. Recursos de iconos/font existentes
conservan sus autorías; números 5×7 proceden de la skill OLED128 del proyecto.

ATmega y Web: fuentes integradas y recursos copiados mediante el modo
`sync-oled-library.mjs --copy-only`, sin ejecutar sus comprobaciones. ESP:
transporte actual reutilizado, sin cambio de wire ni paquete Web promocionado.
El equipo físico y una Web ya empaquetada mantienen su versión instalada hasta
una compilación/promoción/flasheo autorizados. No se realizó commit ni push.
