> **AFR2-20261029-29:** desde dashboard, corto abre menu completo010002; largo al soltar abre pantallas rapidas010003. Ambos accesos conservan retorno al origen por historial. Sustituye el acceso comun de28. Fuente sin compilar/cargar.

> **AFR2-20261029-28:** corto desde dashboard abre el carrusel; largo tambien conserva ese acceso al soltar. Fuente sin compilar/cargar. La reserva del corto que figura en el diseño27 inferior queda sustituida.

# AFR2-20261029-27 — Controles rápidos, radar y PWM manual

Estado: ATmega/ESP/Web implementados en fuente. Sin compilación, validadores, pruebas de navegador, ensayos físicos, promoción Web ni flasheo. Recursos de catálogo/fuente/iconos copiados con `sync-oled-library.mjs --copy-only`, sin ejecutar sus validadores. Firmware nominal ATmega/ESP0.11.0; Web0.4.0. Loader sin cambios de esta ficha. Se conserva la extensión de batería AFR2-20261029-26 dentro de EX01v9.

## Navegación y tarjetas

Se agrega `0x010003` (carrusel), conservando `0x010002` para Configuración y pruebas. Giro recorre seis tarjetas: barrido, servo, motores, seguir línea, rastro y configuración. Corto alterna la función o entra en configuración; no produce movimiento al habilitar motores. Las primeras cinco tarjetas muestran el estado actual, no el estado solicitado. La confirmación del PWM de servo espera el registro aplicado del Timer5; timeout500ms muestra falta de confirmación. Mientras hay una petición pendiente no se permiten toggles repetidos.

SW conserva debounce20ms y umbral800ms. El evento largo se publica exclusivamente después de soltar; no publica corto. Arranque con SW sostenido exige liberar antes de otro gesto. Dashboard ignora corto y giro como navegación; largo abre carrusel. Largo en carrusel vuelve al dashboard; en otros lugares vuelve por historial (selección y scroll incluidos). Volver explícito utiliza el mismo guard. Cancelar una edición numérica/teclado descarta solo esa edición. Cerrar un aviso retorna a la vista subyacente.

Excepciones: el diálogo `0x030038` exige Sí o No y no admite largo. Hall selector/conteo conserva los ScreenCodes y termina con corto; largo al soltar cancela solo el intento actual, preserva otros borradores y muestra aviso. Salir del dominio Hall con borradores conserva su confirmación de guardado.

## Radar y servo

`0x03003b`: opciones Alcance del gráfico / Ángulo del servo. `0x03003c`:4/2/1/0.5m. Los índices0/1/2/3 corresponden a4000/2000/1000/500mm, valor inicial0. El selector modifica RAM al confirmar con corto. Largo sin confirmar conserva el valor anterior. Cambiar escala nunca altera timeout, validez o rango nominal del HC-SR04.

Proyección proporcional por distancia/rango, marcadores rellenos de radio2px para legibilidad; no se infiere tamaño físico de un objeto a partir del eco. Fuera de escala no se dibuja un punto en el borde: el radar conserva distancia numérica e indica `>esc`. Referencias en cm, con decimales exactos si corresponden, en los mismos glifos compactos existentes. El cono nominal existente de15grados totales es una representación aproximada; no es una calibración del haz ni se calcula desde el ancho del auto.

El editor compartido `0x030033` muestra radar compacto sobre barra y valores de ángulo/PWM aplicado/solicitado. El cono usa el ángulo derivado del PWM aplicado; cada detección conserva su ángulo capturado. No hay realimentación de posición mecánica del servo. Pausar el barrido no apaga PWM ni vacía bins/cola. Salir reanuda solo un barrido previamente pausado, preservando muestras y esperando estabilización. STOP, fallo o revocación de movimiento cancelan la reanudación pendiente. Un emisor remoto no toma el servo mientras está en edición local; apagar/STOP sí cancela.

Rastro oculto dibuja solo la última muestra. Adquisición, bins e historial siguen funcionando. Alcance y rastro son preferencias; los estados de marcha no se restauran automáticamente al reiniciar ni al reconectar. La habilitación de motores existente no implica una orden de movimiento.

Timer5 mantiene el PWM software en D13/PB7; se desconectan las salidas alternativas OC0A/OC1C de ese pin sin alterar OC1A/B de motores. Cada ISR programa su próximo compare hacia el futuro desde TCNT5 para evitar una espera hasta wrap si se atendió tarde. Esto es una corrección preventiva en fuente, no una causa física confirmada. Falta comparar la prueba independiente Timer1/OC1C con la salida Timer5 del producto.

## Interfaces EX01v9

Se preservan framing ROBO2/profile2 y todos los IDs previos. Nuevo comando `AFR2_EXT_DRIVE_PWM=0x1e`; feature `AFR2_EXT_F_MANUAL_PWM=0x80000`, máscara conocida `0xfffff`, incluyendo batería0x40000.

