# AFR2-20261029-23 — Investigación de batería por ADC

29/10/2026. Pedido de Koba Chajchir: investigar divisor resistivo para batería de casi 8 V y futura lectura local en submenú y Web. Investigación propia por Codex, sin implementar hardware/software.

[Informe y circuito candidato](../hardware/MEDICION_BATERIA.md): A5/ADC5 libre según mapa/fuente, 10k/10k viable condicionado a alimentación y máximo real; alternativa preferida provisionalmente 15k/10k con mayor margen y C de 100 nF. Incluye referencia AVCC, tolerancia, impedancia, consumo, apagado y protección pendientes. No se identifica el pack como 2S por inferencia.

ATmega ya posee gestor ADC para IR; batería requiere integrar arbitraje, validez/calibración y publicación. Propuesta Sistema → Batería y Web vinculados, sin IDs/versions nuevos ni modificación de contratos activos. Productor/consumidores pendientes conjuntamente. No se deriva porcentaje o corte automático.

Estado atmega/esp/web: sin modificaciones de código, compilación, imágenes ni cargas por esta ficha. Contracts: solo documentación de diseño/investigación; adaptación por archivo incluida en el informe. Sin tests, validadores, pruebas físicas, COM, CAD ni flasheos. No certifica memoria, precisión ni hardware.
