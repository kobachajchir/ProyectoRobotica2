# Plan: KEY y RST del ESP automáticos con transistores

Estado: **implementado en fuente, sin compilar ni probar**, ficha [AFR2-20261106-11](../changes/2026-11-06-11-esp-key-rst-automatico.md). Pendiente: armar el circuito, medir, compilar y probar; retiro completo de `mega_reset` en el código del Loader. No se compiló, flasheó ni midió nada para escribirlo. Contexto: [WIFI_SHIELD.md](WIFI_SHIELD.md), [FLASHING.md](FLASHING.md), [PLAN_PRUEBA_FLASHER_MEGA.md](PLAN_PRUEBA_FLASHER_MEGA.md).

## 1. Decisiones tomadas (por el usuario)

1. Se **elimina** el método de carga por D0/D1 con RESET del Mega a GND. Queda solo el **flasher en software** (USART0↔USART1, D18/D19), con RESET del Mega siempre libre. Por eso el ATmega nunca está detenido durante la carga y puede manejar los transistores.
2. KEY y RST se accionan con **un transistor por línea**, en paralelo con el pulsador del shield OSOYOO (se puentea el pulsador; el shield no expone pines).
3. Pines de control: **D32 → KEY** y **D33 → RST** (ATmega2560: D32 = PC5, D33 = PC4). Hay que confirmar que siguen libres antes de implementar (HC-SR04 usa D30/D31 = PC7/PC6; los PC4/PC5 que menciona WIFI_SHIELD.md son del UNO y no aplican).
4. El flasher y el Loader deben ofrecer **modo manual o automático**. Manual es el predeterminado y funciona aunque no estén soldados los transistores.

5. **No interferir con otros usos del shield.** Otros alumnos pueden dejar el circuito desconectado, sacar los cables de control, cargar otro firmware en el Mega o usar el ESP con firmware AT. En todos esos casos el circuito **no debe apretar KEY ni RST ni reiniciar nada**, y el shield debe comportarse como uno sin modificar.

## 1.1 Requisitos de convivencia (obligatorios)

**Hardware**
- El estado por defecto del circuito es **apagado**: las dos resistencias de 10 kΩ (R3, R4) van de la base a GND **del lado del transistor**, no del lado del Mega. Así, aunque se saquen los cables de D32/D33 o el Mega no tenga firmware, la base queda a masa y el transistor no conduce.
- Cables de control (D32, D33) **desmontables** (Dupont con ficha o regleta). Desconectarlos deja el circuito inerte. El GND de los emisores puede quedar conectado sin efecto.
- Con el transistor apagado, el colector en las líneas KEY/RST solo aporta una fuga de nanoamperios y pocos pF: no afecta al pull-up, al arranque ni a la lectura de GPIO0. Mantener los cables cortos para no sumar capacitancia ni antena en RST.
- El pulsador sigue funcionando a mano: el transistor está en paralelo, no lo reemplaza.
- El circuito no toca TX, RX, 3V3, SDA ni SCL. Un ESP con firmware AT, o cualquier otro, no lo ve. KEY solo se aprieta durante una carga automática pedida a propósito.
- Marcar en el PINOUT, y en la serigrafía si hay PCB, que D32/D33 están reservados para esta función: quien quiera usar esos pines para otra cosa **debe desconectar los dos cables de control**.

**Firmware del flasher y del producto**
- Al arrancar, D32/D33 quedan como **entradas sin pull-up** (alta impedancia). No se configuran como salidas hasta que llega una orden automática explícita, y al terminar se devuelven a entrada.
- El arranque del Mega, el reinicio del Mega (DTR al abrir/cerrar el puerto) y el bootloader no deben mover esos pines: **abrir el puerto o reiniciar el Mega no reinicia al ESP**.
- Ninguna rutina de producto ni diagnóstico aprieta KEY o RST por su cuenta, ni al arrancar ni en el watchdog. RST solo se pulsa por pedido del usuario, y KEY solo dentro de una carga automática.
- Tiempo máximo de KEY apretado (a fijar); al vencer o al reiniciarse el Mega, ambos pines vuelven a entrada.

**Loader**
- El modo automático es **opt-in por perfil**, desactivado por defecto, y no se activa sin que el usuario confirme que el circuito está armado.
- Antes de cualquier orden de KEY/RST, el Loader identifica el flasher AFR2 con una consulta inocua. Si no responde (otro firmware, ninguno, o puerto equivocado), **no envía órdenes de control**, avisa y ofrece el modo manual. Así no se mandan bytes de control a un programa que los tomaría como datos.
- En modo manual no se envía nada de este protocolo y no se tocan los pines.

