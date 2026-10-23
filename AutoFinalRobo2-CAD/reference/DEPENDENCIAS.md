# Dependencias recuperables

No se instaló ni descargó ninguna dependencia en esta entrega. La versión de KiCad y la herramienta de modelado se definirán cuando comience el diseño.

Para una biblioteca, modelo o herramienta que se descargará fuera de Git, registrar:

| Dato | Contenido requerido |
| --- | --- |
| Identidad | Nombre y finalidad |
| Origen | URL exacta y proveedor/autor |
| Revisión | Versión, etiqueta o commit fijo |
| Integridad | SHA-256 del archivo recuperado |
| Permisos | Licencia conocida y atribución |
| Recuperación | Pasos o script revisable, sin credenciales |
| Ubicación | vendor/ o .downloads/, relativa al módulo |
| Uso | Proyectos/archivos que lo requieren y si se modificó |

No ejecutar scripts heredados de una descarga por su presencia. Las bibliotecas estándar de KiCad y dependencias de herramientas se recuperarán mediante sus gestores o instaladores cuando se autorice; sus manifests/lockfiles/configuración sí se versionan.

Los nueve insumos de supplied no tienen aún origen remoto y licencia identificados; se mantienen versionados. No declararlos recuperables solo por parecer descargados. Las adaptaciones propias se versionan en mecanica o kicad/libraries.
