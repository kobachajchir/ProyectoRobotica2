> **AFR2-20261029-19 compilado y ATmega cargado:** pedido humano29/10/2026. ATmega/ESP/Web esp y LittleFS completados; promoción local Web a ESP/data/dist. Carga COM8 final13:32:50,80066bytes escritos/verificados, avrdude exit0. ESP/LittleFS sin cargar. SRAM estática7085bytes; sin prueba física/dinámica. Evidencia en AutoFinalRobo2-Contracts/documentation/builds/2026-10-29-19/README.md. Estados inferiores sin build/carga son históricos.

> **AFR2-20261030-12:** la Web mantiene la presentación táctil conectada durante el giro local. Modelo, pill conectada y OLED comparten sectores15grados; pie OLED `Angulo: N grados` actualizado junto con la banda gráfica. Sigue siendo rumbo solicitado, sin yaw físico. TOUCH_UI conserva el cuerpo4bytes/potencia confirmada incorporado en07 (EX01v10 y posteriores); el formato3bytes/ACK vacío descrito abajo es histórico. Solo fuente, sin build/carga; [detalle](../changes/2026-10-30-12-touch-heading-centered.md).

> **AFR2-20261030-15:** bitmap sustituido por silueta aportada por el usuario, recorte66x60 →24x22,66bytes PROGMEM. Rotación en ventana36x36 dentro de banda64x40; mismo recurso y algoritmo en Web. Las medidas16x24/48bytes inferiores son históricas. Sin build/carga; [ficha](../changes/2026-10-30-15-touch-user-silhouette.md).

# Control táctil y suscripción OLED — AFR2-20261029-19

Estado: fuente adaptada en ATmega, ESP y Web; sin compilaciones, tests, validadores, navegador, flasheo ni hardware. No promovido a ESP/data/dist.

## Visor bajo demanda

`robot.screen.subscribe {enabled:boolean}` termina en ESP; no introduce polling ni push de framebuffer en AVR. Sesión inicialmente deshabilitada. Web cuenta visores montados/activos (sección Pantalla y modal en vivo), exige documento visible y envía activación antes de consultar. Renueva cada1000ms; ESP caduca a3500ms. Cierre, ocultación, pérdida de enlace y limpieza envían desactivación. Una desconexión elimina la sesión.

ESP exige suscripción vigente para `robot.screen.get`, `robot.screen.status` y `robot.ui.action`. `robot.screen.changed` solo se envía a suscriptores. La captura conserva META/PAGE/RELEASE,1024bytes reales y revisión; no hay render local como fallback. Una transferencia ya iniciada puede consumir sus páginas hasta terminar antes de procesar el cierre encolado. Una vez procesado, no hay nuevas capturas/metadatos; la caducidad cubre cierres que no llegan. No elimina telemetría de sensores ni heartbeat de conexión.

## Volver

`robot.ui.action` añade `action=back`, wire0x22: revisión uint32LE, acción3, delta0. Conserva las comprobaciones de revisión/generación/edad, privacidad, parada y editor. Requiere feature0x20000. Localiza el ítem real cuyo rótulo comienza con Volver, lo selecciona y confirma mediante Afr2Ui_Apply; por ello conserva servicios, historial y confirmaciones contextuales. No agrega un gesto de atrás al encoder físico. En Web aparece debajo de las filas1–3 y flechas, solo en menús; la biblioteca local tiene el mismo atajo.

## Propiedad táctil

Nuevo comando `robot.touch.ui`, EX01v8/ROBO2 perfil2, ID0x24 y feature0x20000. Cuerpo3bytes: enabled0/1, direction0..8, heading0..23. direction0..7=adelante, diagonal derecha, derecha, atrás derecha, atrás, atrás izquierda, izquierda, adelante izquierda;8=reposo. heading=rumbo visual solicitado antihorario, pasos15grados. ACK correcto sin cuerpo extra (envolvente de correlación existente); broker devuelve enabled/confirmed. Timeout Web2000ms. Errores/rangos se rechazan; feature ausente no se presume compatible.

Al activar por primera vez, rechaza teclado/editor o medición Hall activa y control local en movimiento. Detiene misiones y radar; las renovaciones no vuelven a detener motores. No inicia movimientos ni habilita requisitos pendientes. Durante la sesión, ignora navegación física y bloquea GOTO/modo/prueba individual e inicio de secuencia. Conserva la UI subyacente y la restaura al liberar. La renovación/actualización se agrupa en un único envío pendiente; mantiene la última dirección/rumbo, sondeo de cambios100ms, renovación700ms y caducidad AVR2500ms. Cierre/ocultación/desconexión envían enabled=false; la caducidad resuelve pérdida de ese envío. STOP y TTL de movimientos existentes conservados.

El giro de dos dedos/Q-E sigue siendo la simulación local vigente: enviar heading actualiza el bitmap OLED, no genera yaw físico ni lo presenta como orientación medida. La nueva pantalla tampoco envía su framebuffer si no hay visor.

## Dibujo y memoria

ScreenCode0x030035. Aviso Control tactil / Activado1000ms; luego bitmap superior monocromo16x24 y tres chevrones según gesto. Bitmap propio48bytes XBM LSB más tabla seno Q8 de24int16 (48bytes), ambos PROGMEM; equivalentes TypeScript y PNG/PBM en Web/public/icons. Rotación inversa entera, recorte idéntico C/TS; sin framebuffer adicional. Afr2Ui incorpora12bytes de estado (antes de alineación). Pila/flash finales y SRAM total pendientes de compilación autorizada; no se certifica margen.

Entrada/salida y fin del aviso recomponen la pantalla. Cambios posteriores solo limpian/dibujan la banda x32..95,y12..51; texto superior/inferior permanece. El servicio diferencial existente transmite únicamente chunks con bytes distintos de lo confirmado; no hay segunda pila I2C ni DMA. No se comprobó paridad diferencial ni visual/física.

## Procedencia y despliegue

Overlay usa FloatingModal nativo y material del ST2026-Radal ya preservado en Web/reference/st2026 (autoría original conservada); añade scrim del tema al blur24px/saturate1.2. Icono, rotación y adaptación son fuentes propias AFR2. Inventario por archivo en provenance/touch-oled-20261029-19.json.

Se requieren fuentes coordinadas Web/ESP/ATmega. Sin promoción LittleFS, carga ni ejecución de sync-oled-library.mjs en esta entrega. Generador ampliado para conservar los recursos nuevos en el próximo build autorizado. Cambios concurrentes AFR2-20261029-20 en la biblioteca de menús requieren su propia revisión de port/hashes; no se declaran compilados por esta ficha.