## 2. Controles previos para el usuario (hacer antes de soldar)

Todo con el shield **sin alimentar** salvo el control 4. Tester en continuidad/ohmios. Referencia de masa: el GND de la cabecera del shield o del Mega.

| # | Control | Cómo | Resultado esperado | Si da otra cosa |
| --- | --- | --- | --- | --- |
| 1 | Pulsador KEY suelto | Una punta en GND, la otra en cada pata/pad del pulsador KEY, de a una | Sin continuidad en las patas de señal | Si una pata ya conduce a GND suelto, es la pata de masa; anotarla |
| 2 | Pulsador KEY apretado | Igual, manteniendo KEY apretado | Una pata del pulsador pasa a **~0 Ω a GND**. Esa es la pata de señal (GPIO0) | Si ninguna baja a GND, o baja a unos kΩ, avisar: puede haber resistencia en serie |
| 3 | Pulsador RST apretado | Igual que 2 con RST | Una pata baja a **~0 Ω a GND** (reset) | Ídem |
| 4 | Reposo alimentado | Con el shield alimentado, tensión de las patas de señal contra GND, sin apretar | Cercana a **3,3 V** en KEY y en RST (pull-up) | ~0 V: la línea está forzada a masa; cerca de 5 V: no tocar y avisar |
| 5 | Las dos masas | Continuidad entre la pata de masa de KEY, la de RST y el GND del Mega | Las tres unidas | Si no, usar siempre el GND del Mega para los emisores |
| 6 | Marcar el transistor | Leer el marcado del encapsulado y confirmar el orden de patas con la hoja de datos | Ver §3 | No soldar sin confirmarlo |

**Criterio de decisión**

- Controles 2, 3 y 4 cumplidos: el pulsador pone la señal a masa, hay pull-up a 3,3 V → **sirve NPN** (o MOSFET N), como se diseña abajo.
- La señal se activa llevándola a 3,3 V (activo alto): NPN **no sirve**; habría que reevaluar con PNP o con otra etapa. Parar y avisar.
- Hay resistencia en serie de más de unos 100 Ω al apretar: igual funciona, pero medir el nivel bajo real con el transistor puesto (Vce(sat) más la caída) y avisar.

## 3. Transistor y cableado (para armar)

Componentes por canal (dos canales idénticos): **1 NPN de pequeña señal**, **1 resistencia 1 kΩ** (base serie), **1 resistencia 10 kΩ** (base a masa). Total: 2 transistores, 2×1 kΩ, 2×10 kΩ, cables cortos. Se sugiere 2N3904 o BC547; **no dan el mismo orden de patas**. Con el lado plano hacia vos y las patas hacia abajo:

- **2N3904:** de izquierda a derecha **E – B – C**.
- **BC547:** de izquierda a derecha **C – B – E**.
- Verificar siempre con la hoja de datos del fabricante y el marcado del propio componente (otros NPN, como el 2N2222, cambian según versión).

| Pata | Canal KEY (Q1) | Canal RST (Q2) |
| --- | --- | --- |
| **Colector (C)** | pata de **señal** del pulsador KEY (GPIO0) | pata de **señal** del pulsador RST (reset) |
| **Emisor (E)** | **GND** (pata de masa del pulsador KEY o GND común) | **GND** (pata de masa del pulsador RST o GND común) |
| **Base (B)** | un extremo de R1 (1 kΩ) y un extremo de R3 (10 kΩ) | un extremo de R2 (1 kΩ) y un extremo de R4 (10 kΩ) |

**Redes (nets)**

| Red | Une |
| --- | --- |
| `KEY_CTRL` | Mega **D32** – R1 (1 kΩ) |
| `Q1_B` | R1 – base de Q1 – R3 (10 kΩ) |
| `ESP_KEY` | colector de Q1 – pata de señal del pulsador KEY (no se desuelda el pulsador) |
| `RST_CTRL` | Mega **D33** – R2 (1 kΩ) |
| `Q2_B` | R2 – base de Q2 – R4 (10 kΩ) |
| `ESP_RST` | colector de Q2 – pata de señal del pulsador RST |
| `GND` | emisor de Q1, emisor de Q2, extremo libre de R3 y de R4, GND del Mega, GND del shield |

Reglas de montaje:

