# 23. Fuentes y procedencia

| ID | Fuente | Uso |
| --- | --- | --- |
| S1 | BioLog_informe_v4.pdf, 87 páginas; aportado como referencia. | Estructura y repertorio matemático. No aporta dimensiones de AutoFinalRobo2. |
| S2 | AutoFinalRobo2-CAD/.../revision_v17/laminas_20261102/AFR2_Fuente_Laminas_20261102.f3d | Copia geométrica nativa usada en la documentación del 09/11. |
| S3 | Contracts/contracts/afr2_geometry.h y motion_reference.json | Radio/diámetro nominal, L y signos de movimiento. |
| S4 | Atmega/include/afr2_product.h; src/services/afr2_product.c | CPR y configuración nominal de rueda/sensores. |
| S5 | Web/src/features/collector/models/pvc25-v17-blanco.json | Cadena de articulaciones, marcos, TCP y límites. |
| S6 | Web/src/features/collector/armKinematics.ts; features/control/sequences/mecanumKinematics.ts; features/control/motionReference.ts | Alcance restringido de IK, mezclador, inversa y signos de contrato. |
| S7 | CAD/.../revision_v17/README.md; ALCANCE_VIGENTE.md; ENSAMBLES_Y_MOVIMIENTO.md; SELECCION_ACTUADORES.md; SELECCION_PVC.md | Configuración v17, transmisiones y materiales nominales. |
| S8 | CAD/.../revision_v17/analisis_conjunto_20261101/INFORME.md | Resultados históricos de masas, pares y estabilidad. |
| S9 | Contracts/hardware/INFORME_PINOUT_ROBOT_20261106.md y PINOUT.md | Electrónica de plataforma y añadidos propios. |
| S10 | Contracts/README.md; docs/VERSIONADO.md; contracts/wire_catalog.json | Arquitectura, trazabilidad y compatibilidad. |
| S11 | CAD/.../revision_v17/laminas_20261109/interfaces_nativas.json y vistas_completas.json | Extracción de 66 definiciones, 237 parámetros nativos y vistas reproducibles. |

R1. Lynch y Park, Modern Robotics, recurso oficial 13.2: robots móviles con ruedas omnidireccionales. https://modernrobotics.northwestern.edu/nu-gm-book-resource/13-2-omnidirectional-wheeled-mobile-robots-part-1-of-2/

R2. Lynch y Park, Modern Robotics, recurso oficial 4.1.1: producto de exponenciales y marcos cinemáticos. https://modernrobotics.northwestern.edu/nu-gm-book-resource/4-1-1-product-of-exponentials-formula-in-the-space-frame/

R3. FEETECH, ficha técnica oficial STS3250, tensiones y pares nominal/de bloqueo. https://www.feetechrc.com/Data/feetechrc/upload/file/20240120/6384135881578380868917773.pdf

Las fuentes locales se citan respecto de D:/ProyectoRobotica2. El identificador de linaje de la copia CAD es urn:adsk.wipprod:dm.lineage:EcPmilmLR5e65I7_wwDoSg; captura de geometría 02/11/2026 11:39:50 UTC. La extracción de interfaces del 09/11 reutiliza esa copia: no es una afirmación de que se haya capturado una geometría posterior.
