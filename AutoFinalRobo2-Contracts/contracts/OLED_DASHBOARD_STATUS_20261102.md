# Dashboard OLED y familia de pantallas

AFR2-20261102-14 · 02/11/2026. Fuentes ATmega y réplica Web; no compiladas,
probadas, validadas en navegador ni flasheadas.

## Composición 010001

Se conserva la cabecera blanca de26px con SSID en y2 e IP en y14; las dos
bandas tienen scroll independiente cuando exceden70px, pausa800ms en cada
extremo y paso1px/40ms. El icono de batería se reduce a16px para dejar libre
el voltaje. No se presenta el voltaje como porcentaje de carga.

Debajo, MODO en(2,28) y su valor en(2,40). Nombres compactos estables:
REPOSO, MANUAL, LINEA, RUEDA, GOTO, FALLO, SECUENC y PWM. Se usan los modos
reales del controlador, no el análisis de una cadena de texto.

Tarjeta informativa a derecha: contorno redondeado1px en(62,29), tamaño64×20.
Icono14px y etiqueta7×10 con4px de separación y centro vertical común.
No es un botón ni cambia las acciones del dashboard. El pie conserva corto
Menú010002 y largo al soltar Rápidas010003, con el símbolo L interno existente.

## Estados preparados y prioridad

Los snapshots agregan `dashboard_valid`, `dashboard_mode`, `dashboard_moving`
y `dashboard_fault`. Se preparan en `Afr2App_UiSnapshot` desde el producto y
se combinan con `quick_flags`, cuyo significado existente se conserva.

| Orden | Contenido | Fuente y significado |
| --- | --- | --- |
| Prioridad fija | Advertencia + FALLO | `product.fault != 0` o modo FAULT; permanece hasta desaparecer |
| 1 | Radar + ON | Cabezal con barrido en marcha |
| 2 | Servo + ON | Salida de servo habilitada; no implica posición medida |
| 3 | Motor + OFF | Configuración de motores deshabilitada |
| 4 | Motor + RUN | Configuración habilitada y controlador de movimiento activo; no es velocidad medida |
| 5 | Sensores de línea + ON | Seguimiento de línea en ejecución |
| 6 | Radar + RASTRO | Barrido en marcha y rastro habilitado |

Se incluyen solo estados aplicables. Sin estados relevantes o sin snapshot
preparado: icono de información + `--`. El modo desconocido también es `--`.
No se inventan alertas por tensión, obstáculos o desconexiones sin una señal
de fallo del servicio dueño.

## Temporización e interacción

- Entrada al dashboard: muestra el primer estado aplicable y espera3s.
- Tras3s sin actividad del encoder pasa al siguiente; después mantiene cada
  estado4s. Con un único estado no rota.
- Giro, corto o largo reinician la espera de3s. Mantener SW pulsado también
  pausa: `main.c` transmite el estado raw/debounced del botón, sin esperar su
  liberación. La navegación corta/larga continúa ejecutándose al soltar.
- La rotación se pausa, pero los datos siguen vigentes: un estado que desaparece
  se retira inmediatamente. Un fallo aparece incluso durante una pulsación.
- Cambios del conjunto de estados conservan la opción actual si sigue siendo
  válida y reinician su permanencia4s; al desaparecer un fallo se recupera el
  primer estado aplicable. Un aviso modal conserva su ciclo de cierre habitual.
- Los tiempos se basan en diferencias uint32 del reloj cooperativo, sin delays,
  interrupciones nuevas ni temporizadores de hardware.

## Redibujo y presupuesto

`afr2_dashboard.c` conserva una caché pequeña dentro de `Afr2Ui`, sin heap ni
framebuffer adicional. El estado declarado suma28bytes y el snapshot4bytes,
según los tipos fuente; el tamaño efectivo y el stack no fueron medidos.

La rama del dashboard retorna antes del borrado global. El cambio de opción
limpia únicamente el interior de la tarjeta, respetando sus esquinas; los cambios
de modo limpian solo la columna izquierda. La cabecera sigue su revisión de red,
validez de batería y offsets de texto. Los iconos fijos no se repintan al mover
las bandas SSID/IP. La revisión UI se incrementa cuando el renderer tiene cambios.
El driver OLED existente sigue enviando solo chunks diferentes del framebuffer
reconocido; una copia remota en curso mantiene su bloqueo temporal existente.

## Resto de las pantallas

Se conservan identidades y flujos. Los componentes de cabecera14px invertida,
contador, fuente7×10, progreso124×8, sensores IR, selección y ayudas se reutilizan:

- Menús: tres filas16px y selección invertida; Volver usa la flecha anterior16px
  con texto y padding4px. El retorno usa las guardas existentes.
- Vistas comunes: cabecera con icono de sección, datos en y16/28/40 y retorno
  con encoder largo + flecha. Capacidades usa contador superior1/6..6/6.
- Conexión y datos AP, distancia numérica e IR numéricos usan la cabecera común.
  El gráfico IR conserva sus cinco barras, canal arriba y valor debajo.
- Selectores de motor, editores, modo Wi-Fi, datos de red y estado del encoder
  se ajustan a la misma familia. Teclado conserva máscara de clave y cursor;
  la grilla se sitúa sin invadir el pie.
- Guardar cambios conserva la decisión Sí/No obligatoria, con selección invertida.
  Un largo no guarda, descarta ni evita esa elección.
- Cards rápidas y secuencias mantienen los componentes ya compartidos. El radar
  gráfico, el resumen Hall de cuatro filas, la prueba del encoder y los avisos
  conservan sus composiciones compactas para preservar datos y controles.
- QR sigue siendo la excepción de altura completa con caja62×62, quiet zone y
  columna derecha; conserva generación en el Mega y protección de credenciales.

## Web y entrega

`dashboardRenderer.ts` replica temporización, selección de estados, geometría
y actualización por regiones. La simulación usa el mismo reductor y fixtures
identificados; transmite giro/pulsación y SW mantenido. No envía comandos reales.
El visor conectado sigue mostrando el framebuffer recibido.

La copia de recursos solo ejecuta `sync-oled-library.mjs --copy-only`, sin sus
checks. No cambia EX01, EEPROM, transporte ESP ni un paquete Web instalado.
Se requiere una compilación/promoción/flasheo autorizados para instalarlo; quedan
pendientes memoria/Flash, tiempos, lectura visual física y escaneo QR. No se
realizó commit ni push en esta entrega.
