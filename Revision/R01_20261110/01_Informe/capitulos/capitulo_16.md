# 16. Electrónica, alimentación y responsabilidades

| Subsistema | Referencia disponible | Consideración del informe |
| --- | --- | --- |
| Control de plataforma | ATmega2560; fuentes AVR C / avr-libc | Sensores y motores según pinout y contratos. |
| Conectividad | ESP-12S/ESP8266 en el shield | Firmware propio; no se describe como un módulo AT. |
| Interfaz propia | OLED I2C D20/D21; encoder D2/D3 y pulsador D42 | Alimentación y niveles deben corresponder al módulo real; 5 V fue informado para el montaje. |
| Sensado de batería | A5; divisor 10 kΩ / 10 kΩ y capacitor 100 nF | Se debe caracterizar referencia ADC y tensión máxima admisible. |
| Ultrasonido y radar | D30/D31; servo de barrido D13 | Distancia y consigna angular son magnitudes distintas. |
| ESP↔ATmega | TX1 D18 / RX1 D19 | Respetar niveles eléctricos y catálogo de comandos. |
| Mantenimiento automático | KEY D32 y RESET D33 mediante NPN y resistencia 1 kΩ | Integración física de servicio pendiente según documentación. |
| Brazo | STS3250 a 12 V; STS3032 a 6 V; bus TTL serie | Diseño de potencia y bus debe corresponder a servos STS, no a PCA9685. |

```text
V_bat = ADC/(2^n − 1) · V_ref · (R_superior + R_inferior)/R_inferior
```

Con resistencias iguales, el factor de escala nominal es 2. Debe asegurarse que la entrada nunca supere el límite del microcontrolador. El capacitor atenúa ruido, pero no protege de sobretensión. La tensión medida no equivale directamente a porcentaje de carga sin una curva de batería y condiciones de corriente definidas.

El presupuesto eléctrico debe sumar consumo de control, motores y servos según simultaneidad, picos y caída de tensión. Se necesita decidir distribución de 12 V/6 V, masa común, protecciones, conectores, conductores y parada de potencia. Una BOM mecánica y la preselección de servos no resuelven ese diseño eléctrico.
