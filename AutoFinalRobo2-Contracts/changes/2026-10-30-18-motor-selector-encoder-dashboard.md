# AFR2-20261030-18 — Selector común, gestos y dashboard

ATmega y Web adaptados en fuente. ESP/Loader sin cambios por esta ficha. Sin tests, compilación, validadores, generadores OLED, navegador, promoción Web/LittleFS, cargas ni prueba física. No certifica paridad visual, memoria dinámica ni funcionamiento físico.

## Selector común

Afr2MotorSelector_Render y Afr2MotorSelector_Step: título Elegir un motor centrado en x15,y0; siglas FL/FR/RL/RR en x9/41/73/105,y22; línea14x1 en y34 bajo el motor seleccionado. Giro circular, corto confirma, largo al soltar cancela/vuelve mediante contexto existente. Sin invocar motores desde el renderer ni al girar. Se reutiliza en Hall y cualquier editor AFR2_A_WHEEL (pruebas por motor y menú Hall), manteniendo el inicio del conteo Hall solo tras confirmar. Se corrige count4 del selector Hall para que Render no limite la selección al primer motor.

ScreenCode compartido030044 identifica el selector mientras está activo; no desplaza códigos ni nodos anteriores. Hall conserva su código histórico030036 para catálogo/entrada y030037 para conteo. Definición en documentation/screens.json. C usa borrador ya existente o selection: no agrega buffer framebuffer ni estado de selector AVR.

Web: motorSelector.ts es la copia raster literal. FirmwareLibraryPreview permite giro circular, confirmación/cancelación local sin mandar órdenes; el visor conectado sigue mostrando framebuffer AVR y mantiene las restricciones físicas del asistente Hall y ediciones remotas. MotorSelector.tsx reutiliza las mismas siglas/orden/presentación en pruebas de motor y en el campo de configuración cuando action12 aparece. Flechas mueven borrador con wrap; Enter/Espacio o clic confirma; Escape cancela borrador. Sin cambiar permisos/STOP del flujo consumidor. El catálogo de configuración puede filtrar acciones; no se amplía por esta ficha.

## Encoder

Se mantiene botón corto y bitmap de largo con L central aportado por el usuario17. Icono de giro24 sustituido por XBM propio21x17: encoder13x13 y arco separado a la derecha con punta horaria. Se dibuja nativo en pistas de giro para no perder la flecha por reducción; sustituye recurso32bytes por51bytes (+19bytes de bitmap PROGMEM, presupuesto final sin compilar). Mismos bytes en reformResources.ts.

Afr2Ui_GestureHint/gestureHint admite giro además de corto/largo. Usado en selector, edición numérica, diagnóstico y contadores del encoder; elimina referencias textuales de giro sin icono cuando indicaban interacción. El texto Gire una vuelta de Hall se conserva: describe mover la rueda física, no girar el encoder. EncoderGlyph reutiliza bytes OLED para controles Web, pulsación larga y leyenda de biblioteca; no sustituye render OLED por HTML. Menús de acciones/Volver conservan sus iconos de navegación, que no representan por sí mismos un gesto.

## Dashboard010001

Conserva cabecera de red/IP, batería e iconos. Cuerpo muestra solo modo actual centrado y elimina estado redundante de motores tanto del snapshot HOME como del dibujo. Pie: botón corto Menu y largo Rapidas, con sus pictogramas. Acciones reales conservadas: corto010002, largo al soltar010003.

## Integración y límites

Sin cambios de framing ni campos wire; agrega identidad de pantalla. Catálogo generado pendiente del próximo build autorizado; recursos y algoritmos Web portados manualmente y fuentes revisadas actualizadas en firmwareRenderer.sources.json. En esa revisión se retiró una inserción de selector de credenciales ubicada incorrectamente en firmwareNode (refería index fuera de ámbito); se conserva en firmwareMenuItem, donde corresponde.

Procedencia por archivo en ../provenance/motor-selector-encoder-dashboard-20261030-18.json. Autorías anteriores y bitmap de usuario conservados; nuevos helpers/layouts/arco son adaptación propia. Paquetes ESP instalados no reciben estos cambios hasta reconstrucción y despliegue autorizados.
