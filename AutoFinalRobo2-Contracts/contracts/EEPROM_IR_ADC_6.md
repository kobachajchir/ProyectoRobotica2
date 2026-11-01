# Perfil IR por ADC10 / journal v6 — AFR2-20261030-03

Pedido explícito del usuario, 30/10/2026: adquirir IR1..IR5 como entradas analógicas, con valores desde cero hasta el máximo del ADC. Cambia la selección de adquisición; no acredita que la barra entregue intensidad analógica ni confirma la causa de los cinco ceros reportados.

## Productor y pines

- IR1 = A4/PF4/ADC4; IR2 = A3/PF3/ADC3; IR3 = A2/PF2/ADC2; IR4 = A1/PF1/ADC1; IR5 = A0/PF0/ADC0. Alimentación/pinout físicos sin cambio. Batería conserva A5/ADC5 con su divisor.
- Nuevo default `line_kind=2`, ADC10 sin signo, raw0..1023; referencia AVCC existente, reloj ADC125kHz nominal con F_CPU16MHz/prescaler128. ADLAR=0 y selección de banco bajo mediante MUX5=0.
- DDRF0..4 entradas, sin pull-ups; DIDR0 bits0..4 en1 para apagar buffers digitales al usar ADC. La lectura efectiva es `ADC`, no `PINF`. Al seleccionar explícitamente modo digital1 se vuelven a habilitar esos buffers. A5 se conserva separado.
- Arbitraje existente de un solo ADC con batería: termina conversiones en curso y no intercala batería dentro de un barrido IR. Conversión de descarte tras cada selección de canal IR y tras seleccionar batería. Adquisición cooperativa, sin esperas activas, Arduino AVR, DMA ni ISR nueva.
- Raw se publica después del barrido completo; calibración/umbral solo condicionan máscara y control. Se conservan filtro2de3, umbrales existentes (512 por defecto), polaridad y separación rawValid/control-valid. Valores0 y1023 son lecturas posibles, no marcadores de ausencia. No afirmar frecuencia/latencia física sin ensayo.

## Persistencia

`AFR2_CONFIG_VERSION=6`. Conserva dos slots128bytes, payload60bytes desde v5, CRC y commit. Lee v1..v6 con las migraciones previas conservadas. Para cualquier registro anterior a6 cuyo modo no sea2, establece en RAM `line_kind=2` y `line_calibrated=0`. Una calibración ya ADC se conserva. No se alteran umbrales, inversión, Hall/CPR/geometría/motores, encoder, radar ni otras preferencias. Un registro6 conserva posteriores elecciones explícitas de modo0/1/2.

La migración no escribe EEPROM automáticamente. Guardar EEPROM conserva el flujo explícito/asíncrono existente. No borrar preferencias para aplicar este cambio. El firmware anterior puede rechazar registros6; downgrade no garantizado.

## Consumidores y compatibilidad

Sin versión nueva de EX01 ni cambios de IDs/longitudes/offsets. `AFR2_EXT_IR_GET`0x62 ya contiene kind2 y cinco u16LE0..1023. ESP traduce a `kind:"adc"`, `raw`, `rawValid`, `calibrated`, `ageMs`, `bits` y `valid`. La Web ya rotula `ADC / 1023` y escala barras a1023. OLED0x030016/0x030032 y su copia TS ya usan máximo1023 para modo2. No se modifican layouts, ScreenCodes, recursos, navegación ni hashes de render por esta entrega.

El diagnóstico de servicio separado conserva su lectura digital directa: no es el producto y este cambio no lo convierte en un diagnóstico ADC. No se ejecuta ni carga ese servicio.

## Estado real

ATmega: fuentes de defaults/configuración/ADC/journal adaptadas; sin compilar, ejecutar ni cargar. ESP/Web: productores/consumidores existentes revisados por lectura, sin cambios de código ni nueva compilación; no se acredita que las versiones cargadas correspondan a esta fuente. Sin tests, validadores, navegador, generadores, COM, flasheos ni pruebas físicas. No certifica precisión, memoria dinámica suficiente ni funcionamiento del sensor.

Una salida digital medida por ADC puede dar principalmente extremos0/1023. El pedido de muestrear tensión no cambia el circuito interno de la barra ni asegura niveles intermedios. La causa original queda pendiente de evidencia física/versión cargada.
