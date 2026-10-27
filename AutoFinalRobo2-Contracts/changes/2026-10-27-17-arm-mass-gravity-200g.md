# AFR2-20261027-17 — Presupuesto de masas y cargas gravitatorias con 200 g

27/10/2026. El usuario pide primero masas, materiales, centros de masa y esfuerzos en el brazo totalmente extendido con carga máxima de 200 g, para dimensionar después refuerzos y estudiar tensiones. Autoriza estos cálculos. No pide todavía implementar nuevos nervios ni producir despieces.

## Entrega y método

Se extraen 264 cuerpos del maestro v06, con volúmenes/centroides nativos High. El material Steel asignado por defecto no se usa para pesar las piezas: se aplican densidades de referencia a las fabricadas y masas de catálogo a servos/rodamientos, con supuestos explícitos para GX, electrónica y accesorios. Se separa el peso propio de la carga útil.

Se guarda copia nativa `AFR2_Brazo_PVC32_Estatica_200g_v06`, postura J1…J6 = 90/90/0/0/0/0°, pinza 109,8° y parámetro de carga 200 g. El maestro v06 se conserva. Las fuerzas se aplican en un modelo analítico de gravedad, no en un estudio FEA de Fusion. La cinemática analítica se coteja con la pose nativa, sin discrepancias mayores a 10⁻⁹ mm.

[Índice del análisis](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/analisis_cargas_200g/README.md): capítulos de masas/CG, esfuerzos/reacciones y tensiones nominales/criterios de refuerzo; CSV por cuerpo y de reacciones, JSON, fuentes de cálculo, diagrama PNG/SVG y copia F3D.

## Resultados de la primera etapa

- PETG: 823,13 cm³, 1.045,38 g si fuera macizo. Las fracciones efectivas 70/50 % son sensibilidad de masa, no porcentajes equivalentes de relleno ni resistencia certificada.
- Masa añadida al auto con carga: 2,229 kg, incluida electrónica y montaje; sin carga 2,029 kg. CG global del conjunto añadido (−13,22; 120,33; 153,03) mm; no incluye el auto original.
- Centro del cubo a 458 mm del hombro. Los 200 g ejercen 1,961 N y aportan 0,898 N·m al hombro.
- Peso propio más carga: hombro 2,911 N·m, codo 1,552 N·m, inclinación J5 0,427 N·m. Con reducción 2:1/eficiencia supuesta 0,85, servo de hombro 17,46 kgf·cm; codo directo 15,83 kgf·cm estáticos.
- Con criterio preliminar de margen 2, el codo requiere 31,65 kgf·cm frente a 21,5 kgf·cm máximos de bloqueo publicados del DS3218. No se confunde torque de bloqueo con torque continuo. El codo y el agarre requieren revisión de selección además del refuerzo estructural.
- J1 recibe 2,933 N·m de flexión/vuelco y unos 367 N radiales por apoyo con separación de 8 mm; no se interpreta el par gravitatorio nulo de yaw como ausencia de carga. El engrane de hombro recibe unos 97 N tangenciales y 35 N de separación radial.
- PVC: tensión nominal a la salida libre proximal 1,97/1,00 MPa, contribución estimada a caída vertical 0,85–0,95 mm considerando solo tramos expuestos. No se atribuye esta deformación al brazo completo ni se certifican manguitos/asientos con ella.

Se proponen criterios de refuerzo local en cartuchos, manguitos, raíces de horquilla y dientes, junto a mayor separación de apoyos donde quepa manteniendo la base baja. No se modifica aún la geometría. Se preparan cargas y condiciones para el análisis de tensiones posterior: contactos, precargas, orientación FDM, fuerza de cierre y aceleración pendientes de definir.

| Módulo | Estado de esta ficha |
| --- | --- |
| CAD | v06 conservada, copia horizontal de estudio y cálculo estático documentado |
| Atmega | Sin cambios de esta ficha |
| ESP | Sin cambios de esta ficha |
| Web | Sin cambios de esta ficha; no se promueve perfil ni modelo con capacidad de 200 g |
| Loader | Sin cambios de esta ficha |
| Contracts | Alcance, hipótesis, fuentes y resultados registrados |

Sin FEA, fabricación ni ensayo físico. La capacidad estructural, térmica, el agarre real y la estabilidad del AGV no quedan acreditados. El pedido de 200 g es un requisito de estudio, no una capacidad útil ya aceptada del diseño.
