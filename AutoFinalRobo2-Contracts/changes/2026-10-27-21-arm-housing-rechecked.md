# AFR2-20261027-21 — Housing GX3270BLS y revisión CAD v08

27/10/2026. El usuario pide hacer/pulir el housing del nuevo motor y recomprobar el conjunto con las modificaciones. Esta instrucción autoriza las comprobaciones geométricas ejecutadas en esta entrega.

## Antes y después

La v07 tenía una cuna integrada, pero faltaba completar la protección lateral/posterior y repetir comprobaciones tras reforzar. El análisis inicial detectó 12 penetraciones internas y una entre conjuntos. En una copia v08 se agregan paredes ventiladas de 3 mm, salida de cable propuesta y radios exteriores, y se corrigen reservas de tapa 608, insertos, pestañas, tornillo central, abrazadera proximal y refuerzo frente al XL4016.

El housing conserva un cuerpo continuo PETG rojo, cuatro M3 y eje/apoyos existentes; la base sigue baja. Se guardan el [ensamble editable y housing/motor independientes](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v08/README.md),163 parámetros, procedimiento de modelado y evidencia. Maestros previos conservados.

## Resultado comprobado

- 0 penetraciones internas reportables y 0 en la postura nativa entre conjuntos; umbral 0,001 mm³.
- 3.141 configuraciones CAD muestreadas con 0 penetraciones reportables: seis ejes, pinza, rejilla de muñeca, pan y J1/pan. Las posturas exactas y límites del muestreo están en el informe; no equivale a todo el espacio continuo.
- 25 sólidos impresos continuos, 6 ejes y 12 rodamientos completos con contacto; housing una sola región. Historial sin advertencias/errores activos.
- Pinza accionada nativamente:50,000 mm a 109,8° y 60,011 mm a 133,7°. Mínimos/centros/máximos de J1…J6 accionados. J3 conserva 180° de recorrido CAD dentro de 270° nominales del servo.
- Masas/CG/estática y escenarios analíticos anteriores recalculados:2.277,49 g añadidos al auto con 200 g; PETG macizo 1.058,94 g; aumento neto 10,46 g respecto de v07.

J1 sigue sin satisfacer la frenada hipotética 30°/s–50 ms:16,27 kgf·cm frente a 13 anunciados del MG946R. Apoyos J1, alimentación, horn real, tolerancias, resistencia y frenada física siguen abiertos. No se presenta el housing como solución a esos límites.

| Módulo | Estado de esta entrega |
| --- | --- |
| CAD | Housing v08 guardado/exportado, comprobaciones geométricas autorizadas y cálculo bajo hipótesis |
| Contracts | Registro, índice y changelog actualizados |
| Atmega | Sin cambios, compilación ni flasheo |
| ESP | Sin cambios ni promoción de activos |
| Web | Sin cambios; geometría v08 no promovida |
| Loader | Sin cambios |

Sin tests de software, builds, flasheos, FEA ni ensayos físicos. Los resultados CAD no certifican carga de 200 g, velocidad real, fuerza de agarre, resistencia FDM o estabilidad. Despieces/planos siguen diferidos. PWM conserva su significado de consigna, no posición medida.
