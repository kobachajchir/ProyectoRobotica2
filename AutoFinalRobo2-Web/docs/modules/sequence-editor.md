> **Vigente AFR2-20261025-02:** [táctil local y giro simulado](../../../AutoFinalRobo2-Contracts/contracts/WEB_TOUCH_SIMULATION.md). Sesión local sin comandos, vista superior/flechas compartidas y ruedas por cursor en GOTO/secuencias. No se usa pose medida. Web compilada; visual/móvil pendientes. Descripciones previas conservadas como historia.

> AFR2-20261025-01: [control directo7](../../../AutoFinalRobo2-Contracts/contracts/DIRECT_CONTROL_7.md). Se eliminan todas las operaciones y estados de permiso y la habilitación OLED. Las acciones se envían directamente; solo permanecen conexión/capacidades, límites, estado ocupado, parada/caducidad y calibración. Fuente compilada; runtime pendiente. Estados inferiores históricos.

# Editor y ajustes — AFR2-20261024-12

Conexión y apariencia tiene Información de hardware y Comandos de reinicio
como desplegables principales. Hardware agrupa conexión, ESP, ATmega y arranques
en secciones separadas; Última información es fecha/hora de la última respuesta
correcta, junto al botón Actualizar. Apariencia usa botones. No hay selector de
control ni dirección/conexión manual; WebSocket mantiene el host del navegador.

Chasis y Pistas independientes conservan borradores separados al alternar.
La cantidad de pistas activas se puede reducir y ampliar, incluso hasta cero;
las ocultas conservan su borrador y aportan consigna cero. FL/FR/RL/RR son
botones separados. Seleccionar una pista aplica acento al contenedor. La papelera
junto a Editable abre el FloatingModal animado existente; confirmar elimina
esa pista y bloques, y Deshacer los recupera. No cambia autorías del componente.

Finalización es terminal: no admite nuevas inserciones ni ordenar bloques
después. Quitar el terminal vuelve a permitir agregar. Movimiento y rampas
admiten tiempo(ms) o recorrido de rueda(mm); espera conserva tiempo. En Chasis,
distancia refiere a la rueda de mayor recorrido, incluso durante un pivote.
La simulación estima duración integrando la magnitud de velocidad de rueda.

Documento `afr2-sequence-2`, claves de biblioteca v2, importación v1 compatible;
la biblioteca v1 queda como respaldo y solo se lee si aún no hay v2. Conserva
pistas inactivas/borradores vacíos locales. Importación externa sigue validada.

Editor64bloques/pista y10min; hardware32registros y120s máximos. Un programa
por distancia se carga mediante EX01v6 por rueda, independiente o con barreras
para Chasis. Tiempo solo conserva el formato anterior. Memoria/Hall/calibración
siguen bloqueando el envío físico. No usar tiempo de simulación como evidencia
de llegada ni orientación real.

[Contrato EX01v6](../../../AutoFinalRobo2-Contracts/contracts/EX01_SEQUENCE_DISTANCE_6.md).
Compilado sin tests, navegador, flasheo ni ejecución de placas. El tamaño con
gates0 no aprueba la memoria de la rama física habilitada.
