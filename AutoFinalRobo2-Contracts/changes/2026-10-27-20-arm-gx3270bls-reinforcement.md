# AFR2-20261027-20 — Brazo v07: actuador de codo y refuerzos

27/10/2026. El usuario autoriza continuar la revisión e implementar el nuevo actuador. Se parte del cálculo estático de 200 g y del informe/plan18; se conservan los maestros anteriores.

## Problema y resultado

El DS3218 del codo requería15,83 kgf·cm estáticos y no alcanzaba el objetivo preliminar×2. Se selecciona para CAD el **Yipin GX3270BLS270°**, ofrecido en Argentina por Starware SKU 2982, con 90 g y63 kgf·cm máximos anunciados a 7,4 V. No se trata ese máximo como par continuo o frenado garantizado.

Se crea primero su geometría paramétrica y después se reemplaza el housing del codo: una pieza PETG roja con manguito, horquilla, cuna, bancos de montaje, cuatro M3/arandelas/insertos. Se conserva eje Ø8 y los dos 608; se prolonga el acople metálico para alcanzar el nuevo estriado. Solapes del primer tubo pasan17→27 mm y se agregan nervios en su encastre, torre y cartucho/columnas de base. No se eleva la montura ni se cambian las longitudes de los caños.

[Ensamble v07 y fuentes](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v07/README.md) · [Informe de implementación y cargas](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/analisis_cargas_200g/07_implementacion_y_cargas_v07.md).

## Entrega concreta

- F3D completo v07 y F3D independiente del motor, ambos editables; 158 parámetros con expresiones/procedencia.339 componentes incluyen referencias y auxiliares.
- Modelo/parametrización propios a partir del plano Yipin publicado por el proveedor. Manifest con URL, hash, recuperación y licencia desconocida. Cotas inferidas y discrepancia de altura identificadas; no se importa CAD ajeno.
- Masas/CG e inercias de 271 cuerpos extraídos; densidades y masas comerciales aplicadas externamente.2.267,03 g añadidos al auto con 200 g y1.048,48 g de PETG equivalente macizo.
- Estática actualizada: J2 salida2,925 N·m; demanda del servo 17,54 kgf·cm con 2:1/η0,85. J3 conserva 15,83 kgf·cm porque el motor nuevo está aguas arriba de su eje.
- Cálculo rígido ilustrativo de frenada: un eje por vez a 30°/s,200/100/50 ms, más sensibilidad J3 a 600°/s sin carga de catálogo. Se conservan estados, fuerzas/momentos y seis componentes locales por sección.
- En30°/s–50 ms, picos al servo J1/J2/J3 de 16,23/26,91/25,56 kgf·cm. J1 supera los13 anunciados del MG946R: ese caso no queda aprobado. Sensibilidad600°/s: J3 llega144,68 kgf·cm, también superior al anuncio del nuevo servo.

## Límites e interfaces

Se conserva la monturaZ = 6…12 mm, hombro 209 mm, contraplaca inferior única y8 mm entre apoyosJ1. La flexión calculada de base llega4,505 N·m en un caso ilustrativo, equivalente a≈563 N por apoyo en el par idealM/d: los nervios no resuelven la separación insuficiente por sí solos.

Alimentación propuesta7,4 V regulados y hasta 7 A de bloqueo anunciados para el servo nuevo. Batería, regulador, disipación, horn y cotas reales siguen pendientes. No se cambia PWM de firmware ni se atribuye a la consigna una posición medida. Perfil Web y GLB anteriores no se actualizan a v07. Despieces diferidos.

| Módulo | Estado de esta entrega |
| --- | --- |
| CAD | Geometría nativa guardada/exportada y cálculo bajo hipótesis; sin aceptación de carga |
| Contracts | Ficha, índice y changelog actualizados |
| Atmega | Sin cambios ni compilación/flasheo |
| ESP | Sin cambios ni promoción de activos |
| Web | Sin cambios; no representa automáticamente la v07 |
| Loader | Sin cambios |

Se ejecutó el modelado y el cálculo de ingeniería solicitado. **Sin tests, compilaciones, validadores, barridos CAD de movimiento/interferencias, FEA, fabricación ni ensayo físico.** Los resultados geométricos de v06 no se transfieren a v07. Falta dinámica simultánea/AGV, aceleración previa, rotor, holgura/impactos, tensiones/contactos FDM y aceptación de los actuadores bajo carga.
