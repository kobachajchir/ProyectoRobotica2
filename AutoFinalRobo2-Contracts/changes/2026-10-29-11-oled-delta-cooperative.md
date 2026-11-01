# AFR2-20261029-11 — OLED diferencial, cooperativo y de baja prioridad

> **Carga completada y verificada, 29/10/2026 11:18:15:** usuario confirmó el
> asistente del Loader; job `98d7039a98724d26b2fd5e9c422c3544` completed,
> COM8, firma0x1e9801 y74200bytes de flash verificados, exit0. SHA256 final
> `dd91f00004a0e1d20acbf1760bafe902b946130ce1c60f06f5609231f4ab1d68`.
> La recompilación desde Loader cambió el hash respecto del build externo
> inicial03302a17...; no se cargó el HEX inicial. [Evidencia y registros](../documentation/builds/2026-10-29-11/README.md).
> Sin ensayo físico posterior de UI/motores ni certificación dinámica de pila.
> Estados anteriores de espera/sin compilación inferiores son históricos.

> **Build posterior autorizado, 29/10/2026:** producto compilado con
> `make -B firmware`, exit0 sin errores/advertencias, y después desde Loader
> `build_mega` (producto/comunicación/biblioteca), job
> `da141088d6df49acb19205ceb16c050d` completed. Flash74200bytes
> (text73106+data1094), SRAM estática6885bytes(data1094+bss5791), margen1307bytes.
> `.su`: main257bytes, mayor función individual además de main150bytes;
> no constituyen un límite de cadena completa/pila/ISR ni prueba dinámica.
> HEX SHA256 `03302a17fc89387d6858a92154f6df7c61ba55da52ece7b68223734c925c9a29`.
> Carga pedida por usuario enCOM8: primer job rechazado sin escritura por falta
> de preparación de sesión; se compiló desde Loader y se solicitó carga nueva
> `98d7039a98724d26b2fd5e9c422c3544`. Al registrar este párrafo está esperando
> el asistente físico «Preparar y cargar ATmega», writingStarted=false.
> Las menciones de fuente sin compilar inferiores describen la entrega previa.

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex, 29/10/2026.

Pedido: revisar el driver original AutoMicro portado, transmitir solo páginas
modificadas y conservar/reanudar el envío cediendo a otras tareas del micro.

## Referencia revisada

Se leyó `AutoFinalRobo2-Atmega/reference/f4/Core/Src/ssd1306.c`: cola por página,
pending_refresh durante transferencia, estados de comandos/datos, selección
por prioridad y liberación del bus. En esta copia UpdateScreen encola todas
las páginas y SendForState envía página desde columna0; no se encontró allí
el recorte exacto por primera/última columna recordado por el usuario.
No se atribuye a esa referencia una capacidad no localizada. La adaptación
AVR implementa la comparación diferencial pedida y conserva el principio
asíncrono por estados, con avr-libc/TWI IRQ en lugar de HAL/DMA.
No se modificó reference/ ni se accedió a registros de D:/AutoProyecto.

## Adaptaciones por archivo

| Archivo | Cambio |
| --- | --- |
| Atmega/include/afr2_oled.h | Imagen confirmada1024bytes, validez por tramo16columnas, cursores rotativos de página/tramo, TX33bytes en lugar de129 y estado pausa/reanudación. API Afr2Oled_Pause. |
| Atmega/src/display/afr2_oled.c | Compara bytes finales con lo confirmado por ACK; detecta primera/última columna diferente dentro de cada tramo16. Posiciona página/columna explícitamente y envía control+1..16bytes. Un solo intento de envío y comparación de hasta una página por llamada. Prioridad rotativa evita monopolio de la parte superior. |
| Atmega/src/main.c | Elimina refresco completo periódico de10. Llama al servicio OLED al final y pausa nuevas transmisiones si quedan bordes de encoder por consumir o una respuesta UART pendiente. |
| Atmega/service/i2c_uart0_diagnostic.c | Elimina marcado completo periódico del diagnóstico que ya no corresponde al modelo diferencial. Sigue usando el mismo driver y contador; sin cambio de dibujo. |
| Web/src/features/oled/firmwareRenderer.sources.json | Fingerprint actualizado tras revisar que todas las primitivas/layout/glifos y framebuffer lógico permanecen iguales. SHA256 del texto normalizado LF como el generador Web, no bytes CRLF del checkout. |
| Atmega/README.md, INFORME_ADAPTACION.md y docs/COMPOSICION.md | Estado fuente y presupuesto pendiente. |

Atmega/Web son AutoFinalRobo2-Atmega/AutoFinalRobo2-Web. Código nuevo original
AFR2, autorías previas preservadas. No cambian ScreenCodes, pantallas, recursos
de dibujo, navegación ni wire. No hay algoritmos visuales nuevos que portar a
Web; solo se revisa el transporte físico, inexistente en el rasterizador Web.

## Semántica de actualización

- dirty marca candidatos, no obliga a escribir: borrar/redibujar lo mismo no
  envía bytes. Páginas iguales quedan estáticas físicamente.
- Comparación exacta, sin hashes/colisiones. Tras init/error, contenido físico
  desconocido: se invalida la referencia y se sincroniza completo por tramos.
- El TX snapshot no cambia en vuelo. Solo después de ACK de datos y STOP se
  copian esos bytes a la imagen confirmada. Un redibujo durante TX se vuelve
  a comparar y no se pierde. Fallo no confirma parcialmente lo transmitido.
- Comando y datos son etapas separadas. Callback en main solo registra avance;
  no encadena envío. El primer Task posterior cede y fija espera1ms antes
  de la siguiente etapa, sin espera activa; servicio al final de main. TWI IRQ mueve
  un byte por entrada. No se introduce prioridad configurable de IRQ AVR.
- Pausa deja terminar la transferencia en vuelo; no corta arbitrariamente un
  byte I2C. Al reanudar conserva página/columna/longitud/snapshot y fase.
- Un bloque de datos máximo tiene16bytes+control+dirección:162clocks, alrededor
  de1.62ms a100kHz sin stretching, sobrecarga o retardos de ISR. El CPU no espera
  ese tiempo. El límite de timeout Manager se conserva en40ms.
- El primer pintado completo tiene más overhead por fraccionar; la mejora
  principal está en actualizaciones parciales y disponibilidad del CPU, no en
  acelerar un cuadro entero ni prometer coherencia visual atómica por frame.

## Memoria y estado real

ATmega: implementado y revisado por lectura, **sin nueva compilación, tests,
validadores, COM, flasheo o prueba física** de esta revisión. El HEX de10 NO
incluye11. No se debe cargarlo esperando actualización diferencial.

La imagen confirmada requiere1024bytes; TX se reduce96bytes y se agregan
metadatos. Incremento estimado954bytes por handle en AVR y se quita reloj4bytes
del main. Frente a5931bytes estáticos del build10, margen de pila/ISR se reduce
significativamente; no se acredita SRAM suficiente por escribir la fuente.
Nueva compilación y revisión de memoria/pila quedan pendientes de autorización.
No se duplica framebuffer en stack, no hay malloc ni caché probabilística.

ESP/Loader/CAD sin cambios; Web solo fingerprint, sin build/promoción ESP.
No se afirma paridad física por revisión ni se habilitan funciones de movimiento.
Confirmación del usuario sobre diagnóstico09 no acredita el driver11 nuevo.
