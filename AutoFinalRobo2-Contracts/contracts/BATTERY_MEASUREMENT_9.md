# Batería y confirmación de reinicios — AFR2-20261029-26

29/10/2026. Fuente integrada en ATmega/ESP/Web, sin compilar ni ejecutar. Complementa EX01 anterior con versión **9**, feature `0x40000` y máscara `0x7ffff`. Framing ROBO2/perfil2 y `afr2-ws-1` conservados. Deben actualizarse ambos firmwares y la Web juntos; la negociación existente exige igualdad de versión, no se afirma compatibilidad con EX01v8.

## Circuito y adquisición

A5/PF5/ADC5, divisor **10 kΩ + 10 kΩ**, sin diodo, **104/100 nF** entre nodo A5 y masa, en paralelo con resistencia inferior. Toma del voltímetro después de llave; corte comunicado por usuario. Fuente en `hardware/MEDICION_BATERIA.md`. A5 reservado en fuente por esta revisión; montaje/calibración no comprobados. No se conecta BAT+ directamente a A5. JTAG deshabilitado como ya requería IR/PF4. Sin Arduino AVR, DMA ni nuevo dispositivo I2C.

Arranca deshabilitado, sin dato ficticio. `Realizar medicion de bateria` inicia/reinicia adquisición periódica en RAM hasta reset. No modifica EEPROM, no enciende motores, no guarda una calibración ni deduce conexión física. Con batería desconectada y USB conectado puede medirse cero; no significa porcentaje de carga ni prueba de cableado. Repetir Measure vuelve a pending y reinicia la secuencia de esa sesión de adquisición.

Un solo dueño de ADMUX/ADCSRA en afr2_sensors.c: termina conversiones IR en curso antes de tomar ADC5. Batería usa espacios entre barridos IR, aun con IR digital o apagado. Prescaler128, AVCC, entrada sin pull-up, buffer digital ADC5 deshabilitado sin deshabilitar buffers IR digitales. Se descarta primera conversión de batería tras selección de canal y promedian8 muestras, separadas al menos10ms tras cada muestra; temporización cooperativa, no promesa de frecuencia real. Si alguna muestra llega a1023, se publica fuera de rango aunque el promedio sea menor. El gestor reinicia la conversión al cambiar line_kind, conservando arbitraje.

Referencia nominal **5000 mV**, relación2/1. `millivolts = (raw * 10000 + 512) / 1024` con división entera y producto32bits. Estado sin calibrar explícito. No porcentaje, alarma por química, corte automático, corriente ni autonomía.

## Wire

Comando **0x55** (`AFR2_EXT_BATTERY`), sin colisión con red0x53/0x54 ni sensores0x60..0x6A. Sesión/transacción/status existentes: prefijo de solicitud8bytes y respuesta9bytes. Cuerpo de solicitud exactamente1byte: **0 leer cache, 1 iniciar/reiniciar**. Valores/longitudes distintos son BAD_REQUEST. Productor ATmega autoritativo; requiere extensión negociada y no reinicio en curso. Sin token de control motor. Parser ESP comprueba longitud, estado, formato, relación, referencia y coherencia del resultado.

Respuesta OK: cuerpo23bytes; total32 con prefijo. Todos los enteros multibyte little-endian:

| Offset del cuerpo | Tipo | Campo |
| --- | --- | --- |
| 0 | u32 | bootId no nulo |
| 4 | u32 | sampleId, cero sin promedio completo |
| 8 | u32 | ageMs, AFR2_AGE_UNKNOWN si no hay muestra |
| 12 | u8 | 0 off, 1 measuring, 2 valid, 3 out-of-range, 4 stale |
| 13 | u16 | promedio raw0..1023; cero en off/pending |
| 15 | u16 | mV calculados; consumidores no los presentan como válidos fuera de estado2 |
| 17 | u16 | referencia5000mV nominal |
| 19 | u8 | numerador divisor2 |
| 20 | u8 | denominador divisor1 |
| 21 | u8 | calibrated=0 |
| 22 | u8 | canal5 |

