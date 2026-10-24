# Dependencias recuperables

Actualización 23/10/2026, AFR2-20261023-06: se descargó un primer lote CAD para aprobación, sin instalar herramientas ni comenzar el modelado. [Lista y diferencias](../docs/REVISION_CANDIDATOS_3D.md), [manifiesto por archivo](candidatos-2026-10-23.json) y [recuperación](Recuperar-Candidatos.ps1). Ejecutar el script de recuperación con PowerShell desde cualquier checkout; resuelve rutas relativas al módulo y conserva archivos existentes. No importa modelos ni ejecuta pruebas. Las descargas se guardan en vendor/revision-2026-10-23, ignorado por Git. Fusion sigue siendo la herramienta prevista.

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
