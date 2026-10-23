# AMI-20261005-07 — controles de motores y balance plegable

- Fecha: 2026-10-05 (America/Buenos_Aires).
- Repositorio de origen: Web ESP01.
- Referencia de código: cambios locales del checkout Web, conservando trabajo previo.
- Estado del origen: implementado en fuentes; sin compilación ni revisión visual.
- Área: presentación y flujo de acciones existentes WebSocket/UNER de motores.

## Contrato anterior y nuevo

La Web mostraba Activar balance, Desactivar balance, Detener marcha y Parar
motores y balance como cuatro acciones separadas, con un formulario siempre
visible. Ahora hay un botón que alterna activar/desactivar y un único Detener,
integrados con Consultar estado en la barra inferior de Control de motores.
Detener elige `disableBalance:true`: elimina la marcha, apaga motores y desarma
el balance. También detiene la simulación local de esta página. Para reanudar
compensación se debe activar balance de nuevo; no se mantiene una acción visible
independiente de detener marcha conservando el armado.

Los ajustes se agrupan en Balance, cerrado al entrar, con expansión/contracción
local mediante `aria-expanded`, `aria-controls` y contenido `hidden`. Inclinación
y giro tienen slider y número, sin enviar al arrastrar. Mover y Probar siguen
siendo pulsos explícitos de 750 ms. TEST queda anidado y cerrado inicialmente.

Diagnóstico agrega Balance activado con fondo verde o Balance desactivado con
fondo rojo, según `motionArmed` confirmado por F4. `balanceActive` se conserva
para describir compensación efectiva; armado no acredita estabilidad mecánica.
Sin estado o conexión real, se muestra Balance sin confirmar con tono neutral.
No hay cambio optimista al enviar o al recibir `requestAccepted`.

IDs, nombres, campos, unidades, endian y longitudes permanecen como AMI-20261003-01:

| Acción | UNER / JSON | Request |
| --- | --- | --- |
| Consultar estado | `0x7B` / `stm.motion.get` | Vacío / `{}` |
| Alternar balance | `0x7C` / `stm.balance.setEnabled` | `enabled:u8` 0/1 / `{enabled:boolean}` |
| Mover | `0x7D` / `stm.motion.set` | `forward_percent:f32 LE, turn_percent:f32 LE, duration_ms:u16 LE`, 10 bytes; JSON `{forwardPercent:angle/6*100,turnPercent,durationMs:750}` |
| Detener | `0x7E` / `stm.motion.stop` | `disable_balance:u8=1` / `{disableBalance:true}` |
| Prueba TEST | `0x7F` / `stm.motor.test` | Dos `f32 LE` y `duration_ms:u16 LE`, 10 bytes; JSON `{leftPercent,rightPercent,durationMs:750}` |

Todos responden con su mismo ID y ocho bytes: `[status,armed,balance_active,
command_active,safety_stop,reject_reason,car_mode,motor_test_active]`. Se conservan
los campos JSON `status`, `motionArmed`, `balanceActive`, `commandActive`,
`safetyStop`, `rejectReason`, `carMode`, `motorTestActive`. No se agrega evento,
screen code, comando, persistencia ni parámetro remoto nuevo. Los ajustes siguen
siendo inclinación ±6 grados y giro/esfuerzos ±100 %. La telemetría usa el snapshot
vigente `0x64`, de 92 bytes, sin cambiar su formato.

## Flujo completo

1. Un único controlador Web en Control de motores comparte lectura entre ajustes,
   acciones e indicador. El panel ya usado por calibración Balance reutiliza la
   misma presentación y acciones con su propio controlador.
2. Web solicita a ESP por API JSON v1 con `requestId`; ESP encamina a F4 por UART
   UNER y correlaciona la respuesta con el cliente/nodo solicitante.
3. `requestAccepted` mantiene su significado de encolado. Solo la respuesta final
   válida de F4 confirma el cambio de estado mostrado.
4. F4 conserva permisos, modos, armado, guardas MPU y parada. Activar requiere
   estado disponible y no modo TEST. Mover conserva armado, ausencia de parada y
   modo IDLE/GOTO; probar exige TEST y desarmado. Detener se permite con conexión
   real aun si la consulta inicial no confirmó estado, como la acción anterior.
5. Se conservan timeout Web de 2000 ms, lectura periódica aproximadamente cada
   segundo, consultas de telemetría y rechazo de `status` distinto de cero. Las
   operaciones no se reintentan automáticamente. Requiere sesión STM; ESP/F4
   siguen rechazando órdenes sin autorización o fuera de condiciones válidas.
6. Expandir/cerrar y ajustar sliders solo modifica UI. Terminar Mover devuelve
   objetivo a 0 grados conservando balance; Detener explícito lo desarma. La
   ejecución de secuencias físicas mantiene el bloqueo existente, independiente
   de poder enviar pulsos reales.

## Impacto en repositorios

| Repositorio | Estado | Acción o motivo concreto |
| --- | --- | --- |
| FirmwareF4 | compatible | El handler vigente `UNER_App_HandleMotion` ya interpreta stop=1 como Stop y Disarm y publica armado en payload[1]. No se modifica código. |
| Firmware ESP01 | compatible | `cmd_stm_motion` ya convierte `enabled`/`disableBalance` booleanos en el byte existente, y el registry conserva la respuesta de ocho bytes. No se modifica código. |
| Web ESP01 | origen | Hook compartido, botón alternante, Detener único, ajustes plegables, iconos, lectura de estado y tokens de identidad visual. |
| AutoMicroQtDesktop | no_aplica | Cambio de composición exclusivo de Web. No cambia el contrato consumido por Qt ni se modifica su interfaz. |

## Compatibilidad y orden de integración

No cambia el wire ni requiere firmware nuevo respecto de AMI-20261003-01. El
bundle Web debe reconstruirse y cargarse cuando el usuario lo solicite. Firmware
anterior a los comandos de motores sigue sin habilitar movimiento por una
aceptación optimista. Publicar esta ficha no despliega la Web ni acredita
sincronización de binarios o clones.

## Evidencia y límites

Lectura de los componentes Web, del handler F4, del dispatch/registry ESP y de las
fichas vigentes. Implementación y documentación en fuente; sin tests, compilación,
validadores, inspección visual en ejecución, tráfico real ni prueba física por
instrucción del usuario. No se regenera ni copia data ESP. Los demás cambios
locales se conservan. La Web queda sin commit ni push de implementación; solo se
publican esta ficha y la fila correspondiente del registro central.

## Actualizaciones posteriores

- 2026-10-05: reorganización solicitada aplicada en fuente; revisión visual,
  reconstrucción/carga y prueba física pendientes de solicitud del usuario.
