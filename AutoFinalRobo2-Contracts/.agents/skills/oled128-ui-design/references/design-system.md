# Antecedentes del sistema visual OLED128-1

**Especificación vigente:** [componentes OLED128-3](components.md) y
[dashboard](dashboard.md). Este archivo
conserva medidas y propuestas de la primera iteración para explicar su origen,
no prescribe la navegación antigua. El pedido posterior reemplaza largo=principal
por largo=volver, las palabras de retorno por iconografía, unifica barras de
carga/progreso y fija números arriba/valores abajo en sensores. El gráfico puede
mostrar ocho barras simultáneas; no hay límite de tres. Mini3×5 es una variante
registrada para números compactos. Cuando difieran, aplicar OLED128-2.

Borrador inicial, 01/11/2026. Esta guía especializa UI y flujo para OLED 128×64.
No describe una migración ejecutada. Distinguir los requisitos de familia del
usuario, los patrones observados en AutoFinalRobo2 y las nuevas composiciones.

## 1. Lenguaje y espacio

128 columnas, 64 filas; origen arriba a la izquierda, x=0..127, y=0..63.
Una sola tinta: píxel encendido/apagado. No depender de color, grises, sombra,
antialiasing o animación para explicar estados. La ampliación Web conserva el
raster; no se rediseña la OLED como una tarjeta HTML.

Fuente base heredada: **7×10**, avance 7 px. Ancho total: 18 caracteres completos
(126 px). Con un título acompañado por icono y texto desde x=21 quedan 107 px:
15 caracteres completos. En la etiqueta de menú x=25..117 quedan 93 px: 13
caracteres completos y recorte parcial. Son presupuestos geométricos, no una
razón para abreviar todo de forma incomprensible.

Textos de OLED en español sin acentos mientras se use el corpus ASCII32..126
actual, con `?` como fallback. «ñ», «°», flechas Unicode o emoji no son glifos
disponibles por defecto; usar texto ASCII, bitmaps registrados o extender la
fuente deliberadamente. Las palabras del documento pueden llevar acentos.
No cambiar fuente por pantalla. Una tipografía grande futura será un recurso
de familia explícito, no un escalado improvisado.

Rejilla de partida:

| Zona | Menú heredado | Contenido con pie, propuesto |
| --- | --- | --- |
| Cabecera | título y=0, fuente10 | y=0..13, icono14 y título x=21,y=1 |
| Cuerpo | filas y=16,32,48 de16px | y=16..49 |
| Pie | no existe: tercera fila llega a63 | y=52..63, navegación/ayuda |

**No agregar un pie debajo de tres filas**: no hay espacio. Menú y dashboard
son plantillas distintas. Si el título es largo, simplificarlo; reservar
marquesina para el texto de la opción enfocada, no todo el contenido.

## 2. Iconos como vocabulario

Regla solicitada: una identidad visual por concepto, compartida entre proyectos.
No dibujar otro encoder porque cambió la función que lo menciona. Mantener IDs
semánticos estables, independiente de números enum de cada firmware.

| ID de familia | Significado | Recurso base disponible |
| --- | --- | --- |
| `input.encoder` | control UI rotativo con pulsador | Encoder,13×13 |
| `nav.back` | regresar al contexto | Volver,16×16 |
| `status.info` | información | Info,16×16 |
| `status.warning` | problema/advertencia | OperationWarning,16×16 |
| `network.wifi` | conexión Wi-Fi | Wifi,19×16 |
| `network.ap` | punto de acceso | APWifi,15×16 |
| `network.link` | enlace/comunicación | Link,16×16 |
| `system.settings` | configuración | Config,16×16 |
| `action.refresh` | actualizar/reiniciar, con verbo contextual | Refrescar,16×16 |
| `sensor.group` | sensores en conjunto | Sensors,14×16 |
| `tool.calibration` | prueba/calibración | Tool,16×16 |
| `device.phone` | interfaz del teléfono | Smartphone,9×16 |
| `actuator.motor` | motor/movimiento | Motor,16×16 |
| `storage.preferences` | preferencias/guardado | Prefs,16×16 |
| `sensor.radar` | barrido y distancia | Radar,16×16 |
| `actuator.servo` | servomotor | Servo,16×16 |

