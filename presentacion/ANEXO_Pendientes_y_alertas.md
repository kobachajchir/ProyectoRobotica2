# Anexo · Qué se puede afirmar y qué falta

Para exponer con precisión. «Medido» = con instrumento propio; «código» = leído en fuente, no ejecutado en el auto.

## 1. Lo que SÍ se puede afirmar

| Tema | Afirmación | Nivel |
| --- | --- | --- |
| Arquitectura | 6 módulos, un Git; firmware propio ATmega (C) y ESP (Arduino), Web React, Loader C# | código |
| Enlace | ROBO2 perfil 2 + EX01 v13; 54 comandos catalogados | código |
| Versión | 1.27.0 única para ATmega, ESP y Web | código |
| Odometría | 370 cpr, avance 1000 mm = 1000 mm | **medido** |
| Odometría lateral | 1000 mm contados = 775 mm reales (4 pruebas) → factor 0,775 | **medido** |
| Brazo | v17: 70/60/55/65/78,5 mm, PVC25/20, 5 STS3250 + 2 STS3032 | CAD |
| Masas | 2,345 kg brazo; 4,045 kg conjunto (con 1,5 kg de auto aprox. y 200 g) | CAD + estimación |
| Estabilidad | FS frontal 1,023 < 1,2; lateral 3,63; atrás 1,86 (01/11/2026) | cálculo sobre CAD |
| Simulador y misión | definidos y programados en la Web | código (simulación) |
| OLED + encoder | configuración validada con diagnóstico independiente (29/10/2026, confirmada por el usuario) | **medido/observado** |

## 2. Lo que NO hay que afirmar

- Que el robot «completo» (base + brazo) funciona físicamente. El brazo no está fabricado ni integrado eléctricamente (bus TTL de servos, 12 V/6 V, protecciones sin definir).
- Precisión, repetibilidad, velocidad máxima, carga útil o autonomía: no hay campaña de medición.
- Resistencia estructural: no hay FEA ni ensayos; PLA con E = 1500 MPa **hipotético**.
- Que KEY/RST automáticos funcionan: circuito sin armar/medir (estado 06/11/2026).
- Que el Loader flashea con hardware: no comprobado.
- Que los signos Mecanum/Hall están verificados: vienen de sentidos informados (`physicalVerification: false`).
- Que el GOTO llega a ±10 mm: es la **tolerancia de control**, no un error medido.
- Que el PCB del panel está validado: sin ERC/DRC; KiCad conserva +3V3 mientras el montaje usa 5 V.
- Que la envolvente de trabajo (burbuja) está calculada: pendiente.

## 3. Inconsistencias detectadas en el repositorio (resolver o aclarar al exponer)

1. **L (suma de semidistancias):** 170 mm (firmware/contrato) vs 220 mm (propuesta del simulador). Usar 170 mm.
2. **Eslabones del recolector:** texto con 130/120 mm (v02 histórico) vs v17 con 70/60. Usar v17.
3. **Textos «sin compilar»** de fichas viejas conviven con builds posteriores; el estado vigente está en `INFORME_MAESTRO.md` y los README de cada módulo.
4. **Plan GOTO** (08/11) dice «solo simulado»; desde 1.23.x existe envío por tramos.
5. **Capturas del Loader** son de la v1.2.0; el módulo está en 2.4.0 (fuente).
6. **Pinout PDF del 27/10** es histórico; usar `INFORME_PINOUT_ROBOT_20261106` y `PINOUT.md`.

## 4. Datos que faltan para completar la presentación

- Institución, asignatura, docentes, integrantes y fecha de exposición (portada).
- Foto real del prototipo y del panel (FOTO-02, 65) y de las pantallas OLED (FOTO-48…52).
- Cualquier medición nueva (peso real del auto y del brazo, deriva de rumbo, tiempo de ciclo) para reemplazar hipótesis.
- Confirmación de qué pin (D32/D33) maneja cada cable de color en el circuito NPN.

## 5. Plan de mediciones propuesto (si se quiere cerrar «Resultados» en la exposición)

Pesar auto + brazo + batería · medir CPR y signos por rueda · avance/lateral/giro de 1000 mm / 90° con cinta · deriva en 3 m · tensiones KEY/RST en
reposo y apretado · tiempo de carga del ESP con el circuito. Cada ensayo: hipótesis, instrumento, repeticiones, criterio de aceptación.

## 6. Sugerencia de tiempos (si la exposición dura 20 min)

Intro 1,5 · Modelos 1,5 · Pinout 1,5 · Firmwares 2 · Matemática auto 2 · GOTO 1,5 · Brazo/conjunto 2 · Esfuerzos 2 · Simulador 1,5 · Interfaz 1,5 ·
Loader 1 · NPN 1 · Encoder/OLED 1,5 ≈ 21 min. Para 15 min: filminas 3, 4 y 10 en un minuto cada una.
