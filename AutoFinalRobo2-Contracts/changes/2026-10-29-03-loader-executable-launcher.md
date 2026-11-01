# AFR2-20261029-03 — Ejecutable de inicio AFR2 Loader

Pedido expreso: crear ejecutable final identificable por nombre e icono que ejecute Iniciar. Se mantiene 1.1.0.

Autoría Koba Chajchir / AFR2 conservada; adaptación mediante Codex por pedido humano.

Archivos:
- `AutoFinalRobo2-Loader/launcher/Launcher.cs`: lanzador WinForms sin consola que ejecuta Iniciar.ps1 desde su propia carpeta, transmite argumentos y muestra errores de inicio. Metadatos AFR2 Loader 1.1.0.0. Mantiene las opciones PowerShell del Iniciar.cmd existente.
- `Generar-Ejecutable.ps1`: genera icono desde el recurso original y compila AFR2 Loader.exe; no abre GUI.
- `assets/brand/afr2-loader.ico`: icono derivado del logo AFR2 original.
- `Compilar.ps1`: incrusta también el icono en el ejecutable real e incluye su hash en la identidad de compilación.
- `.gitignore` raíz: excluye ejecutables generados de la raíz del Loader; fuentes y recurso de icono conservados.
- `README.md`, `MANUAL.html`: entrada recomendada mediante AFR2 Loader.exe y necesidad de conservar la carpeta completa.

Resultado: `AutoFinalRobo2-Loader/AFR2 Loader.exe` generado. Aplicación real compilada en LOCALAPPDATA/AutoFinalRobo2Loader/bin/AFR2Loader.exe. Compilación de ambos completada con salida 0 usando Windows PowerShell y .NET Framework. El primer intento bajo PowerShell 7 no encontró csc.exe por la diferencia de runtime; se utilizó Windows PowerShell y se agregó búsqueda alternativa del compilador al generador.

La compilación de esta ficha fue autorizada por el pedido de crear el ejecutable. No se ejecutó el lanzador ni la GUI, no se hicieron tests ni validadores, no se abrió COM ni se compiló/cargó firmware. Esta evidencia acredita compilación del Loader 1.1, incluidos los cambios anteriores en fuente; no acredita comportamiento visual o físico. ATmega/ESP/Web/CAD sin cambios por esta ficha. El EXE es un acceso de inicio distribuido junto a los demás archivos, no un paquete autocontenido ni instalador.
