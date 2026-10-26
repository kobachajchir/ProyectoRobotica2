> **Fuente14/EX01v8:** Timer3 ECHO D31/25µs, periodo configurable70..1000ms y timeout32ms; Timer5 D13/50Hz con pulso pendiente/aplicado. Radar37bins + cola16 y snapshots añadidos, sin memoria/ISR/pila medida ni nueva compilación. Timer4 permanece libre; no se recablea. [Contrato8](../contracts/SENSORS_RADAR_8.md). El presupuesto histórico inferior no acredita la fuente14.

> **AFR2-20261024-11:** IRQ INT4/5 en D2/D3 y FIFO32bytes (31útiles), consumo acotado32entradas/vuelta en main; SW D42 muestreado. Hall PCINT2 A8–A15 x1, cuatro comprobaciones desenrolladas y captura pendiente antes de publicar UI. No se modifica prioridad fija ni se anidan ISR. [Contrato](INTERRUPCIONES_ENCODER_HALL.md). Fuente sin compilar, mapa/pila/ciclos actuales sin medir; build08 no acredita este cambio. Gates Hall/memoria0 conservados.

> Vigente **AFR2-20261024-04**: **modelo final incluido en ESP**, paquete **1.450.201 bytes gzip** (49,96% menos que los 2,90 MB iniciales). WebP, JS/CSS minificados y dependencias/componentes optimizados. Layout candidato: flash 4 MiB / LittleFS nominal 3 MiB, solicitado por el usuario; flash fisica sin identificar. Web full/esp y firmware ESP compilados; activos promovidos a data/dist. Sin navegador, imagen FS ni flasheo. Atmega y comandos conservan el estado de02. [Evidencia](../changes/2026-10-24-04-final-model-littlefs.md). Los estados previos inferiores son historicos.

# Recursos y tiempos

AFR2-20261024-02: firmware de producto AVR compilado; existe mapa y reporte de
secciones/pila en documentation/builds/2026-10-24-02. Las estimaciones históricas
inferiores no sustituyen esa evidencia. Quedan sin medir tiempos, IRQ y hardware;
secuencias conservan AFR2_SEQUENCE_MEMORY_APPROVED=0 por bloqueo de aprobación.

Fuente19: Timer0 reloj1ms; Timer1/2 PWM; Timer3 CTC25us solo durante eco;
Timer5 compare software para servo D13; Timer4 libre. Hall propuesto PCINT2
A8..A15 con macro deshabilitada. Candidatos sin medición IRQ/SRAM/pila.
[Asignaciones, defaults y límites](../docs/ATMEGA_IMPLEMENTACION_19.md).
El presupuesto anterior es diseño histórico, no mapa de enlace de esta entrega.


El [ATmega2560 de Microchip](https://www.microchip.com/en-us/product/ATmega2560) dispone de 256 KiB flash, 8 KiB SRAM, ADC 10 bits, cuatro USART y TWI. Mega usa reloj nominal de 16 MHz; comprobar placa/fuses. Las asignaciones y cifras siguientes son diseño, no mediciones.

| Recurso | Propuesta | Restricción |
| --- | --- | --- |
| Timer1 OC1A/OC1B | PWM D11/D12 | No usarlo para servo |
| Timer2 OC2A/OC2B | PWM D10/D9 | No usarlo para tick |
| Timer3 | Tick o muestreo ECHO, por cerrar | D5 es dirección motor: no habilitar salida OC |
| Timer4 | Tiempo fino, por cerrar | D6/D7/D8 son dirección: no habilitar OC |
| Timer5 | Reserva servo/captura | Resolver conexión específica |

D13 puede temporizarse con compare software sin apropiarse del PWM Timer1. ECHO D31/PC6 no tiene ICP ni INT/PCINT en esa señal: mantener cableado requiere muestreo temporal con resolución acotada, p.ej. IRQ breve; main sin límite de latencia no garantiza captura. Alternativa: recablear a ICP disponible y documentar.

## Presupuesto SRAM tentativo

Framebuffer 1024 B; snapshot de página 129 B más estado; UART RX/TX 256+256 B; dos slots UNER de 128 B más parser; estado control/UI/sensores objetivo 1500 B; reservar al menos 2000 B para pila/ISR/margen. No es mapa de enlace medido. No malloc en flujo de control.

Fuentes/iconos/textos/tablas en PROGMEM con accesores AVR; const no basta. Si recursos cruzan 64 KiB, revisar colocación y accesores far. Payload 128 B sería cambio de contrato: calibración IR anterior tiene 186 B, por lo que requiere recorte de catálogo o fragmentación. El framing original admite hasta 255 y no se cambia en silencio.

## Ritmos iniciales

Encoder UI 1 ms, debounce SW 20 ms, largo 800 ms; control 10 ms; línea 5-10 ms; OLED dirty pages con objetivo 5-10 Hz; telemetría 5-10 Hz. HC-SR04 intervalo/timeout pendientes de ficha del módulo instalado. Son metas ajustables. uint32 con resta modular y lectura multibyte atómica breve; nada de dibujo, JSON o floats en ISR.

1024 bytes I2C a 100 kHz consumen como mínimo unos 92 ms (9 bits/byte), más comandos; a 400 kHz unos 23 ms. Liberar CPU entre transferencias y comprobar pull-ups/cableado antes de subir velocidad.

115200 8N1 equivale a unos 11520 B/s; RX 256 B cubre unos 22 ms. A 16 MHz, U2X=1/UBRR=16 da aproximadamente 117647 baud (+2,12 %): evaluar margen o negociar 57600. Es cálculo, no ensayo de robustez.

Hipótesis Hall H1 del anexo: 11 ciclos/vuelta de rotor, 75:1 y x4 → 3300 cuentas/vuelta salida. A 150 rpm: 8250 eventos/s por rueda, 33000 en cuatro; unos 485 ciclos CPU entre eventos a 16 MHz. Evaluar x1/x2, PCINT o contador externo antes de comprometer x4. PPR/reducción/variante instalada no confirmados.

Baud propio de aplicación separado del AT de fábrica y de upload: [shield](WIFI_SHIELD.md). La ficha ESP-12S indica nominalmente 4 MiB; no es un presupuesto de Web/LittleFS aprobado hasta identificar flash real y reparto. El `esp12e` candidato no realiza esa identificación.


## Asignación fuente actual — AFR2-20261023-15

Timer0 CTC1ms (16MHz/prescaler64/OCR249) para composición UART/UI; no habilita
OC0A/B. UART1 RX/TX128B cada ring,127 útiles; ROBO2 payload64/frame76 y parser88B
por campos. TWI100kHz/TWBR72 y prescaler1 por IRQ; OLED único cliente del Manager.
Framebuffer1024B + TX129B + comandos4B. Estructuras/rings en torno1,7KiB más
constantes residentes/pila/runtime; NO mapa de enlace. Mantener reserva de2KiB
para pila/margen y medir CPU/IRQ/render/poll al autorizar pruebas. Contradice los
buffers tentativos anteriores solo para estas composiciones; no asigna Hall,
HC-SR04 ni PWM de producto. VCC/pull-ups/controlador siguen pendientes.
