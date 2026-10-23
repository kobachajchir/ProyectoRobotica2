# AMI-20261019-01 — Sentido base del encoder fisico invertido

- Fecha: 2026-10-19 (America/Argentina/Buenos_Aires)
- Repositorio de origen: FirmwareF4
- Referencia de codigo: commit local c544378, rama work/nueva-etapa
- Estado del origen: implementado en fuente; sin compilacion ni carga a placa
- Area: flujo de entrada local / navegacion de pantallas

## Contrato anterior y nuevo

Antes, ENC_Task_N10ms usaba el delta del contador TIM4 corregido por wrap y luego aplicaba invertDirection. Ahora invierte ese delta como orientacion base del montaje fisico, antes de aplicar la misma preferencia. Con el mismo valor guardado, cada sentido fisico produce el desplazamiento o ajuste contrario al anterior.

No se cambia encoderInverted, su valor predeterminado false, su persistencia ni su reporte. La opcion Invertir giro sigue invirtiendo respecto de la nueva base. La correccion alcanza las entradas fisicas del encoder, incluidos menus, ajustes y teclado OSK; no se limita a un renderer.

Los comandos remotos 0x57 TRIGGER_ENCODER_ROTATE_LEFT y 0x58 TRIGGER_ENCODER_ROTATE_RIGHT siguen despachando UE_ROTATE_CCW y UE_ROTATE_CW directamente desde UNER, sin pasar por el muestreo fisico. No cambian IDs, screen codes, rutas, payloads, respuestas, eventos ni permisos.

## Flujo completo

1. El encoder fisico cambia el contador TIM4. ENC_Task_N10ms lee el contador y corrige el wrap.
2. Se invierte el delta como orientacion base; despues se aplica invertDirection sin modificar preferencias.
3. Los pasos con signo y la direccion se derivan del delta corregido. Encoder_MainTask entrega el evento al UiEventRouter existente.
4. El handler activo ejecuta la navegacion o el ajuste inverso al anterior para el mismo giro fisico y preferencia. Se conservan los pulsadores y las guardas existentes.
5. La entrada local no tiene transporte, correlacion, ACK, respuesta final ni timeout propios. Las acciones remotas conservan sus guardas, autenticacion, respuestas y errores existentes; no se agrega una secuencia remota.

## Impacto en repositorios

| Repositorio | Estado | Accion o motivo concreto |
| --- | --- | --- |
| FirmwareF4 | origen | Invierte el delta fisico antes de la preferencia en Core/Src/encoder.c. |
| Firmware ESP01 | no_aplica | No procesa el contador fisico; 0x57/0x58 y sus reenvios conservan el contrato. |
| Web ESP01 | no_aplica | El cambio solo afecta la lectura fisica F4; la entrada remota conserva su significado en el dispatcher F4. |
| AutoMicroQtDesktop | no_aplica | El cambio solo afecta la lectura fisica F4; la entrada remota conserva su significado en el dispatcher F4. |

## Compatibilidad y orden de integracion

Para activar la correccion hay que compilar y cargar FirmwareF4. No requiere recompilar ESP01/Web/Qt ni migrar o restaurar preferencias. La publicacion de esta ficha no publica el commit de F4 ni instala firmware. No se afirma interoperabilidad probada de los consumidores.

## Evidencia y limites

Inspeccion y edicion de fuente: Core/Src/encoder.c, Encoder_MainTask en app_tasks.c y despacho remoto en uner_app.c. Se consultaron el .ioc, main.h, main.c y las skills centrales. Se preservan los cambios preexistentes ajenos al encoder.

No se ejecutaron tests, compilaciones, validaciones, flasheo ni prueba fisica por indicacion del usuario. El cambio de sentido instalado queda pendiente de compilar y cargar F4.
