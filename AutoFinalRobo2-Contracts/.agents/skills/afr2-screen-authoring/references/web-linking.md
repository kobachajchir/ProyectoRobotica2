> AFR2-20261029-27: largo al soltar vuelve por contexto; Inicio abre carrusel y Si/No obligatorio no admite retorno. Contrato vigente: Contracts/contracts/QUICK_RADAR_ACTUATORS_9.md.

# Copia Web de la biblioteca ATmega

Rutas desde D:/ProyectoRobotica2. La fuente de verdad de diseño es AutoFinalRobo2-Atmega/src/{display,ui}; la identidad compartida vive en AutoFinalRobo2-Contracts/documentation/screens.json. Copia ejecutable de dibujo en AutoFinalRobo2-Web/src/features/oled/{oledRasterizer,firmwareRenderer}.ts; recursos generados en generated/firmwareLibrary.ts.

tools/sync-oled-library.mjs copia literalmente catálogo, enumeraciones, glifos Font7x10, XBM y recursos de secuencias desde el código AVR activo. Se integra en inicio/dev/build Web. No ejecutar el generador o builds sin el permiso de ejecución vigente. Si no hay permiso, actualizar las fuentes vinculadas y declarar pendiente su regeneración/compilación. No ejecutar scripts históricos ni validadores por esta skill.

Al cambiar primitivas/layouts C, portar los algoritmos correspondientes a TypeScript: parámetros de dibujo enteros, formato vertical LSB, ASCII32..126/fallback?, Rect de contorno, FillRect relleno, INVERT XOR, recorte TextWindow, escalado XBM conservando aspecto, firstVisible, textos dinámicos, filas16px y scroll con pausa800ms/paso40ms. Conservar redibujo parcial y reloj uint32; comparación de frames requiere el mismo snapshot preparado, instante y framebuffer anterior. Copiar recursos y actualizar el catálogo no basta para portar un algoritmo nuevo. Revisar los hashes de firmwareRenderer.sources.json después de la adaptación; el build se detiene cuando un origen revisado cambia.

Para un menú estático, la biblioteca local puede mostrar su diseño con una etiqueta explícita de preview y sin ejecutar comandos. Vistas/avisos que dependen de sensores/red/secuencia requieren datos preparados explícitos; no rellenarlos con cero ni presentar una muestra como estado real. El visor LiveOledViewer recibe framebuffer vía EX01; nunca lo reemplaza con un render local. Identificar límites de muestreo, páginas físicas y falta de ensayos, incluso cuando los bytes recibidos se conservan.

Registro mínimo por pantalla: mismo ScreenCode en catálogo AVR y Contracts; renderer C y contraparte Web; rol y variante; campos/validez/unidad; padres y retorno; selección/firstVisible/acciones; iconos/fuentes; permisos y privacidad. Al modificar wire, coordinar ESP y documentación de comandos antes de anunciar soporte. Sin cambios de wire, conservar transporte framebuffer existente.

No inventar un segundo botón Menú/Atrás: giro selecciona, corto confirma, largo SW abre principal y su liberación no confirma; regreso mediante Volver. Conservar autorías y registrar hashes en Contracts/provenance. Evidencia de fuente/compilación y evidencia visual/física son estados distintos.