Estos 16 recursos se adjuntan como fuentes. No todos son distinguibles a8px;
no dar por aprobada su reducción. El código histórico comparte Sensors y Prefs
con motivos parecidos: registrarlo como deuda, no extender la ambigüedad.
Hall de rueda requiere identidad propia si hay que diferenciarlo; no usar el
icono del mando UI por llamarse también «encoder».

### Icono, etiqueta y gesto

- Una referencia al control usa `[input.encoder]`, no la palabra «encoder».
  Ejemplo de entrada: icono + `Interfaz`; diagnóstico: icono + `Giros: 12`.
  «Encoder UI» escrito en el título actual es un caso a migrar, no un patrón
  que ya satisfaga esta nueva regla.
- El icono identifica un objeto. La acción añade verbo: icono de motor +
  `Iniciar`, `Detener` o `PWM`. Un motor dibujado solo no determina qué ocurrirá.
- Una ayuda de entrada distingue **giro**, **pulsación** y **mantener**.
  Usar icono + texto breve (`Giro`, `OK`, `0.8s Menu`) o símbolos de gesto
  documentados. No asumir que el icono estático del encoder explica los tres.
- En un menú de navegación, conservar texto de tarea junto al icono. No
  sustituir todos los nombres de funciones por pictogramas sin contexto.
- En una serie de valores, poner el icono del subsistema una vez en cabecera;
  las filas pueden decir `Giros`, `Clics`, `Largos` sin repetirlo tres veces.
- En matrices conocidas IR1..IR5 o FL/FR/RL/RR, los identificadores sirven como
  etiquetas de canal; cinco iconos iguales añadirían ruido.

Tamaños: caja16 en filas de menú,14 en cabecera propuesta,8 en pies sencillos
como Volver. Centrar conservando proporción. Reusar el bitmap, sin filtros ni
deformaciones. Si un recurso complejo necesita variante pequeña, dibujarla y
registrarla como variante del mismo ID; no suponer que ya existe.

## 3. MENÚ-3: la única familia de menús

Hereda el menú de tabla desplazable actual. Título fijo arriba; tres filas
verticales. Cada fila mide128×16. Primera y=16, segunda32, tercera48.
Icono dentro de caja16×16 desde x=3; etiqueta desde x=25, y=fila+3. El marco
rectangular de1px ocupa toda la fila enfocada; marca `>` en x=120. No cambiar
entre marco, fondo invertido o cursor parpadeante según el submenú.

La selección mueve el foco de una fila a otra; al salir de las tres visibles
cambia `firstVisible`. El icono y el marco quedan quietos mientras el texto
seleccionado demasiado largo se desplaza dentro de su ventana. Herencia:
pausa inicial800ms,1px/40ms, pausa final800ms y reinicio; girar reinicia pausa.
Los datos numéricos críticos no se leen mediante marquesina.

Opciones no disponibles conservan su lugar y su nombre; `!` sustituye el
marcador del extremo. Confirmar explica el requisito que falta. Un foco no
significa que el servicio esté activado. Movimiento, Wi-Fi conectado o guardado
no se infieren a partir de la posición del cursor.

Por defecto heredar navegación circular del proyecto actual; conservar la
misma política en toda la familia. No hacer una pantalla circular y otra con
topes sin una razón declarada. Volver es última opción en submenús y restituye
la selección/desplazamiento anterior. En listas dinámicas usar IDs de opción
para no confirmar otro elemento si la lista cambia durante un gesto.

## 4. ESTADO-3: dashboard y diagnóstico

**Observado:** las vistas actuales muestran título y tres líneas en y=13,25,37,
con un retorno en y≈52. Inicio resume modo, autoridad/control y salidas.
Diagnósticos muestran contadores/errores o estado de subsistema.