Petición de12bytes: prefijo reservado u32=0; forwardPct i16LE en4; strafePct i16LE en6; turnPct i16LE en8; ttlMs u16LE en10. Porcentajes enteros[-100,100], TTL[200,1000]ms. Éxito devuelve el envelope de resultado sin payload adicional. Web/ESP: `robot.drive.pwm` con esos cuatro campos. No se reinterpretan los mm/s/mrad/s de `robot.drive`.

Mezcla AVR FL=f-s-t, FR=f+s+t, RL=f+s-t, RR=f-s+t; normaliza por max(100,pico) y aplica el límite PWM configurado, signos y rampas existentes. Modo nuevo7=`manual-pwm`, añadido sin renumerar0..6. STATE mantiene16bytes; su byte4, antes reservado, informa motor_ready0/1. La respuesta legacy presenta el PWM manual como manual, conservando profile2.

PWM manual no requiere CPR, geometría ni HC-SR04. Exige motor habilitado, ausencia de fallo/calibración activa y respeta propiedad del control, TTL, presencia Web y control-gap watchdog. Hall vigila pulsos cuando está disponible sin requerir CPR. No aplica PID de velocidad a porcentajes. Prueba individual también respeta el bloqueo de motores. Deshabilitar motores detiene las salidas y bloquea nuevas órdenes; no cancela un radar independiente salvo STOP general.

Web anuncia `manual_pwm` por separado de `mecanum_drive`; la UI manual usa porcentajes del límite configurado. GOTO, secuencias y seguimiento conservan requisitos calibrados y protección frontal. Los comandos anteriores de velocidad permanecen disponibles con sus unidades anteriores.

Acciones UI añadidas70..79: alcance, rastro, dirty visual, descartar visual, toggle radar, toggle servo, toggle motor, toggle línea, entrar/salir de editor servo. CONFIG solo autoriza los editores numéricos anteriores y alcance/rastro70/71, no acciones arbitrarias. Los toggles físicos de movimiento no se exponen mediante selección sintética del OLED; se mantienen APIs Web explícitas y STOP prioritario. El diálogo visual de alcance/rastro sí admite la elección remota Sí/No, con captura y requisitos vigentes; el asistente y guardado Hall permanecen locales.

## Persistencia y conexión

EEPROMv5 agrega dos bytes al payload58 anterior: alcance en58 y rastro en59, payload60. Carga registros válidos v1..4 con alcance4m/rastro visible, sin perder calibraciones ni reescribir al cargar. Conserva doble registro128bytes, CRC y commit final cooperativo. Los guardados existentes conservan toda la configuración; la confirmación de salida visual usa ese guardado común. No descarta otros cambios al elegir No: restaura únicamente alcance/rastro guardados. No se limpia un borrador antes del commit real; se impiden cambios de configuración concurrentes con escritura.

ESP reserva una ranura por sesión para hello/alive validados, independiente de la cola UART. STOP conserva prioridad. No se aumenta el timeout de sesión para ocultar bloqueos. El cliente tiene un solo cierre/reintento por generación y un latido pendiente por vez. Conserva código, motivo y fecha del último cierre; estado/counters Wi-Fi y WS y bootId del ESP se exponen como `connectionDiagnostics`. Sistema muestra esos datos como último reporte, sin afirmar que una lectura anterior prueba el estado de radio en el instante del corte.

Wi-Fi usa solo el reintento del servicio, desactiva autorreintento paralelo del SDK, reconecta a la red guardada y no reinicia una conexión estable. Los escaneos de listados solo se inician por comando. AP-only se restaura después del escaneo explícito; un timeout no se presenta como cancelación del SDK mientras siga en ejecución.

## Aceptación pendiente y despliegue

No se ejecutó esta matriz: tarjetas/retornos y rechazo sin falsear estado; largo800ms/boot-held/rebote; cuatro escalas y muestra fuera de vista; editor con/sin barrido, STOP y pérdida de enlace; IR digital/ADC0/1023; Hall borradores/cancelación/guardado y corte de alimentación EEPROM; PWM sin CPR ni HC, cada rueda/dirección, bloqueo/TTL/STOP/desconexión; WS con Mega ausente/demorado, router caído y escaneo explícito.

Revisar ELF/map y pila/ISR antes de aceptar memoria:7085bytes estáticos y1107libres corresponden al build19 anterior, no a estas fuentes ni al agregado de batería. No se agregó framebuffer ni fuente raster; se reutilizan buffers y glifos existentes, textos estáticos nuevos del renderer en PROGMEM. OLED mantiene envío diferencial por chunks16 y servicio cooperativo existentes. Pantalla negra, latencia y movimiento necesitan ensayo autorizado.

El último registro disponible instaló el flasher temporal en Mega COM8 a13:36:03 del29/10/2026; no prueba su estado actual. Antes de ensayar, identificar firmware efectivo y versiones de las dos placas/Web. Despliegue pendiente: compilar y medir memoria con autorización, empaquetar/promover Web a LittleFS, cargar ESP/FS y restaurar producto AVR si se utilizó el puente. No cargar solo una parte ni afirmar compatibilidad con un ESP antiguo de máscara estricta. No se abrieron puertos en esta intervención.
