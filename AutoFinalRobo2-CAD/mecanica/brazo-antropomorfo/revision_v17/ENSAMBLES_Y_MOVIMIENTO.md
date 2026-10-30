# Entrega del modelo 3D: ensambles y movimiento

Modelo editable: `AFR2_Brazo_v17_Base_V7_J3_Remoto.f3d`.
Estado aplicado: `estado_cad_v17.json` y `progreso_J3.jsonl`.
Los scripts preparados no equivalen a operaciones aplicadas en Fusion.

## Organizacion

| Articulacion | Grupo movil | Motor y transmision | Recorrido articular conservado |
|---|---|---|---|
| J1 | Giro de base | STS3250 en montura baja; horn y adaptador al eje apoyado en dos 6000 | -90 a +90 grados |
| J2 | Brazo PVC | STS3250 fijo en torre; engranajes 18/72 y eje de entrada con dos 625 | -10 a +125 grados |
| J3 | Antebrazo PVC | STS3250 sobre primer eslabon junto a J2; correa HP3M 20/50 hasta el codo | -90 a +90 grados |
| J4 | Giro axial de muneca | STS3250 sobre antebrazo; engranajes 24/24 | -90 a +90 grados |
| J5 | Inclinacion de muneca | STS3250 en horquilla, horn y brida al eje apoyado en dos 625 | -90 a +90 grados |
| J6 | Giro de herramienta | STS3032, horn y eje con apoyos conservados | -90 a +90 grados |
| Pinza | Dos mordazas de V7 | STS3032, pinon y cremalleras simetricas | J7: -90 a +90 grados, equivalentes a 0..180 de referencia del servo |

Son limites de las articulaciones del modelo, no limites de firmware ni
una declaracion de ausencia de choques para toda combinacion de ejes.
Los ejes auxiliares de engranajes y correa siguen sus vinculos de movimiento;
no deben limitar adicionalmente el recorrido de la articulacion principal.
El eje auxiliar J3 queda sin limites independientes: Fusion normalizaba
el intento de intervalo mayor de una vuelta a135..225 grados. Se conserva
el tope de J3_Codo y la relacion2.5 del vinculo. Cada corredera de la pinza
conserva -18.85..+18.85mm. El inventario nativo registra47uniones y5vinculos.

Los housings se integran en los encastres o monturas correspondientes.
Las tapas, ejes, rodamientos, horns y fijaciones siguen siendo componentes
separados para conservar una secuencia de montaje y futura documentacion.
La pinza es la de V7, con sus guias y cremalleras; no se retoma la pinza
compacta descartada. La correa de J3 no cruza el giro axial de J4.

## Alcance autorizado ahora

Terminar geometria, montajes, ensambles y limites en Fusion. Se difieren
calculos de masas, esfuerzos, resistencia, capacidad de actuadores,
estabilidad y optimizacion de material hasta indicacion del usuario.
Los calculos y revisiones anteriores conservan su geometria y fecha de
referencia: no se actualizan automaticamente por guardar un F3D nuevo.
Tampoco se generan aun despieces ni planos de fabricacion.

La integracion electrica TTL y alimentaciones de los nuevos actuadores
queda pendiente; la electronica de referencia conservada en el CAD no
representa cableado funcional de estos servos.
