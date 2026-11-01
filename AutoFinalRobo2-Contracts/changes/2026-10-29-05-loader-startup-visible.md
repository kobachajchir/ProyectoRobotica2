# AFR2-20261029-05 — Ventana visible al finalizar bienvenida

Pedido: corregir inicio que deja la ventana oculta/minimizada y mostrar el número de versión en la animación. Conserva 1.1.0.

Autoría Koba Chajchir / AFR2 conservada; adaptación mediante Codex.

- `AutoFinalRobo2-Loader/src/Program.cs`: elimina Opacity=0 de la ventana principal. Su visibilidad deja de depender de un callback posterior al cierre de la bienvenida.
- `src/StartupPresentation.cs`: al recibir Shown restaura la ventana si está minimizada, asegura opacidad/visibilidad y solicita primer plano; repite al concluir la bienvenida. La versión dinámica pasa a una línea propia, blanca y en negrita debajo del nombre; mensaje de carga y Esc quedan separados. Libera la fuente adicional al cerrar.

Causa abordada en fuente: la transición anterior ocultaba explícitamente MainForm y dependía de la finalización de otro hilo para mostrarla. No se afirma haber reproducido el estado reportado.

Aplicación recompilada con salida 0 mediante Compilar.ps1/Windows PowerShell, dentro de la continuidad del ejecutable solicitado. El acceso AFR2 Loader.exe inicia la aplicación actualizada. Sin ejecución GUI, tests, validadores, COM ni hardware; pendiente comprobación visual del primer plano y la versión en la sesión del usuario. ATmega/ESP/Web/CAD sin cambios.
