# AFR2-20261109-12 — Interfaz y entorno de recolección en Armado

Versión: **1.13.0 (subversión)**.

Pedido: tres pantallas de interfaz a la derecha durante su explicación, primera red «Koba», y piso de malla con varios cubos alrededor al comenzar el giro. Reutilizar pantallas y modelos reales.

## Fuente implementada

- `AssemblyInterfaceShowcase.tsx` dibuja dashboard, radar gráfico y redes encontradas mediante el renderer OLED real `renderFirmwareScreen`, los mismos recursos de fuente/iconos del firmware y `OledCanvas128x64`. No son capturas aproximadas ni pantallas nuevas. Los ScreenCodes se obtienen del catálogo compartido por sus nodos existentes. No se modifica diseño ni navegación AVR/Web.
- Datos de ejemplo explícitos: red Koba, Taller_2G y Robotica_Lab, RSSI −48 dBm, batería 7,8 V y ecos preparados. Sin conexiones, escaneo ni comandos al hardware. La lista es determinista para conservar continuidad al retroceder. No representa redes detectadas ni telemetría del equipo.
- Durante OLED/encoder (16,62–21,96 s de la narración), las tres vistas entran por la derecha con fade, desplazamiento, escala leve y una pequeña rotación de perspectiva. El radar barre usando el mismo reloj de narración. Al salir, se difuminan juntas. Preferencia de movimiento reducido: vistas sin desplazamiento, escala ni barrido.
- Cámara y textos reservan espacio para la columna de pantallas; layout compacto para celulares y pantallas bajas. Se mantiene el mismo Play/Pausa y scrubbing de la narración. No se incorporan bibliotecas ni imágenes adicionales.
- `AssemblyCollectionEnvironment.tsx` aparece al comenzar la órbita (reloj mecánico 0,87, aproximadamente 39 s): cuadrícula de 5 cm sobre un piso tenue, cinco clones del GLB real de cubo de 50 mm, entrada escalonada y una marca de destino en el piso que desaparece al cerrar la pinza. Los cubos secundarios quedan alrededor; la caja que recoge el robot conserva su modelo y movimiento existentes.
- El entorno queda fuera del cálculo de encuadre del producto para conservar el acercamiento final. Geometrías/texturas de cubos compartidas; solo se clonan materiales para los fades. La cuadrícula y materiales propios se liberan al desmontar.

Archivos: `AssemblyPage.tsx`, `AssemblyScene.tsx`, nuevos `AssemblyInterfaceShowcase.tsx` y `AssemblyCollectionEnvironment.tsx`, y `styles/assembly.css`. Versionado compartido actualizado en header de Contracts, package.json y lockfile Web.

Estado: Web=fuente implementada; ATmega/ESP=solo versión; CAD/Loader=sin cambios. Sin tests, compilaciones, validaciones, ejecución en navegador, renders, promoción Web o carga al ESP por instrucción del usuario. Encuadre y legibilidad pendientes de revisión visual solicitada.


## Compilación solicitada mediante Loader

El job `f5b92255cac74647ab87f957972e37a4`, aprobado manualmente, ejecutó build:esp, TypeScript/Vite y buildfs. Log: `C:/Users/kobac/AppData/Local/AutoFinalRobo2Loader/logs/20261109-233002-389-877660.txt`. Paquete inicial: 19 archivos, 2.527.868 B gzip, estimación FS 2.686.976 B; imagen LittleFS de 3.121.152 B. No se cargó al ESP.

Aunque el proceso devolvió éxito, el registro de mklittlefs contiene «unable to open» para `AFR2_Sensor_Bateria_Cableado.glb.gz` y «error adding file». Además, la lista explícita de Vite omitía los dos audios. La imagen inicial no se considera completa ni apta para carga.

Corrección en la misma versión 1.13.0 antes de cargar: nombre corto `AFR2_Sensor_Bateria.glb` (exportador CAD y referencia de entrega actualizados, sin modificar geometría), audios incluidos explícitamente en Vite, MIME audio/ogg y audio/mp4 coordinados entre manifest y storage.cpp del ESP. El empaquetador rechaza nombres de componentes de 32 bytes o más, límite del LittleFS de este ESP8266, antes de promover el paquete. Se conserva la relación servedPath=route+.gz exigida por firmware. Debe recompilarse también ESP por los MIME nuevos.

Solicitud Loader `06b672a7a03d4be1976a30084c142b41`: build_esp, pendiente de aprobación manual al registrar esta nota. Web/LittleFS debe repetirse después; los tamaños iniciales no incluyen voz. Sin flasheo ni pruebas de navegador/físicas.


### Resultado final del Loader

Firmware ESP compilado correctamente: log `20261109-233548-146-80a5a0.txt` del directorio de logs del Loader en LOCALAPPDATA. BIN 466.544 B; programa 462.399/1.044.464 B; RAM estática 46.676/81.920 B (57 %). Advertencia existente de función draftToken sin usar. Sin carga.

El primer intento corregido de filesystem (`2c7db4599cfe42ea934d56f67199384f`) terminó rechazado por cambio de fuentes/perfil durante preparación. La repetición posterior desde GUI sí terminó correctamente: log `C:/Users/kobac/AppData/Local/AutoFinalRobo2Loader/logs/20261109-233826-649-4a3823.txt`, final 23:39:17 del 09/11/2026. Incluye ambos audios, circuito con nombre corto y los restantes modelos, sin el error anterior de adición de archivos.

Paquete Web 1.13.0: 21 archivos; 2.697.316 B gzip. Estimación de ocupación FS con reservas: 2.867.200 B, exactamente el presupuesto preventivo Web configurado. Partición/imagen LittleFS: 3.121.152 B; margen frente a la estimación: 253.952 B (no medición de espacio libre montado). SHA-256 de littlefs.bin: d2b5195d0fcd655c4b9a07119563b068d4486fdf687bca9d6a32ed045fa3d703. Firmware y filesystem preparados, sin flasheo, navegador ni prueba física. Estos resultados sustituyen el estado pendiente anterior.