Edad mayor a2000ms implica stale. Saturación no equivale a tensión exacta del pack. Timeout/correlación/sesión siguen ROBO2: UART800ms, WS2500ms; BUSY se presenta como tal y no se reenvía Measure automáticamente. No nuevo ACK/evento asincrónico: la respuesta a Measure confirma inicio y estado pending; lecturas Get posteriores muestran promedios completados. No se cancela adquisición al cerrar la vista; reset vuelve a off.

## Web y OLED

`system.battery.get {}` → 0x55/0; `system.battery.measure {}` → 0x55/1. Respuestas WS completed con `bootId,sampleId,state,valid,ageMs,raw,millivolts,referenceMv,dividerNumerator,dividerDenominator,calibrated,analogPin`. off/pending: age/raw/mV null; range/stale: mV null, raw/edad presentes. `battery_voltage` anuncia servicio negociado, no montaje probado ni lectura calibrada. El ESP no inventa voltios. Web comprueba esquema y valores.

Provider único consulta cache cada1000ms con Web visible y capability disponible; nunca inicia adquisición automáticamente. Serializa por requestCommand y no crea toasts por cada consulta. Desconexión/cambio de destino o arranque invalidan dato y respuestas previas; edad Web suma de forma conservadora viaje/cola y tiempo desde recepción. Error de transporte deja caducar el dato; sin dato o >2000ms no se presenta tensión actual.

Pill junto a Red, mismo material/tokens, botón44px y etiqueta accesible, abre modal Batería con estado, edad, raw y botón de medición. También en Configuración → Batería. Icono con4barras por **bandas de voltaje**: cero=0, mayor de0 y menor de6V=1, [6,7)V=2, [7,8)V=3, >=8V=4. Son rangos visuales solicitados, no porcentaje, umbrales de descarga ni clasificación de salud. Sin dato/obsoleto/rango: contorno discontinuo y sin barras activas. Voltios siguen visibles en móvil; sin animación nueva ni color de alarma por umbral no calibrado.

OLED: Sistema0x030026 agrega Batería **0x030039**, con Realizar medicion / Ver medicion / Volver. Medir abre **0x03003A**, tres líneas de snapshot y Volver explícito al submenú. Acción69. Giro selecciona, corto confirma, largo abre principal sin generar corto al soltar. Reutiliza renderer genérico y fuente/iconos existentes; no recurso gráfico nuevo. Copia TS adapta lookup de Sistema (Reinicio5, Batería6, Volver7); preview identificado muestra estado sin medición, nunca datos simulados. Visor live sigue usando framebuffer1024bytes, con suscripción vigente. Generador de recursos/catálogo **pendiente**, no ejecutado sin autorización.

## Confirmación de reinicios Web

Botones Reiniciar ESP/ATmega abren FloatingModal compartido. Confirmar y reiniciar es el único camino que envía `system.reboot {target,confirm:true}`. Cancelar, Escape, cierre y fondo cierran sin enviar. Foco inicial en Cancelar, retorno al invocador; se impiden confirmaciones dobles y se comprueba conexión antes del envío. Se cierra confirmación si se pierde la conexión necesaria. Las restricciones de salidas detenidas y ACK/arranque observados del firmware se conservan; aceptación no acredita arranque nuevo. UI remota OLED sigue sin autorizar reinicios por click sintético.

## Evidencia y despliegue

Solo edición/lectura de fuente. No tests, compilaciones, validadores, generador OLED, navegador, promoción ESP/data/dist, imagen LittleFS, COM, carga ni ensayos físicos. No se modifican binarios instalados. Las fuentes deben regenerarse/compilarse/desplegarse juntas cuando sea autorizado.

Memoria: campos de batería suman11bytes al snapshot de sensores AVR y acumulación estática10bytes nominales por tipos AVR; sin framebuffer nuevo ni crecimiento del snapshot UI. Este conteo por campos no acredita tamaño final del binario, pila/ISR, SRAM/heap suficientes ni latencia. Revisión de memoria/paridad/calibración física pendiente.
