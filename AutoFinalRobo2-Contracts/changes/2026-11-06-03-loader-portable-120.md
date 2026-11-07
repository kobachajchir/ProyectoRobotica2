# AFR2-20261106-03 — Loader 1.2.0 compartible

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex.
Pedido: generar una versión para compartir, sin dependencia de rutas del equipo
del autor y con configuración editable; retirar la pestaña ATmega redundante,
manteniendo sus funciones necesarias en Flasher/Operación.

## Entrega

- Carpeta: `D:/ProyectoRobotica2/AFR2-Loader-dist-1.2.0`.
- ZIP: `D:/ProyectoRobotica2/AFR2-Loader-dist-1.2.0.zip`.
- Tamaño ZIP: **1395047 bytes**; 22 archivos de entrega más manifiesto/directorios.
- SHA-256: `4f2494cb360881ee4bf97879e7d9693f331742a0f48e3bb3234c92c15302ac54`.
- GUI/MCP precompilados: versión de archivo **1.2.0.0** en ambos ejecutables.
  El usuario receptor no necesita fuentes ni compilador C# para abrirlos.
- `distribucion.json` registra rutas relativas, tamaños y hashes; el ZIP tiene
  archivo SHA-256 acompañante. Directorio/ZIP son regenerables y están excluidos
  del Git; fuentes, empaquetador, guías y procedencia sí se conservan.

## Cambios de portabilidad

- `{App}` se resuelve en la carpeta de distribución actual; el script incluido
  deja de guardarse con la ruta absoluta de la instalación del desarrollador.
- Las rutas relativas se anclan a la aplicación y no al directorio de trabajo.
  Se rechazan variables sin resolver, rutas ambiguas dependientes de unidad
  actual y referencias circulares.
- `{AtmegaDir}`, `{EspDir}` y `{WebDir}` permiten que imágenes y entradas sigan
  la carpeta de módulo configurada. Ya no exigen una raíz/ubicación común.
  `{Repo}` sigue siendo una comodidad opcional; cuando se descubre un árbol
  padre se almacena relativo a la aplicación.
- Herramientas, scripts, intérprete, argumentos, proyectos, entornos, archivos,
  baudios y memoria siguen siendo editables en Configuración. Descubrimiento
  usa PATH/instalaciones del usuario actual y respeta PLATFORMIO_CORE_DIR.
  El intérprete PowerShell predeterminado usa SystemRoot del receptor.
- Datos personales por usuario en LOCALAPPDATA; AFR2_LOADER_DATA_DIR permite
  elegir otra carpeta. Los accesos Iniciar.ps1/Mcp.ps1 del paquete también
  aceptan -Datos. GUI/MCP deben compartir ese destino. El prompt MCP incorpora
  la ruta y el destino de datos efectivos de la computadora que lo genera.
- No se cambia silenciosamente un perfil existente: rutas absolutas elegidas
  anteriormente por el usuario deben revisarse al trasladar ese perfil. El ZIP
  distribuye una plantilla sin herramientas ni ubicaciones del autor.
- Empaquetador toma la versión desde LoaderVersion y admite -Destination.
  No sobrescribe entregas. Copia una lista explícita de runtime, recursos,
  scripts, plantillas y documentación; nunca datos de LOCALAPPDATA ni proyectos.
  Incluye las referencias documentales enlazadas por el manual y ejemplos MCP
  relativos. No copia fuentes, dependencias instaladas ni firmware.
- Compilar.ps1 descubre csc de .NET Framework desde Windows si el host es
  PowerShell7; no depende del runtime de Codex. Se desactiva salida debug/PDB.
- Metadatos de ambos ejecutables toman la versión de la aplicación.

## Navegación

Se elimina la pestaña ATmega redundante. Flasher conserva instalación,
comprobación y restauración, y ofrece Preparar firmware del Mega para ir al
destino ATmega de Operación. Configuración conserva campos por placa; monitor
conserva lectura ATmega. No se retira firmware ni soporte AVR del backend/MCP.
Se conserva la disposición lateral previa de etapas; se deshabilita su scroll
y las filas comparten el espacio visible.

## Requisitos y evidencia

Windows10/11 + .NET Framework4.8 para GUI; Windows PowerShell5.1 para scripts/MCP.
AVR GNU/Make/avrdude, PlatformIO, Python/esptool y Node/npm cuando se necesiten
son dependencias externas configurables, no incluidas. El script Web AFR2
requiere Web y ESP hermanos por el promotor del propio proyecto; para otra
distribución de módulos se puede elegir un script propio. Explicado en
AutoFinalRobo2-Loader/PORTABILIDAD.md y LEEME.txt del paquete.

Compilación de GUI y MCP autorizada expresamente. Auditoría estática del ZIP,
manifiesto/hashes, ejecutables, plantillas y enlaces del manual: sin discrepancias,
sin rutas del autor y sin entradas fuera de la carpeta del ZIP. Se abrió la GUI y se revisó mediante capturas; no se ejecutaron pruebas
automatizadas del firmware ni procesos de sus herramientas.
No se abrieron COM ni se compiló/flasheó ATmega/ESP/Web. No se afirma prueba en
una segunda computadora ni validación física de la aplicación.

Estado por módulo: Loader compilado/empaquetado y contenido auditado;
ATmega/ESP/Web/CAD sin cambios por esta ficha. El trabajo concurrente de
Wi-Fi/IR/OLED y su documentación permanece intacto.

Procedencia y archivos: `../provenance/loader-portable-20261106-03.json`.

## Apariencia y teclado
Se agregan Claro/Oscuro/Sistema, con paleta del proyecto, preferencia local y
contraste en controles, menús y enlaces. Iconos de desplegables separados del
borde y del texto. Aviso KEY/RST con casilla vacía enfocada, instrucciones
sin selección; Espacio alterna y continuar exige confirmación separada.
Revisión visual y límites en docs/design-system/loader-120.md. El ejecutable
de la distribución final también fue abierto y revisado. No se provocó una
intervención física para probar el aviso; ese foco se verificó en fuente.
