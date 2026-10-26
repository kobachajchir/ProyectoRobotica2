# AFR2-20261025-16 — Radar con volumen y controles de escaneo

25/10/2026. Pedido humano: mejorar el plano frontal del radar con líneas 3D
y aspecto tecnológico/neón, seleccionar Desde/Hasta en una sola barra, y usar
barras con entradas numéricas para velocidad, estabilización e intervalo.
Conservar la identidad existente y escribir «Configuracion» sin acento.

## Cambio y comportamiento

Antes, el radar usaba trazos planos y una cámara cenital; los parámetros eran
campos numéricos independientes. Ahora tiene proyección oblicua, plataforma
con espesor, anillos tubulares, marcas angulares, contorno elevado y brillo
rubí. Los ecos son hitos tridimensionales ubicados por ángulo/distancia reales;
su altura es un recurso gráfico y no una medición de altura del obstáculo.
Las etiquetas métricas pertenecen a la escena. No se altera la escala del auto.

- Zona de escaneo: dos selectores en una barra 0–180°, paso/separación mínima
  de 5°. Desde y Hasta también admiten escritura. No se cruzan.
- Sector inmediato en la escena; «Vista previa · sin aplicar» distingue la
  edición de lo aceptado por el robot. El histórico no se recorta por el sector.
- Velocidad 1–30°/s, estabilización 50–500 ms e intervalo 70–1000 ms:
  barra y campo vinculado, con pasos enteros como admite el protocolo.
- El texto se confirma con Enter o blur, se limita al rango y se redondea al
  paso. Vacío/no numérico restaura el valor anterior; Escape descarta la edición.
  La barra refleja la confirmación y el ajuste se anuncia de forma accesible.
- Aplicar sigue enviando `robot.radar.configure`; editar no escribe en hardware.
  El inicio se bloquea si hay cambios pendientes para evitar barrer otra zona.
  Se conservan los bloqueos por desconexión, radar activo y PWM habilitado.
- «Corregir giro invertido» explica que intercambia pulsos extremos y mantiene
  90° en el centro. No modifica la semántica ni la calibración del firmware.
- «Configuracion» en la barra lateral y «Configuracion y apariencia» en título,
  tooltip y nombre accesible del modal, reemplazando también el texto mal codificado.

La aguja sigue el ángulo recibido y conserva interpolación con reduced motion.
No hay un barrido decorativo ni lecturas generadas desde la animación. Canvas
renderiza bajo demanda, sin bloom ni dependencias nuevas. Las barras siguen al
puntero sin easing. El mapa permanece visible al desplazarse en escritorio;
en anchos menores se conserva una columna.

## Estado real

| Módulo | Estado de esta entrega |
|---|---|
| ATmega | Sin cambio de fuente/protocolo/OLED; conserva compilación14 |
| ESP | Sin cambio de firmware ni activos servidos; conserva compilación14 |
| Web | Fuente16 implementada; sin compilar, ejecutar, renderizar ni probar |
| Contracts | Ficha, contrato Web, manual/cobertura VIS-037 y estado actualizados |
| CAD | Sin intervención; conservadas fuentes/cambios locales |

No se ejecutaron tests, compilaciones, validadores, navegador, flasheos ni
promoción de activos a ESP. La compilación14 no cubre esta nueva interfaz.
El borrador visual0.1.9 ya estaba autorizado; la dirección actual está Confirmada,
la composición queda Propuesta y su resultado visual No verificado.

Fuentes principales: `AutoFinalRobo2-Web/src/features/sensors/RadarScene.tsx`,
`SensorsPage.tsx`, `ScanControls.tsx`, `src/styles/sensors.css` y
`src/components/navigation/RouteNav.tsx`. Modelo3D/rig, IR, ruedas y navegación
OLED conservan su implementación; los cambios concurrentes de otros módulos
no forman parte de esta ficha. Sin commit/publicación solicitado.
