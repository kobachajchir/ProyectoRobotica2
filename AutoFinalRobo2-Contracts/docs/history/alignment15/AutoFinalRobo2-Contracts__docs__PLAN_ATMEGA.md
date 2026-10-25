> **Fuente vigente AFR2-20261024-11:** encoder CLK/DT en D2/D3 por INT4/INT5, SW D42; cola y acciones en main. Hall PCINT2 A8–A15 con preferencia de captura por software, macro0. **Esta adaptación no está compilada ni probada**; build08 corresponde a la fuente anterior. ESP/Web conservan su estado10. [Ficha11](../changes/2026-10-24-11-encoder-hall-interrupts.md). Los estados previos inferiores son históricos.

> **Continuidad AFR2-20261024-08:** [estado por módulo](ESTADO_IMPLEMENTACION.md) y [EX01v5](../contracts/EX01_NETWORK_UI_5.md). Firmware0.7.0,48nodos,11avisos, IP/AP local, iconos/scroll y Sistema → Reinicio compilados; Web actualizada/promovida. Clave AP excluida de HTTP/WS/captura protegida; clave STA oculta. SRAM estática4797bytes no acredita pila; memoria/Hall0 se conservan. No pruebas físicas/flash ni nuevo acceso de navegador. CAD/pinout intactos. Estados inferiores históricos.

