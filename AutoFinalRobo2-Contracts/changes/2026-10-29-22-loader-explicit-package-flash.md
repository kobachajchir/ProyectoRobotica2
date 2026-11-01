# AFR2-20261029-22 — Configurar paquete sin iniciar operaciones

Pedido humano: seleccionar el paquete/configuración no debe llevar automáticamente al asistente o al flasheo; solo iniciar carga al pulsar Flashear.

Loader1.1.4. En fuente no había una llamada directa a Flash desde SelectAtPackage; sí existía detección ESP implícita en cambios de sección FileSystem y al pulsar Compilar/Revisar con capacidad no confirmada. Ese camino abría el asistente KEY/RST y se retiró. Solo Consultar tamaño de flash/Detectar ESP inicia la detección. Elegir sección registra la elección local, sin consultar puertos ni cambiar automáticamente al destino anterior. Compilar/Revisar sin capacidad confirmada informa el requisito y retorna.

SelectAtPackage invalida siempre la preparación AT previa, incluso al elegir nuevamente el mismo archivo; informa preparación pendiente y conserva el foco en el selector. No detecta, compila/revisa ni flashea por selección. Flash está bloqueado durante configuración/sincronización/transición serie y el botón solo se habilita con preparación vigente. Revisar el paquete termina sin llamar a Flash: la carga sigue conectada únicamente a sus acciones explícitas. Las peticiones MCP explícitas y sus reglas de aprobación se conservan; no se cambian preferencias de autoaceptación.

Archivos: MainForm.EspModes.cs, MainForm.Preparation.cs, MainForm.cs, JobState.cs. Versión1.1.4. No cambia firmware ATmega/ESP/Web ni protocolo físico. GUI/MCP compilados correctamente con csc/.NET Framework, sin errores ni advertencias. Sin tests, operaciones COM, carga ni pruebas físicas por esta revisión. Loader anterior ya cerrado al recargar; se abre la nueva versión.
