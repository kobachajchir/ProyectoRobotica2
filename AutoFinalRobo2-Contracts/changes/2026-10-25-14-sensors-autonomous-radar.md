> **Actualización de compilación14:** pedido humano posterior autoriza compilar/corregir. ATmega/ESP/Web full: exit0, sin errores ni advertencias finales. [Informe](../documentation/builds/2026-10-25-14/README.md). Sin tests, validadores independientes, navegador, montaje físico ni flasheos. Paquete local full generado; no promovido a ESP/data/dist. El texto de entrega sin compilar inferior es histórico.

# AFR2-20261025-14 — Sensores y radar autónomo

25/10/2026. Pedido humano: listado Sensores, HC-SR04, control angular, IR con gráfica/valores como F4, radar OLED/Three.js, comandos completos, Wi-Fi/AP y animación del servo en todas las escenas excepto Modelo3D.

Decisiones confirmadas: ECHO D31/25µs, barrido por pasos, autonomía al salir/desconectar, ángulo derivado del PWM aplicado, manual visual0.1.9 y modal de Red propio. [Contrato completo](../contracts/SENSORS_RADAR_8.md).

| Módulo | Estado real |
|---|---|
| ATmega | Fuente: servicio de cabeza, PWM por ciclo, radar/IR y comandos60..6A, paradas separadas, 53nodos/64identidades. Sin compilar ni probar |
| ESP | Fuente: negociación EX01v8, traducción/longitudes, consultas, evento de pantalla, wifi.info y HALT de tracción. Sin compilar ni probar |
| Web | Fuente: tres tareas, radar Three.js, controles y estado; animación común excluyendo Modelo3D, biblioteca OLED copiada, pill/modal Red y Configuración. Sin build ni navegador |
| Contracts | Header LE, catálogo, pantallas, navegación, conexiones, identidad/procedencia y esta ficha actualizados |
| CAD | Fuentes y cambios del usuario conservados, sin rediseño ni operación CAD; rig solo en clones Web |

No se cambió hardware/cableado, EEPROM, Documentacion/ ni reference/. No se promocionaron bundles a ESP ni se flasheó. No se ejecutó ninguna suite, compilación, validador ni revisión de navegador. La única generación ejecutada fue la copia de recursos OLED con --copy-only, sin sus comprobaciones de build.

## Qué explica el cambio

Antes: sensores mezclados, consultas manuales, servo sin estado remoto y vistas OLED genéricas. Después: productor común en ATmega, consumidores Web/ESP con unidades/edad, radar autónomo por ángulo y pantallas hermanas identificables. El STOP explícito incluye el servo; la supervisión por pérdida de enlace conserva radar y corta tracción. La animación sigue PWM confirmado y no altera lecturas. Modelo3D conserva pose original.

## Pasada posterior pendiente (no ejecutada)

- Compilar firmware, biblioteca/communication y ESP/Web; revisar SRAM estática, pila e ISR, y paquete LittleFS.
- Revisar catálogo de41comandos, errores/longitudes, mezcla de versiones, pérdida de sesión/boot/cola y rollover.
- Corto/giro/largo, retorno a selección/scroll, salto entre hermanas, código/revisión y descarte de acciones antiguas.
- Comparar framebuffer C/TS con el mismo snapshot/tiempo, iconos, clipping y estados desconocidos, sin sustituir visor conectado.
- Medir TRIG/ECHO/timeout/cadencia en D30/D31 y PWM/cambio de ciclo D13; calibrar extremos/centro y comprobar orientación frontal.
- Verificar asociación ángulo/eco, inversión, cambios de dirección, pérdida de muestras y snapshot/delta.
- Comprobar autonomía al cambiar tarea, ocultar/cerrar Web o reiniciar ESP; STOP completo, fallos y arranque sin reanudación.
- Probar IR digital/ADC10 bits con/sin calibración y Hall sin cablear; ninguna ausencia equivale a cero.
- Navegador escritorio/móvil, ambos temas, foco/teclado/reduced motion, recuperación WebGL y ausencia de movimiento en Modelo3D.
- AP/STA/AP_STA por ambas IP, RSSI del router sin atribuirlo al cliente AP, claves ausentes y señal desconocida.

La inspección física, compilación y revisión visual son evidencias independientes. La publicación Git, cuando se solicite, tampoco las sustituye.

Revisión de barrido independiente: cada inicio cambia scanRevision para descartar mapas anteriores aunque no se haya observado la parada. La vista fija puede superponer la última muestra válida con ángulo/PWM conocido sobre el histórico; los puntos fuera de escala se cuentan. Ningún punto se genera desde la interpolación visual.
