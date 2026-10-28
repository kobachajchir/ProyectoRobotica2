# Protocol Studio

AFR2-20261028-09/10. Fuente adaptada sin compilar, probar ni observar en navegador.

Entrar en **Desarrollador → Protocol Studio → Traductor**. Pegar la captura
completa del monitor, incluidas sus cabeceras `--- ... ---`, dejar Entrada en
HEX y pulsar **Traducir captura** o Ctrl+Enter. Elegir Texto escapado para ver
texto junto a `\xHH`, o UTF-8 / ASCII / Latin-1. Los bloques convertidos tienen
botones Copiar; si el navegador bloquea el portapapeles se selecciona el texto
para copiar manualmente. El selector de bytes ofrece HEX, RealTerm, C, Python
y decimal. El resultado se descarta al editar la entrada para evitar presentar
una conversión anterior como resultado del nuevo texto.

El mapa permite seleccionar una trama, candidato o bloque crudo para analizarlo.
«Stream continuo» muestra todos los bytes agrupados por rango, sin limitarse a
las vistas previas de las tarjetas. Los frames ROBO2 se desglosan con ruta, comando, N, sesión,
transacción, resultado, campos disponibles y XOR. Los binarios fuera de protocolo
se conservan; no se traducen a comandos inventados. BOOT0x52 es un evento BT01
sin correlación. La semántica de los cuerpos es parcial y se indica junto a
ellos. Framing correcto no confirma ejecución ni negociación de una sesión.

**Constructor** es la pestaña inicial, como en AutoMicro. Prepara una petición
ESP→ATmega mediante parámetros guiados, o una respuesta ATmega→ESP con resultado
y cuerpo manual. El catálogo se agrupa por función. Las variantes de GOTO y
secuencias usan sus longitudes actuales; las identidades fijas y reserved:u32=0
se agregan automáticamente. El campo HEX manual reemplaza los parámetros de
solicitud; vaciarlo recupera ese formulario. N y XOR son calculados.

La estructura del paquete se presenta por campos con offsets, bytes y explicación
desplegable. Es la misma visualización del análisis de una captura. Las cuatro
salidas HEX, RealTerm, C y Python aparecen juntas con copia individual; decimal
se ofrece como salida adicional. «Abrir en el traductor» permite
inspeccionar el resultado localmente. No transmite bytes ni abre COM. Las
sesiones/transacciones iniciales son ejemplos; no se obtienen del hardware.

Cada traducción admite hasta64KiB de bytes y262144caracteres. El texto de un
monitor en modo HEX conserva la codificación de los bytes originales, aunque su
cabecera diga UTF-8; datos binarios pueden ser inválidos como UTF-8. Arrays C o
Python aceptan mezcla de enteros decimales y bytes explícitos `0xHH`;
`frame = bytes([...])` también se reconoce. No se aceptan columnas de
offsets/ASCII ni prefijos arbitrarios de logs.

[Contrato y límites](../../../AutoFinalRobo2-Contracts/contracts/WEB_PROTOCOL_STUDIO.md)
· [cambio](../../../AutoFinalRobo2-Contracts/changes/2026-10-28-09-web-protocol-studio.md).
[Matriz de continuidad con AutoMicro](../../../AutoFinalRobo2-Contracts/docs/PROTOCOL_STUDIO_AUTOMICRO.md).
«Cómo usar el estudio» abre ayuda integrada. El paquete ESP anterior no se actualizó en esta entrega.
