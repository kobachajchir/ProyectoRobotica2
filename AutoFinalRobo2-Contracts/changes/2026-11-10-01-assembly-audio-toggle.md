# AFR2-20261110-01 — Botón de audio en Armado

Versión: **1.14.0 (subversión)**.

Botón junto a Play para activar/silenciar la narración. Audio activado inicialmente; icono propio de altavoz con ondas cuando está activo y altavoz con cruz cuando está silenciado, integrado al registro único de iconos Web. Estado, tooltip y nombre accesible actualizados con aria-pressed.

El botón modifica muted del elemento de audio sin detenerlo: audio y escena siguen el mismo reloj y conservan sincronización. Su interacción por puntero, toque o teclado está excluida del manejador que pausa por navegación manual. La elección se conserva durante la visita, también al pausar, reanudar y reiniciar el recorrido; no se persiste entre montajes de página.

Archivos: Web `AssemblyPage.tsx`, `useAssemblyPlayback.ts` e `components/ui/icons/registry.ts`. Header compartido y versiones package.json/lock actualizados.

Estado: Web=fuente; ATmega/ESP=solo versión; CAD/Loader=sin cambios. Sin tests, compilaciones, validaciones, navegador ni flasheo. El paquete 1.13.0 compilado previamente no incluye este botón.
