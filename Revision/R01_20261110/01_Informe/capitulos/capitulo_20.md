# 20. Aportes propios y mejoras sobre el proyecto de referencia

BioLog se toma como referencia de organización y profundidad matemática, no como fuente de medidas de este robot. El informe propio debe presentar una línea coherente que conecte geometría real, transformación, actuación, medición y evidencia. Las figuras de otro proyecto no deben reutilizarse como si representaran piezas o resultados de AutoFinalRobo2.

| Mejora documental/metodológica | Implementación propuesta para este proyecto |
| --- | --- |
| Trazabilidad de cada número | Asociar dimensión, matriz y resultado a descriptor, plano, contrato o medida. |
| Dos robots en una sola cadena | Derivar la transformación mundial del TCP incluyendo base Mecanum y montura. |
| Convenciones consistentes | Aplicar explícitamente los signos del mezclador, encoder, CAD y visor. |
| Resultados con límites | Separar geometría, simulación, estimación y medición física en cada tabla. |
| Fabricación reproducible | Unir BOM, hojas por definición, ocurrencias y archivos editables. |
| Incertidumbre | Explicar cómo holgura, cuentas Hall y error de montaje afectan la tarea. |
| Estabilidad operativa | Usar margen al apoyo y postura de transporte como restricciones de planificación. |
| Diagnóstico transversal | Relacionar OLED, Web, telemetría, errores y herramientas de mantenimiento. |

## 20.1 Visuales que aportan al informe final

Se proponen láminas de marcos y ejes sobre la geometría real; cadena de transformaciones con colores por marco; comparación de ramas de cinemática inversa; vectores de rueda en avance, lateral y giro; recorrido real frente a odometría; error del TCP por pose; esquema de potencia y comunicación; y mapa de evidencias y pendientes.

Las burbujas o envolventes de trabajo lateral y superior quedan como pendiente ligado al aviso de diseño terminado que solicitó el usuario. Deben salir de geometría, límites, TCP y criterio de colisión declarados, con contorno alcanzable distinto del contorno operativo seguro. Esta entrega no ejecuta los prompts diferidos ni inventa radios de alcance para imitarlos.

Estas mejoras distinguen la propuesta del informe de referencia por la integración móvil, los contratos comunes y la correspondencia entre modelo matemático, piezas e implementación. Todavía deben materializarse y medirse los resultados físicos antes de considerarlas prestaciones demostradas.
