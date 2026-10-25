# AFR2-20261023-21 — Controles OLED, avisos cerrables, giro gráfico y secuencias de acciones

Pedido humano: implementar en la parte Web el plan *«Controles OLED, avisos cerrables y secuencias de acciones»*, adaptando los patrones de controles OLED y líneas de tiempo de la base de referencia `AutoMicro` a `AutoFinalRobo2` con superficies sólidas (`--surface-solid`), acento carmesí/rubí (`--action`), overlays glass y una tarea visible por vez (`VIS-018`).

| Módulo | Estado real de esta ficha |
| --- | --- |
| `atmega` | **Diseñado (`sin cambios en fuente AVR`)**. Perfil ROBO2 3 (`0x07–0x0E` y `0x10–0x15`), PWM/dirección, lease de 1500 ms y buffer estático de 32 segmentos pendientes de integración y medición de SRAM/pila. |
| `esp` | **Diseñado (`sin cambios en fuente ESP`)**. Validación, arbitraje de dueño de mando y traducción de `control.renew` y `robot.sequence.*` pendientes de integración en `broker` y `mega_link`. |
| `web` | **Adaptado en fuente y compilado (`tsc -b`, `vite build` y paquete LittleFS verificados con 0 errores)**. Avisos cerrables compartidos, controles OLED siempre visibles con estado largo retenido hasta soltar, giro gráfico sobre el eje, editor/persistencia `afr2-sequence-1`, simulador Mecanum de 10 ms y cliente de ejecución física. |
| `shared` | **Diseñado y documentado**. Catálogo `commands.json`, `capabilities.json`, `PROTOCOL.md`, `UART_PROFILE_3.md`, `WEB_OLED_CONSUMER.md`, `WEB_CONTROL_MAP.md`, manual visual `0.1.8` (`VIS-018`) y manifiesto de procedencia actualizados. |

---

## 1. Avisos con X (`DismissibleNotice` + `ConnectionContext`)

- **Componente compartido**: `src/components/ui/DismissibleNotice.tsx` reemplaza los avisos fijos de parada en cabecera (`RouteNav.tsx`) y modales (`FloatingModal.tsx`), mientras los toast de Sonner permanecen `bottom-right` con su propio botón `X`.
- **Identificación y cierre sincronizado**: cada actualización de `stopRecord` incrementa `noticeId` (`sending` → `confirmed` / `unconfirmed-offline`). Cerrar el aviso (`dismissStopNotice`) oculta ese `noticeId` en ambas ubicaciones y evita que reaparezca al cambiar de ruta o abrir un modal, conservando intacto el estado real de `STOP`, conexión y mando. Una nueva orden `STOP` o un cambio posterior de resultado genera un nuevo `noticeId` y vuelve a mostrar el aviso.
- **Accesibilidad**: botón `X` accesible por teclado (`Enter`/`Space`), área táctil mínima de `44×44 px` (`2.75rem`), texto multilínea sin truncado y foco visible carmesí/rubí.

## 2. Controles OLED adaptados (`EncoderControl` + `LiveOledViewer`)

- **Disposición e iconografía**: reutiliza de `ScreenStreamWorkspace.tsx` la disposición de controles físicos, las flechas `TurnArrowIcon` y el icono matricial `MonoBitmapEncoderIcon` (`Icon_Encoder_bits`) en un componente común para `/oled` y el modal flotante.
- **Siempre visibles + sincronización manual**: muestra siempre **Giro izquierdo, SW y Giro derecho** junto al botón **Actualizar pantalla**, incluso cuando estén deshabilitados, indicando el motivo explícito junto al control (enlace incompleto, capacidad remota no implementada, captura caducada o solicitud en curso).
- **SW único y umbral de 800 ms**: sin botón `USER`, `Menú` ni `Atrás` (retroceso solo por la opción `Volver` en pantalla). Pulsación corta (`< 800 ms`) confirma al soltar; pulsación larga (`>= 800 ms`) envía `menu` una sola vez, cambia a color de atención (`#f59e0b` / `#fbbf24`) y texto específico, y **mantiene el estado visual largo hasta soltar**, aunque la solicitud ya esté en curso (`available` separado de `busy`). Soltar después de largo nunca envía confirmación corta.
- **Cancelación y autoridad**: cubre `setPointerCapture`, `pointercancel`, `lostpointercapture`, `blur`, `visibilitychange`, cierre del modal, pérdida de disponibilidad y teclado `Space`/`Enter` ignorando `event.repeat`. La pantalla se actualiza únicamente desde la respuesta/snapshot del controlador, sin navegación optimista.

## 3. Giro sobre el eje (`ChassisSpinGraphic`)

