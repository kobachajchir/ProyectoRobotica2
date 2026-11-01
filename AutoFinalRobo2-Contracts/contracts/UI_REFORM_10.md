# AFR2-20261030-07 — UI coordinada, EX01v10 / EEPROMv8

Estado: fuentes AVR, ESP y Web adaptadas; sin generación, compilación, validadores, tests, navegador, promoción, carga ni ensayo físico. Sustituye únicamente los pendientes cubiertos de05/06. No acredita memoria disponible, paridad ni hardware.

## Wire y permisos

Framing ROBO2/perfil2, ruta, epoch, secuencia, checksum y resultados conservados. EX01 pasa de9 a10 una sola vez; ESP exige versión exacta antes de habilitar producto. Los comandos nuevos usan el mismo MegaLink y la misma cola; STOP y supervisión retienen prioridad.

| Comando | Request body | Response body OK, sin envelope de9bytes |
|---|---|---|
| 0x11 STATE | vacío | primeros16bytes conservados, byte16 configurationDirty 0/1, bytes17..18 longPressMs u16LE;19bytes totales |
| 0x20 UI_META | vacío | primeros13bytes conservados; byte13 itemCount dinámico (0 en vistas/avisos);14bytes |
| 0x24 TOUCH_UI | enabled:u8, direction:u8 0..8, heading:u8 0..23, powerPercent:u8 0..100 | powerPercent:u8 confirmado;1byte |
| 0x32 DIAG_POLL | vacío | active:u8,jobId:u32LE,challenge:u32LE;9bytes |
| 0x33 DIAG_RESULT | jobId:u32LE,challenge:u32LE;8bytes | vacío |

DIAG es un trabajo de origen OLED: un clic físico crea un job distinto de cero; el ESP consulta a intervalos nominales400ms y devuelve el patrón jobId XOR0xAF203307 una vez. Un único trabajo simultáneo. El AVR compara id/patrón y plazo, rechaza respuestas tardías/de otros jobs, muestra éxito solo tras coincidencia. Sin reintento automático del resultado. Timeout por defecto2000ms, rango1000..5000/paso250, ajuste de sesión no persistente. Se deriva del polling y del deadline800ms existente, no de una medida física. Salir de la vista no anuncia cancelación; el resultado puede notificarse después. Desconexión y timeout producen error, nunca éxito.

TOUCH conserva lease2500ms y takeover con STOP al activar. Renovaciones solo presentan rumbo/dirección/porcentaje. Porcentaje confirmado no significa PWM aplicado ni velocidad medida. Aviso único1200ms desde el último cambio. Web ignora confirmación sin porcentaje coincidente. Preview de conducción no solicita la UI del robot. STOP/heartbeat no esperan a que termine el aviso.

Acciones UI nuevas81 largo(ms),82 consulta dirty,83 potencia manual local porcentual,84 iniciar diagnóstico,85 timeout,86 toggle rueda y87 toggle GOTO. CONFIG sigue admitiendo únicamente editores numéricos encontrados en catálogo, sin invocación arbitraria.84/86/87 y selector80 no se ejecutan por clic UI remoto.37..40 dejan de ser editores. La potencia manual local83 ajusta las consignas porcentuales locales, por defecto100 para conservar el comportamiento anterior; no modifica el límite PWM. El valor visual0..255 es redondeo de255*p/100.

## EEPROM

Versión8: payload63bytes en los mismos dos registros128bytes, CRC16 y commit último conservados. Los primeros60bytes mantienen formato; offsets60..61 long_press_ms u16LE, offset62 servo_inverted u8. Lectura de1..7 completa800ms/0 sin escribir automáticamente. Preserva CPR, signos, umbrales y otros ajustes. Dimensiones nominales y Hall instalado se normalizan en RAM desde el perfil fijo; un registro anterior no reintroduce geometría editable. Escritura explícita desde Preferencias. Largo800..3000 en pasos100; se aplica al lector cuando SW está liberado, sin cambiar debounce20ms ni añadir ajuste de corto. Inversión de servo requiere servo/radar desactivados; descartar restaura también la inversión del servicio o informa rechazo.

