# Medición de batería por ADC — propuesta AFR2-20261029-23

> **Fuente AFR2-20261029-26:** [medición local y Web implementadas en fuente](../contracts/BATTERY_MEASUREMENT_9.md): A5, divisor10k/10k sin diodo, capacitor104 a masa; habilitación explícita, referencia nominal sin calibrar. Pantallas/pill con icono por voltaje y confirmación de reinicios. Sin compilar/cargar/probar. Las secciones de investigación inferiores conservan sus estados históricos.

Investigación del 29/10/2026 solicitada por Koba Chajchir. Elaboración propia por Codex; autorías de fuentes conservadas. Diseño candidato, sin conexión, implementación ni prueba. El usuario informa un máximo aproximado de casi 8 V: química, celdas, tensión máxima de carga y comportamiento del cargador no están confirmados.

## Propuesta vigente sin diodo — AFR2-20261029-25

Koba Chajchir aclara que la toma del voltímetro no tiene tensión sin batería ni durante programación y que la batería tiene una llave que corta su tensión. Se adopta esa toma de tensión de batería después de la llave para el divisor. Montaje comunicado por el usuario; no medido por el agente. Se retira el diodo de la propuesta: no hace falta para bloquear una batería que la llave ya desconecta y se evita su error de caída. Esto no equivale a una protección certificada frente a transitorios o a lógica averiada/apagada mientras la llave siga conectada.

```text
Toma batería + después de llave ── 10 kΩ ──┬── A5 (entrada ADC)
                                         │
                                         ├── 10 kΩ ── GND común
                                         │
                                         └── 100 nF ─ GND común
```

Recalculado SIN diodo: `Vpin = Vbat / 2`; `Vbat ≈ 2 × raw × Vref_real / 1024`. Para batería de 6,0 / 7,4 / 8,0 / 8,4 V corresponden 3,0 / 3,7 / 4,0 / 4,2 V en A5. A 8 V consume 0,400 mA y cada resistencia disipa 1,60 mW; a 8,4 V consume 0,420 mA y disipa 1,764 mW por resistencia. Resistencias de 10 kΩ, 1 %, 1/4 W son candidatas. El escenario 8,4 V no identifica la química instalada. Con tolerancias opuestas de 1 %, a 8,4 V el nodo puede llegar a 4,242 V; mantener referencia AVCC y confirmar margen real antes de montaje definitivo. La alternativa 15k/10k sigue disponible si se necesita más margen, pero esta revisión desarrolla la opción mitad/mitad solicitada.

El capacitor de 100 nF va **entre el mismo nodo A5/unión de las dos resistencias y GND**, en paralelo con la resistencia inferior. A5 es entrada de sensado, no una salida de alimentación. Preferir capacitor cerámico de 100 nF, por ejemplo de tensión nominal 16 V, sin polaridad, cerca de A5 y de masa lógica. No va en serie con A5 ni entre BAT+ y A5.

Su función es filtrar variaciones rápidas/ruido de motores y conmutación y aportar carga local durante el muestreo del ADC. No modifica la relación continua 1:2 ni limita sobretensiones sostenidas. Con R equivalente = 10k || 10k = 5 kΩ, la constante de tiempo nominal es 0,5 ms y el corte aproximado 318 Hz; unos 3 ms después de conectar representan seis constantes de tiempo en este modelo. Puede complementarse con promedio en software para suavizar la indicación; no elimina por sí solo todo ruido ni acredita precisión. Todo sigue en diseño, sin pruebas físicas ni cambios de atmega/esp/web.

Procedencia de esta revisión: aclaración humana de cableado, cálculos propios por Codex y conceptos eléctricos del diseño previo. Archivos adaptados: este informe; ficha `changes/2026-10-29-25-battery-switched-divider.md`; enlaces de README y CHANGELOG. Autorías anteriores conservadas. Las revisiones con diodo y las advertencias basadas en una toma permanentemente alimentada se conservan debajo como antecedentes, no como circuito vigente para la toma conmutada comunicada.

## Antecedente con diodo — AFR2-20261029-24

Pedido posterior de Koba Chajchir: recalcular suponiendo una caída de 0,7 V en un diodo en serie. Se interpreta ubicado ANTES del divisor, en la rama de medida, con ánodo hacia BAT+ y cátodo (banda) hacia la resistencia superior. No se coloca en la alimentación general del robot; ese caso requeriría otro dimensionamiento.

```text
BAT+ ── ánodo D cátodo ── R superior ──┬── A5
                                      ├── 100 nF ── GND
                                      └── R inferior ── GND
BAT− ──────────────────────────────────────────────── GND Mega
```

Modelo aproximado pedido, con el diodo conduciendo y ADC sin saturar:

`Vpin = (Vbat − 0,7 V) × Rinferior / (Rsuperior + Rinferior)`

`Vbat ≈ raw × Vref_real / 1024 × (Rsuperior + Rinferior) / Rinferior + 0,7 V`

