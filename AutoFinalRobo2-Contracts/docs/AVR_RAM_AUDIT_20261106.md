> Implementación posterior [AFR2-20261106-04](../changes/2026-11-06-04-avr-ram-qr.md): SRAM5614, margen2578, finish QR29; ambos buffers conservados. El análisis inferior corresponde al build anterior y sus estimaciones.

# Auditoría de RAM AVR y opciones de optimización — 06/11/2026

Pedido: localizar consumo de RAM y estudiar estrategias de microcontroladores. **Análisis estático de fuentes, mapa y .su existentes; sin modificar firmware, compilar, validar, cargar ni realizar pruebas físicas nuevas.** El build auditado es el de AFR2-20261106-02. [Datos extraídos](AVR_RAM_AUDIT_20261106.json), con SHA-256 del mapa, y [evidencia anterior](../documentation/builds/2026-11-06-02/README.md).

## Distribución medida

SRAM: data 1650 + bss 5977 + noinit 9 = **7636/8192 bytes**; margen antes de pila **556**. Flash: **123586 bytes**. Podemos intercambiar algo de flash y tiempo de lectura por RAM; el problema principal actual es SRAM.

| Bloque | Bytes SRAM estáticos | Contenido |
| --- | ---: | --- |
| OLED | 2140 | framebuffer 1024 + copia confirmada 1024 + control/transferencia 92 |
| Product | 1721 | secuencias 763, Wi-Fi 647, configuración 63, sensores 56, resto 192 |
| .data | 1650 | 1616 bytes de literales de texto, 32 de tablas QR, active_slot 1, relleno 1 |
| Cabezal/radar | 722 | cola de 16 muestras 256, última muestra 16, 37 bins × 11 = 407, control 43 |
| UI | 463 | incluye cuatro avisos 156, edición de credenciales 97 e historial 24 |
| Enlace ROBO2 | 199 | parser, respuesta, sesión y callbacks |
| Aplicación | 143 | incluye copia de configuración guardada 63 y borradores Hall |
| UART RX+TX | 256 | dos colas de 128, sin contar índices |
| Imagen journal EEPROM | 128 | registro completo preparado para escritura cooperativa |
| Otros + noinit | 214 | encoder, I2C, sensores, índices, estado de arranque, etc. |
| **Total** | **7636** | Los componentes internos de Product/OLED no se vuelven a sumar |

Tamaños globales del mapa; desgloses de estructuras derivados de sus campos AVR de 1/2/4 bytes, coherentes con el total. Los símbolos .rodata de strings pueden compartir dirección por combinación de literales: no sumar sus tamaños sin descontar solapamientos. Los 1616 son el espacio único restante de .data después de tablas/variable/relleno, no promesa de recuperarlo todo.

## 1. Textos y formatos en flash — primera prioridad

El mapa muestra, entre otros, reason de afr2_app.c (~424 bytes de sección), sus textos de tarea/snapshot (~201), app_wire (~293), network_screen (~280) y screens (~275). Son secciones atribuidas, con posibles literales compartidos. Hay textos constantes y formatos que no necesitan vivir en RAM.

- Añadir interfaces explícitas para strings flash: notificación, motivo de indisponibilidad y líneas de red con lectura PROGMEM; conservar las rutas RAM para SSID/IP/resultados variables.
- Convertir formatos constantes a snprintf_P/PSTR; migrar llamadas de mensajes a un NotifyFlash que copie directamente desde flash a la cola ya existente, sin buffer temporal adicional.
- La función reason puede devolver texto flash si todos sus consumidores se adaptan; no pasar un puntero PROGMEM a strlen/strncpy/renderer RAM ordinarios.
- No basta poner const. En este AVR clásico, .rodata termina en .data; PROGMEM exige acceso adecuado. La implementación actual ya lo hace bien para fuente, iconos y gran parte del catálogo: conservarlo.
- Estimación de trabajo: **1000–1400 bytes estáticos recuperables**, por confirmar con build. Techo de textos encontrados: 1616, no ahorro garantizado. Prioridad baja en riesgo respecto a alterar colas de control, pero exige revisar cada consumidor y comparar raster.
- Mantener los recursos de lectura pgm_read_* near dentro de la ventana válida de 64 KiB. El ATmega2560 tiene flash mayor: revisar el mapa PROGMEM tras mover tablas; un puntero near de 16 bits no puede direccionar arbitrariamente toda la flash.

