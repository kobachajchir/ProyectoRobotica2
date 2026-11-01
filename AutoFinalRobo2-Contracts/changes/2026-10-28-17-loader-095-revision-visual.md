# AFR2-20261028-17 — Loader 0.9.5: corrección KEY y revisión visual

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex.

## Pedido y alcance

El usuario corrige el nombre físico del botón a **KEY**, solicita mejorar orden,
contraste, separación, alineación y recortes de la interfaz y autoriza abrirla,
capturar pantallas y repetir la revisión. Se conserva la versión **0.9.5**.
Esta ficha sustituye la referencia Q de la ficha16; no cambia GPIO ni cableado.

## Cambios

- `ActionButton.cs`: botón WinForms con dibujo propio, icono centrado, separación
  explícita de10px entre icono y texto, márgenes internos, foco de teclado,
  estados hover/pulsado/deshabilitado e icono deshabilitado legible. Conserva la
  semántica Button y su operación por teclado.
- Desplegables de Compilar, Flashear y Guardar: renderizador propio de ToolStrip,
  verde oscuro con texto/flecha blancos, separador y área de flecha de36px.
  No dependen del fondo del renderizador nativo de Windows. Menús y botones
  comunes comparten alineación y separación; deshabilitado usa gris legible.
- Operación: etapas en banda horizontal de cuatro columnas, sin AutoScroll y
  vacías al iniciar. Se seleccionan por la operación solicitada: compilar,
  detectar, instalar flasher, restaurar backup o cargar firmware. Los eventos
  incorporan pasos reales adicionales si corresponden. No se muestran pasos de
  escritura/verificación durante una detección. Los builds MCP se distinguen del
  destino seleccionado en la ventana. Se conservan estados tipados y tooltips.
- Distribución adaptable: configuración/detalles en un área desplazable separada;
  etapas/registro y acciones inferiores conservan su espacio al reducir la
  ventana. Solo la primera área puede desplazarse; la banda de etapas nunca.
- Flasher ATmega vuelve a registrarse directamente junto a Operación. La
  operación de quitar/reinsertar la pestaña dejaba el tab ausente en la GUI.
- Campos agrupados con altura/alineación comunes; texto completo del resumen
  de memoria; selector de método alineado y casilla breve «Guiar KEY / RST».
- Monitor: puerto y acciones, parámetros RX, vista, herramientas y TX separados.
  Altura de herramientas calculada según las filas que ocupan; panel TX mayor,
  con Enviar/Detener completos y espacio para el estado. Sin cambios al dueño
  del COM, cola de envío, bytes, terminadores ni registros.
- Configuración conserva el filtro por placa/subsección. Pie de botones alineado
  y sin la reserva excesiva de espacio. Historial/MCP usan tablas claras con
  cabeceras diferenciadas. Eventos conserva su comportamiento.
- Conversor: Pegar junto a Copiar HEX normalizado y Limpiar debajo de la entrada;
  resultado y su copia separados. Conserva conversión explícita y nota inferior.
- KEY reemplaza Q en textos actuales de GUI, asistentes, errores, operaciones,
  descripciones MCP, perfil y manual. Los informes históricos conservan su fecha.

## Revisión y límites

Se compiló **solo el Loader** con csc de .NET Framework y se ejecutó su GUI por
pedido expreso. Se realizaron capturas y varias iteraciones sobre Operación,
Flasher, Configuración, Codex/MCP, Historial, Monitor, Eventos y Conversor.
Se revisaron ventanas de aproximadamente1220×880 y1000×750. Las capturas se
mostraron durante la conversación; no se agregan binarios regenerables a Git.

Una muestra temporal fuera de `src/`, identificada en el título como
«PREVISUALIZACIÓN de etapas · sin operación ni COM», invocó únicamente los
métodos de presentación ResetWorkflow/ShowStep con datos de ejemplo. Se observó
una carga ESP con siete pasos completos en dos filas, sin scroll, y registro
visible a1000×750. **La muestra no ejecutó una operación ni acredita sus etapas.**

No se pulsaron Compilar/Flashear/Detectar/Abrir puerto/Enviar ni se ejecutaron
herramientas de firmware. Sin pruebas físicas, TX real, COM abierto, flasheos,
tests de firmware ni prueba de conexión MCP por el agente. La GUI inicia su
puente local habitual; esto no se presenta como una conexión comprobada.
Otros DPI, temas de alto contraste y operaciones físicas siguen sin comprobar.

Estado: Loader compilado y revisado visualmente; ATmega/ESP/Web/CAD sin cambios
de producto por esta ficha. Procedencia por archivo en
`../provenance/loader-095-revision-visual-20261028.json`.
