# Organización del proyecto y Git

AFR2-20261023-04, 23/10/2026. Por pedido del usuario, D:/ProyectoRobotica2 es el único repositorio Git de trabajo. Antes había cuatro repositorios independientes, limpios y sin remotos. Ahora sus carpetas se conservan como módulos, junto con AutoFinalRobo2-CAD. No hay submódulos ni Git anidados activos.

| Módulo | Responsabilidad |
| --- | --- |
| AutoFinalRobo2-Atmega | Firmware C/avr-libc para Mega |
| AutoFinalRobo2-ESP | Firmware Arduino ESP8266/LittleFS |
| AutoFinalRobo2-Web | React/Vite |
| AutoFinalRobo2-Contracts | Especificaciones, cambios AFR2, evidencia y coordinación |
| [AutoFinalRobo2-CAD](../../AutoFinalRobo2-CAD/README.md) | KiCad, pieza nueva, modelos de componentes y ensamblaje completo del auto |

README.md, AGENTS.md, INFORME_MAESTRO.md, .gitignore y AutoFinalRobo2.code-workspace en la raíz explican la organización global. Contracts sigue siendo el punto de entrada documental. El workspace apunta a la raíz y muestra todos los módulos.

## Qué se versiona

Fuentes propias y referencias históricas preservadas; documentos/planos aportados; archivos CAD editables; huellas/símbolos/modelos propios; scripts de generación; manifests y lockfiles de dependencias; configuración compartida sin secretos. Los futuros package-lock.json, pnpm-lock.yaml, yarn.lock o equivalentes deben conservarse para recuperar versiones exactas.

El .gitignore raíz excluye node_modules, .pio, entornos Python, cachés de paquetes/herramientas, builds/dist/artifacts, compilados y preferencias locales/secretos. En CAD excluye .downloads, vendor recuperable, exportaciones, renders y backups/locks automáticos. Guardar URL, versión, licencia, checksum y pasos de obtención en reference/DEPENDENCIAS.md antes de depender de una descarga no versionada. Las bibliotecas propias y las adaptaciones sí se versionan.

No se ignoran globalmente PDF, STEP, STL, GLB, FCStd ni bibliotecas KiCad: pueden ser insumos únicos o fuentes. Las exportaciones regenerables se guardan en AutoFinalRobo2-CAD/exports y quedan fuera de Git. Un entregable que deba conservarse como evidencia puede versionarse en una ubicación explícita, documentando el motivo y su procedencia. No se eliminó evidencia existente para reducir el tamaño.

Los nueve modelos que estaban en CADS se trasladaron a AutoFinalRobo2-CAD/reference/supplied. Su URL, versión y licencia todavía no están identificadas, por lo que se conservan como insumos; [inventario](../../AutoFinalRobo2-CAD/reference/insumos-locales.json). CADS/README.md señala la nueva ubicación. No se descargó ni instaló software para esta reorganización.

## Historial y recuperación

La rama activa es codex/monorepo-cad. El commit de integración tiene como padres los cuatro HEAD anteriores: conserva sus commits/autores sin reescribirlos y reúne los archivos bajo sus carpetas actuales. Los commits antiguos muestran las rutas internas de cada repo original; a partir de la integración se usan rutas con prefijo de módulo.

| Etiqueta de conservación | HEAD anterior |
| --- | --- |
| archive/pre-monorepo/atmega | 72238e3f6ef87730049febd8d750dd9957f118f4 |
| archive/pre-monorepo/contracts | 42e50d3549270a84fd8a07461da0928ae3e566ad |
| archive/pre-monorepo/esp | a6d10ce0cd1f6bbd51ee2439b27e1d06b2393ad9 |
| archive/pre-monorepo/web | f649f072045eb683a54f679619ddd08a70c6613c |

Los metadatos originales completos se guardan en D:/ProyectoRobotica2/.git/legacy-repositories/AutoFinalRobo2-{Atmega,Contracts,ESP,Web}.git. Incluyen índice, configuración, reflogs y referencias auxiliares locales. Es un resguardo local dentro de .git: no se sube ni viaja en un clone. Los cuatro historiales de commits sí son alcanzables desde la rama de integración. No se crearon remotos ni se hizo push.

Para consultar una versión anterior sin alterar archivos: `git show archive/pre-monorepo/contracts:README.md` desde la raíz. Para recuperación completa, conservar primero el trabajo actual y la carpeta .git global; restaurar metadatos originales requiere una operación explícita y coordinada, porque reactivaría repos anidados y no recuperaría por sí sola el contenido antiguo del checkout. No borrar resguardos ni ejecutar reset/clean como mantenimiento automático.

Las operaciones de esta entrega fueron inventario/lectura, edición documental, traslado de insumos y Git local con hooks deshabilitados. No hubo tests, builds, validadores, ERC/DRC, flasheos ni pruebas físicas.