> **Avance AFR2-20261024-07:** pantalla OLED de secuencias, nombres/iconos PROGMEM, INFO/REBOOT/BOOT y UART drenada antes de reset compilados. [Matriz actual](ESTADO_IMPLEMENTACION.md#atmega) · [Contrato v4](../contracts/EX01_SYSTEM_SEQUENCE_4.md). SRAM estática4295bytes; memoria/Hall0 y calibración pendientes, sin prueba física. No se recorta MENU_PLAN ni se inicia CAD. Los estados previos inferiores son históricos.

> Estado vigente **AFR2-20261024-02**, 24/10/2026: integración ROBO2/perfil2 + EX01v3 compilada en Atmega/ESP/Web; GOTO completo, configuración por catálogo compartido y jobs Wi-Fi OLED conectados. Paquete Web actualizado en ESP/data/dist. Sin pruebas de navegador/hardware ni flasheo; secuencias físicas mantienen bloqueo de memoria. Ver [matriz vigente](../contracts/EX01_ALIGNMENT_3.md) y [evidencia](../changes/2026-10-24-02-project-alignment.md). Los estados previos que siguen son históricos.

# Implementación ATmega: plan, matriz y bitácora

Inicio23/10/2026; cierre de fuente/documentación24/10/2026. Ficha: AFR2-20261023-19.
Este documento distingue código fuente, complementos pendientes y comprobaciones físicas.
No se ejecutan tests, compilaciones, validadores ni flasheos sin pedido expreso.

## Decisiones del usuario

- Este chat implementa ATmega y contratos; no modifica fuente ESP/Web concurrente.
- Prioridad: árbol completo de navegación, Sistema, sensores, Wi-Fi y notificaciones estilo F4.
- Un solo WebSocket Web activo, a implementar en ESP; no sustituir la sesión existente.
- Wi-Fi: elegir teclado OLED o modal Web solicitado desde el auto. En ambos casos confirmar Conectar desde OLED.
- OK termina el teclado; SW largo cancela teclado y vuelve al contexto anterior. Fuera del teclado, largo abre menú.
- Notificaciones con barra temporal, duración predeterminada 3000 ms y restauración del contexto.
- Motores cableados; Hall S1/S2 todavía sin conectar. No interpretar los 11 pulsos como calibración.
- Test Hall manual de una vuelta de rueda, cuentas medidas por rueda, guardar/descartar explícitos.

## Matriz de correspondencia con el maestro

| ID | Requisito | Estado de entrega (fuente, no ejecución) | Complemento / condición |
| --- | --- | --- | --- |
| M01 | Reloj, UART1, TWI/Manager, OLED, encoder | Integrado previamente + main de producto | Sin build/placa |
| M02 | Árbol completo, scroll, selección, edición | afr2_menu_catalog/screen:44 nodos, editores/teclado | Catálogo actualizado; puente/visor Web pendiente |
| M03 | Sistema, versiones, capacidades, diagnóstico | afr2_app: vistas y datos reales/invalid | Versiones ESP/Web por WIFI_UPDATE, no inventadas |
| M04 | Notificaciones F4, cola, progreso, restauración | afr2_screens: cola4,3000ms y cierre | Réplica por framebuffer; sin paridad gráfica total F4 |
| M05 | Wi-Fi local y solicitud modal Web | afr2_app/app_wire: dos rutas, revisión/Conectar | ESP outbox/borradores/modal Web/un solo WS pendientes |
| M06 | Estado, STOP, lease, watchdog | afr2_product/watchdog/app_wire | Validación de TTL, sesión y autoridad escrita |
| M07 | PWM/dirección cuatro motores, pruebas acotadas | afr2_motor/product: Timer1/2, rampa y prueba100..2000ms | Signos físicos por comprobar |
| M08 | Línea5 digital/ADC, normalización/calibración | afr2_sensors/app: cinco canales, mayoría, umbrales | Tipo/polaridad/calibración manual reales |
| M09 | HC-SR04, echo/timeout; servo opcional | afr2_sensors: Timer3/5, ventanas/timeout | Medición temporal física y orientación |
| M10 | Hall y calibración manual por vuelta | afr2_sensors/product/app: PCINT2 y test/guardar | Macro0 y hall_wired0: conectar ocho señales/niveles |
| M11 | Velocidades, PID, mezcla Mecanum | afr2_product: control10ms, Q8 y cuatro acumuladores | CPR, signos, PID y geometría sin calibrar |
| M12 | Odometría y pose | afr2_product/app_wire: marco, secuencia, unidades | Hall/geom válidos; ESP afr2-pose-1 pendiente |
| M13 | Seguimiento de línea | afr2_product: centroide, mezcla y condiciones de parada | Línea/Hall calibrados, potencia/configuración |
| M14 | GOTO preparar/iniciar/cancelar/resultado | afr2_product/app: relativo XY, estados/timeout | Odometría calibrada; precisión física no medida |
| M15 | EEPROM versión/CRC, recuperación, guardado | afr2_preferences: dos registros, commit incremental | Sin credenciales; cortes de energía por comprobar |
| M16 | UI remota y framebuffer coherente | afr2_app_wire: META/PAGE/ACTION/RELEASE | Captura600ms, revisión4s, prioridad local; puente ESP pendiente |
| M17 | Nuevos comandos y servicios remotos | afr2_app_wire + afr2_product_wire.h: EX01 opt-in | Perfil2/base preservado; ESP no negocia EX01 aún |
| M18 | Fuente/recursos/autores/adaptación por archivo | provenance/atmega-product.json y guía de implementación | Referencias/autores preservados; memoria no medida |
| M19 | Compilación/mapas/IRQ/interoperabilidad/banco | No ejecutado | Pedido expreso requerido |

## Orden de entrega

1. Catálogo/nav/teclado/notificaciones, interfaces de servicios y documento complementario.
2. Productores de estado/seguridad/potencia/sensores/preferencias y conexión de las acciones.
3. Hall/calibración/PID/odometría/línea/GOTO con capacidades condicionadas.
4. Extensión ROBO2 negociada, UI remota, solicitudes Wi-Fi y documentación de ESP/Web.
5. Lectura de fuente/diff y cierre documental; comprobaciones ejecutables separadas.

## Registro incremental

- Inicio: leídos maestro/roadmap, reglas de módulos, MENU_PLAN, pinout/recursos,
  skill OLED y referencias F4 menús/OSK/notificaciones. Conservadas ediciones concurrentes.
- Reservado AFR2-20261023-19 para esta implementación. Existen dos fichas históricas
  15 distintas: `15-robo2-atmega-ui` y `15-esp-web-build-fixes`; citar nombre completo
  al referenciarlas. No se renumeran evidencias históricas de otros trabajos.

## Criterio de cierre

Cada fila debe vincular implementación y contrato, indicar estado real y pruebas pendientes.
Fuente escrita no equivale a compilada, ni protocolo compilado a hardware funcionando.
No completar el goal si hay trabajo obligatorio pendiente; mantener bloqueos concretos por hardware/integración.

## Cierre24/10: evidencia y continuación

M01..M18 están cubiertos en fuente/documentación dentro del alcance ATmega.
M19 queda expresamente sin ejecutar por instrucción del usuario. La aceptación
física y el complemento ESP/Web no están completados y no se anuncian operativos.
La aceptación global del proyecto sigue pendiente; esta entrega cierra la fase
de implementación autorizada en fuente/documentación sin pruebas. El goal de
fuente no exige acreditar hardware ni ejecutar M19 sin autorización expresa.

- [Implementación, archivos, defaults y límites](ATMEGA_IMPLEMENTACION_19.md).
- [EX01: comandos, recepción, respuestas y validación en ambos extremos](../contracts/ROBO2_PRODUCT_EXTENSION.md).
- [IDs CONFIG congelados y rangos](../contracts/ATMEGA_CONFIG_ACTIONS.json).
- [Procedencia por archivo y versiones preservadas](../provenance/atmega-product.json).
- [Ficha de entrega19](../changes/2026-10-23-19-atmega-product.md).
- [Guía complementaria guardada en firmware ESP](../../AutoFinalRobo2-ESP/docs/ROBO2_EX01_ATMEGA.md).

Durante la revisión de fuente se separaron la frescura del radio y el timeout de
trabajos, se invalidan acciones tras navegación/notificaciones asíncronas, se
limpia la integral de ruedas con consigna cero y se cancela GOTO al cambiar de
modo. El reemplazo de fuentes se hizo con respaldo y comparación SHA previa;
no se ejecutaron compiladores ni analizadores automáticos.

Las fichas20/21 concurrentes se conservan:20 compila Wi-Fi ESP/Web independiente,
pero no implementa el outbox/modal del auto;21 añade diseño de secuencias perfil3
y cliente Web. Ese perfil3 y su ejecutor no pertenecían a la matriz acordada aquí:
quedan como siguiente integración explícita, con IDs incompatibles con EX01.


