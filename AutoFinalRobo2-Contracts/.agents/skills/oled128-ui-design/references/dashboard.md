# Dashboard OLED128-3

Dirección visual revisada a partir de la imagen de monitor proporcionada por
el usuario. Referencia guardada en `../assets/inspiration/dashboard-monitor.png`;
autoría original no identificada. Se usa como inspiración, no como especificación
de funciones del robot ni instrucción para incorporar datos de un servidor.
La imagen tiene más área que128×64: heredar jerarquía y composición, no densidad.

## Qué conservar de la referencia

- Cabecera de contraste alto que identifica la sección.
- Información agrupada en bloques con alineación común.
- Una lectura que domina visualmente y secundarios subordinados.
- Iconos funcionales pequeños, bordes precisos y tipografía de píxel.
- Alternancia controlada de superficies llenas y vacías para crear jerarquía.

No trasladar CPU/GPU/discos, ilustración decorativa, ticker, todos los estados
o marcos internos simultáneamente. Evitar un HUD lleno de detalles sin tarea.
No agregar unidades o estados que los servicios reales no proporcionen.

## Composición común

Cabecera0..13 invertida: fondo encendido con icono, título y contador en negativo.
Sección identificada a izquierda; página fija a derecha. La inversión se aplica
al componente completo para que iconos y letras conserven legibilidad y padding.

Cuerpo y=17..48 con **hasta dos paneles**. Contorno1px, esquinas recortadas2px;
no sombras o bordes distintos por tarjeta. Espacio4px entre paneles. Panel
invertido indica importancia dentro del layout, no foco, selección ni ON.
Los estados se explican mediante icono/cifra/etiqueta; el foco de menú conserva
marco y cursor de su propia familia. No hacer paneles interactivos sin indicar
qué hará OK en el contexto.

Pie50..63 con EncoderHint común. Giro recorre páginas hermanas; OK ejecuta
la entrada de contexto, si está definida, y largo retorna al padre. En Inicio
la entrada abre principal; la raíz omite una flecha sin destino.

## Dos variantes registradas

**paired-status**: panel izquierdo62×32 y derecho62×32, separados4px.
Icono y estado en la fila superior, etiqueta breve debajo. Un bloque puede
tener énfasis invertido. Ejemplo de resumen: motorOFF y Wi-FiSTA. Dos estados
relevantes; no seis estadísticas comprimidas.

**hero-metric**: panel principal76×32, secundario48×32, separados4px.
La lectura principal usa los numerales registrados de5×7 escalados a10×14,
con avance12px; unidad en la segunda línea con fuente7×10. Secundario con
etiqueta breve y cifra7×10. El icono de cabecera da el contexto del subsistema
y evita repetirlo en cada cifra.

No improvisar una fuente o un tamaño distinto por página. El corpus de
`../assets/numerals5x7.json` es un recurso nuevo de la propuesta, aún no integrado
en el renderer activo. Signo/decimal ocupan celdas y se presupuestan con la cifra.
Si la lectura no cabe, cambiar de variante/precisión autorizada; no recortar.

La propuesta incluye tres páginas con datos ficticios:

| Página | Variante | Información |
| --- | --- | --- |
| Resumen | paired-status | MotorOFF / Wi-FiSTA |
| Posición del servo | hero-metric | 90grados / pulso aplicado1500us |
| Movimiento | hero-metric | consigna150mm/s / PWM40 |

El PWM de servo no es posición medida; el ángulo se infiere del pulso en el
producto actual. La consigna de velocidad no equivale a velocidad medida.
Al implementar, usar etiquetas y fuentes de datos del servicio, no fixtures.
Si un panel PWM no deja espacio a su unidad, reservarla en el título o contexto
explícito (`PWM` de servo enus; de motor con signo y rango registrado).

## Carga indeterminada

Mantener la barra de familia con bloque móvil. Cuando no existe fracción
calculable, omitir el porcentaje por completo. La zona inferior puede quedar
vacía. No rellenarla con una explicación obvia como `Sin porcentaje`.
El nombre de tarea puede mostrarse una sola vez; evitar `Conectar`, `Conectando`
y `Esperando conexión` a la vez si significan lo mismo.

## Relación con el resto

La nueva jerarquía del dashboard no cambia las ocho barras simultáneas de
SensorBarGroup. Las fuentes compactas siguen dedicadas a números de canal
y valores pequeños; las letras principales conservan7×10. No crear paneles
individuales grandes para cada sensor a costa de perder visión del conjunto.
El contraste de cabecera es común en el atlas actualizado, incluidos menús,
carga y avisos. La barra carga/progreso conserva su geometría anterior.

Estos son artefactos de diseño generados. No se afirma funcionamiento de
firmware, legibilidad física o fidelidad a una captura del equipo.
