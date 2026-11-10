# Historial de R02

## 10/11/2026 · Informe D2

**Estructura nueva.** Se reescribió el informe con el orden de un trabajo integrador de Robótica II, tomando como modelo el ejemplo de informe aportado: portada, resumen, índices de contenido/figuras/tablas, 1 Aplicación y requisitos, 2 Antecedentes, 3 Diseño, 4 Modelos, 5 Trayectorias, 6 Control e interfaz, 7 Validación, 8 Discusión, 9 Conclusiones, bibliografía y anexos (índice de planos, archivos, símbolos).

**Contenido agregado respecto de D1**

- Aplicación concreta: recolección de cajas de 50 mm / 200 g con entrega en una zona circular, con la secuencia que implementa el simulador Web (AFR2-20261110-03/05).
- Base del proyecto: qué se conserva del kit OSOYOO y qué se reemplaza; manipuladores de referencia; tabla de alternativas.
- Registro de decisiones 23/10–10/11 tomado de las fichas AFR2.
- Control real del firmware leído de `afr2_product.c`: PID Q8 por rueda cada 10 ms, rampa, ley GOTO con velocidad mínima y escalado, seguidor de línea, fallas F1–F4 y cortes por enlace.
- Protocolo ROBO2 (trama, prefijos, 54 comandos por familia) y descripción de ESP, Web, OLED, Loader y versionado.
- 116 citas enlazadas a hojas del libro de planos y Anexo A con las 72 láminas principales.
- Figuras propias: arquitectura, cadena cinemática, patrones Mecanum, diagrama de control y simulación GOTO.

**Resultados nuevos (de modelo, no físicos)**

- Verificación numérica independiente (Python, 20 000 configuraciones): la tabla DH coincide con la cadena del descriptor v17 en posición y orientación (error 2·10⁻¹⁶ m); la inversa recupera la pose con error 1,6·10⁻¹⁵; el Jacobiano coincide con diferencias finitas (1,3·10⁻⁸); A⁺A = I.
- Simulación de la ley GOTO del firmware: llega en 3,7–5,6 s a 150 mm/s y se detiene ~10 mm (o 1,4°) antes del destino por terminar en el borde de la tolerancia; el camino diagonal no es recto por saturar vx y vy por separado.

**Correcciones a D1**

- D1 decía que no había un solucionador numérico general de seis ejes. La Web sí lo tiene (`armMotion.ts`, mínimos cuadrados amortiguados con varias semillas, tolerancia 1 mm / 1°, recorrido con chequeo de colisiones). Se describe en §4.4.3.
- Se quitaron del texto principal las salvedades repetidas; la distinción documentado/medido/calculado/estimado queda en las tablas y en §8.

**Sin cambios:** planos v17 y pinout (se usan los de R01), código, versión de producto (1.23.2). No se ejecutaron tests, compilaciones, validaciones ni flasheos. Las envolventes de trabajo del brazo siguen diferidas hasta el aviso de diseño terminado.

Estado: **en revisión**, sin aprobación como informe final.
