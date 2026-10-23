# Recursos y tiempos

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
