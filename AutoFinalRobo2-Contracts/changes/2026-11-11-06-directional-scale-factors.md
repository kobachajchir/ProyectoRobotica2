# AFR2-20261111-06 · Escala de factores por dirección

Versión: **1.27.0 (subversión: función compatible)**. ATmega y Web adaptados en fuente; ESP solo versión compartida. Sin builds, tests, generadores, validadores, navegador, puertos ni cargas.

Nuevo acceso **Pruebas → Pruebas examen → Escala de factores**, con seis valores separados: adelante, atrás, izquierda, derecha, giro derecha e izquierda. Rango 0..2000 (0.000..2.000), paso 0.001, con 1000=1.000. Laterales arrancan en 0.775; el resto en 1.000. Cada cambio modifica solo su dirección. Los factores multiplican la odometría, no el PWM ni la meta de 1000 mm.

Editor numérico habitual: giro ajusta borrador, corto aplica en RAM, largo descarta el editor. Guardar EEPROM/Descartar cambios explícitos y confirmación Sí/No obligatoria al salir con cambios pendientes. Cero bloquea esa componente en lazo cerrado. Conducción manual PWM conserva el comportamiento de la ficha 05.

Pantallas compartidas **030050..030056**, catálogo, renderer AVR y copia raster TS; simulador con ejemplos explícitos y biblioteca sin datos con `--`. EEPROM v12/78 bytes: v1..11 conservan ajustes y reciben factores predeterminados en RAM, sin escritura automática. Nuevas acciones locales 115..120, sin CMD ni claves CONFIG nuevas.

[Contrato, flujo, tabla de IDs y límites](../contracts/DIRECTIONAL_SCALE_FACTORS_20261111.md) · [Procedencia por archivo](../provenance/directional-scale-factors-20261111.json).

Pendiente: compilación autorizada, presupuesto de memoria/pila, comparación raster y aceptación física. El guardado usa la configuración general como los demás menús: también guarda o descarta otros ajustes pendientes si los hubiera. La escala nueva reinicia el marco de pose al aplicarse.
