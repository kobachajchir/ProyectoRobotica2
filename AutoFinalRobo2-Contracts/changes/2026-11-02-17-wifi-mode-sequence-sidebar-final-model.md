# AFR2-20261102-17 — Modo Wi-Fi superior, controles laterales y modelo final

02/11/2026. Pedido explícito de ajustes de interfaz.

- Conexión: Cambiar modo y Configurar se trasladan a un div propio antes del selector de secciones. El modo se toma de una respuesta recibida del ESP; sin dato se muestra Sin dato actual. Se siguen las actualizaciones mientras no exista una elección pendiente y se reinicia la sincronización al desconectar. Cambiar la selección no envía comandos; Aplicar modo conserva wifi.mode y Configurar conserva el formulario existente.
- Barra lateral: Operar, Preparar, Explorar y Sistema tienen tintas action, warning, info y success, tomadas del tema.
- Programas/secuencias: Simulación y ejecución se sitúa junto al visor, a la derecha desde 1200 px; en anchos menores se apila. Controles, bloqueos y handlers conservados.
- Catálogo del brazo: retirado el descriptor procedural PVC32 AGV v03 y su helper local. V17 final queda como único modelo registrado; el selector condicional existente desaparece al haber una sola opción. Se preservan el descriptor final, los puntos guardados y los archivos CAD históricos.

Sin cambios de protocolo, productores, firmware, IDs o escenas. atmega/esp sin cambios; web fuente modificada; shared/Contracts documentación. Sin tests, compilaciones, validaciones, navegador, promoción ESP ni flasheos. Composición y comportamiento en ejecución No verificados. Sin commit ni publicación solicitados.
