# AFR2-20261031-06 — Simuladores, montaje v17 y eje del SG90

Pedido humano: reorganizar los simuladores con Emil/Apple/Visual Identity,
fondo celeste y suelo crema; corregir montaje sobre el auto y giro excéntrico
del servo con referencia al CAD.

Web: escena amplia, matemática opcional, lectura jerarquizada, controles por
tarea, puntos y motores avanzados plegables, superficies/acciones existentes.
La parada detiene también el brazo; conserva el halt del motor de simulación.
Tokens locales VIS-045 para cielo/suelo/retícula, sin cambiar la identidad global.

Se corrige el descriptor v17 y su generador: montar requiere convertir del CAD
original al origen recortado del brazo y al auto centrado en Web. La montura
queda en[27.819,0,79.041]mm en vez de[-14,0,94.64]mm. Datos y procedencia en
CAD/mecanica/brazo-antropomorfo/revision_v17/alineacion_web_20261031.json.

Se usa el eje cilíndrico del SG90 del CAD y un helper compartido Control/Simulación.
Blender separa eje/horn/tornillo de la carcasa; salida, soporte y sensor giran
juntos. Auto GLB público880232bytes, revisión de caché actualizada. Brazo780972
y cubo1784bytes conservados. Materiales preservados, sin nueva decimación.
Fuente Blender y script editables; CAD nativo sin mutaciones.

**Estados:** atmega/esp sin cambios de fuentes/protocolo; web fuente y activo
público actualizados. Paquetes dist/artifacts/ESP permanecen en revisión05;
sin compilación, tests, validadores, navegador ni flasheo. Lecturas CAD/GLB
son evidencia de marcos y composición, no verificación en ejecución ni física.

[Detalle y comparación](../../AutoFinalRobo2-Web/docs/simuladores-3d-20261031.md).
[Cobertura visual](../docs/design-system/coverage-20261031-06.json).