Dirty compara campos y arrays explícitamente, no padding. Descartar se oculta cuando está limpio. Se invalidan navegación/capturas si cambia el conjunto o significado de filas. UI_META y JSON de captura transmiten itemCount dinámico para que los controles laterales Web no usen el total estático anterior al ocultamiento. Confirmación Si/No rectangular obligatoria; largo no la evade. El guardado se confirma después del commit asíncrono.

## Identidades y recursos

Se conservan IDs previos. Nuevas03003e prueba física encoder,03003f prueba ESP y030040 componente de potencia.030028 pasa a seis páginas.010003 mantiene siete tarjetas y selector03003d. Primer largo en prueba encoder acredita el gesto; otro largo sale; giro selecciona Volver y clic sale. UI remota no acredita eventos físicos. Contadores de prueba se reinician al entrar.

Catálogo reform[] reorganizado se resuelve igual para dibujo y acción. Estado agrupa OLED/encoder/enlace; Preferencias agrupa encoder/inversión de servo/guardar/descartar. Principal siempre reinicia desde dashboard. Radar restaura selección y firstVisible. Inicio/Detener se decide contra estado actual de radar, línea, rueda y GOTO; STOP general queda independiente. Páginas acotadas muestran actual/total y dirección. Nueva flecha horaria alrededor del encoder:32bytes PROGMEM, autoría AFR2 sobre forma de encoder previa atribuida a kobac. Sin framebuffer adicional.

Web incluye reformCatalogue.ts y reformResources.ts como contrapartes fuente explícitas del C mientras el generado permanece anterior. sync-oled-library.mjs queda preparado para reform[], pero no fue ejecutado. Tras regeneración autorizada deben retirarse las compensaciones que el generado ya cubra. No editar los bytes del generado para simular una ejecución. Hashes revisados corresponden al port fuente, no a una prueba de paridad.

## Radar y red

La adquisición reemplaza la casilla de5grados más próxima al ángulo derivado del PWM aplicado; no es feedback físico de eje. Web agrupa deltas/snapshot en la misma casilla para evitar duplicados por redondeo. Reemplaza la casilla aun cuando el eco nuevo sea inválido. Se mantienen las otras casillas. Con rastro activo no se dibuja además una segunda copia permanente de latest; sin rastro se dibuja solo latest. Web sustituye también la posición del eco actual inválido y conserva secuencia/boot/scan para descartar datos viejos.

RSSI STA fresco llega al snapshot con validez, distinto de AP/desconectado/desconocido. Evaluadas bandas gráficas -55/-67/-75dBm. Se conserva icono fijo: no hay mapa actualizado de flash/pila para aprobar la variante. Cuatro XBM19x16 completos costarían192bytes brutos,144 adicionales si se reutiliza el actual, más tabla/código; máscaras reducirían recursos pero no se midió el código resultante. Snapshot suma3bytes de campos RSSI antes de padding. Decisión prevista por el plan, no pendiente de implementación funcional obligatoria.

## Presupuesto y evidencia

Nuevos campos persistentes3bytes por Afr2Config; encoder añade2bytes; UI añade campos de selección/prueba/potencia, timestamp y firma de filas; app añade estado/job/timeout y potencia local; ESP añade buffer STATE3bytes y datos del trabajo. Shadow configs y temporales también crecen. Sin cálculo final de tamaños enlazados, máximo de pila/ISR o tiempos; ninguna reserva se declara suficiente. No se añade framebuffer1024bytes ni se abre I2C/UART/COM por esta tarea.

Geometría y método: ../hardware/GEOMETRIA_FIJA_CAD.md. Matriz por sección y comprobaciones futuras: ../changes/2026-10-30-07-ui-integral.md. Autores previos y referencias originales preservados.
