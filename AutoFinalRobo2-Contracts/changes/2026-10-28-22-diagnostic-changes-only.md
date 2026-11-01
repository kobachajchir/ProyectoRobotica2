# AFR2-20261028-22 — Depuración solo ante cambios, barrido cada10s

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex, 28/10/2026.
Pedido explícito: imprimir solo si hay cambios y volver a escanear cada10s.

## Adaptaciones y procedencia por archivo

- `AutoFinalRobo2-Atmega/service/i2c_uart0_diagnostic.c`: elimina heartbeat
  del encoder. Conserva eventos agregados hasta10reportes/s, contadores
  independientes por sentido y pulsador. El barrido inicial establece una
  referencia por dirección: imprime ACK/errores y resumen, omite NACK normales.
  Después compara resultado/estado TWI/niveles/recuperación por dirección;
  imprime cambios, incluyendo pérdida de ACK (`LOST`) y recuperación. Omite
  encabezados/resúmenes sin cambios; cambios de tx_dropped habilitan resumen.
  Arrays de referencia:336bytes de datos estáticos añadidos por definición;
  no es medición de SRAM total ni certificación de pila. Programa barrido
  cada10000ms entre inicios, sin solapamiento si un barrido demora más. R
  conserva adelanto manual del siguiente, sin forzar reporte de datos iguales.
  Banner actualizado a22. Código nuevo original AFR2; autorías conservadas.
- `AutoFinalRobo2-Atmega/tools/flash-i2c-encoder-diagnostic.ps1`: banner esperado22;
  conserva compilación forzada antes de cargar el HEX separado.
- `AutoFinalRobo2-Contracts/hardware/OLED_I2C_DIAGNOSTICO.md`: comportamiento
  vigente, silencio en reposo, referencia inicial y cadencia entre inicios.
- README/CHANGELOG Contracts: enlace a esta ficha.

## Estado real

ATmega: solo cambios fuente y lectura manual; sin tests, compilaciones,
validadores, ejecución, COM o flasheos. No hay HEX actualizado generado por el
agente. El usuario debe ejecutar el script preparado para incorporar el cambio.
No se modifican pinout, dirección del producto, decoder, pantallas o navegación.
ESP/Web/Loader/CAD sin cambios por esta ficha. Ninguna prueba física acreditada.
