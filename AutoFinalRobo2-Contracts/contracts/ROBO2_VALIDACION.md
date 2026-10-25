> AFR2-20261025-01: las reglas anteriores de acceso quedan sustituidas por [control directo EX01v7](DIRECT_CONTROL_7.md). Sin adquisición, renovación, liberación ni habilitación OLED. Los formatos anteriores solo se conservan donde DIRECT_CONTROL_7 mantiene compatibilidad.

# ROBO2: criterios comunes de recepción y validación

AFR2-20261023-17: revisión de fuente y build ESP exitosos; [evidencia de compilación](../documentation/builds/2026-10-23-17-robo2-esp/README.md). Los casos de esta matriz siguen pendientes de ejecución y no se presentan como aprobados por compilar.

AFR2-20261023-15. Especificación y casos esperados, **no pruebas ejecutadas**.
Norma: [perfil2](UART_PROFILE_2.md). Implementación única del framing:
[afr2_wire.h](afr2_wire.h). Endpoint Mega y receptor ESP la incluyen sin copiar.

## Mismo framing, distintos roles

| Nivel | Ambos extremos | Mega al recibir | ESP al recibir |
| --- | --- | --- | --- |
| Cabecera | ASCII ROBO2,5 bytes exactos | Igual | Igual |
| Longitud | N<=64, total12+N | Petición exacta13hello/8resto | Respuesta OK23/13/16/22/31/11; error9 |
| Tokens/versión | 3A,A5,01 exactos, cubiertos por XOR | Igual | Igual |
| Ruta | Nibble alto emisor, bajo receptor | Emisor3/receptor1 | Emisor1/receptor3 |
| Integridad | XOR de cada byte previo al checksum | Igual | Igual |
| Prefijo | Sesión/transacción u32LE no cero | Sesión negociada, transacción creciente | Coinciden con sesión/transacción en vuelo |
| Comando | Validar catálogo, dirección y forma | Ejecutar solo handler permitido | Mismo CMD pedido; no ACK genérico |
| Semántica | No completar con datos fuera de contrato | Hello AFR2/perfil2; resto sin cuerpo | Producto/perfil/result/rangos/caps/coherencia |
| Efecto | Solo después de pasar controles | Actualiza supervisión y ejecuta | Actualiza enlace y entrega al broker |

Las rutas no son máscaras permisivas. Un paquete con receptor correcto y
emisor incorrecto también se descarta. El parser común conoce 31/13, el endpoint
decide cuál acepta según rol. No hay mensajes iniciados por Mega en este
catálogo; no tratar una respuesta inesperada como evento ni nuevo pedido.

## Casos para una verificación futura expresamente solicitada

| Entrada/condición | Resultado esperado común o por rol |
| --- | --- |
| Frame válido dividido en cualquier byte o concatenado | No entregar hasta completar; recibir una sola vez cada frame |
| Ruido, UNER antiguo, minúsculas, ROBO sin2 | Descartar; una siguiente R puede iniciar nueva cabecera |
| Cabecera parcial seguida de otra R | Conservar esa R para resincronizar |
| Token A o B distinto, versión distinta, XOR incorrecto | Sin dispatch/heartbeat ni acción; contador del motivo |
| Builder sin capacidad12+N, puntero nulo con datos, N>64 o ruta ajena | Devuelve0 antes de escribir; no publicar TX parcial |
| N=65..255 con ROBO2 dentro de payload | Drenar declarado sin escribir fuera de64bytes ni ejecutar cabeceras internas |
| N=0 o1..7 con XOR correcto | Framing completo, ningún handler de catálogo ejecutable; sin respuesta correlacionada en Mega |
| Ruta32,11,33,F1,1F o0 | Descartar; nunca broadcast implícito |
| Eco de31 en ESP o13 en Mega | Parser puede conocer ruta; endpoint descarta destinatario/emisor incorrectos |
| Gap>50 ms a mitad de trama | Reiniciar; no combinar prefijo viejo y sufijo nuevo |
| Sesión0 o transacción0 | Mega BAD_REQUEST si prefijo completo; ESP nunca acepta como respuesta en vuelo |
| Hello con AFR2/perfil/longitud incorrectos | Mega BAD_REQUEST; ESP descarta respuesta incompatible, no ready |
| Pedido sin hello o sesión distinta | Mega SESSION_REQUIRED, no ejecutar ni renovar supervisión |
| Transacción repetida/menor de sesión actual | Mega BAD_REQUEST, no repetir acción ni renovar heartbeat |
| Respuesta de sesión anterior, CMD distinto, tx distinta o sin pedido | ESP descarta, no resuelve promesa ni refresca enlace |
| Respuesta correcta ya fuera del plazo800 ms al procesar RX | ESP descarta y vence pendiente; no aceptar un éxito tardío de esa iteración |
| Error result1..4 con N9 | ESP correlaciona error; no interpreta cuerpo de éxito |
| Result>4, error con cuerpo u OK de longitud incorrecta | ESP descarta; pedido sigue hasta respuesta válida o timeout |
| Flag2, mode fuera de0..5/255, stopped fuera0/1/255, lease distinto0 | ESP descarta cuerpo semánticamente inválido |
| Cap reservada/hello sinUART, lineBits altos, valid sincap/edad, encoders valid sinwired | ESP descarta respuesta incoherente |
| Estado conocido sin capability STATE | ESP descarta; sin productor solo se admite mode/stopped/age unknown |
| Sensor inválido con padding0 | null en Web; nunca muestra una lectura inventada |
| Sensor válido con edad>2000 ms | Web valid=false, conserva edad conocida; sin valor vigente |
| STOP sin callback o capability | Mega unavailable; ESP/Web jamás confirman stopped/completed |
| STOP OK con stopped distinto1 o mode distinto0 | ESP descarta; ningún completed por simple ACK |
| RX overflow/error | Revocar sesión, descartar entrada incierta, callback STOP si existe; ESP MEGA_LINK_DOWN para pendiente |
| No respuesta800 ms / no petición válida>1500 ms | ESP timeout/retira caps; Mega revoca y solicita parada si hay hook |
| STOP durante TX parcial o lectura en vuelo | Terminar frame iniciado, enviar STOP sin mezclar; lectura SUPERSEDED |
| Contador de transacciones agotado | ESP cambia sesión/no reutiliza, negocia antes de otro pedido |
| Rollover de uptime u32 | Usar restas modulares; no comparar fechas absolutas |

Los contadores de framing son diagnósticos locales u16 saturados; no viajan
por la UART ni son un registro de ensayos. Un frame estructuralmente bueno
contado en `frames` puede ser descartado después por rol/correlación/semántica.
La fuente no implementa seguridad criptográfica ni cola de sesiones históricas.

## Vector de diseño para hello (no ejecutado)

Petición ESP→Mega, sesión1, transacción1, perfil2:

```text
52 4F 42 4F 32 0D 3A A5 01 31 01
01 00 00 00 01 00 00 00 41 46 52 32 02 E4
```

N=13, total25; último E4 es XOR diseñado de todos los anteriores. El receptor
debe negociar únicamente tras completar y validar, no al reconocer ROBO2.
Cambiar ruta/tokens/payload sin recalcular checksum falla primero integridad;
para aislar otra validación en un futuro caso, conservar XOR coherente.
