# Factores de odometría por dirección — AFR2-20261111-06

Versión de producto 1.27.0; EEPROM v12. Fuente, sin compilar, ejecutar generadores, probar ni cargar.

## Semántica

Seis enteros `u16`, en milésimas, rango inclusivo 0..2000, paso 1: adelante, atrás, izquierda, derecha, giro derecha (CW), giro izquierda (CCW). La OLED muestra `X.XXX`: 775 → 0.775; 1000 → 1.000; 1250 → 1.250; 2000 → 2.000. El límite superior es una decisión de implementación para permitir subir de 1.000.

Se escala la componente **medida** en el marco del chasis, antes de llevarla a coordenadas globales. El signo de `dx` elige adelante/atrás; el de `dy`, izquierda/derecha; el de `angle`, CCW/CW. Las diagonales aplican los factores de sus componentes. Un cambio no modifica los otros cinco factores, CPR, diámetro, radio, mezcla de motores ni PWM manual. Los factores se usan en la odometría de todo el producto, incluidos GOTO y examen; no son multiplicadores de potencia ni de la meta solicitada.

Una meta lateral de examen sigue siendo 1000 mm. Al bajar el factor se cuenta menos distancia para el mismo movimiento físico y el auto recorre más antes de completar la meta. Para ajustar después de medir: factor nuevo = factor actual × distancia real / distancia solicitada; redondear a milésimas y repetir la medición en las condiciones de uso.

**Cero:** se admite y se guarda. Bloquea iniciar la componente correspondiente en Drive/GOTO cerrados; examen deshabilita la dirección con factor cero. También se comprueba cada consigna del control cerrado. No divide por cero ni deja que la misión espere indefinidamente sin contar esa componente. PWM manual y prueba PWM de rueda conservan su funcionamiento.

## Pantallas y navegación

Pruebas → Pruebas examen → **Escala de factores** (`030050`, nodo AVR 79). Pruebas examen pasa de 3 a 4 filas. Las listas de desplazamiento y giros conservan sus cinco opciones y códigos, con los offsets internos ajustados.

Escala de factores tiene nueve filas desplazables: Adelante, Atrás, Izquierda, Derecha, Giro derecha, Giro izquierda, Guardar EEPROM, Descartar cambios, Volver. Se reutilizan cabecera, iconos, filas 16 px, foco, scroll y editor numérico de la familia existente; no se agregan bitmaps ni framebuffer.

| Editor | ScreenCode | Acción local | Inicio/migración |
|---|---|---|---|
| Adelante | 030051 | 115 | 1.000 |
| Atrás | 030052 | 116 | 1.000 |
| Izquierda | 030053 | 117 | 0.775 |
| Derecha | 030054 | 118 | 0.775 |
| Giro derecha (CW) | 030055 | 119 | 1.000 |
| Giro izquierda (CCW) | 030056 | 120 | 1.000 |

Giro selecciona; corto abre el editor con el valor actual. En el editor, giro cambia solo el borrador, corto aplica esa dirección en RAM y vuelve a la lista, largo al soltar descarta solo ese borrador. La edición/aplicación exige salidas detenidas, ninguna escritura EEPROM ni calibración Hall activa. Aplicar o descartar ajustes reinicia el marco de pose por el mecanismo existente; no recalcula una pose vieja con otra escala.

Guardar EEPROM usa el guardado general existente. Al salir de la lista con configuración sin guardar, tanto Volver como largo abren el diálogo obligatorio **Sí/No** (`030038`): Sí escribe y espera el commit real, No restaura la configuración guardada. El largo no evita la elección. Como en los otros menús de calibración, Guardar/Descartar abarcan la configuración pendiente completa; si solo se editó un factor, los demás valores se conservan.

Las acciones 115..120 son **locales**, no nuevas claves CONFIG. Se mantiene la política existente que impide editar calibraciones por pulsaciones sintéticas remotas. La pantalla en vivo transmite el framebuffer AVR real; el visor no inventa factores. La biblioteca sin datos muestra `--` en el editor; el simulador usa factores explícitos de ejemplo y permite recorrer el flujo sin mandar órdenes al auto.

## EEPROM y transporte

Payload v12: 78 bytes, frente a 66 de v11. Se conservan orden y offsets de los primeros 66 bytes. Al final se agregan seis `u16LE` (offsets 66, 68, 70, 72, 74, 76), en el orden de la tabla. Los dos registros de 128 bytes, CRC, secuencia y marcador de commit conservan sus posiciones y procedimiento asíncrono.

v1..11 se leen con el decodificador y migraciones existentes; se inicializan los seis factores con los valores de la tabla **en RAM, sin autoguardar**. Los registros v12 se validan por longitud/CRC/rangos. No cambian EX01, CMD, bodies, commands.json ni wire_catalog.json: no se expone una nueva operación de configuración por UART.

## Recursos, estados y límites

ATmega: fuente de configuración, migración, odometría, menús/editor y guardas. Web: renderer vinculado, metadatos de catálogo actualizados mediante edición focalizada, vistas sin datos y simulador. ESP: solo versión compartida 1.27.0; no nuevos handlers. No se ejecutó `sync-oled-library.mjs` ni otro generador, pruebas, typecheck, builds, validadores, navegador, COM, promoción LittleFS o flasheos.

Los seis factores suman 12 bytes por `Afr2Config`; producto y copia guardada suman 24 bytes estáticos, además del crecimiento de copias temporales en pila (carga EEPROM y callback). Los registros EEPROM y buffers OLED no crecen. Sin nuevo build no se afirma margen SRAM/flash/pila suficiente ni paridad raster ejecutada. Fuente y registro de procedencia no acreditan exactitud de 1 m o de giros físicos.
