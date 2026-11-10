# 2. Requisitos y arquitectura del sistema

| Requisito | Traducción técnica | Estado documental |
| --- | --- | --- |
| Moverse en el plano | Tres velocidades de cuerpo: avance, lateral y giro; cuatro ruedas accionadas. | Mezcla Mecanum y signos definidos en contratos. |
| Manipular un objeto | Seis ejes del brazo y accionamiento auxiliar de pinza. | Geometría v17 y límites CAD; operación física completa pendiente. |
| Observar el entorno | Sensores de línea, ultrasonido con barrido y telemetría. | Pinout y fuentes disponibles; caracterización metrológica pendiente. |
| Operar localmente | OLED y encoder con navegación consistente. | Contrato de interacción común a firmware y Web. |
| Operar y diagnosticar a distancia | ESP8266, Web y protocolo con estados y errores. | Arquitectura implementada en fuentes; no se infiere disponibilidad de un despliegue. |
| Documentar y reproducir | CAD editable, BOM, láminas, contratos y herramientas propias. | Este borrador y el nuevo libro amplían esa cobertura. |

El ATmega concentra la interacción con motores y sensores de la plataforma. El ESP8266 proporciona conectividad y transporte hacia la interfaz Web. Contracts mantiene las definiciones compartidas, las unidades y el catálogo de comandos. El Loader es una aplicación C# independiente para las tareas de mantenimiento autorizadas. El CAD contiene la representación mecánica y sus insumos.

La cadena de operación se describe como operador → Web/OLED → comando con parámetros y estado → comunicación ESP↔ATmega → actuadores, con retorno por telemetría. Las funciones del brazo deben respetar el bus TTL serie de los servos seleccionados. Las placas Nano/PCA9685 presentes en representaciones CAD anteriores son referencias de ocupación y no prueban una electrónica válida para servos STS.

## 2.1 Restricciones de diseño

Se conserva la pinza V7 y la revisión v17 de PVC25/20. Se evita atribuir al robot la geometría histórica de PVC32 o longitudes de brazo de versiones anteriores. Los valores de masa obtenidos por volumen y densidad son estimaciones. Las superficies de dientes representadas por discos de paso no se consideran perfiles de engranajes fabricables.
