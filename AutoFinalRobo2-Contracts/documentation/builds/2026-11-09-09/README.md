# Comparación ESP 1.10.4 → 1.10.5

Pedido: «Hace todos los puntos y despues compara cuanto bajo».
Ambas fuentes se compilaron con el mismo `ESP/platformio.ini`, entorno
`esp12s_candidate`, espressif8266 4.2.1 y dependencias fijadas. La base es el
checkout al comenzar este pedido, incluidos los cambios sin commit y la
corrección OOM08. No es `HEAD` ni el firmware1.10.2 instalado del log.

## RAM estática y flash

| Medida | Antes | Después | Diferencia |
|---|---:|---:|---:|
| RAM estática (PlatformIO) | 51.172 B (62,5 %) | 46.644 B (56,9 %) | −4.528 B (−8,85 %) |
| `.data` | 1.592 B | 1.592 B | 0 B |
| `.rodata` en RAM | 14.748 B | 10.772 B | −3.976 B |
| `.bss` | 34.832 B | 34.280 B | −552 B |
| Instancia global `broker` (`nm`) | 6.576 B | 5.956 B | −620 B |
| `BrokerRequest`, declaraciones nativas | 404 B | 296 B | −108 B |
| Flash (PlatformIO) | 459.699 B | 462.319 B | +2.620 B |

Los seis pedidos residentes (cola4 + STOP + pendiente) ahorran648 B brutos.
El estado de capacidades y el control HTTP agregan datos: el ahorro neto de
`.bss` es552 B. Ahorro estático total: **4,42 KiB**.

RAM estática = `.data` + `.rodata` + `.bss`, como informa PlatformIO.
`.noinit`=56 B en ambas imágenes. El margen respecto de81920 B pasa de30748 B
a35276 B **antes de heap y pila**; no es una lectura de RAM libre en ejecución.

[Datos, secciones y SHA-256](memory-comparison.json).
Extractos de compilación: [antes](before-build.txt), [después](after-build.txt).
La base mostró una advertencia previa: `draftToken` sin usar.

## JSON con datos iguales

Se compilaron en Windows los métodos reales `status`, `capabilities` y
`draftStatus` de ambas fuentes con ArduinoJson7.4.2. Red/reloj/heap son datos
fijos; `F()` representa el mismo texto. Se comprobaron igualdad del catálogo
completo y del resto del estado con enlace activo y caído. No se usó el heap
del proceso Windows como si fuera memoria del ESP.

| Ejemplo con enlace activo | Antes | Después | Reducción |
|---|---:|---:|---:|
| Respuesta `system.alive` | 3.758 B | 195 B | 94,8 % |
| `system.status` sin cambios de capacidades | 3.679 B | 602 B | 83,6 % |
| Estado con capacidades actualizadas | 3.679 B | 3.679 B | 0 %; catálogo conservado |

Con2 latidos/s y1 estado/s sin cambios, el ejemplo pasa de11195 a992 B/s de
JSON de salida (−91,1 %). Excluye tramas WS/TCP, solicitudes, arranque y otros
servicios. Son ejemplos reproducibles, no una captura real ni bytes de heap.

[Fixture C++](status-fixture.cpp) · [Tramas y tamaños](status-comparison.json).
Compilador: `C:/Qt/Tools/mingw1310_64/bin/g++.exe`, C++17, includes de
`ESP/.pio/libdeps/esp12s_candidate/ArduinoJson/src` y `Contracts/contracts`.
El ejecutable comprueba también copia de credenciales, IDs de64 caracteres,
cuerpo/generación y reinicialización de la unión de pedidos.

## Comprobaciones realizadas

- ESP antes/después: compilación PASS, sin carga.
- Web: `node_modules/.bin/tsc.cmd -b --pretty false`, PASS.
- [Cliente Web real con transporte simulado](client-protocol-check.cjs): PASS;
  saludo completo, latido/estado parciales conservan capacidades; el cambio
  revoca PWM y reconectar descarta los permisos previos.
- [Cola HTTP real con servidor simulado](http-queue-check.py): PASS;
  descarga grande única, archivos pequeños independientes, cancelación,
  timeout, espera por memoria y límite de admisión. Esperar no abre el archivo
  ni crea su respuesta de envío.

Scripts desde la raíz del proyecto, sin puertos. El script HTTP genera sus
stubs/ejecutable en `%TEMP%/afr2-memory-20261109-09` usando MinGW.

Sin AVR/Loader, navegador, flasheos ni pruebas físicas. Heap mínimo,
fragmentación real y ausencia del OOM durante Wi-Fi/HTTP/WS siguen pendientes
en la placa. Los4528 B sí son ahorro estático medido con el compilador ESP.