- Cables a las patas de señal de **10 cm o menos**; soldar al lado del pulsador, sin quitarlo, y con poco estaño para no puentear patas vecinas.
- No conectar nada a 5 V ni a 3,3 V: los transistores solo hacen de interruptor a masa. Los pull-ups ya existen en el módulo (control 4).
- El pin de control en alto = botón apretado; en bajo o en alta impedancia = suelto. Los 10 kΩ garantizan "suelto" durante el arranque del Mega.
- D32 y D33 salen de la cabecera doble de pines digitales 22-53 del Mega, que el shield no cubre. Identificar por la serigrafía de la placa y por la hoja de datos, no por la posición en una foto.
- Verificar con el tester, sin alimentar, que no hay continuidad entre `ESP_KEY`, `ESP_RST` y 5 V.

## 4. Plan de implementación

**Firmware ATmega**
1. Reservar D32 (PC5) y D33 (PC4) en `board_pinmap.h`; salidas en bajo al iniciar, siempre.
2. Flasher (`service/esp_uart_bridge.c`): modo manual (no toca pines) y modo automático (órdenes: entrar en modo carga, reinicio normal, soltar KEY, estado).
3. Secuencia de entrada en carga: KEY alto; 20 ms; RST alto 100 ms; RST bajo; ~200 ms con KEY apretado; el flasher queda en puente transparente hasta la orden de soltar KEY.
4. Seguridad: tiempo máximo con KEY apretado (a fijar), y soltar ambos al reiniciarse el Mega.
5. Firmware de producto: opcionalmente solo reinicio normal de RST (KEY nunca se aprieta fuera de la carga).

**Decisiones abiertas que el diseño debe resolver**
- Cómo viaja la orden por el mismo USB que lleva los datos al ESP (modo de comandos antes del puente transparente, o secuencia de escape con tiempos de guarda), sin corromper la carga.
- Cómo se evita que abrir el puerto reinicie el Mega por DTR justo después de la orden, y cómo se suelta KEY al terminar (comando explícito, o cierre de puerto con reinicio).

**Loader**
1. Opción por perfil: KEY/RST *Manual* (predeterminado) o *Automático*; guardada fuera del repositorio con el resto de la configuración.
2. En automático, no mostrar el aviso físico y enviar la orden antes de esptool (`--before no_reset`); en manual conservar el asistente actual.
3. Registrar en el log el modo usado y si KEY quedó soltado.

**Eliminación del método D0/D1**
Quitar del Loader, de las ayudas y de la documentación vigente el método "RESET Mega a GND, D0/D1". Alcance estimado: unos 77 archivos con menciones; **no tocar** `docs/history`, evidencias y diagnósticos ya registrados. Archivos principales: FLASHING.md, CARGA_USB_COM16.md, PINOUT.md, WIFI_SHIELD.md, MEGA_ESP_PUENTE_SOFTWARE.md, PLAN_PRUEBA_FLASHER_MEGA.md, `Profile.cs` y `Operations.*` del Loader, `helpContent.ts` de la Web, `Makefile` y `service/README.md` del Atmega.

**Documentación y CAD**
- Registrar D32/D33 en el PINOUT y su generador; agregar este circuito a WIFI_SHIELD.md.
- Si se hace una PCB, reservar esta etapa (dos NPN y cuatro resistencias) y cuatro puntos de soldadura al shield.

## 5. Criterios de aceptación (a medir por el usuario)

1. Con el tester: D32 o D33 en alto lleva `ESP_KEY` o `ESP_RST` a menos de ~0,3 V; en bajo, vuelven a ~3,3 V.
2. Con los pines en alta impedancia o el Mega en reset, ningún pulsador queda apretado.
3. Carga completa con esptool en modo automático, con hash verificado, sin tocar los pulsadores.
4. El modo manual sigue funcionando igual que hoy.
5. Tras la carga, KEY queda suelto y el ESP arranca el programa.
6. **Sin cables de control:** con D32/D33 desconectados, `ESP_KEY` y `ESP_RST` siguen en ~3,3 V y el ESP no se reinicia al encender, al apagar ni al abrir el puerto del Mega.
7. **Otro firmware en el Mega** (vacío, solo bootloader, o uno ajeno que no use D32/D33): mismo resultado que el punto 6.
8. **ESP con firmware AT** y el circuito instalado: responde a `AT` igual que sin el circuito, sin reinicios espontáneos.
9. **Circuito completamente retirado:** el shield se comporta como uno de fábrica, con los pulsadores a mano.
10. El Loader no envía órdenes de control a un puerto que no responde la identificación del flasher AFR2.

Estos criterios no están acreditados: hoy no hay mediciones ni pruebas.
