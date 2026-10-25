> AFR2-20261025-01: las reglas anteriores de acceso quedan sustituidas por [control directo EX01v7](DIRECT_CONTROL_7.md). Sin adquisición, renovación, liberación ni habilitación OLED. Los formatos anteriores solo se conservan donde DIRECT_CONTROL_7 mantiene compatibilidad.

# EX01v6 — Bloques por tiempo o recorrido de rueda

AFR2-20261024-12. Firmware0.8.0; ROBO2/perfil2 y afr2-ws-1 conservados.
Extiende [v5](EX01_NETWORK_UI_5.md). Nuevo bit
`AFR2_EXT_F_DISTANCE=0x4000`, máscara0x7fff. No cambia red, reinicios, OLED,
ScreenCodes ni framing. ESP exige versión>=6 y bit anunciado antes de enviar
las nuevas variantes; conserva los mensajes temporales anteriores.

## Preparación y carga

`robot.sequence.begin` admite `executionMode` opcional:

| Valor | Cuerpo BEGIN 0x40 | Semántica |
| --- | --- | --- |
| timed, predeterminado | 13bytes, anterior | Segmentos conjuntos por tiempo |
| independent-wheels | 14bytes, byte13=1 | Cada rueda avanza su lista independientemente |
| barrier-wheels | 14bytes, byte13=2 | Todas las ruedas completan una fase antes de avanzar |

Los bytes0..12 siguen siendo ownerToken4, sequenceToken4, count1 y total4 LE.
`count` es cantidad de registros UART, no de pistas ni bloques visuales.
`totalDurationMs` es el máximo de seguridad en los dos modos nuevos.

APPEND 0x41 conserva los 30bytes temporales de v4. En los modos nuevos usa:

| Offset | Campo | Formato |
| --- | --- | --- |
| 0 | ownerToken | uint32LE |
| 4 | sequenceToken | uint32LE |
| 8 | index | uint8, consecutivo0..31 |
| 9 | wheel | uint8: FL0, FR1, RL2, RR3 |
| 10 | completion | uint8: time0, distance1 |
| 11 | movementKind | uint8; ESP envía wheels6 |
| 12 | limit | uint32LE, ms o mm según completion |
| 16 | timeoutMs | uint32LE |
| 20 | rampMs | uint32LE |
| 24 | fromMmS | int16LE |
| 26 | toMmS | int16LE |

WS identifica esta variante mediante `wheel: fl|fr|rl|rr` e incluye
`completion: time|distance`, `limit`, `timeoutMs`, `rampMs`, `fromMmS`,
`toMmS`. Para tiempo, limit=timeoutMs y múltiplo de10. Distancia:1..10000mm,
al menos una velocidad distinta de cero. Ramp/timeout positivos, múltiplos
de10, ramp<=timeout, velocidades limitadas también por configuración AVR.
No se mezclan formatos de registro dentro de una carga. COMMIT/START/CANCEL,
sus respuestas y la respuesta STATUS de19bytes permanecen iguales.

## Ejecución

AVR conserva listas enlazadas acotadas dentro del mismo buffer640bytes:
union de32 segmentos20bytes o32 instrucciones20bytes. Añade contadores y
estado por rueda; esto requiere volver a aprobar SRAM/pila antes de habilitar.
No asignación dinámica. COMMIT verifica cantidad, sumas de watchdog y fases.
En barrera exige el mismo número de pasos en las cuatro ruedas.

Cada entrada captura el contador Hall y calcula:

`objetivoPulsos = ceil(distanciaMm * cuentasPorVuelta / (pi * diametroMm))`.

Acumula la magnitud de los incrementos Hall, de modo que un cambio de signo
en una rampa suma recorrido. Se trata de recorrido de rueda, no desplazamiento
neto del chasis. Requiere cuentas/vuelta no nulas, geometría, Hall, pose válida,
configuración de motores y ausencia de fallo. La velocidad interpola durante
rampMs y luego conserva el valor final hasta alcanzar el objetivo. Timeout
por rueda/global o incremento Hall incoherente produce fallo y parada.

En modo independiente `index` indica cantidad de instrucciones completadas;
no identifica una rueda activa única. En barrera también cuenta instrucciones,
no bloques de chasis. Terminado usa index255 y estado completed. El wrapper
OLED existente muestra ejecución de ruedas y tiempo máximo, sin nuevo layout.
STOP, pérdida de lease/enlace o reinicio no reanudan la ejecución.

## Compilador Web y límites

Los programas solo temporales mantienen la carga anterior. Cuando existe
distancia, Web crea instrucciones por rueda: timeout=3*duraciónEstimada+1000ms
para distancia; tiempo conserva su duración. Total=mayor suma por rueda+1000
en independiente, o suma de máximos por fase+1000 en barrera. Máximo32
instrucciones y120000ms; el editor no trunca programas que excedan ese límite.

En Chasis, distancia corresponde a la rueda con mayor recorrido estimado;
las otras reciben distancia proporcional o espera si su velocidad es cero.
Esto incluye pivotes. No significa automáticamente metros de avance del centro.
La simulación usa velocidades ideales; Hall/calibración cuantizan el recorrido
físico. Deshabilitar una pista produce consigna cero para esa rueda.

## Estado real

Fuente de productores/consumidores compilada; sin intercambio ni ejecución
verificados. `AFR2_HALL_CONNECTED=0` y `AFR2_SEQUENCE_MEMORY_APPROVED=0`
se conservan. El feature de distancia no se anuncia disponible con esos gates.
La compilación con gates0 puede eliminar por optimización la rama física;
sus cifras no certifican recursos de una futura compilación habilitada.
[Evidencia12](../documentation/builds/2026-10-24-12/README.md).
