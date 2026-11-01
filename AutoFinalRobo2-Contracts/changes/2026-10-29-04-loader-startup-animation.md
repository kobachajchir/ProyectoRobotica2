# AFR2-20261029-04 — Bienvenida animada del Loader

Pedido: animación de inicio inspirada en la Web, mostrando el icono mientras carga la UI. Versión conservada 1.1.0.

Autoría Koba Chajchir / AFR2 conservada; adaptación mediante Codex. Referencias leídas: AutoFinalRobo2-Web/src/components/ui/StartupWelcome.tsx y estilos startup.css. No se modifica la Web ni se incorpora su código React al Loader.

Archivos adaptados:
- `AutoFinalRobo2-Loader/src/StartupPresentation.cs`: bienvenida WinForms en hilo STA independiente para mantener la animación mientras se construyen los controles. Logo original, fondo azul oscuro y halo estático, nombre/versión y mensaje de preparación. Entrada de 250 ms, escala 0,95→1 y opacidad, salida de 200 ms con cubic-bezier(.23,1,.32,1). Sin espera mínima artificial. Esc omite la bienvenida sin cancelar la carga. Con animaciones Windows desactivadas o contraste alto elimina la escala y usa salida suave de 120 ms. Libera imágenes, fuentes y timer al cerrar; errores de bienvenida no bloquean el inicio principal.
- `src/Program.cs`: presenta bienvenida tras confirmar instancia única y argumentos, prepara MainForm y revela la ventana al terminar la salida. El modo MCP stdio conserva su retorno previo y no muestra bienvenida.
- `MANUAL.html`, `README.md`: documentan inicio, omisión y límites.

Compilación autorizada dentro de la continuidad del pedido de crear el ejecutable: completada con salida 0 mediante Compilar.ps1/Windows PowerShell. Se corrigió durante compilación una propiedad no disponible en .NET Framework, reemplazándola por lectura nativa de SPI_GETCLIENTAREAANIMATION. El lanzador AFR2 Loader.exe sigue ejecutando Iniciar.ps1 y abre la aplicación actualizada.

Sin ejecutar GUI, tests, validadores, hardware, COM, compilación de firmware ni flash. Pendiente revisión visual de ritmo, DPI y transición sobre Windows. ATmega/ESP/Web/CAD sin cambios por esta ficha. La bienvenida indica preparación de interfaz, no progreso medido ni comunicación con placas.
