# AFR2-20261031-07 — Giro axial del conjunto del SG90

El usuario observa que el conjunto del sensor sigue girando fuera del eje.
La implementación06 aplicaba al nodo GLB de CapaSuperior un punto expresado
en BlenderZ-up. La lectura del JSON del GLB público muestra que ese nodo tiene
rotación identidad y usa glTFY-up. El punto debe convertirse con(x,z,-y).

Se corrige el punto de[-5.150,75.000,39.150]mm a
[-5.150,39.150,-75.000]mm en el marco local del nodo. No cambia el eje del CAD:
se corrige la convención con la que se interpreta. La dirección local es+Y.

Un marco fijo guarda posición y orientación respecto de la cubierta y un hijo
recibe exclusivamente el ánguloY. Salida/horn/tornillo, soporte y HC-SR04
se atan a ese hijo conservando la postura de reposo. La carcasa permanece fija.
Control y Simulación reutilizan el helper; no se cambia el GLB ni el brazo.

Estados: web fuente corregida, atmega/esp sin cambios. Se documenta la
conversión en el registro CAD. Sin tests, builds, validadores, navegador ni
flasheo por instrucción del usuario. Pendiente de revisión visual en ejecución;
no se presenta la lectura del JSON como prueba del movimiento renderizado.