- **Vista superior del chasis Mecanum**: `src/features/control/ChassisSpinGraphic.tsx` dibuja el chasis superior con las 4 ruedas a 45° y flecha circular antihoraria (`ccw`) u horaria (`cw`).
- **Control manual y bloques**: sustituye los botones textuales de rotación en `ControlPage.tsx` por dos controles con etiquetas visibles **«Girar izquierda»** y **«Girar derecha»** (feedback inmediato al presionar, mando sostenido por puntero capturado o teclado y parada al soltar/cancelar), y se reutiliza como símbolo de los bloques `pivot` en el editor de secuencias. Representa la acción solicitada, sin anunciar giro físico medido.

## 4. Editor de secuencias, persistencia `afr2-sequence-1` y simulador Mecanum

- **Tarea `Secuencias` en Control**: al seleccionarla en `TaskSections`, muestra el mapa (`ControlMap`), la barra de transporte y el editor (`SequenceEditorPanel`), ocultando los formularios de las otras tareas.
- **Dependencias de reordenamiento**: se declaran en `package.json` las versiones de la referencia (`@dnd-kit/core` `6.3.1`, `@dnd-kit/sortable` `10.0.0`, `@dnd-kit/utilities` `3.2.2`), complementadas con movimiento por teclado (`Alt+←` / `Alt+→`) y botones explícitos (`‹` / `›`).
- **Dos niveles de edición**:
  - **Movimientos (`movements`)**: bloques de desplazamiento constante (`vx`, `vy`), rampa lineal, giro sobre el eje (`pivot`), curva (`curve`), espera (`wait`) y finalización terminal (`finish`, `0 ms`).
  - **Cuatro ruedas (`wheels`)**: pistas sincronizadas `FL`, `FR`, `RL`, `RR` con velocidad firmada (`mm/s`), rampas, esperas y finalización, bajo una única escala temporal y cursor compartido. Una rueda cuya pista termine antes permanece en `0 mm/s`.
  - **Conversión sin pérdida**: en modo `Movimientos`, las 4 pistas derivadas son de consulta; el botón **«Crear copia avanzada»** genera una copia editable de 4 ruedas en la biblioteca registrando `provenance` y conservando el programa original.
  - **Propiedades y edición**: panel lateral en escritorio y `FloatingModal` en móvil; añadir, duplicar, eliminar, renombrar (doble clic o campo), reordenar y deshacer/rehacer (`Undo`/`Redo`). Bloquea ediciones durante la reproducción.
- **Documento `afr2-sequence-1` y persistencia**: unidades `mm`, `ms`, `mrad`; autoguardado en `localStorage`, biblioteca de programas, exportación JSON e importación con validación previa de versión, estructura y límites (`64` bloques por pista, `600 000 ms` / `10 min`, duraciones múltiplos de `10 ms`). Archivos dañados o incompatibles muestran aviso cerrable conservando el trabajo actual.
- **Simulador determinista Mecanum**: integración pura en pasos de `10 ms` y animación con `requestAnimationFrame`; perfil inicial ilustrativo `lxPlusLyMm = 300 mm` y `maxWheelSpeedMmS = 800 mm/s` etiquetado como no calibrado; conversión a ruedas sin recortes silenciosos de velocidad; separación explícita en `ControlMap` entre **Simulación** y **Robot conectado** para que la pose simulada jamás ingrese a `robotState`. Ocultar la página pausa la simulación.

## 5. Ejecución física e interfaces compartidas

- **Sobre `afr2-ws-1` y ROBO2 perfil 3**: se incorporan `control.renew` (lease de `1500 ms` renovado cada `500 ms` con Control visible y supervisado) y `robot.sequence.begin`, `append`, `commit`, `start`, `cancel` y `status` ([UART_PROFILE_3.md](../contracts/UART_PROFILE_3.md), `0x07–0x0E` y `0x10–0x15`, límite físico inicial de `32` segmentos y `120 s`).
- **Supervisión estricta**: en físico solo existen **Iniciar en robot** y **Cancelar ejecución**; progreso consultado cada `250 ms` sin solapamiento; habilitado únicamente con enlace, lease, motores, STOP, capacidad `sequence_runner` y perfil físico calibrado; cancelación inmediata al perder conexión, ocultar la página, abandonar Control o pulsar `STOP`.

## 6. Verificación y límites

- A pedido expreso del usuario, se instalaron las dependencias `@dnd-kit/core@6.3.1`, `@dnd-kit/sortable@10.0.0` y `@dnd-kit/utilities@3.2.2` y se ejecutó `npm run build` en `AutoFinalRobo2-Web`:
  - `tsc -b`: 0 errores de tipos.
  - `vite build`: 652 módulos transformados, `dist/index.html` (`1,691.54 kB`, `gzip: 463.13 kB`).
  - `postbuild` (`node tools/package-web.mjs`): paquete LittleFS generado en `artifacts/littlefs/dist` (6 archivos, `2,893,807 bytes gzip`).
- No se ejecutaron pruebas de hardware ni flasheos.