Base técnica: [AVR-LibC: Data in Program Space](https://avrdudes.github.io/avr-libc/avr-libc-user-manual/pgmspace.html), [Microchip AVR42787](https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ApplicationNotes/ApplicationNotes/Atmel-42787-AVR-Software-User-Guide_ApplicationNote_AVR42787.pdf). Usar APIs disponibles en la toolchain AVR GCC 7.3 instalada; no adoptar funciones de documentación nueva sin comprobar la versión local.

## 2. Secuencias deshabilitadas que igualmente reservan RAM

AFR2_SEQUENCE_MEMORY_APPROVED vale 0 por defecto y el Makefile/build registrado no lo habilitan. Afr2Sequence_Available devuelve falso, pero Afr2Product conserva toda la estructura.

Afr2Sequence: unión de 32 × 20 = **640 bytes**, kinds **16**, metadatos/estado **107**: total **763**. La unión entre pasos normales y ruedas ya está optimizada; no hay dos buffers de 640.

Opción recomendada: almacenamiento y código de carga/ejecución condicionados al perfil compilado. Cuando la función está deshabilitada, mantener el estado/respuesta de indisponibilidad necesarios y no reservar la unión ni kinds: **656 bytes de ahorro bruto candidato**, con más posible al retirar trabajo por rueda. Revisar todos los accesos e inicialización: eliminar solo el array del header no basta. Cuando se habilite la función, su RAM debe volver a presupuestarse. Esta opción conserva las funciones actualmente habilitadas; no habilita secuencias por sí sola.

Si se necesita mantener secuencias de 32 pasos habilitadas en el mismo firmware, mantener el buffer y usar primero las otras optimizaciones. Bajar a 16 pasos ahorraría 328 bytes entre registros/kinds, pero cambia capacidad pública y no se propone como cambio silencioso.

## 3. Pila QR — corregir el pico, además de liberar estáticos

finish usa uint8_t matrix[353]. Su frame .su ocupa **380 bytes**. main reserva **305**, Afr2Ui_Render **155 dynamic,bounded**, Afr2Qr_Render **37**: estimación de la cadena **877**, antes de interrupciones y subllamadas. No es medición física ni suma de todos los máximos del programa.

Evitar la copia local de 353 y usar un área de trabajo ya reservada. Candidato: zona de acknowledged del OLED, mediante una API de préstamo acotado durante el raster cooperativo. Antes de usarla hay que invalidar los chunks afectados y marcarlos dirty; mantener tx/command estables si hay I2C pendiente. El callback I2C se ejecuta cooperativamente y escribe la imagen confirmada, por lo que el préstamo debe quedar dentro de una fase sin cesión ni callback reentrante; verificar esas condiciones por código. Tras devolverla, el OLED puede necesitar resincronizar esos chunks. **No compartir ciegamente** la imagen confirmada ni tx con un escritor ISR.

Ahorro candidato: ~353 bytes del pico de pila, con algún costo de reenvío. No es ahorro de SRAM estática y no se suma como tal a los 656 anteriores. Cambiar matrix a static por sí solo traslada 353 bytes de pila a bss y no reduce el consumo total concurrente.

También conviene separar el despacho QR del renderer general para evitar mantener locales de otras vistas. Separar la construcción/render de UiSnapshot del frame persistente de main puede permitir reutilizar esa pila durante protocolo/control. No convierte el snapshot en gratis: durante el raster sigue vivo. Requiere comprobar .su y ensamblado; añadir helpers/inline sin medir puede empeorar el resultado.

## 4. OLED: gran oportunidad, pero segunda etapa

La copia acknowledged de 1024 permite comparar exactamente lo que el panel confirmó frente a los nuevos píxeles, incluso si hay un redraw durante TX. El tx separado mantiene la transferencia estable. El driver además respeta I2C Manager, recuperación y envío cooperativo.

Eliminar acknowledged recuperaría **hasta 1024 bytes brutos**, pero exige rediseñar el seguimiento de dirty por píxel/chunk, cambios durante transmisión, confirmación de generación, errores y redibujos que borran/reponen contenido idéntico. Puede aumentar tráfico, latencia y parpadeo. Un checksum pequeño introduce colisiones y puede omitir un cambio real; no equivale a comparación exacta. No lo haría primero: flash para strings y secuencias deshabilitadas aportan margen sin sustituir esta garantía.

Render por páginas/tiles reduciría framebuffer, pero obliga a reevaluar toda UI, scroll, QR y captura remota consistente. No es una sustitución transparente del buffer de 1024 actual. Mantener envío cooperativo y servicios prioritarios siempre.

## 5. Optimizaciones secundarias

| Opción | Ahorro candidato | Condición / costo |
| --- | ---: | --- |
| Journal EEPROM: conservar 79 bytes útiles, producir padding cero al guardar y CRC en streaming al leer | 49 estáticos brutos | Respetar registros de 128, CRC sobre bytes originales de versiones anteriores, commit último y escritura de un byte por tick; sin cambiar EEPROM por ahorrar RAM |
| Radar: cola de 16 a 8 muestras | 128 estáticos | Reduce retención; puede perder muestras ante consultas lentas. Cambia capacidad acordada, requiere aceptación de pérdida/latencia |
| Radar: compactar id/ms de 37 bins | Depende de formato | Hoy 407 bytes necesarios para datos/edad/identidad; diferencias de 16 bits exigen manejar wrap y bin viejo explícitamente, sin fingir frescura |
| IPv4 guardada como 4 bytes en vez de texto de 16, AP y STA | 24 estáticos brutos | Formatear únicamente al presentar; preservar snapshots/contratos y no sumar temporales persistentes equivalentes |
| Wi-Fi/teclado: reutilizar borradores bajo propiedad explícita | Hasta un buffer de 97, antes de metadatos | El estado anterior y edición pueden coexistir; confirmar/cancelar/avisos/reinicio deben preservar semántica. No hay unión segura demostrada aún |
| Retirar umbrales IR obsoletos del objeto RAM | 20 estáticos entre config y saved_config | Mantener decoder/encoder EEPROM antiguo y respuesta wire, revisar copias; no borrar otras calibraciones |

La página de ocho SSID ya ocupa 264 bytes y cumple el plan. No reducir silenciosamente a cuatro ni truncar nombres para aparentar ahorro. Tampoco recortar RX/TX UART, la cola de encoder o los avisos como primer recurso: son parte del manejo de ráfagas y entrada.

No encontré asignaciones malloc/calloc/realloc/free en src del producto: no hay evidencia de fragmentación por heap que explique este consumo. Meter malloc no crea RAM. Mover todo a EEPROM añade latencia/desgaste; no sustituye la memoria de trabajo del control.

Float de 32 bits e int32_t ocupan ambos cuatro bytes: convertir odometría a punto fijo puede cambiar CPU/flash y precisión, pero no promete liberar esa RAM. Las estructuras AVR existentes ya suman sus campos sin relleno significativo: packed no es una solución general. No achicar timestamps/contadores críticos sin demostrar rango y manejo de wrap.

## Orden propuesto y criterio de cierre

1. Migrar textos/formatos estáticos a PROGMEM y medir ahorro individual.
2. Retirar almacenamiento de secuencias del perfil donde ya están deshabilitadas, conservando metadatos/API necesarios.
3. Eliminar la matriz QR en pila con préstamo de scratch correctamente invalidado; revisar frame persistente de main y renderer.
4. Solo si todavía hace falta, optimizar journal y almacenamiento de red. Dejar rediseño OLED y reducción de historia radar para un cambio de mayor alcance.

Proyección, **no resultado medido**: recuperar 1000–1400 de strings y 656 de pasos deshabilitados bajaría estáticos de 7636 a **5980–5580**, dejando **2212–2612 bytes antes de pila**. La mejora QR reduce por separado su pico. Las cifras pueden variar por nuevos metadatos, buffers temporales y combinación de literales.

Objetivo inicial propuesto: alrededor de 2 KiB libres antes de pila, más análisis de las cadenas de llamadas e ISR y una reserva sobre el pico real. Un porcentaje libre no certifica seguridad. Repetir build/size/map/.su y comparar C/Web tras cada grupo, cuando se autorice ejecución; luego medir marca de agua de pila con QR, red, guardado, sensores y UART concurrentes en una prueba física autorizada. No cambiar salidas motor ni autorización de secuencias para medir memoria.

Ya se usan -Os, secciones separadas y gc-sections. LTO puede evaluarse aparte, pero no elimina por sí mismo buffers vivos; puede cambiar inlining/pila. mcall-prologues/mrelax apuntan principalmente a tamaño de código y no garantizan RAM. Evitar mint8 o opciones que supongan ausencia de interrupciones. [Opciones AVR de GCC 7.3](https://gcc.gnu.org/onlinedocs/gcc-7.3.0/gcc/AVR-Options.html).

El bloqueo de despliegue de AFR2-20261106-02 se mantiene: este informe presenta opciones, no una corrección ya compilada o probada.