**Patrón propuesto:** cabecera de subsistema, hasta tres filas en y=16,28,40
y pie de navegación. Una fila equivale a un dato o estado, no a una acción.
No desplazamiento automático de dashboards ni carrusel temporal.

Prioridades al construirlo:

1. Estado operativo: qué está haciendo el equipo y si está detenido/activo.
2. Dato principal relevante para la tarea: medida, etapa o consigna/aplicado.
3. Condición que cambia la decisión: conexión, fallo, calibración o validez.

No meter versión, IP, cinco sensores y cuatro motores en Inicio. Esos datos
pertenecen a vistas de detalle. El dashboard no es un inventario de variables.
Mostrar autoridad de control cuando haya mandos locales/remotos concurrentes.

Texto alineado a izquierda; números alineados a derecha, con precisión estable
y unidad constante. Dos composiciones permitidas, con variantes nombradas:

- `plain`: etiqueta desde x=0, valor termina antes de x=127; útil con un solo
  subsistema identificado en cabecera.
- `icon-row`: icono de8px desde x=0, texto desde x=14. Etiqueta de hasta6
  caracteres termina antes de x=56; valor de hasta9 caracteres empieza no antes
  de x=64 y termina antes de127. Incluye signo, decimal y unidad en ese ancho.
  Si no cabe, usar otra unidad autorizada o pantalla de detalle; no superponer.

No mezclar ambas en el mismo dashboard sin relación funcional. Repetir el icono
en filas solo si cada fila representa un subsistema diferente. Para `Giros`,
`Clics`, `Largos`, usar el icono UI en cabecera y filas `plain`.

Ejemplo conceptual (los corchetes nombran bitmaps, no texto a imprimir):

```text
[motor] Estado
Modo        Reposo
Salidas        OFF
Control     Local
[volver] Volver
```

Las dos columnas son de etiqueta/valor; **no** son dos tarjetas de dashboard.
Con fuente7×10, preferir esta jerarquía vertical. Dividir en dos columnas de
64px solo para pares comparables y etiquetas cortas; identificadores y unidad
deben seguir visibles. No usar mosaicos diminutos para aumentar la densidad.

## 5. MEDIDA-1: una magnitud protagonista

Variante propuesta cuando importa una lectura: cabecera y unidad, lectura
central, secundario de validez/edad y pie. Con la fuente disponible conservar
7×10 o usar una fuente grande real ya integrada. El proyecto actual no aporta
por esta guía una nueva tipografía grande.

Reservar ancho para máximos/mínimos, signo y `--`; distinguir medición de
consigna. Ejemplo servo: ángulo aplicado y PWM aplicado no prueban posición
física. No etiquetar PWM como realimentación de posición.

## 6. EDITOR-1: un borrador por vez

Herencia actual: etiqueta y=0, valor y=23, ayuda de aplicar y=40 y descartar
y=53. Nueva variante de cabecera con icono mantiene el valor protagonista y
el mismo orden. No mezclar lista de navegación con valor en modificación.

Fijar rango, incremento, unidad y valor inicial confirmado. El giro modifica
un borrador, corto solicita aplicar, resultado confirma aceptación/fallo.
Largo descarta y abre principal, sin guardar como efecto lateral. Volver a una
pantalla no debe reejecutar el último comando. «Aplicado» y «guardado persistente»
son estados distintos. Un servicio asíncrono requiere estado de espera real.

Las ayudas de encoder usarán el icono y el gesto/verbo. El recurso de13×13 no
cabe nativo en un pie de12px: usar variante pequeña registrada o una zona de
ayuda más alta, sin recortarlo ni desplazar datos por accidente.

## 7. GRÁFICO: conservar la posición de los canales

Patrones observados:

- IR: cinco barras verticales, orden1..5, en x=8+24i, cajas14×28 desde y=14.
  Las etiquetas de canal están debajo; ausencia no equivale a barra de cero.
