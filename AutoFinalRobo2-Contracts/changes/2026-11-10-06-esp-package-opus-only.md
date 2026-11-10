# AFR2-20261110-06 — Armado sin narración en ningún bundle y sin botón de volumen

Versión: **1.15.0**, conservada como corrección antes de cargar (regla 3).

## Origen

El usuario corrió `build:esp`. `tsc` y `vite build` pasaron, pero `tools/package-web.mjs` rechazó el paquete con «Paquete ESP demasiado grande: estimación 2875392, presupuesto 2867200 bytes». Se pasaba por 8.192 bytes, un bloque de la estimación de `web-budget.mjs`.

La primera corrección quitaba solo el `.m4a` del perfil ESP. El usuario pidió además retirar el botón de volumen de Armado y, finalmente, **no dejar la narración en ningún bundle**. Esa es la decisión vigente; sustituye la variante «solo Opus en ESP».

## Cambios

- `vite.config.ts`: `runtimeAssets` ya no emite `audio/armado-narracion.opus` ni `audio/armado-narracion.m4a`, ni en el perfil ESP ni en el completo. Libera unos 175 KB de la estimación ESP (35,89 + 138,71 kB, redondeados a bloques de 8 KiB). Las grabaciones originales siguen en `public/audio/` como insumo único, sin referencias.
- `AssemblyPage.tsx`: se eliminan el elemento `<audio>`, sus fuentes, el aviso de error de narración y el botón de volumen (`data-assembly-audio`, agregado en AFR2-20261110-01). Los textos de Play pasan a «Reproducir recorrido · 43,9 s» y «Pausar recorrido».
- `useAssemblyPlayback.ts`: Play/Pausa sigue funcionando en silencio. El reloj ahora es el tiempo de cuadro (`requestAnimationFrame`) recorrido sobre el mismo cronograma de `assemblyNarration.ts` (43,9 s y las mismas marcas), así que el ritmo de la animación no cambia. Pausa ante scroll, toque, tecla, pérdida de foco o pestaña oculta, como antes. Se retiran `audioRef`, `audioError`, `onAudioError`, `audioEnabled` y `toggleAudio`.
- Los iconos `audio-on`/`audio-off` quedan en el registro sin uso. Los MIME `.opus`/`.m4a` siguen en `package-web.mjs` y `storage.cpp`, sin efecto.
- El firmware del ESP no cambia: el manifiesto define los archivos servidos.

Estado: Web=fuente; ESP/ATmega/CAD/Loader=sin cambios. No se volvió a ejecutar el build ni el empaquetado, así que la estimación nueva no está medida.