| Vbat | Vpin con 10k/10k | Vpin con 15k/10k |
| --- | --- | --- |
| 6,0 V | 2,65 V | 2,12 V |
| 7,4 V | 3,35 V | 2,68 V |
| 8,0 V | 3,65 V | 2,92 V |
| 8,4 V | 3,85 V | 3,08 V |

Cálculos propios nominales: a 8,4 V, corriente de la rama 0,385 mA con 10k/10k o 0,308 mA con 15k/10k; potencia por resistencia 1,48 mW en el primer caso y 1,42/0,95 mW en el segundo. Con resistencias 1 % opuestas y caída fijada artificialmente en 0,7 V, Vpin máximo a 8,4 V sería aproximadamente 3,889 V o 3,117 V. Sigue siendo preferible dimensionar el margen eléctrico sin confiar en una caída mínima de 0,7 V: usar también el escenario sin caída de la sección original.

**Qué bloquea el diodo:** corriente desde la rama hacia BAT+ y, con conexión correcta en esta rama, conducción directa ante polaridad invertida. No bloquea BAT+ → divisor → pin → protección interna → VCC cuando el Mega está apagado: ese recorrido va en el sentido permitido del diodo. Invertir el diodo bloquearía también la medición normal. La desconexión/aislamiento con lógica apagada sigue pendiente. Fuente del mecanismo general: [Microchip, Power through I/O](https://onlinedocs.microchip.com/oxy/GUID-D6FD9F43-8DC4-476E-A55E-AA21FA1CC865-en-US-4/GUID-E76F3635-A8C2-4E47-B408-07867944877A.html); no transfiere límites eléctricos de otros dispositivos al Mega.

**Caída real:** 0,7 V se usa como hipótesis solicitada, no constante garantizada. Depende del diodo, corriente y temperatura; consultar, por ejemplo, las curvas de [Vishay 1N4148](https://www.vishay.com/docs/81857/1n4148.pdf). No se selecciona aquí una pieza definitiva. Si Vf real fuera 0,5 V y software sumara 0,7 V, el voltaje de batería se sobreestimaría en aproximadamente 0,2 V. Calibrar el conjunto; no aplicar la suma a datos ausentes o al diodo sin conducción. El diodo introduce un error adicional frente al divisor solo.

Actualización solo documental por Codex; estado atmega/esp/web sin cambios, sin implementación, pruebas, compilación ni flasheo. Las secciones siguientes conservan los cálculos originales SIN diodo; esta sección contiene la variante solicitada CON diodo. Firmware, Web y submenú siguen propuestos.

## Circuito candidato original, sin diodo

Sí es posible medir la batería mediante divisor y ADC del Mega. A5/PF5/ADC5 aparece libre en hardware/PINOUT.md y en las fuentes propias consultadas. Es candidato, no reserva ni cableado confirmado. A0–A4 son línea y A8–A15 son Hall. A5 también es JTAG/TMS: el producto ya deshabilita JTAG por software en Afr2Sensors_Init; conservar esa condición.

```text
BAT+ medible (*) ── R superior ──┬── A5 / ADC5 Mega
                               │
                               ├── C 100 nF ── GND
                               │
                          R inferior
                               │
BAT− / GND común ───────────────┴── GND Mega
```

(*) Elegir el punto de batería conmutado y resolver las secuencias de alimentación antes de montar. BAT+ es la tensión que se quiere medir; no una salida de motor ni el 5 V regulado. No asumir que VT del voltímetro del kit ya está dividido o aislado. El esquema representa un filtro, no una protección completa ante sobretensión o inversión.

| Opción, resistencias 1 %, 1/4 W | 8 V batería | 8,4 V batería | Resistencia equivalente | Consumo a 8,4 V |
| --- | --- | --- | --- | --- |
| R superior 10 kΩ / inferior 10 kΩ | 4,00 V | 4,20 V | 5 kΩ | 0,42 mA |
| R superior 15 kΩ / inferior 10 kΩ | 3,20 V | 3,36 V | 6 kΩ | 0,336 mA |

Cálculos nominales propios. 8,4 V es un escenario de diseño si fuera un pack Li-ion convencional 2S de 4,2 V/celda; no identifica la batería instalada. La primera opción satisface la idea mitad/mitad. Se prefiere provisionalmente 15 kΩ/10 kΩ por margen adicional. Con tolerancias opuestas de 1 %, a 8,4 V serían hasta aproximadamente 4,242 V o 3,400 V respectivamente. Esto no cubre picos desconocidos de motores/cargador.

Colocar resistencias y C cerca de la entrada; retorno a masa lógica sin compartir un tramo de alta corriente de motores. El filtro tiene constante de tiempo nominal 0,5/0,6 ms; esperar al menos unos 3 ms después de habilitarlo antes de adquirir, además del asentamiento del ADC. El divisor consume mientras esté conectado.

## Límites y referencia

La hoja de datos del ATmega2560 especifica ADC de 10 bits, rango de entrada de GND a VREF y fuente recomendada de aproximadamente 10 kΩ o menos. El límite absoluto del pin es −0,5 a VCC+0,5 V y no es un rango normal de trabajo. Ambas opciones tienen impedancia nominal inferior a 10 kΩ. Fuente: Microchip, capítulos 26 y 31, [DS40002211A](https://ww1.microchip.com/downloads/en/devicedoc/atmega640-1280-1281-2560-2561-datasheet-ds40002211a.pdf).

El producto selecciona AVCC mediante REFS0. Mantenerla: las referencias internas de 1,1/2,56 V no admiten estas tensiones divididas dentro de su rango de conversión. No conectar el divisor a AREF. Correspondencia de pines: [Mega oficial](https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf).

Para el cálculo ideal, fuera de saturación:

`Vbat ≈ raw × Vref_real / 1024 × (Rsuperior + Rinferior) / Rinferior`

Con 5,000 V de referencia, el paso nominal referido a batería es 9,77 mV para mitad/mitad y 12,21 mV para 15k/10k. Son resoluciones, no precisión garantizada. Calibrar referencia y relación con multímetro cuando se autoricen mediciones; asumir 5,000 V introduce el error de la alimentación. Otra posibilidad futura es estimar AVCC leyendo la referencia interna calibrada, dentro del mismo gestor ADC. Mostrar inicialmente dos decimales y distinguir calibración pendiente.

**Batería presente y Mega apagado:** el divisor puede inyectar corriente por las protecciones del pin y alimentar parcialmente la placa. Un interruptor general compartido no demuestra que ambas tensiones suban y bajen juntas. Para una instalación permanente, resolver aislamiento/desconexión de la rama de medida durante apagado y transitorios; si la batería queda conectada con lógica apagada, usar una etapa de conmutación/aislamiento adecuada cuyo diseño queda pendiente. No usar un GPIO directo como interruptor de BAT+. Tampoco tratar diodos a 5 V como solución al apagado: pueden realimentar ese riel. No se da por seguro el montaje solo con estas dos resistencias.

## Integración propuesta, sin contrato wire activo

- ATmega: añadir adquisición cooperativa a `src/platform/afr2_sensors.c`, que ya posee ADMUX/ADCSRA y lectura analógica alternativa de IR. Un único dueño del ADC, sin Arduino ni DMA. Atender batería también con IR digital/deshabilitado; manejar reinicios del ADC al cambiar line_kind, asentamiento/cambio de canal y descarte inicial cuando corresponda. Entrada sin pull-up, buffer digital de ADC5 deshabilitado sin tocar los buffers digitales de IR. Muestrear, por ejemplo, 10 veces/s y publicar un promedio corto a 2 Hz; frecuencias candidatas, no rendimiento probado.
- Datos: proponer milivoltios enteros, raw, referencia usada, estado de calibración, secuencia/instante y validez. No medir corriente ni deducir autonomía. Configuración inicialmente deshabilitada hasta confirmar instalación; una entrada flotante no demuestra batería conectada. Cero voltios no permite distinguir batería ausente, cable cortado o tensión realmente nula. Saturación debe indicar fuera de rango.
- Enlace: ATmega produce la medición; ESP transporta por ROBO2/WS; Web consume el mismo dato. Definir después versión, capability, IDs/offsets, longitudes, endian, unidades y compatibilidad conjunta. No anexar bytes a mensajes actuales sin negociación. Esta investigación no asigna comandos, ScreenCodes ni nuevas versiones.
- OLED: proponer `Sistema → Batería`, lectura de voltios, estado/edad, calibración y opción `Volver`. Largo SW abre menú principal; corto confirma. Implementar con la skill local y su copia Web en la misma entrega.
- Web: mostrar voltaje medido, estado y antigüedad; sin dato recibido, mostrar indisponibilidad. Como criterio candidato, pasar a stale tras 2 s sin muestra; requiere implementar el mismo significado de edad en productor y consumidores. Reinicio/desconexión invalidan el dato. Vista previa OLED identificada, sin telemetría ficticia.
- No añadir porcentaje, umbrales de batería baja, corte de motores o BMS hasta identificar química/celdas y requisitos. El voltaje de pack bajo carga no acredita carga restante ni balance entre celdas.

## Estado y pendientes

ATmega/ESP/Web: sin cambios de fuente, imágenes ni despliegue por esta investigación. No se modifica el pinout activo, el árbol implementado, los contratos binarios ni CAD. Sin tests, compilaciones, validadores, COM, flasheos ni pruebas físicas. No se afirma memoria suficiente ni compatibilidad física.

Antes de cerrar componentes/cableado: identificar batería y máximo de carga; confirmar accesibilidad de A5 en el montaje con shields, referencia mínima real y esquema de encendido/apagado; resolver protección frente a transitorios, inversión y lógica apagada. Después implementar juntos productor, enlace y consumidores; mediciones y pruebas solo mediante pedido expreso.

## Procedencia por archivo

- `hardware/MEDICION_BATERIA.md`: documento nuevo propio, cálculos nominales y propuesta; fuentes técnicas enlazadas arriba, lectura de pinout y gestor ADC existentes.
- `changes/2026-10-29-23-battery-adc-research.md`: registro nuevo propio del alcance y estados.
- `README.md` y `CHANGELOG.md`: enlace a investigación; conservación del contenido y autorías previos.
