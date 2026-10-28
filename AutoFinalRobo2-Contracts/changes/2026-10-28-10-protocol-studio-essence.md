# AFR2-20261028-10 — Esencia de AutoMicro e identidad AFR2 en Protocol Studio

28/10/2026. Pedido humano: cotejar funcionalidad e interfaz con AutoMicro,
conservar todas sus ideas y aplicar consistencia con el trabajo visual actual.

Se recupera el flujo instrumental del estudio: Constructor inicial, parámetros
por comando, catálogo agrupado, override HEX, estructura por campos/offsets,
cuatro salidas simultáneas, puente hacia Traductor, mapa de captura seleccionable,
stream continuo, análisis por bloque y ayuda. Se preserva el codec ROBO2 y todas
las conversiones agregadas en01. Arrays C/Python combinan decimales y `0xHH`
sin alterar el valor de los enteros; se reconoce `frame = bytes([...])`.

Los formularios usan campos actuales de AVR/Contracts, incluidos los formatos
temporal/por rueda y variantes GOTO. No se copian IDs, longitudes o rutas UNER.
La estructura visual se comparte entre constructor e inspector; la copia y sus
fallbacks se extraen a componentes comunes para no duplicar comportamiento.

| Antes01 | Ahora02 | Motivo |
|---|---|---|
| Constructor solo manual | Parámetros y HEX avanzado | Recuperar la construcción guiada de AutoMicro |
| Resumen textual de bytes | Campos desplegables y grupos de bytes | Explicar cómo se compone el paquete |
| Un selector de salida | Cuatro formatos juntos y decimal adicional | Conservar comparación y copia inmediata |
| Lista de detalles por bloque | Mapa seleccionable, stream y análisis | Recuperar la exploración de capturas |
| Ayuda solo documental | Modal de ayuda integrado | Mantener el flujo autocontenido |

Identidad: skill `visual-identity-system` en APLICAR, manual0.1.9 previamente
autorizado para Desarrollador y primitivas; dirección Confirmada, composición
Propuesta, aspecto No verificado. Tokens rubí/carmesí, superficies sólidas,
Button/Field/TaskSections/FloatingModal/notify propios. Sin paleta paralela,
dependencias nuevas ni nuevas animaciones.

[Matriz detallada](../docs/PROTOCOL_STUDIO_AUTOMICRO.md),
[contrato vigente](../contracts/WEB_PROTOCOL_STUDIO.md),
[procedencia02](../provenance/web-protocol-studio-essence-20261028.json) y
[cobertura VIS-046](../docs/design-system/coverage-20261028-10.json).

| Módulo | Estado real de esta ficha |
|---|---|
| web | Adaptación en fuente, con revisión manual; sin compilar ni ejecutar |
| atmega | Sin cambios; no se acredita prueba física |
| esp | Firmware y data/dist sin cambios; paquete Web anterior |
| Loader / CAD | Sin intervención |

Sin tests, compilaciones, validadores, navegador, servidor, puertos, flasheos,
promoción ESP, commit ni push. No se tocó D:/AutoProyecto. La similitud del flujo
se cotejó en código; no se afirma equivalencia funcional/visual probada.
