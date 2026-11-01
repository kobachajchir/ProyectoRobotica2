# AFR2-20261028-20 — Depuración de encoder y pulsador por USART0

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex, 28/10/2026.

Pedido explícito: agregar depuración del encoder hacia ambos lados y pulsador
al diagnóstico I2C preparado en19. Conserva USART0 D0/RX y D1/TX a115200 8N1,
objetivo `i2c-diagnostic-firmware` y futuro HEX separado del producto.

## Procedencia y adaptaciones por archivo

Código original AFR2, autorías previas conservadas. No se copia reference/.

| Archivo | Adaptación |
| --- | --- |
| Atmega/service/i2c_uart0_diagnostic.c | Inicializa captura IRQ existente D2/CLK, D3/DT y SW D42. Reutiliza decoder de producto y antirrebote20ms/largo800ms. Agrega contadores por sentido A(+)/B(-), presión/liberación, corto/largo, niveles, transiciones inválidas y overflow. Reporta actividad como máximo cada100ms y estado en reposo cada1s. Banner revisión20. |
| Atmega/Makefile | Incluye fuente/header del decoder en el objetivo separado. |
| Atmega/service/README.md, README.md, INFORME_ADAPTACION.md | Documentan ampliación y estado fuente. |
| Contracts/hardware/OLED_I2C_DIAGNOSTICO.md | Pines, formato, contadores por intervalo, saturación, boot-held, resolución candidata, sentido físico sin calibrar y límites por cola/latencia de impresión. |
| Contracts/README.md, CHANGELOG.md | Enlace a esta ficha. |

Atmega/ y Contracts/ representan AutoFinalRobo2-Atmega/ y
AutoFinalRobo2-Contracts/. El encoder original y su cola no se modifican.
No hay nuevos renderers, menús, ScreenCodes o acciones UI: largo solo se
reporta como evento; no ejecuta navegación. Sin cambios de contrato wire.

## Estado y límites

ATmega: solo fuente, lectura manual. Sin compilar, tests, validadores, ejecutar,
abrir COM, cargar ni pruebas físicas. No hay nuevo HEX generado; los tamaños
flash/SRAM/pila y la respuesta física permanecen sin comprobar. La dirección
I2C del producto se conserva. ESP/Web/Loader/CAD sin cambios por esta ficha.

El diagnóstico no lee EEPROM: 4 transiciones por paso y sentido sin invertir.
A/B son direcciones lógicas opuestas, sin atribuir horario/antihorario al
montaje. La cola captura durante impresión; overflow se informa y reinicia el
parcial. SW se muestrea en main, con huecos por salida serie: no certifica
captura de pulsos breves. Corto/largo excluyentes, sin clic al liberar un largo.
No se envían comandos OLED, se guardan preferencias ni se mueven actuadores.

[Guía completa](../hardware/OLED_I2C_DIAGNOSTICO.md#encoder-y-pulsador--revisión20).
