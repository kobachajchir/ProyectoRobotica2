# AFR2-20261028-07 - Busqueda simultanea de par, masa y resistencia

28/10/2026. Objetivo explicito creado a pedido humano. [Informe de la iteracion v11](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/busqueda_actuadores_v11/README.md). No se modelan housings nuevos antes de cerrar la preseleccion. Se conserva Fusion v09.

DS51150 en J2/J3 supera el filtro de par/masa a30grados/s aun con206g por motor, reservas de montaje y transmision metalica. El caso reforzado pesa3519g con carga200g. Pero no satisface la frenada50ms desde las velocidades maximas de catalogo: J1 demanda440kgfcm frente a63 de bloqueo anunciado. El valor30grados/s es referencia previa, no una velocidad minima exigida por el usuario. No se acepta la combinacion como seleccion final.

Los dientes del hombro tambien limitan: modulo1.5,Z20/Z40 y ancho6mm dan una referencia Lewis85MPa en el caso reforzado. Mas par de motor no acredita los dientes impresos. Acero, ancho, contacto y acople se deben cerrar conjuntamente.

Se estudia otra arquitectura con ST-3215-C047/12V multivuelta, par nominal10kgfcm y reducciones externas12/28/24/4/12/4. Existe oferta argentina STS3215/12V/30kgfcm en Starware SKU3116; no esta confirmada su correspondencia con la fichaC047. El estudio incluye80g por motor, presupuestos de reductores300/300/300/75/150/75g, eficiencia0.50 y200g de potencia fija. Masa total3919g; velocidades iniciales de salida24.75/10.61/12.38/74.25/24.75/74.25grados/s incluyendo+10% de tolerancia de velocidad. Frenada50ms con200g.

En5502 estados fuente de262 trayectorias y su expansion de J1, los cocientes par nominal/demanda superan2, con minimo2.11 en J2. Es un filtro analitico condicionado a que existan reductores con esas prestaciones, masas e inercias. No es CAD fabricable ni curva de frenado acreditada. El motor conserva control de posicion mediante encoder integrado/busTTL; no usa PCA9685 como servoPWM. Multivuelta pierde contador al apagar: inicializacion pendiente.

Se propagan las cargas STS a la geometria J1 actual: referencia P0=507N, requisito normal FDM14.73MPa con n2/Kt2, y requisitos de husillo/fijaciones en JSON. PLA PrintaLot sin perfil/ensayo acreditado; engranajes, housings reales, alimentacion y estabilidad del auto siguen abiertos. El objetivo permanece activo, sin aprobacion fisica del conjunto ni compra.

| Modulo | Estado de esta entrega |
|---|---|
| atmega / esp / web | Sin cambios de producto, compilacion ni flasheo |
| CAD | Calculo externo pedido, datos/fuentes y presupuesto; sin nuevo CAD, FEA ni choques de nueva geometria |
| Loader | Sin intervencion |
| Contracts | Registro y enlaces |

Se versionan scripts y resumenesJSON/CSV, con hashes y fuentes. Estados completos regenerables en CAD/exports, excluidos por politica global. Sin tests de software, builds ni ensayos fisicos.
