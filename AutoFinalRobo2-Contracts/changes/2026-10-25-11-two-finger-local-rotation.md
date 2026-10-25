# AFR2-20261025-11 — Giro local con dos dedos

25/10/2026. Pedido humano: girar el auto sobre su propio eje con dos dedos,
hacia izquierda/derecha, como simulación. No se amplía el control físico.

useManualDrivePointers conserva un dedo para traslación y reconoce dos
contactos touch dentro del área para twist. IDs/orden estables; separación
mínima40px, umbral angular3°, delta normalizado al cruzar±180°. Se acumula el
ángulo de gesto y se publica como máximo una vez por frame. No inercia ni
zoom por distancia entre dedos; el auto gira, la cámara queda fija.

Al entrar el segundo dedo cancela el target previo y fija Simulación local,
incluso si había robot conectado. No manda robot.drive con wz, comandos nuevos
o yaw físico. Si existía una consigna real, usa su callback de liberación
existente; no inicia ninguna nueva. Reconectar no promueve la sesión local.

Al levantar/cancelar un dedo, perder captura/foco/visibilidad, salir del área,
resize/STOP o tercer contacto/multifuente, termina el gesto y drena contactos
restantes. No convierte al dedo restante en traslación. Nuevo ciclo primario
recupera una liberación perdida; abrir otra sesión reinicia contactos/rumbo.

Modelo rota sobreY desde el mismo centro y conserva orientación al soltar.
Rumbo0..360° visible, con indicación izquierda/derecha; el número no anuncia
cambios por frame a lectores de pantalla. Q/E giran15° por pulsación nueva,
0 centra, campos/Ctrl0 conservan comportamiento nativo. No se promete soporte
del gesto nativo de trackpad que no exponga dos PointerEvents touch.

Tras el giro, un dedo sigue la dirección de pantalla: velocidad se transforma
al marco del cuerpo y limita por rueda. Ruedas en pivot usan offsets angulares
acumulados Mecanum, L300mm/R40mm nominales, sumados a la fase ya existente.
No sensor/odometría real, dinámica ni calibración. La barra de velocidad
afecta traslación; el ángulo de giro sigue los dedos. Reduced motion conserva
la manipulación principal y suprime giro secundario de ruedas/chevrones.

Cinco fuentes: hook nuevo, Overlay, ManualDriveModel, RobotModel (offset opcional)
y CSS de lectura. Chevrones09/cámara60%10 y GLB intactos. Consumidores de
RobotModel sin offset conservan el flujo anterior; tipos de toda Web compilan.
ESP/ATmega0.9.0/EX01v7/build01,30comandos/59ScreenCodes y gates0 sin cambios.
ESP solo activos Web11. Sin nueva dependencia/asset, flasheo, navegador,
tests/validadores independientes, teléfono o pruebas físicas; gesto y transición
de fuentes/captura no comprobados en dispositivo. VIS-034 Propuesta, no aceptación.
[Contrato](../contracts/WEB_TOUCH_SIMULATION.md),
[build11](../documentation/builds/2026-10-25-11/README.md),
[procedencia](../provenance/two-finger-local-rotation-20261025.json).
