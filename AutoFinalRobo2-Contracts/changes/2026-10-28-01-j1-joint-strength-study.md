# AFR2-20261028-01 — Comprobaciones resistentes de J1 y sus uniones

28/10/2026. El usuario pide continuar las comprobaciones de J1/uniones y confirma que filamento y perfil de impresión todavía no están definidos.

## Resultado

Se entrega un [estudio navegable](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/analisis_J1_uniones/README.md), con extracción CAD de 35 secciones y evaluación de 7.075 estados de estática/frenadas individuales de 30°/s. Se amplían los datos v08 a ambos sentidos de J1 y 19 orientaciones finales; son 379 estados fuente. No se ejecutó barrido de interferencias para esas trayectorias adicionales. Se distinguen cargas del motor, rodamientos/cartucho, acople y anclajes; no se atribuye al plástico una resistencia certificada.

- 608: reacción radial máxima 565,20 N y C₀/P₀ de referencia 2,42. Alojamiento y apoyos siguen condicionados.
- Acople D/PETG: presión media necesaria≥35,49 MPa en modelo sin fricción; reparto uniforme en media cara≈70,98 MPa. Es contacto idealizado, no tensión FEA ni rotura demostrada.
- Eje Ø8 con plano/agujero: tensión normal nominal máxima 94,13 MPa. El acero real sigue sin identificar.
- Bastidor idealizado de cuatro barras: máximo 8,78 MPa nominal; criterio provisional n=2/Kt=2 exige≈35,11 MPa. Postes del servo:5,68 MPa nominales y≈22,72 MPa requeridos con ese criterio. Sin aprobación FDM.
- Anclajes:≈65 N de tracción incremental máxima en A; dos repartos ideales de placa rígida. Sin pretensión/contacto unilateral/prying ni aceptación de la placa o el acrílico.

Se calculan sensibilidades de separación de apoyos, diámetro de eje, contraplaca, nuevo motor y brida metálica. El objetivo dimensional propuesto es eje Ø10, dos 6000 y 16 mm entre centros, motor más bajo, brida Ø60/cubo Ø20 con cuatroM3 en PCD 50, y reacción del servo repartida. **No es un ensamble implementado o liberado.**

El estudio BRep temporal contra 680 cuerpos conserva la altura de plataforma: el GX3270BLS con caraZ=56 mm solo choca con la cabeza C actual; con una envolvente avellanada al ras no tiene penetraciones reportables contra los cuerpos incluidos. La nueva cuna/eje/brida/apoyos y el barrido del futuro ensamble no están comprobados.

## Alcance y entrega

Se conservan el maestro v08, sus parámetros y las geometrías anteriores sin modificación persistente. Cálculos, CSV, JSON de cargas/secciones y figura explicativa versionados; integración en índice del informe, CAD y maestro. Se corrige un enlace local mal escrito del informe v08.

| Módulo | Estado de esta entrega |
| --- | --- |
| CAD | Comprobación analítica de secciones/barras/fijaciones y estudio de espacio temporal; v08 sin editar |
| Contracts | Ficha, índice y changelog actualizados |
| Atmega | Sin cambios |
| ESP | Sin cambios ni promoción |
| Web | Sin cambios ni modelo promovido |
| Loader | Sin cambios |

Resultado: J1 no queda aprobado para la frenada de diseño. Se identifican detalles que requieren rediseño y requisitos de material, en lugar de declarar seguridad por ausencia de choques. Sin FEA de sólidos/contactos, tests de software, builds, flasheos, fabricación o ensayos físicos. Los despieces siguen diferidos. Cálculo de cargas y comprobaciones numéricas de equilibrio ejecutados por pedido del usuario.
