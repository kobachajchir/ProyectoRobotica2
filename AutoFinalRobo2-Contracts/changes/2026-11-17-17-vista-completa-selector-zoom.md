# AFR2-20261117-17 - Vista completa del seguidor: selector de zoom de los simuladores

Autoría: Koba Chajchir / AFR2.
Versión: 1.39.5 (subversión mínima).

Estados:
- web = en fuente;
- atmega = solo versión;
- esp = solo versión.

## Pedido

Poner en la vista completa del seguidor el mismo selector de zoom del simulador, arrancando con el zoom por defecto.

## Cambios (Web)

- `src/components/three/SimulationViewport.tsx`: `SimulationZoom` acepta `info` opcional, el texto de escala de cada vista; con `null` lo oculta. El valor por defecto mantiene el de los simuladores.
- `src/features/control/LineFollowerLiveView.tsx`: con `fill` (vista completa), el `SimulationViewport` lleva como overlay el mismo `SimulationZoom` (− / porcentaje editable / +).
  - 100 % es el encuadre ajustado.
  - Arranca en **60 %**, el zoom por defecto de la vista completa (AFR2-20261117-16).
  - Va de 10 % a 250 %, en pasos de 10 %.
  - El porcentaje multiplica el zoom de encuadre de la cámara (`distance`).
  - Texto de escala: «Escala real · trama cada 4 cm».
  - La vista normal no lleva selector.
- `src/styles/line-follower.css`: en el visor, el selector queda a la izquierda del riel lateral, sin taparlo. En celular se oculta el texto de escala.

## Estado de validación

Revisado en `npm run dev`, en el visor:
- el selector aparece en 60 %;
- «+» acerca (70 %);
- no se superpone con el riel;
- sin errores en consola.

Sin compilar.
