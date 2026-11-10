# 1. Resumen, problema y alcance

El proyecto integra una base de cuatro ruedas Mecanum con un brazo de seis articulaciones rotacionales y una pinza de dos mandíbulas. La base aporta movilidad en el plano y el brazo permite orientar y posicionar la herramienta. El objetivo académico es explicar esa integración mediante geometría trazable, cinemática, medición, control y una arquitectura de comunicación común.

El problema técnico central consiste en transformar una consigna de desplazamiento o manipulación en movimientos coordinados, manteniendo explícitas las unidades, referencias espaciales y limitaciones del montaje. La movilidad amplía el alcance del brazo, pero también modifica la estabilidad del conjunto y exige distinguir el movimiento de la plataforma del movimiento relativo del manipulador.

El sistema incluye seis módulos bajo un único repositorio: firmware ATmega, firmware ESP8266, interfaz Web, contratos, CAD y cargador Windows. El informe cubre el robot como sistema; no presenta como terminadas las piezas que carecen de tolerancias de fabricación ni la integración física del bus de servos.

| Elemento | Estado que puede sostenerse con las fuentes |
| --- | --- |
| Geometría del brazo | Modelo nativo v17; documentación extraída de la copia F3D del 02/11/2026. |
| Nuevo libro mecánico | 66 definiciones fabricadas o adaptadas, con vistas, cotas nominales, interfaces y cortes documentales. |
| Matemática | Modelo analítico vinculado a dimensiones y referencias del descriptor Web; derivaciones del borrador, sin nueva comprobación numérica o física. |
| Software | Fuentes y contratos disponibles. La lectura del código no demuestra su comportamiento en la placa o en el navegador. |
| Resultados físicos | Se distinguen datos informados por el usuario, estimaciones CAD y ensayos todavía pendientes. |

## 1.1 Objetivo general y objetivos específicos

Desarrollar y documentar una plataforma móvil con capacidad de manipulación, relacionando el diseño mecánico con modelos matemáticos y una interfaz de operación reproducible.

Los objetivos específicos son describir las transformaciones de ambos robots; relacionar velocidades de ruedas con movimiento planar; convertir pulsos Hall en desplazamiento; derivar la cinemática directa e inversa del brazo; documentar transmisiones y piezas; explicar el sistema de control y comunicación; y formular criterios medibles para la futura puesta en marcha.

## 1.2 Criterio de evidencia

En todo el texto se usan cuatro niveles: dato nominal de fuente, geometría CAD, implementación en código y comportamiento físico medido. Un valor del modelo no reemplaza una medición. Una captura de la Web representa una visualización; una animación de ensamble no demuestra que el mecanismo pueda montarse o trabajar bajo carga.