- Radar: semicírculo con centro aproximado(64,49), radios hasta38px, escala4m,
  ángulo y distancia asociados a la muestra. No presentar ecos inválidos como
  obstáculos ni unir muestras antiguas con una posición nueva sin contexto.
- Servo: número/estado, barra de rango y pulso aplicado/objetivo. El marcador
  representa el valor que conoce el sistema, no una medición inexistente.

Una gráfica tiene hermana numérica cuando la cifra exacta importa. Orden,
escala, unidad y canales iguales en ambas. Giro alterna sin apilar historial;
corto regresa al listado y largo abre principal, según el perfil actual.
No convertir esta excepción en «corto siempre significa atrás».

## 8. PROGRESO: tarea o secuencia

Herencia: barra segmentada en y=0..6; icono central16×16 desde(56,10); nombre
y=28, tiempo y=42, etapa/retorno y=54. Completadas rellenas, actual parcial,
pendientes en contorno. El límite de segmentos depende del ancho: con muchas
etapas usar progreso agregado y contador; no dibujar segmentos de ancho cero.

Mostrar tarea, etapa actual, progreso respaldado por servicio y resultado.
Tiempo transcurrido no es porcentaje salvo duración conocida. Salir de la
pantalla y cancelar la operación son acciones diferentes. Si existe Detener,
definir su acceso; nunca esperar el cierre de un aviso para poder detener.

## 9. AVISO y CONFIRMACIÓN

Aviso heredado genérico: barra de tiempo de2px arriba, icono16×16 en(56,6),
título y=25, detalle y=39 y cierre y=53. Avisos de red tienen icono a izquierda
y datos más largos con recorte/desplazamiento. No todos son diálogos que piden
decidir: algunos informan un evento y vuelven al contexto.

En la familia nueva, escoger uno de esos subtipos por contenido y mantenerlo.
Aviso breve: qué pasó + qué puede hacer el usuario. Evitar «Error1» sin causa.
No llamar éxito a «solicitud enviada». Datos sensibles se protegen en réplica.

Confirmación propuesta, cuando una acción necesita decisión explícita:
resumen legible, opciones de la familia MENÚ-3, foco inicial en Cancelar.
El número de filas disponibles se calcula después del resumen; no superponer
dos líneas de pregunta con tres filas de16. Usar una pantalla previa de revisión
si el resumen es largo. No añadir confirmación a cada navegación rutinaria.
Una alarma persistente no desaparece por el mismo timeout de un aviso informativo.

## 10. TEXTO/RED y TECLADO

Vista de red observada: icono14 y título desde x=21; SSID, IP y estado en líneas
separadas, con Volver abajo. Los valores variables pueden desplazarse; la
etiqueta permanece fija. Una IP no debe confundirse con estado de conexión.

Teclado observado: título, campo editado/máscara y20 posiciones visibles en
dos filas de10; caracteres ASCII, borrar y OK. Foco por contorno. Giro elige,
corto añade, OK pasa al campo siguiente o solicita terminar. Largo descarta y
abre principal. En este proyecto los límites son32 para SSID y63 para clave;
no universalizarlos a otros campos. Contraseñas no van a logs ni snapshots
remotos. No guardar automáticamente al pasar de SSID a clave.

## 11. Estados y consistencia entre proyectos

Registrar: activo, disponible, no disponible, esperando, válido, sin dato,
vencido y fallo. Diferenciarlos con texto breve y marcadores consistentes, no
solo parpadeo o «seleccionado». Una medición válida de cero sigue siendo cero.
Mantener el orden de canales, las abreviaturas y la unidad entre vistas.

Para un nuevo proyecto crear un perfil pequeño de diferencias: controles,
fuentes disponibles, unidades y capacidades. Heredar plantillas/iconos; no
copiar nombres de robot, módulos de firmware, direcciones I2C ni ScreenCodes
de AutoFinalRobo2 como si fueran universales.
