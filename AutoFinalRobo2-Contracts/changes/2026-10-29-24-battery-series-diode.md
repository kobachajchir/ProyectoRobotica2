# AFR2-20261029-24 — Recálculo de batería con diodo en serie

29/10/2026. Pedido de Koba Chajchir: incorporar diodo y calcular con caída de 0,7 V. Elaboración y cálculos propios por Codex; autorías previas conservadas.

Se documenta la variante con diodo antes del divisor, ánodo a BAT+, cátodo a resistencia superior. Con 10k/10k: 8 V → 3,65 V y 8,4 V → 3,85 V en ADC; reconstrucción aproximada Vbat = 2 × Vpin + 0,7 V. También se recalcula 15k/10k. Se distingue hipótesis de caída fija de comportamiento real y se aclara que el diodo no impide alimentar VCC desde el pin con Mega apagado: la corriente circula en sentido directo.

Adaptaciones por archivo: `hardware/MEDICION_BATERIA.md` agrega circuito, tabla, fórmulas, tolerancias, consumo, limitaciones y fuentes Microchip/Vishay, conservando el diseño original identificado sin diodo; esta ficha registra alcance; `README.md` y `CHANGELOG.md` agregan enlace.

Estado atmega/esp/web: sin cambios de fuente, wire, ScreenCodes, imágenes ni despliegue. Solo investigación documental; no hardware probado, tests, validadores, compilación, COM ni flasheo. Aislamiento en apagado y calibración pendientes; no se afirma una solución eléctrica terminada.
