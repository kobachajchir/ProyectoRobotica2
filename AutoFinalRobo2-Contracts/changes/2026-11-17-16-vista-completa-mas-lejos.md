# AFR2-20261117-16 - Vista completa del seguidor: cámara más alejada

Autoría: Koba Chajchir / AFR2.
Versión: 1.39.4 (subversión mínima).

Estados:
- web = en fuente;
- atmega = solo versión;
- esp = solo versión.

## Pedido

En la vista completa (visor de seguimiento), menos zoom: que el auto se vea de más lejos.

## Cambios (Web)

- `src/features/control/LineFollowerLiveView.tsx`:
  - **Cámara:** `Scene` recibe `distance`, que multiplica el zoom de encuadre. Con `fill` (vista completa) vale 0,6: el auto ocupa cerca de la mitad del alto y se ve más cinta alrededor. La vista normal no cambia (1).
  - **Tamaño de la escena:** el piso con la trama, la pista y la cinta pasan de 1,6 m a 4 m (`FLOOR_SIZE`), igual que `SimulationViewport floorSize`, para que la cámara alejada no muestre el borde en pantallas anchas. La trama sigue cada 4 cm.

## Estado de validación

Revisado en `npm run dev`, en el visor (Programas → Seguidor de línea → Vista completa):
- a 800 px y a 1920 px el auto se ve más lejos y el piso cubre todo el ancho;
- sin errores en consola.

Sin compilar.
